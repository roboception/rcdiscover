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

#include "socket_linux.h"

#include "socket_exception.h"
#include "operation_not_permitted.h"
#include "utils.h"

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
#include <algorithm>

namespace rcdiscover
{

const in_addr_t &SocketLinux::getBroadcastAddr()
{
  static const in_addr_t bcast = htonl(INADDR_BROADCAST);
  return bcast;
}

const sockaddr_in& SocketLinux::getDestSockAddr() const
{
  return dst_addr_;
}

SocketLinux SocketLinux::create(const in_addr_t dst_ip, const uint16_t port,
                                std::string iface_name)
{
  return SocketLinux(AF_INET, SOCK_DGRAM, IPPROTO_UDP, dst_ip, port,
                     std::move(iface_name));
}

std::vector<SocketLinux> SocketLinux::createAndBindForAllInterfaces(
    const uint16_t port)
{
  std::vector<SocketLinux> sockets;

  ifaddrs *addrs;
  getifaddrs(&addrs);

  int i = 0;

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
        const in_addr_t s_addr =
            reinterpret_cast<struct sockaddr_in *>(addr->ifa_addr)->
            sin_addr.s_addr;

        uint16_t local_port = 0;

        {
          // limited broadcast
          sockets.emplace_back(SocketLinux::create(getBroadcastAddr(), port, name));

          sockaddr_in addr;
          addr.sin_family = AF_INET;
          addr.sin_port = 0;
          addr.sin_addr.s_addr = s_addr;
          sockets.back().bind(addr);
        }

        {
          // get port to which the limited broadcast socket is bound to
          struct sockaddr_in local_address;
          socklen_t address_length = sizeof(local_address);
          getsockname(sockets.back().sock_,
                      reinterpret_cast<sockaddr *>(&local_address),
                      &address_length);
          local_port = local_address.sin_port;
        }

        {
          // limited broadcast receiver
          sockets.emplace_back(SocketLinux::create(htonl(INADDR_ANY), port, name));

          sockaddr_in addr;
          addr.sin_family = AF_INET;
          addr.sin_port = local_port;
          addr.sin_addr.s_addr = htonl(INADDR_ANY);
          sockets.back().bind(addr);
        }

        {
          // directed broadcast
          sockets.emplace_back(
                SocketLinux::create(
                  reinterpret_cast<struct sockaddr_in *>(baddr)->
                  sin_addr.s_addr, port, name));
          sockaddr_in addr;
          addr.sin_family = AF_INET;
          addr.sin_port = local_port;
          addr.sin_addr.s_addr = htonl(INADDR_ANY);
          sockets.back().bind(addr);
        }
      }
    }

    ++i;
  }

  freeifaddrs(addrs);
  addrs = nullptr;

  return sockets;
}

SocketLinux::SocketLinux(int domain, int type, int protocol,
                         in_addr_t dst_ip, uint16_t port,
                         std::string iface_name) :
  Socket(std::move(iface_name)),
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

  const int yes = 1;
  if (::setsockopt(sock_,
                  SOL_SOCKET,
                  SO_REUSEPORT,
                  reinterpret_cast<const char *>(&yes),
                  sizeof(yes)) == -1)
  {
    throw SocketException("Error while setting socket options", errno);
  }
}

SocketLinux::SocketLinux(SocketLinux &&other) :
  Socket(std::move(other)),
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

    throw SocketException("Error while binding to device \"" + device + "\"",
                          errno);
  }
}

}
