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
#ifndef FORCE_IP_H
#define FORCE_IP_H

#ifdef WIN32
#include "socket_windows.h"
#else
#include "socket_linux.h"
#endif

namespace rcdiscover
{

/**
 * @brief Class for sending GigE Vision FORCEIP_CMD to camera.
 */
class ForceIP
{
  public:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:
    /**
     * @brief Constructor.
     * Sets up sockets.
     */
    ForceIP();

    /**
     * @brief Send FORCEIP_CMD.
     * @param mac the destination MAC address
     * @param ip the desired IP address
     * @param subnet the desired subnet mask
     * @param gateway the desired default gateway
     *
     * @note If \p ip is set to 0, the camera will perform a reconnect.
     */
    void sendCommand(std::uint64_t mac, std::uint32_t ip,
                     std::uint32_t subnet, std::uint32_t gateway);

  private:
    std::vector<SocketType> sockets_;
};

}

#endif // FORCE_IP_H
