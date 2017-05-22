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
#include <string.h>
#include <errno.h>

namespace rcdiscover
{

namespace
{

#ifdef WIN32
inline void close(int sock)
{
    closesocket(sock);
}
#endif

inline std::string geterror(int errnum)
{
#ifdef WIN32
    char tmp[256];
    strerror_s(tmp, 256, errnum);
    return std::string(tmp);
#else
    char tmp[256];
    return std::string(strerror_r(errnum, tmp, 256));
#endif
}

/*
  Returns a vector with broadcast IPs of all interfaces for sending the request
  via directed broadcast to each interface explicitely.
*/

std::vector<uint32_t> getBroadcastIPs()
{
  std::vector<uint32_t> list;

#ifdef WIN32
  // get ip table (need to try several times due to race condition)
  MIB_IPADDRTABLE *iptab=0;

  {
    ULONG len=0;
    for (int i=0; i<5; i++)
    {
      DWORD ipRet=GetIpAddrTable(iptab, &len, false);
      if (ipRet == ERROR_INSUFFICIENT_BUFFER)
      {
        free(iptab);
        iptab=reinterpret_cast<MIB_IPADDRTABLE *>(malloc(len));
      }
      else if (ipRet == NO_ERROR)
      {
        break;
      }
      else
      {
        free(iptab);
        iptab=0;
        break;
      }
    }
  }

  // extract and store broadcast addresses

  if (iptab != 0)
  {
    for (DWORD i=0; i<iptab->dwNumEntries; i++)
    {
      const MIB_IPADDRROW &row = iptab->table[i];

      uint32_t baddr=row.dwAddr&row.dwMask;
      if (row.dwBCastAddr)
      {
        baddr|=~row.dwMask;
      }

      list.push_back(baddr);
    }

    free(iptab);
  }
#else
  struct ifaddrs *ifap=0;
  if (getifaddrs(&ifap) == 0)
  {
    struct ifaddrs *p=ifap;
    while (p != 0)
    {
      if (p->ifa_addr != 0)
      {
        struct sockaddr *b=p->ifa_broadaddr;

        if (b != 0 && b->sa_family == AF_INET)
        {
          list.push_back(reinterpret_cast<struct sockaddr_in *>(b)->sin_addr.s_addr);
        }
      }

      p=p->ifa_next;
    }

    freeifaddrs(ifap);
  }
#endif

  return list;
}

}

Discover::Discover()
{
  // create socket

  sock=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (sock == -1)
  {
    throw std::ios_base::failure(geterror(errno));
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
    throw std::ios_base::failure(geterror(errno));
  }

  // configure for broadcasting

  int yes=1;
  if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<char *>(&yes), sizeof(yes)) != 0)
  {
    close(sock);
    throw std::ios_base::failure(geterror(errno));
  }

  // configure socket as non blocking

#ifdef WIN32
  u_long imode=1;
  if (ioctlsocket(sock, FIONBIO, &imode) != 0)
#else
  if (fcntl(sock, F_SETFL, O_RDWR|O_NONBLOCK) == -1)
#endif
  {
    close(sock);
    throw std::ios_base::failure(geterror(errno));
  }
}

Discover::~Discover()
{
  close(sock);
}

void Discover::broadcastRequest()
{
  std::vector<uint32_t> list=getBroadcastIPs();

  // send discover cmd message as broadcast

  uint8_t discovery_cmd[8]={0x42, 0x11, 0, 0x02, 0, 0, 0, 1};

  struct sockaddr_in addr;

  for (size_t i = 0; i < list.size(); i++)
  {
    // uint32_t v=ntohl(list[i]);
	// std::cout << ((v>>24)&0xff) << "." << ((v>>16)&0xff) << "." << ((v>>8)&0xff) << "." << (v&0xff)
	//           << std::endl;

    memset(&addr, 0, sizeof(addr));
	addr.sin_addr.s_addr = list[i]; // directed broadcast to subnet
	addr.sin_port = htons(3956); // GigE Vision control port
	addr.sin_family = PF_INET;

	if (sendto(sock, reinterpret_cast<char *>(discovery_cmd), 8, 0, reinterpret_cast<struct sockaddr *>(&addr),
	           sizeof(addr)) != 8)
	{
	  close(sock);
	  throw std::ios_base::failure(geterror(errno));
	}
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
#ifdef WIN32
	  int naddr = sizeof(addr);
#else
	  socklen_t naddr = sizeof(addr);
#endif
	  memset(&addr, 0, naddr);

      int n=recvfrom(sock, reinterpret_cast<char *>(p), sizeof(p), 0, reinterpret_cast<struct sockaddr *>(&addr), &naddr);

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
