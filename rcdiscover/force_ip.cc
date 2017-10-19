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
#include "force_ip.h"

#include "socket_exception.h"
#include "gige_request_counter.h"

namespace rcdiscover
{

ForceIP::ForceIP() :
  sockets_(SocketType::createAndBindForAllInterfaces(3956))
{
  for (auto &socket : sockets_)
  {
    socket.enableBroadcast();
    socket.enableNonBlocking();
  }
}

void ForceIP::sendCommand(const uint64_t mac, const uint32_t ip,
                          const uint32_t subnet, const uint32_t gateway)
{
  std::vector<std::uint8_t> force_ip_command(64);
  force_ip_command[0] = 0x42;
  force_ip_command[1] = 0x00;   // flags
  force_ip_command[2] = 0x00;   // command: FORCEIP_CMD: 0x0004
  force_ip_command[3] = 0x04;   // command: FORCEIP_CMD: 0x0004
  force_ip_command[4] = 0x00;   // length
  force_ip_command[5] = 64 - 8; // length

  force_ip_command[10] = static_cast<std::uint8_t>(mac >> 5); // MAC address
  force_ip_command[11] = static_cast<std::uint8_t>(mac >> 4); // MAC address
  force_ip_command[12] = static_cast<std::uint8_t>(mac >> 3); // MAC address
  force_ip_command[13] = static_cast<std::uint8_t>(mac >> 2); // MAC address
  force_ip_command[14] = static_cast<std::uint8_t>(mac >> 1); // MAC address
  force_ip_command[15] = static_cast<std::uint8_t>(mac >> 0); // MAC address

  force_ip_command[28] = static_cast<std::uint8_t>(ip >> 3); // IP address
  force_ip_command[29] = static_cast<std::uint8_t>(ip >> 2); // IP address
  force_ip_command[30] = static_cast<std::uint8_t>(ip >> 1); // IP address
  force_ip_command[31] = static_cast<std::uint8_t>(ip >> 0); // IP address

  force_ip_command[44] = static_cast<std::uint8_t>(subnet >> 3); // subnet
  force_ip_command[45] = static_cast<std::uint8_t>(subnet >> 2); // subnet
  force_ip_command[46] = static_cast<std::uint8_t>(subnet >> 1); // subnet
  force_ip_command[47] = static_cast<std::uint8_t>(subnet >> 0); // subnet

  force_ip_command[60] = static_cast<std::uint8_t>(gateway >> 3); // gateway
  force_ip_command[61] = static_cast<std::uint8_t>(gateway >> 2); // gateway
  force_ip_command[62] = static_cast<std::uint8_t>(gateway >> 1); // gateway
  force_ip_command[63] = static_cast<std::uint8_t>(gateway >> 0); // gateway

  for (auto &socket : sockets_)
  {
    std::tie(force_ip_command[6], force_ip_command[7]) =
        GigERequestCounter::getNext();

    try
    {
      socket.send(force_ip_command);
    }
    catch(const NetworkUnreachableException &)
    {
      continue;
    }
  }
}

}
