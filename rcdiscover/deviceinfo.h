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

#ifndef RCDISCOVER_DEVICEINFO
#define RCDISCOVER_DEVICEINFO

#include <string>
#include <stdint.h>

namespace rcdiscover
{

class DeviceInfo
{
  public:

    DeviceInfo();

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

    const uint64_t getMAC() const { return mac; }

    /**
      Returns the current IP address of the device.

      @return Pointer to 4 bytes with an IPv4 address.
    */

    const uint32_t getIP() const { return ip; }

    /**
      Returns the current subnet mask of the device.

      @return Pointer to 4 bytes with an IPv4 subnet mask.
    */

    const uint32_t getSubnetMask() const { return subnet; }

    /**
      Returns the current IP address of the devices gateway.

      @return Pointer to 4 bytes with an IPv4 address.
    */

    const uint32_t getGateway() const { return gateway; }

    /**
      Returns the manufacturer name.

      @return Manufacturer name.
    */

    std::string getManufacturerName() const { return manufacturer_name; }

    /**
      Returns the model name.

      @return Model name.
    */

    std::string getModelName() const { return model_name; }

    /**
      Returns the device version.

      @return Device version.
    */

    std::string getDeviceVersion() const { return device_version; }

    /**
      Returns manufacturer specific information.

      @return Manufacturer info.
    */

    std::string getManufacturerInfo() const { return manufacturer_info; }

    /**
      Returns the serial number.

      @return Serial number.
    */

    std::string getSerialNumber() const { return serial_number; }

    /**
      Returns the user name.

      @return User name.
    */

    std::string getUserName() const { return user_name; }

    /**
      Returns true if the MAC addresses conicide.
    */

    bool operator == (const DeviceInfo &info) const { return mac == info.mac; }

  private:

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