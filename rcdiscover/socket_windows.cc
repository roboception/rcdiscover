/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Raphael Schaller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "socket_windows.h"

#include "socket_exception.h"

#include <iphlpapi.h>
#include <iostream>
#include <map>

namespace rcdiscover
{

const ULONG &SocketWindows::getBroadcastAddr()
{
  static const ULONG baddr = htonl(INADDR_BROADCAST);
  return baddr;
}

SocketWindows SocketWindows::create(const ULONG dst_ip, const uint16_t port, std::string iface_name)
{
  return SocketWindows(AF_INET,
                       SOCK_DGRAM,
                       IPPROTO_UDP,
                       dst_ip,
                       port,
                       std::move(iface_name));
}

static std::map<int, std::string> getInterfaceNames()
{
  PIP_ADAPTER_INFO adapter_info;
  adapter_info = static_cast<IP_ADAPTER_INFO *>(malloc(sizeof(IP_ADAPTER_INFO)));
  ULONG buflen = sizeof(IP_ADAPTER_INFO);

  if(GetAdaptersInfo(adapter_info, &buflen) == ERROR_BUFFER_OVERFLOW)
  {
    free(adapter_info);
    adapter_info = static_cast<IP_ADAPTER_INFO *>(malloc(buflen));
  }

  std::map<int, std::string> result;
  if(GetAdaptersInfo(adapter_info, &buflen) == NO_ERROR)
  {
    PIP_ADAPTER_INFO adapter = adapter_info;
    while (adapter)
    {
      result.emplace(adapter->Index, adapter->AdapterName);
      adapter = adapter->Next;
    }
  }
  return result;
}

std::vector<SocketWindows> SocketWindows::createAndBindForAllInterfaces(
  const uint16_t port)
{
  std::vector<SocketWindows> sockets;

  const auto interface_names = getInterfaceNames();

  {
    // limited broadcast

    ULONG table_size = 0;
    PMIB_IPFORWARDTABLE table = nullptr;

    int result = NO_ERROR;
    for (int i = 0; i < 5; ++i)
    {
      result = GetIpForwardTable(table, &table_size, false);

      if (result == NO_ERROR)
      {
        break;
      }
      else if (result == ERROR_INSUFFICIENT_BUFFER)
      {
        free(table);
        table = (PMIB_IPFORWARDTABLE)malloc(table_size);
      }
    }
    if (result != NO_ERROR)
    {
      throw SocketException("Error while getting forward table",
                            ::WSAGetLastError());
    }

    for (unsigned int i = 0; i < table->dwNumEntries; ++i)
    {
      PMIB_IPFORWARDROW row = &table->table[i];

      if (row->dwForwardDest == getBroadcastAddr() &&
          row->dwForwardMask == ULONG_MAX &&
          row->dwForwardType == MIB_IPROUTE_TYPE_DIRECT)
      {
        const auto iface = interface_names.find(row->dwForwardIfIndex);
        if (iface != interface_names.end())
        {
          sockets.emplace_back(SocketWindows::create(getBroadcastAddr(), port, iface->second));

          sockaddr_in src_addr;
          src_addr.sin_family = AF_INET;
          src_addr.sin_port = 0;
          src_addr.sin_addr.s_addr = row->dwForwardNextHop;

          sockets.back().bind(src_addr);
        }
      }
    }
  }

  {
    // directed broadcast

    PMIB_IPADDRTABLE table = nullptr;
    ULONG table_size = 0;

    int result = NO_ERROR;
    for (int i = 0; i < 5; ++i)
    {
      result = GetIpAddrTable(table, &table_size, false);

      if (result == NO_ERROR)
      {
        break;
      }
      else if (result == ERROR_INSUFFICIENT_BUFFER)
      {
        free(table);
        table = (PMIB_IPADDRTABLE)malloc(table_size);
      }
    }
    if (result != NO_ERROR)
    {
      throw SocketException("Error while getting ip addr table",
                            ::WSAGetLastError());
    }

    for (unsigned int i = 0; i < table->dwNumEntries; ++i)
    {
      PMIB_IPADDRROW row = &table->table[i];

      if (row->dwAddr == htonl(INADDR_LOOPBACK))
      {
        continue;
      }

      const auto iface = interface_names.find(row->dwIndex);
      if (iface != interface_names.end())
      {
        const ULONG baddr = row->dwAddr | (~row->dwMask);

        sockets.emplace_back(SocketWindows::create(baddr, port, iface->second));

        sockaddr_in src_addr;
        src_addr.sin_family = AF_INET;
        src_addr.sin_port = 0;
        src_addr.sin_addr.s_addr = htonl(INADDR_ANY);

        sockets.back().bind(src_addr);
      }
    }
  }
  return sockets;
}

SocketWindows::SocketWindows(int domain,
                             int type,
                             int protocol,
                             const ULONG dst_ip,
                             const uint16_t port,
                             std::string iface_name) :
  Socket(std::move(iface_name)),
  sock_(INVALID_SOCKET),
  dst_addr_()
{
  sock_ = ::WSASocket(domain, type, protocol, nullptr, 0, 0);
  if (sock_ == INVALID_SOCKET)
  {
    throw SocketException("Error while creating socket", ::WSAGetLastError());
  }

  dst_addr_.sin_addr.s_addr = dst_ip;
  dst_addr_.sin_family = AF_INET;
  dst_addr_.sin_port = htons(port);
}

SocketWindows::SocketWindows(SocketWindows&& other) :
  Socket(std::move(other)),
  sock_(INVALID_SOCKET),
  dst_addr_(other.dst_addr_)
{
  std::swap(sock_, other.sock_);
}

SocketWindows& SocketWindows::operator=(SocketWindows&& other)
{
  std::swap(sock_, other.sock_);
  return *this;
}

SocketWindows::~SocketWindows()
{
  if(sock_ != INVALID_SOCKET)
  {
    ::closesocket(sock_);
  }
}

const SOCKET &SocketWindows::getHandleImpl() const
{
  return sock_;
}

void SocketWindows::bindImpl(const sockaddr_in& addr)
{
  if (::bind(sock_,
              reinterpret_cast<const struct sockaddr *>(&addr),
              sizeof(addr)) == SOCKET_ERROR)
  {
    throw SocketException("Error while binding to socket", ::WSAGetLastError());
  }
}

void SocketWindows::sendImpl(const std::vector<uint8_t>& sendbuf)
{
  auto sb = sendbuf;

  WSABUF wsa_buffer;
  wsa_buffer.len = sb.size();
  wsa_buffer.buf = reinterpret_cast<char *>(sb.data());

  DWORD len;
  if (::WSASendTo(sock_,
             &wsa_buffer,
             1,
             &len,
             0,
             reinterpret_cast<const struct sockaddr *>(&dst_addr_),
             sizeof(dst_addr_),
             nullptr,
             nullptr) == SOCKET_ERROR)
  {
    int err = ::WSAGetLastError();
    // WSAENETUNREACH=10051, WSAEHOSTUNREACH=10065
    if (err == WSAENETUNREACH || err == WSAEHOSTUNREACH)
    {
      throw NetworkUnreachableException(
            "Error while sending data - network unreachable", err);
    }
    throw SocketException("Error while sending data", err);
  }
}

void SocketWindows::enableBroadcastImpl()
{
  const int yes = 1;
  if (::setsockopt(sock_,
                SOL_SOCKET,
                SO_BROADCAST,
                reinterpret_cast<const char *>(&yes),
                sizeof(yes)) == SOCKET_ERROR)
  {
    throw SocketException("Error while setting socket options",
                          ::WSAGetLastError());
  }
}

void SocketWindows::enableNonBlockingImpl()
{
  ULONG imode = 1;
  if (::ioctlsocket(sock_, FIONBIO, &imode) == SOCKET_ERROR)
  {
    throw SocketException("Error while setting socket options",
                          ::WSAGetLastError());
  }
}

}
