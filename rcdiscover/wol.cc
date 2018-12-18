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

#include "wol.h"

#ifdef WIN32
#include "socket_windows.h"
#else
#include "socket_linux.h"
#endif

#include "socket_exception.h"

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
    result[i] = static_cast<uint8_t>((data >> (((num - 1 - i)*8))) & 0xFF);
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

    try
    {
      socket.send(sendbuf);
    }
    catch(const NetworkUnreachableException &)
    {
      continue;
    }
  }
}

}