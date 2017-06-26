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

#include "wol_windows.h"

#include "wol_exception.h"
#include "operation_not_permitted.h"

#include <ios>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

#include <iphlpapi.h>

namespace rcdiscover
{

WOL_Windows::Socket WOL_Windows::Socket::socketUDP()
{
  return Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

WOL_Windows::Socket WOL_Windows::Socket::socketRaw()
{
  return Socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
}

WOL_Windows::Socket::Socket(int domain, int type, int protocol) :
  sock_(INVALID_SOCKET)
{
  sock_ = ::socket(domain, type, protocol);
  if (sock_ == INVALID_SOCKET)
  {
    throw WOLException("Error while creating socket", ::WSAGetLastError());
  }
}

WOL_Windows::Socket::Socket(Socket&& other) :
  sock_(INVALID_SOCKET)
{
  std::swap(sock_, other.sock_);
}

WOL_Windows::Socket& WOL_Windows::Socket::operator=(WOL_Windows::Socket&& other)
{
  std::swap(sock_, other.sock_);
  return *this;
}

WOL_Windows::Socket::~Socket()
{
  if(sock_ != INVALID_SOCKET)
  {
    ::closesocket(sock_);
  }
}

const SOCKET &WOL_Windows::Socket::getHandle() const
{
  return sock_;
}

void WOL_Windows::Socket::bind(const sockaddr_in& addr)
{
  if (::bind(sock_,
              reinterpret_cast<const struct sockaddr *>(&addr),
              sizeof(addr)) == SOCKET_ERROR)
  {
    throw WOLException("Error while binding to socket", ::WSAGetLastError());
  }
}

void WOL_Windows::Socket::sendto(const std::vector<uint8_t>& sendbuf,
            const sockaddr_in& addr)
{
  if (::sendto(sock_,
             reinterpret_cast<const char *>(sendbuf.data()),
             sendbuf.size(),
             0,
             reinterpret_cast<const struct sockaddr *>(&addr),
             sizeof(addr)) == SOCKET_ERROR)
   {
     throw WOLException("Error while sending data", ::WSAGetLastError());
   }
}

void WOL_Windows::Socket::enableBroadcast()
{
  const int yes = 1;
  if (::setsockopt(sock_,
                SOL_SOCKET,
                SO_BROADCAST,
                reinterpret_cast<const char *>(&yes),
                sizeof(yes)) == SOCKET_ERROR)
  {
    throw WOLException("Error while setting socket options", ::WSAGetLastError());
  }
}

WOL_Windows::WOL_Windows(uint64_t hardware_addr) :
  WOL(hardware_addr)
{ }

WOL_Windows::WOL_Windows(std::array<uint8_t, 6> hardware_addr) :
  WOL(std::move(hardware_addr))
{ }

std::vector<uint32_t> WOL_Windows::getBroadcastIPs() const
{
  std::vector<uint32_t> ips;

  MIB_IPADDRTABLE *iptab = reinterpret_cast<MIB_IPADDRTABLE *>(malloc(sizeof(MIB_IPADDRTABLE)));

  DWORD len = 0;
  for (int i = 0; i < 5; ++i)
  {
    const DWORD ipRet = GetIpAddrTable(iptab, &len, false);
    if (ipRet == ERROR_INSUFFICIENT_BUFFER)
    {
      free(iptab);
      iptab = reinterpret_cast<MIB_IPADDRTABLE *>(malloc(len));
    }
    else if (ipRet == NO_ERROR)
    {
      break;
    }
    else
    {
      free(iptab);
      iptab = nullptr;
      break;
    }
  }

  if (iptab != nullptr)
  {
    for (DWORD i = 0; i < iptab->dwNumEntries; ++i)
    {
      const auto& row = iptab->table[i];

      uint32_t baddr = row.dwAddr & row.dwMask;
      if (row.dwBCastAddr)
      {
        baddr |= ~row.dwMask;
      }

      ips.push_back(baddr);
    }

    free(iptab);
    iptab = nullptr;
  }

  return ips;
}

}
