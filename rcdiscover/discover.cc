/*
 * rcdiscover - the network discovery tool for rc_visard
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
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

#include "discover.h"

#include "socket_exception.h"
#include "gige_request_counter.h"

#include <exception>
#include <ios>
#include <iostream>

#ifdef WIN32
#include <winsock2.h>
#include <iphlpapi.h>
#else
#include <sys/socket.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <vector>
#include <future>
#include <string.h>
#include <errno.h>
#include <algorithm>

namespace rcdiscover
{

#ifdef WIN32
typedef SocketWindows SocketImpl;
#else
typedef SocketLinux SocketImpl;
#endif

Discover::Discover() :
  sockets_(SocketType::createAndBindForAllInterfaces(3956))
{
  for (auto &socket : sockets_)
  {
    socket.enableBroadcast();
    socket.enableNonBlocking();
  }
}

Discover::~Discover()
{ }

void Discover::broadcastRequest()
{
  req_nums_.clear();

  std::vector<uint8_t> discovery_cmd{0x42, 0x11, 0, 0x02, 0, 0, 0, 0};

  for (auto &socket : sockets_)
  {
    req_nums_.push_back(GigERequestCounter::getNext());
    std::tie(discovery_cmd[6], discovery_cmd[7]) = req_nums_.back();

    try
    {
      socket.send(discovery_cmd);
    }
    catch(const NetworkUnreachableException &)
    {
      continue;
    }
  }
}

bool Discover::getResponse(std::vector<DeviceInfo> &info,
                           int timeout_per_socket)
{
  // setup waiting for data to arrive

  struct timeval tv;
  tv.tv_sec=timeout_per_socket/1000;
  tv.tv_usec=(timeout_per_socket%1000)*1000;

  // try to get a valid package (repeat if an invalid package is received)

  const auto &req_nums = req_nums_;

  std::vector<std::future<DeviceInfo>> futures;
  for (auto &socket : sockets_)
  {
    futures.push_back(std::async(std::launch::async, [&socket, &tv, &req_nums]
    {
      DeviceInfo device_info(socket.getIfaceName());
      device_info.clear();

      int count = 10;

      auto sock = socket.getHandle<typename SocketType::SocketType>();

      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(sock, &fds);

      while (!device_info.isValid() && count > 0)
      {
        count--;

        if (select(sock+1, &fds, NULL, NULL, &tv) > 0)
        {
          // get package

          uint8_t p[600];

          struct sockaddr_in addr;
#ifdef WIN32
          int naddr = sizeof(addr);
#else
          socklen_t naddr = sizeof(addr);
#endif
          memset(&addr, 0, naddr);

          long n = recvfrom(sock,
                            reinterpret_cast<char *>(p), sizeof(p), 0,
                            reinterpret_cast<struct sockaddr *>(&addr), &naddr);

          // check if received package is a valid discovery acknowledge

          if (n >= 8)
          {
            if (p[0] == 0 && p[1] == 0 && p[2] == 0 &&
                p[3] == 0x03)
            {
              if (std::find(req_nums.begin(), req_nums.end(),
                            std::make_tuple(p[6], p[7])) != req_nums.end())
              {
                size_t len=(static_cast<size_t>(p[4])<<8)|p[5];

                if (static_cast<size_t>(n) >= len+8)
                {
                  // extract information and store in list

                  device_info.set(p+8, len);
                }
              }
            }
          }
        }
        else
        {
          count=0;
        }
      }

      return device_info;
    }));

  }

  bool ret = false;
  for (auto &f : futures)
  {
    info.push_back(f.get());
    ret |= info.back().isValid();
  }

  return ret;
}

}
