/*
 * rcdiscover - the network discovery tool for rc_visard
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

#ifndef RCDISCOVER_WOL_H
#define RCDISCOVER_WOL_H

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

#endif // RCDISCOVER_WOL_H