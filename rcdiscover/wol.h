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

#ifndef WOL_H
#define WOL_H

#include <array>
#include <vector>
#include <memory>
#include <cstdint>

#ifdef WIN32
#include <winsock2.h>
#else
#include <netinet/in.h>
#endif

namespace rcdiscover
{
template<typename Derived>
class WOL
{
  protected:
    explicit WOL(uint64_t hardware_addr) noexcept :
      hardware_addr_(toByteArray<6>(std::move(hardware_addr))),
      udp_{false},
      ip_{nullptr},
      port_{}
    { }

    explicit WOL(std::array<uint8_t, 6> hardware_addr) noexcept :
      hardware_addr_(std::move(hardware_addr)),
      udp_{false},
      ip_{},
      port_{}
    { }

    ~WOL() = default;

  public:
    WOL& enableUDP(uint16_t port)
    {
      ip_.reset(nullptr);
      port_ = std::move(port);
      udp_ = true;
      return *this;
    }

    WOL& enableUDP(uint32_t ip, uint16_t port)
    {
      ip_ = std::unique_ptr<std::array<uint8_t,4>>(
                new std::array<uint8_t,4>(toByteArray(std::move(ip))));
      port_ = std::move(port);
      udp_ = true;
      return *this;
    }

    WOL& enableUDP(std::array<uint8_t, 4> ip, uint16_t port)
    {
      ip_ = std::unique_ptr<std::array<uint8_t,4>>(
                new std::array<uint8_t,4>(std::move(ip)));
      port_ = std::move(port);
      udp_ = true;
      return *this;
    }

    void send() const
    {
      if (!udp_)
      {
        getDerived().send_raw(nullptr);
      }
      else
      {
        send_udp(nullptr);
      }
    }

    void send(const std::array<uint8_t, 4>& password) const
    {
      if (!udp_)
      {
        getDerived().send_raw(&password);
      }
      else
      {
        send_udp(&password);
      }
    }

  protected:
    const std::array<uint8_t, 6>& getHardwareAddr() const noexcept
    {
      return hardware_addr_;
    }

    const std::array<uint8_t, 4>* getIP() const noexcept
    {
      return ip_.get();
    }

    uint16_t getPort() const noexcept
    {
      return port_;
    }

    std::vector<uint8_t>& appendEthernetFrame(
        std::vector<uint8_t>& sendbuf,
        const std::array<uint8_t, 6>& src_hw_addr) const
    {
      for (size_t i = 0; i < hardware_addr_.size(); ++i)
      {
        sendbuf.push_back(hardware_addr_[i]);
      }
      for (size_t i = 0; i < src_hw_addr.size(); ++i)
      {
        sendbuf.push_back(src_hw_addr[i]);
      }
      sendbuf.push_back(0x08);
      sendbuf.push_back(0x42);

      return sendbuf;
    }

    std::vector<uint8_t>& appendMagicPacket(
        std::vector<uint8_t>& sendbuf,
        const std::array<uint8_t, 4> *password) const
    {
      for (int i = 0; i < 6; ++i)
      {
        sendbuf.push_back(0xFF);
      }
      for (int i = 0; i < 16; ++i)
      {
        for (size_t j = 0; j < hardware_addr_.size(); ++j)
        {
          sendbuf.push_back(hardware_addr_[j]);
        }
      }
      if (password != nullptr)
      {
        for (int i = 0; i < 4; ++i)
        {
          sendbuf.push_back((*password)[i]);
        }
      }

      return sendbuf;
    }

  private:
    Derived& getDerived() noexcept
    {
      return static_cast<Derived &>(*this);
    }

    const Derived& getDerived() const noexcept
    {
      return static_cast<const Derived &>(*this);
    }

    template<uint8_t num>
    std::array<uint8_t, num> toByteArray(uint64_t data) noexcept
    {
      std::array<uint8_t, num> result;
      for (uint8_t i = 0; i < num; ++i)
      {
        result[i] = static_cast<uint8_t>((data >> (i*8)) & 0xFF);
      }
      return result;
    }

    void send_udp(const std::array<uint8_t, 4> *password) const
    {
      auto sock = Derived::Socket::socketUDP();

      sockaddr_in addr{};
      addr.sin_family = AF_INET;
      addr.sin_addr.s_addr = htonl(INADDR_ANY);
      addr.sin_port = htons(0);

      sock.bind(addr);

      const bool broadcast = !ip_;

      std::vector<uint32_t> ips;
      if (broadcast)
      {
        ips = getDerived().getBroadcastIPs();
        sock.enableBroadcast();
      }
      else
      {
        ips.push_back(*reinterpret_cast<const uint32_t *>(&(*ip_)[0]));
      }

      for (const auto ip : ips)
      {
        addr.sin_addr.s_addr = ip;
        addr.sin_port = htons(getPort());

        std::vector<uint8_t> sendbuf;
        appendMagicPacket(sendbuf, password);

        sock.sendto(sendbuf, addr);
      }
    }

  private:
    const std::array<uint8_t, 6> hardware_addr_;

    bool udp_;
    std::unique_ptr<std::array<uint8_t, 4>> ip_;
    uint16_t port_;
};
}

#endif // WOL_H
