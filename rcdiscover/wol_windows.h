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

#include <winsock2.h>
#include <iphlpapi.h>

namespace rcdiscover
{

class WOL_Windows : public WOL<WOL_Windows>
{
  friend class WOL<WOL_Windows>;

  protected:
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
          return Socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
        }

      public:
        Socket(int domain, int type, int protocol) :
          sock_(INVALID_SOCKET)
        {
          sock_ = socket(domain, type, protocol);
          if (sock_ == INVALID_SOCKET)
          {
            throw WOLException("Error while creating socket", ::WSAGetLastError());
          }
        }

        Socket(Socket&& other) :
          sock_(INVALID_SOCKET)
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
          if(sock_ != INVALID_SOCKET)
          {
            ::closesocket(sock_);
          }
        }

        const SOCKET &getHandle() const
        {
          return sock_;
        }

        void bind(const sockaddr_in& sockaddr)
        {
          if (::bind(sock_,
                      reinterpret_cast<const struct sockaddr *>(&sockaddr),
                      sizeof(sockaddr)) == SOCKET_ERROR)
          {
            throw WOLException("Error while binding to socket", ::WSAGetLastError());
          }
        }

        template<typename SockAddrT>
        void sendto(const std::vector<uint8_t>& sendbuf,
                    const SockAddrT& sockaddr)
        {
          if (::sendto(sock_,
                     reinterpret_cast<const char *>(sendbuf.data()),
                     sendbuf.size(),
                     0,
                     reinterpret_cast<const struct sockaddr *>(&sockaddr),
                     sizeof(SockAddrT)) == SOCKET_ERROR)
           {
             throw WOLException("Error while sending data", ::WSAGetLastError());
           }
        }

        void enableBroadcast()
        {
          const int yes = 1;
          if (::setsockopt(sock_,
                        SOL_SOCKET,
                        SO_BROADCAST,
                        reinterpret_cast<const char *>(&yes),
                        sizeof(yes)) != SOCKET_ERROR)
          {
            throw WOLException("Error while setting socket options", ::WSAGetLastError());
          }
        }

      private:
        SOCKET sock_;
    };

  public:
    explicit WOL_Windows(uint64_t hardware_addr) :
      WOL(hardware_addr)
    { }

    explicit WOL_Windows(std::array<uint8_t, 6> hardware_addr) :
      WOL(std::move(hardware_addr))
    { }

  protected:
    void send_raw(const std::array<uint8_t, 4> *password) const
    {

    }

    std::vector<uint32_t> getBroadcastIPs() const
    {
      std::vector<uint32_t> ips;

      MIB_IPADDRTABLE *iptab = nullptr;

      for (int i = 0; i < 5; ++i)
      {
        ULONG len = 0;
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
};

}

#endif // WOL_LINUX_H
