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

#include "wol.h"

#ifdef WIN32
#include "socket_windows.h"
#else
#include "socket_linux.h"
#endif

namespace rcdiscover
{

WOL::WOL(uint64_t hardware_addr, uint16_t port) noexcept :
  hardware_addr_(toByteArray<6>(std::move(hardware_addr))),
  port_{port}
{ }

WOL::WOL(std::array<uint8_t, 6> hardware_addr, uint16_t port) noexcept :
  hardware_addr_(std::move(hardware_addr)),
  port_{port}
{ }

void WOL::send() const
{
  sendImpl(nullptr);
}

void WOL::send(const std::array<uint8_t, 4>& password) const
{
  sendImpl(&password);
}

std::vector<uint8_t>& WOL::appendMagicPacket(
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

template<uint8_t num>
std::array<uint8_t, num> WOL::toByteArray(uint64_t data) noexcept
{
  std::array<uint8_t, num> result;
  for (uint8_t i = 0; i < num; ++i)
  {
    result[i] = static_cast<uint8_t>((data >> (i*8)) & 0xFF);
  }
  return result;
}

void WOL::sendImpl(const std::array<uint8_t, 4> *password) const
{
  auto sockets = SocketType::createAndBindForAllInterfaces(port_);

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
