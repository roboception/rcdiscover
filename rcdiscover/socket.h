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
 
#ifndef RCDISCOVER_SOCKET_H
#define RCDISCOVER_SOCKET_H

#include <vector>
#include <cstdint>
#include <string>

struct sockaddr_in;

namespace rcdiscover
{

/**
 * CRTP class for platform specific socket implementation.
 */
template<typename Derived>
class Socket
{
  private:
    Derived &getDerived()
    {
      return *static_cast<Derived *>(this);
    }

    const Derived &getDerived() const
    {
      return *static_cast<const Derived *>(this);
    }

  public:
    explicit Socket(std::string iface_name) : iface_name_(std::move(iface_name))
    { }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

    Socket(Socket &&) = default;
    Socket& operator=(Socket &&) = default;

    /**
     * @brief Returns the native socket handle.
     * @return native socket handle
     */
    template<typename T>
    const T &getHandle() const
    {
      return getDerived().getHandleImpl();
    }

    /**
     * @brief Binds the socket to an interface.
     * @param addr sockaddr_in specifying the interface
     */
    void bind(const sockaddr_in& addr)
    {
      getDerived().bindImpl(addr);
    }

    /**
     * @brief Sends data.
     * @param sendbuf data to send
     */
    void send(const std::vector<uint8_t>& sendbuf)
    {
      getDerived().sendImpl(sendbuf);
    }

    /**
     * @brief Enables broadcast for this socket.
     */
    void enableBroadcast()
    {
      getDerived().enableBroadcastImpl();
    }

    /**
     * @brief Enables non-blocking operation for this socket.
     */
    void enableNonBlocking()
    {
      getDerived().enableNonBlockingImpl();
    }

    /**
     * @brief Returns the interface name.
     */
    const std::string &getIfaceName() const
    {
      return iface_name_;
    }

  private:
    std::string iface_name_;
};

}

#endif //RCDISCOVER_SOCKET_H
