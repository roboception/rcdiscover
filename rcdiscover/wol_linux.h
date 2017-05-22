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

#ifndef WOL_LINUX_H
#define WOL_LINUX_H

#include "wol.h"

#include "wol_exception.h"
#include "operation_not_permitted.h"

#include <ios>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include <netinet/ether.h>
#include <ifaddrs.h>

namespace rcdiscover
{

class WOL_Linux : public WOL<WOL_Linux>
{
  friend class WOL<WOL_Linux>;

  private:

    // RAII wrapper for socket
    class Socket
    {
      public:
        Socket(int domain, int type, int protocol)
        {
          sock = socket(domain, type, protocol);
          if (sock == -1)
          {
            if (errno == EPERM)
            {
              throw OperationNotPermitted();
            }

            throw WOLException("Error while creating socket", errno);
          }
        }

        ~Socket()
        {
          close(sock);
        }

        const int &getHandle() const
        {
          return sock;
        }

      private:
        int sock;
    };

  public:
    explicit WOL_Linux(uint64_t hardware_addr) :
      WOL(hardware_addr)
    { }

    explicit WOL_Linux(std::array<uint8_t, 6> hardware_addr) :
      WOL(std::move(hardware_addr))
    { }

  protected:
    void send_raw(const std::array<uint8_t, 4> *password) const
    {
      Socket sock(AF_PACKET, SOCK_RAW, IPPROTO_RAW);

      ifaddrs *addrs;
      getifaddrs(&addrs);

      for(ifaddrs *addr = addrs;
          addr != nullptr;
          addr = addr->ifa_next)
      {
        if (addr->ifa_name == nullptr ||
            addr->ifa_addr == nullptr ||
            addr->ifa_addr->sa_family != AF_PACKET)
        {
          continue;
        }

        std::cout << "Interface " << addr->ifa_name << std::endl;
        ifreq if_idx;
        memset(&if_idx, 0, sizeof(ifreq));
        strncpy(if_idx.ifr_name, addr->ifa_name, IFNAMSIZ-1);
        if (ioctl(sock.getHandle(), SIOCGIFINDEX, &if_idx) < 0)
        {
          std::ostringstream oss;
          oss << "Error while determining interface index of interface" << addr->ifa_name;
          throw WOLException(oss.str(), errno);
        }

        ifreq if_mac;
        memset(&if_mac, 0, sizeof(ifreq));
        strncpy(if_mac.ifr_name, addr->ifa_name, IFNAMSIZ-1);
        if (ioctl(sock.getHandle(), SIOCGIFHWADDR, &if_mac) < 0)
        {
          throw WOLException("Error while determining interface MAC", errno);
        }
        std::array<uint8_t, 6> src_hw_addr;
        std::copy(
            reinterpret_cast<uint8_t *>(&if_mac.ifr_ifru.ifru_hwaddr.sa_data[0]),
            reinterpret_cast<uint8_t *>(&if_mac.ifr_ifru.ifru_hwaddr.sa_data[6]),
            std::begin(src_hw_addr));

        sockaddr_ll socketaddress;
        socketaddress.sll_family = AF_PACKET;
        socketaddress.sll_protocol = htons(IPPROTO_RAW);
        socketaddress.sll_ifindex = if_idx.ifr_ifru.ifru_ivalue;
        socketaddress.sll_pkttype  = PACKET_OTHERHOST;
        socketaddress.sll_halen = ETH_ALEN;
        socketaddress.sll_addr[0] = getHardwareAddr()[0];
        socketaddress.sll_addr[1] = getHardwareAddr()[1];
        socketaddress.sll_addr[2] = getHardwareAddr()[2];
        socketaddress.sll_addr[3] = getHardwareAddr()[3];
        socketaddress.sll_addr[4] = getHardwareAddr()[4];
        socketaddress.sll_addr[5] = getHardwareAddr()[5];
        socketaddress.sll_addr[6] = 0;
        socketaddress.sll_addr[7] = 0;

        std::vector<uint8_t> sendbuf;

        this->appendEthernetFrame(sendbuf,
                                  src_hw_addr);

        this->appendMagicPacket(sendbuf, password);

        if (sendto(sock.getHandle(),
               static_cast<void *>(sendbuf.data()), sendbuf.size(),
               0,
               reinterpret_cast<sockaddr *>(&socketaddress), sizeof(socketaddress)) == -1)
        {
          throw WOLException("Error while sending UDP packet", errno);
        }
      }

      freeifaddrs(addrs);
    }

    void send_udp(const std::array<uint8_t, 4> *password) const
    {
      Socket sock(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

      sockaddr_in addr{};
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      addr.sin_port = htons(0);

      if (bind(sock.getHandle(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
      {
        throw WOLException("Error while binding to socket", errno);
      }

      const bool broadcast = !getIP();

      std::vector<uint32_t> ips;
      if (broadcast)
      {
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

        const int yes=1;
        if (setsockopt(sock.getHandle(), SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char *>(&yes), sizeof(yes)) != 0)
        {
          throw WOLException("Error while setting socket options", errno);
        }
      }
      else
      {
        ips.push_back(*reinterpret_cast<const uint32_t *>(&getIP()[0]));
      }

      for (const auto ip : ips)
      {
        addr.sin_addr.s_addr = ip;
        addr.sin_port = htons(getPort());

        std::vector<uint8_t> sendbuf;
        appendMagicPacket(sendbuf, password);

        if (sendto(sock.getHandle(),
                   static_cast<void *>(sendbuf.data()),
                   sendbuf.size(),
                   0,
                   reinterpret_cast<sockaddr *>(&addr),
                   (socklen_t)sizeof(sockaddr_in)) != static_cast<int>(sendbuf.size()))
        {
          throw WOLException("Error while sending broadcast UDP packet", errno);
        }
      }
    }
};

}

#endif // WOL_LINUX_H
