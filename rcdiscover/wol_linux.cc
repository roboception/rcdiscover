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

#include "wol_linux.h"

#include "socket_linux.h"

#include <ios>
#include <string>
#include <string.h>
#include <vector>
#include <iostream>
#include <sstream>

namespace rcdiscover
{

WOL_Linux::WOL_Linux(uint64_t hardware_addr, uint16_t port) :
  WOL<WOL_Linux>(hardware_addr, port)
{ }

WOL_Linux::WOL_Linux(std::array<uint8_t, 6> hardware_addr, uint16_t port) :
  WOL<WOL_Linux>(hardware_addr, port)
{ }

void WOL_Linux::sendUdpImpl(const std::array<uint8_t, 4> *password) const
{
  auto sockets = SocketLinux::createAndBindForAllInterfaces(getPort());

  for (auto &socket : sockets)
  {
    std::vector<uint8_t> sendbuf;
    appendMagicPacket(sendbuf, password);

    socket.enableBroadcast();
    socket.enableNonBlocking();

    socket.send(sendbuf);
  }
}

}
