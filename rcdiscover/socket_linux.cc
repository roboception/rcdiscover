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

#include "socket_linux.h"

#include "socket_exception.h"
#include "operation_not_permitted.h"

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <ifaddrs.h>
#include <fcntl.h>

#include <iostream>

namespace rcdiscover
{

const in_addr_t SocketLinux::broadcast_addr_ = inet_addr("255.255.255.255");

const in_addr_t &SocketLinux::getBroadcastAddr()
{
  return broadcast_addr_;
}

const sockaddr_in& SocketLinux::getDestSockAddr() const
{
  return dst_addr_;
}

SocketLinux SocketLinux::create(const in_addr_t dst_ip, const uint16_t port)
{
  return SocketLinux(AF_INET, SOCK_DGRAM, IPPROTO_UDP, dst_ip, port);
}

std::vector<SocketLinux> SocketLinux::createAndBindForAllInterfaces(const uint16_t port)
{
  std::vector<SocketLinux> sockets;

  ifaddrs *addrs;
  getifaddrs(&addrs);

  bool global_broadcast = true;

  for(ifaddrs *addr = addrs;
      addr != nullptr;
      addr = addr->ifa_next)
  {
    auto baddr = addr->ifa_ifu.ifu_broadaddr;
    if (addr->ifa_flags & IFF_UP &&
        addr->ifa_name != nullptr &&
        addr->ifa_addr != nullptr &&
        addr->ifa_addr->sa_family == AF_INET &&
        baddr != nullptr)
    {
      std::string name(addr->ifa_name);
      if (name.length() != 0 && name != "lo")
      {
        sockets.emplace_back(
              SocketLinux::create(
                broadcast_addr_,
                port));

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = 0;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        sockets.back().bind(addr);

        try
        {
          sockets.back().bindToDevice(name);
        }
        catch(const OperationNotPermitted &)
        {
          sockets.back().dst_addr_.sin_addr.s_addr = reinterpret_cast<struct sockaddr_in *>(baddr)->sin_addr.s_addr;
          global_broadcast = false;
        }
      }
    }
  }

  freeifaddrs(addrs);
  addrs = nullptr;

  if (!global_broadcast)
  {
    // one socket for global broadcast on default interface (mostly eth0)
    sockets.emplace_back(SocketLinux::create(broadcast_addr_, port));
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sockets.back().bind(addr);
  }

  return sockets;
}

SocketLinux::SocketLinux(int domain, int type, int protocol,
                         in_addr_t dst_ip, uint16_t port) :
  sock_(-1),
  dst_addr_()
{
  sock_ = ::socket(domain, type, protocol);
  if (sock_ == -1)
  {
    if (errno == EPERM)
    {
      throw OperationNotPermitted();
    }

    throw SocketException("Error while creating socket", errno);
  }

  dst_addr_.sin_addr.s_addr = dst_ip;
  dst_addr_.sin_family = AF_INET;
  dst_addr_.sin_port = htons(port);
}

SocketLinux::SocketLinux(SocketLinux &&other) :
  sock_(-1),
  dst_addr_(std::move(other.dst_addr_))
{
  std::swap(sock_, other.sock_);
}

SocketLinux &SocketLinux::operator=(SocketLinux &&other)
{
  std::swap(sock_, other.sock_);
  return *this;
}

SocketLinux::~SocketLinux()
{
  if (sock_ != -1)
  {
    ::close(sock_);
  }
}

const int &SocketLinux::getHandleImpl() const
{
  return sock_;
}

void SocketLinux::bindImpl(const ::sockaddr_in& addr)
{
  if (::bind(sock_,
             reinterpret_cast<const sockaddr *>(&addr),
             sizeof(sockaddr)) == -1)
  {
    throw SocketException("Error while binding to socket", errno);
  }
}

void SocketLinux::sendImpl(const std::vector<uint8_t>& sendbuf)
{
  if (::sendto(sock_,
              static_cast<const void *>(sendbuf.data()),
              sendbuf.size(),
              0,
              reinterpret_cast<const sockaddr *>(&dst_addr_),
              static_cast<socklen_t>(sizeof(sockaddr_in))) == -1)
   {
     if (errno == 101)
     {
       throw NetworkUnreachableException(
             "Error while sending data - network unreachable", errno);
     }

     throw SocketException("Error while sending data", errno);
   }
}

void SocketLinux::enableBroadcastImpl()
{
  const int yes = 1;
  if (::setsockopt(sock_,
                  SOL_SOCKET,
                  SO_BROADCAST,
                  reinterpret_cast<const char *>(&yes),
                  sizeof(yes)) == -1)
  {
    throw SocketException("Error while setting socket options", errno);
  }
}

void SocketLinux::enableNonBlockingImpl()
{
  int flags = fcntl(sock_, F_GETFL, 0);
  if (flags < 0 || fcntl(sock_, F_SETFL, flags | O_RDWR | O_NONBLOCK) == -1)
  {
    throw SocketException("Error while setting socket non-blocking", errno);
  }
}

void SocketLinux::bindToDevice(const std::string &device)
{
  if (::setsockopt(sock_,
                   SOL_SOCKET,
                   SO_BINDTODEVICE,
                   device.c_str(),
                   static_cast<socklen_t>(device.size())) == -1)
  {
    if (errno == 1)
    {
      throw OperationNotPermitted();
    }

    throw SocketException("Error while binding to device \"" + device + "\"", errno);
  }
}

}
