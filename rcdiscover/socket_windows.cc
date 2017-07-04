/*
* Roboception GmbH
* Munich, Germany
* www.roboception.com
*
* Copyright (c) 2017 Roboception GmbH
* All rights reserved
*
* Author: Raphael Schaller
*/

#include "socket_windows.h"

#include "socket_exception.h"

#include <iphlpapi.h>

namespace rcdiscover
{

const ULONG SocketWindows::broadcast_addr_{inet_addr("255.255.255.255")};

const ULONG &SocketWindows::getBroadcastAddr()
{
  return broadcast_addr_;
}

SocketWindows SocketWindows::create(const ULONG dst_ip, const uint16_t port)
{
  return SocketWindows(AF_INET,
                       SOCK_DGRAM,
                       IPPROTO_UDP,
                       dst_ip,
                       port);
}

std::vector<SocketWindows> SocketWindows::createAndBindForAllInterfaces(
  const uint16_t port)
{
  ULONG forward_tab_size = 0;
  PMIB_IPFORWARDTABLE table = nullptr;

  int result = NO_ERROR;
  for (int i = 0; i < 5; ++i)
  {
    result = GetIpForwardTable(table, &forward_tab_size, false);

    if (result == NO_ERROR)
    {
      break;
    }
    else if (result == ERROR_INSUFFICIENT_BUFFER)
    {
      free(table);
      table = (PMIB_IPFORWARDTABLE)malloc(forward_tab_size);
    }
  }
  if (result != NO_ERROR)
  {
    throw SocketException("Error while getting forward table", ::WSAGetLastError());
  }

  std::vector<SocketWindows> sockets;
  for (unsigned int i = 0; i < table->dwNumEntries; ++i)
  {
    PMIB_IPFORWARDROW row = &table->table[i];
    if (row->dwForwardDest != broadcast_addr_ ||
        row->dwForwardMask != ULONG_MAX ||
        row->dwForwardType != MIB_IPROUTE_TYPE_DIRECT)
    {
      continue;
    }

    sockets.emplace_back(SocketWindows::create(broadcast_addr_, port));

    sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_port = 0;
    src_addr.sin_addr.s_addr = row->dwForwardNextHop;

    sockets.back().bind(src_addr);
  }
  return sockets;
}

SocketWindows::SocketWindows(int domain,
                             int type,
                             int protocol,
                             const ULONG dst_ip,
                             const uint16_t port) :
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
     throw SocketException("Error while sending data", ::WSAGetLastError());
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
    throw SocketException("Error while setting socket options", ::WSAGetLastError());
  }
}

void SocketWindows::enableNonBlockingImpl()
{
  ULONG imode = 1;
  if (::ioctlsocket(sock_, FIONBIO, &imode) == SOCKET_ERROR)
  {
    throw SocketException("Error while setting socket options", ::WSAGetLastError());
  }
}

}
