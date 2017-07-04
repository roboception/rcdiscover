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

#ifdef WIN32
class SocketWindows;
#else
class SocketLinux;
#endif

class WOL
{
  private:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:
    WOL(uint64_t hardware_addr, uint16_t port) noexcept;
    WOL(std::array<uint8_t, 6> hardware_addr, uint16_t port) noexcept;
    ~WOL() = default;

  public:
    void send() const;
    void send(const std::array<uint8_t, 4>& password) const;

  private:
    std::vector<uint8_t>& appendMagicPacket(
        std::vector<uint8_t>& sendbuf,
        const std::array<uint8_t, 4> *password) const;

    template<uint8_t num>
    std::array<uint8_t, num> toByteArray(uint64_t data) noexcept;

    void sendImpl(const std::array<uint8_t, 4> *password) const;

  private:
    const std::array<uint8_t, 6> hardware_addr_;
    uint16_t port_;
};

}

#endif // WOL_H
