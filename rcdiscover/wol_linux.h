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
        static Socket socketUDP()
        {
          return Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        }

        static Socket socketRaw()
        {
          return Socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
        }

      public:
        Socket(int domain, int type, int protocol) :
          sock_(-1)
        {
          sock_ = socket(domain, type, protocol);
          if (sock_ == -1)
          {
            if (errno == EPERM)
            {
              throw OperationNotPermitted();
            }

            throw WOLException("Error while creating socket", errno);
          }
        }

        Socket(Socket&& other) :
          sock_(-1)
        {
          std::swap(sock_, other.sock_);
        }

        Socket& operator=(Socket&& other)
        {
          std::swap(sock_, other.sock_);
          return *this;
        }

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        ~Socket()
        {
          if (sock_ != -1)
          {
            close(sock_);
          }
        }

        const int &getHandle() const
        {
          return sock_;
        }

        void bind(const sockaddr_in& addr)
        {
          if (::bind(sock_, reinterpret_cast<const sockaddr *>(&addr), sizeof(sockaddr)) == -1)
          {
            throw WOLException("Error while binding to socket", errno);
          }
        }

        template<typename SockAddrT>
        void sendto(const std::vector<uint8_t>& sendbuf,
                    const SockAddrT& addr)
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

        void enableBroadcast()
        {
          const int yes = 1;
          if (setsockopt(sock_,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<const char *>(&yes),
                        sizeof(yes)) == -1)
          {
            throw WOLException("Error while setting socket options", errno);
          }
        }

      private:
        int sock_;
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
      auto sock = Socket::socketRaw();

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

        sock.sendto(sendbuf, socketaddress);
      }

      freeifaddrs(addrs);
      addrs = nullptr;
    }

    std::vector<uint32_t> getBroadcastIPs() const
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
};

}

#endif // WOL_LINUX_H
