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

#include "wol_linux.h"

namespace rcdiscover
{

WOL_Linux::Socket::Socket WOL_Linux::Socket::socketUDP()
{
  return Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
}

WOL_Linux::Socket::Socket WOL_Linux::Socket::socketRaw()
{
  return Socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
}

WOL_Linux::Socket::Socket(int domain, int type, int protocol) :
  sock_(-1)
{
  sock_ = ::socket(domain, type, protocol);
  if (sock_ == -1)
  {
    if (errno == EPERM)
    {
      throw OperationNotPermitted();
    }

    throw WOLException("Error while creating socket", errno);
  }
}

WOL_Linux::Socket::Socket(Socket&& other) :
  sock_(-1)
{
  std::swap(sock_, other.sock_);
}

WOL_Linux::Socket::Socket& WOL_Linux::Socket::operator=(Socket&& other)
{
  std::swap(sock_, other.sock_);
  return *this;
}

WOL_Linux::Socket::~Socket()
{
  if (sock_ != -1)
  {
    ::close(sock_);
  }
}

const int &WOL_Linux::Socket::getHandle() const
{
  return sock_;
}

void WOL_Linux::Socket::bind(const sockaddr_in& addr)
{
  if (::bind(sock_, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr)) == -1)
  {
    throw WOLException("Error while binding to socket", errno);
  }
}

void WOL_Linux::Socket::sendto(const std::vector<uint8_t>& sendbuf,
            const sockaddr_in& addr)
{
  if (::sendto(sock_,
             static_cast<const void *>(sendbuf.data()),
             sendbuf.size(),
             0,
             reinterpret_cast<const sockaddr *>(&addr),
             (socklen_t)sizeof(SockAddrT)) == -1)
   {
     throw WOLException("Error while sending data", errno);
   }
}

void WOL_Linux::Socket::enableBroadcast()
{
  const int yes = 1;
  if (::setsockopt(sock_,
                SOL_SOCKET,
                SO_BROADCAST,
                reinterpret_cast<const char *>(&yes),
                sizeof(yes)) == -1)
  {
    throw WOLException("Error while setting socket options", errno);
  }
}

WOL_Linux::WOL_Linux(uint64_t hardware_addr) :
  WOL(hardware_addr)
{ }

WOL_Linux::WOL_Linux(std::array<uint8_t, 6> hardware_addr) :
  WOL(std::move(hardware_addr))
{ }

std::vector<uint32_t> WOL_Linux::getBroadcastIPs() const
{
  std::vector<uint32_t> ips;

  ifaddrs *addrs;
  getifaddrs(&addrs);

  for(ifaddrs *addr = addrs;
      addr != nullptr;
      addr = addr->ifa_next)
  {
    auto baddr = addr->ifa_ifu.ifu_broadaddr;
    if (addr->ifa_name != nullptr &&
        addr->ifa_addr != nullptr &&
        addr->ifa_addr->sa_family == AF_INET &&
        baddr != nullptr)
    {
      ips.push_back(reinterpret_cast<struct sockaddr_in *>(baddr)->sin_addr.s_addr);
    }
  }

  freeifaddrs(addrs);
  addrs = nullptr;

  return ips;
}

}
