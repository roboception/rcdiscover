/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 */

#include "discover.h"

#include <exception>
#include <ios>
#include <iostream>

#ifdef WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <string.h>
#include <errno.h>

namespace rcdiscover
{

Discover::Discover()
{
  // create socket

  sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock == -1)
  {
    throw std::ios_base::failure(std::string(strerror(errno)));
  }

  // bind an arbitrary port to it

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));

  addr.sin_addr.s_addr=htonl(INADDR_ANY);
  addr.sin_port=htons(0);
  addr.sin_family=AF_INET;

  if (bind(sock, reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr)) != 0)
  {
    close(sock);
    throw std::ios_base::failure(std::string(strerror(errno)));
  }

  // configure for broadcasting

  int yes=1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &yes, sizeof(yes)) != 0)
  {
    close(sock);
    throw std::ios_base::failure(std::string(strerror(errno)));
  }

  // configure socket as non blocking

  if (fcntl(sock, F_SETFL, O_RDWR|O_NONBLOCK) == -1)
  {
    close(sock);
    throw std::ios_base::failure(std::string(strerror(errno)));
  }
}

Discover::~Discover()
{
  close(sock);
}

void Discover::broadcastRequest()
{
  // send discover cmd message as broadcast

  uint8_t discovery_cmd[8]={0x42, 0x11, 0, 0x02, 0, 0, 0, 1};

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_addr.s_addr=htonl(-1); // broadcast to local network
  addr.sin_port=htons(3956); // GigE Vision control port
  addr.sin_family=PF_INET;

  if (sendto(sock, discovery_cmd, 8, 0, reinterpret_cast<struct sockaddr *>(&addr),
             sizeof(addr)) != 8)
  {
    close(sock);
    throw std::ios_base::failure(std::string(strerror(errno)));
  }
}

bool Discover::getResponse(DeviceInfo &info, int timeout)
{
  // setup waiting for data to arrive

  fd_set fds;
  FD_ZERO(&fds);
  FD_SET(sock, &fds);

  struct timeval tv;
  tv.tv_sec=timeout/1000;
  tv.tv_usec=(timeout%1000)*1000;

  int count=10;
  info.clear();

  // try to get a valid package (repeat if an invalid package is received)

  while (!info.isValid() && count > 0)
  {
    count--;

    if (select(sock+1, &fds, NULL, NULL, &tv) > 0)
    {
      // get package

      uint8_t p[600];

      struct sockaddr_in addr;
      socklen_t naddr=sizeof(addr);
      memset(&addr, 0, naddr);

      int n=recvfrom(sock, p, sizeof(p), 0, reinterpret_cast<struct sockaddr *>(&addr), &naddr);

      // check if received package is a valid discovery acknowledge

      if (n >= 8)
      {
        if (p[0] == 0 && p[1] == 0 && p[2] == 0 && p[3] == 0x03 && p[6] == 0 && p[7] == 1)
        {
          size_t len=(static_cast<size_t>(p[4])<<8)|p[5];

          if (static_cast<size_t>(n) >= len+8)
          {
            // extract information and store in list

            info.set(p+8, len);
          }
        }
      }
    }
    else
    {
      count=0;
    }
  }

  return info.isValid();
}

}