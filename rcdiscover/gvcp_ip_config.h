/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2026 Roboception GmbH
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
#ifndef RCDISCOVER_GVCP_IP_CONFIG_H
#define RCDISCOVER_GVCP_IP_CONFIG_H

#ifdef WIN32
#include "socket_windows.h"
#include <string>
#else
#include "socket_linux.h"
#endif

#include <cstdint>
#include <stdexcept>
#include <vector>

namespace rcdiscover
{

/**
 * @brief Thrown when a device rejects a GVCP request with
 * GEV_STATUS_ACCESS_DENIED, i.e. another client currently holds control
 * access of the device.
 */
class GvcpAccessDeniedException : public std::runtime_error
{
  public:
    explicit GvcpAccessDeniedException(const std::string &msg) :
      std::runtime_error(msg)
    { }
};

/**
 * @brief Persistent GigE Vision IP configuration of a device.
 *
 * Link-local addressing (LLA) is always enabled on GigE Vision devices as
 * the fallback if neither DHCP nor a persistent IP configuration succeeds,
 * so it is not represented here.
 */
struct IPConfig
{
  bool persistent_ip_enabled;
  bool dhcp_enabled;

  std::uint32_t persistent_ip;
  std::uint32_t persistent_subnet;
  std::uint32_t persistent_gateway;
};

/**
 * @brief Reads and writes the persistent IP configuration of a device via
 * GVCP READREG/WRITEREG requests (GigE Vision bootstrap registers).
 *
 * Unlike discovery and FORCEIP_CMD, which are broadcast and filtered by the
 * device's MAC address, READREG/WRITEREG have no MAC address field and are
 * therefore sent as unicast requests to the device's current, reachable IP
 * address.
 */
class GvcpIPConfig
{
  public:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:
    /**
     * @brief Constructor. Sets up a socket for unicast communication with
     * the device.
     * @param ip current, reachable IP address of the device (host byte
     * order, as returned by DeviceInfo::getIP() or InputIP::getIP())
     */
    explicit GvcpIPConfig(std::uint32_t ip);

    /**
     * @brief Reads the current IP configuration mode and persistent
     * IP/subnet/gateway registers from the device.
     * @throws std::runtime_error if the device does not respond or
     * responds with an error status.
     */
    IPConfig readConfig();

    /**
     * @brief Writes a new IP configuration to the device.
     *
     * Claims GevCCP control access, optionally writes the persistent
     * IP/subnet/gateway registers (only if config.persistent_ip_enabled is
     * set), writes the GevCurrentIPConfiguration mode bits (preserving any
     * other bits in that register and always keeping link-local
     * addressing enabled), and releases control access again.
     *
     * @throws std::runtime_error if the device does not respond or
     * responds with an error status.
     */
    void writeConfig(const IPConfig &config);

  private:
    std::vector<std::uint8_t> sendAndReceive(std::uint16_t cmd,
        std::uint16_t expected_ack_cmd,
        std::uint32_t address,
        const std::vector<std::uint8_t> &payload);

    std::uint32_t readRegister(std::uint32_t address);
    void writeRegister(std::uint32_t address, std::uint32_t value);

    SocketType socket_;
};

}

#endif // RCDISCOVER_GVCP_IP_CONFIG_H
