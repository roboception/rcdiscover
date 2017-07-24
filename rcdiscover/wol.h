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

/**
 * @brief Class for Magic Packet (Wake-on-Lan (WOL)) reset of rc_visard.
 */
class WOL
{
  private:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:
    /**
     * @brief Constructor.
     * @param hardware_addr MAC-address of rc_visard
     * @param port destination UDP port
     */
    WOL(uint64_t hardware_addr, uint16_t port) noexcept;

    /**
     * @brief Constructor.
     * @param hardware_addr MAC-address of rc_visard
     * @param port destination UDP port
     */
    WOL(std::array<uint8_t, 6> hardware_addr, uint16_t port) noexcept;
    ~WOL() = default;

  public:
    /**
     * @brief Send Magic Packet without any data ("password").
     */
    void send() const;

    /**
     * @brief Send Magic Packet with data ("password").
     * @param password data to send
     */
    void send(const std::array<uint8_t, 4>& password) const;

  private:
    /**
     * @brief Appends a magic packet to a data buffer.
     * @param sendbuf buffer to which the magic packet is appended (is modified)
     * @param password Magic Packet password (null if no such password should be
     * included
     * @return reference to input sendbuf
     */
    std::vector<uint8_t>& appendMagicPacket(
        std::vector<uint8_t>& sendbuf,
        const std::array<uint8_t, 4> *password) const;

    /**
     * @brief Converts a larger-than-byte data type to an array of bytes.
     */
    template<uint8_t num>
    std::array<uint8_t, num> toByteArray(uint64_t data) noexcept;

    /**
     * @brief sendImpl Actually send Magic packet with specified data.
     * @param password data to send (null if non)
     */
    void sendImpl(const std::array<uint8_t, 4> *password) const;

  private:
    const std::array<uint8_t, 6> hardware_addr_;
    uint16_t port_;
};

}

#endif // WOL_H
