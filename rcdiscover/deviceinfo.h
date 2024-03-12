/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
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

#ifndef RCDISCOVER_DEVICEINFO
#define RCDISCOVER_DEVICEINFO

#include <string>
#include <tuple>
#include <cstdint>

namespace rcdiscover
{

class DeviceInfo
{
  public:

    explicit DeviceInfo(std::string iface_name);

    /**
      Extracts the RAW GigE Vision information according to the given
      DISCOVERY_ACK package.

      @param raw Pointer to raw message body, excluding header.
      @param len Length of body as specified in the header.
    */

    void set(const uint8_t *raw, size_t len);

    /**
      Clears all information.
    */

    void clear();

    /**
      Checks if the object contains useful information.
    */

    bool isValid() const { return mac != 0; }

    /**
      Return major version of device.

      @return Major version.
    */

    int getMajorVersion() const { return major; }

    /**
      Return minor version of device.

      @return Minor version.
    */

    int getMinorVersion() const { return minor; }

    /**
      Returns 6 bytes with the MAC address of the device.

      @return Pointer to 6 bytes with the MAC address.
    */

    uint64_t getMAC() const { return mac; }

    /**
      Returns the current IP address of the device.

      @return 4 bytes with an IPv4 address.
    */

    uint32_t getIP() const { return ip; }

    /**
      Returns the current subnet mask of the device.

      @return 4 bytes with an IPv4 subnet mask.
    */

    uint32_t getSubnetMask() const { return subnet; }

    /**
      Returns the current IP address of the devices gateway.

      @return 4 bytes with an IPv4 address.
    */

    uint32_t getGateway() const { return gateway; }

    /**
      Returns the manufacturer name.

      @return Manufacturer name.
    */

    const std::string &getManufacturerName() const { return manufacturer_name; }

    /**
      Returns the model name.

      @return Model name.
    */

    const std::string &getModelName() const { return model_name; }

    /**
      Returns the device version.

      @return Device version.
    */

    const std::string &getDeviceVersion() const { return device_version; }

    /**
      Returns manufacturer specific information.

      @return Manufacturer info.
    */

    const std::string &getManufacturerInfo() const { return manufacturer_info; }

    /**
      Returns the serial number.

      @return Serial number.
    */

    const std::string &getSerialNumber() const { return serial_number; }

    /**
      Returns the user name.

      @return User name.
    */

    const std::string &getUserName() const { return user_name; }

    /**
     * First compares the MAC address, then the interface name.
     */

    bool operator < (const DeviceInfo &info) const
    { return std::tie(mac, iface_name) < std::tie(info.mac, info.iface_name); }

    /**
     * Returns the name of the interface on which this device was found.
     */
    const std::string &getIfaceName() const { return iface_name; }

  private:

    std::string iface_name;

    int major;
    int minor;

    uint64_t mac;
    uint32_t ip;
    uint32_t subnet;
    uint32_t gateway;

    std::string manufacturer_name;
    std::string model_name;
    std::string device_version;
    std::string manufacturer_info;
    std::string serial_number;
    std::string user_name;
};

}

#endif
