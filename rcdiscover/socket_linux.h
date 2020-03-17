/*
 * rcdiscover - the network discovery tool for Roboception devices
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

#ifndef RCDISCOVER_SOCKET_LINUX_H
#define RCDISCOVER_SOCKET_LINUX_H

#include "socket.h"

#include <string>

#include <netinet/in.h>

namespace rcdiscover
{

/**
 * @brief Socket implementation for Linux.
 */
class SocketLinux : public Socket<SocketLinux>
{
  friend class Socket<SocketLinux>;

  public:
    /**
     * @brief Type representing the native socket handle type.
     */
    typedef int SocketType;

  public:
    /**
     * @brief Create a new socket.
     * @param dst_ip destination IP address
     * @param port destination port
     * @param iface_name name of the interface
     * @return the created socket
     */
    static SocketLinux create(in_addr_t dst_ip, uint16_t port, std::string iface_name);

    /**
     * @brief Creates sockets for all interfaces and binds them to the
     * respective interface.
     * @param port destination port
     * @return vector of sockets
     */
    static std::vector<SocketLinux> createAndBindForAllInterfaces(uint16_t port);

    /**
     * @brief Constructor.
     * @param domain domain of socket()
     * @param type type of socket()
     * @param protocol protocol of socket()
     * @param dst_ip destination IP address
     * @param port destination port
     * @param iface_name name of the interface
     */
    SocketLinux(int domain, int type, int protocol,
                in_addr_t dst_ip, uint16_t port,
                std::string iface_name);
    SocketLinux(SocketLinux &&other);
    SocketLinux &operator=(SocketLinux &&other);
    ~SocketLinux();

    /**
     * @brief Returns the sockaddr to which the socket is bound.
     * @return sockaddr to which the socket is bound.
     */
    const sockaddr_in& getDestSockAddr() const;

    /**
     * @brief Returns the broadcast address.
     * @return broadcast address
     */
    static const in_addr_t &getBroadcastAddr();

  protected:
    /**
     * @brief Returns the native socket handle.
     * @return native socket handle.
     */
    const int &getHandleImpl() const;

    /**
     * @brief Binds the socket to a specific sockaddr.
     * @param addr sockaddr_in to which to bind the socket
     */
    void bindImpl(const sockaddr_in &addr);

    /**
     * @brief Sends data.
     * @param sendbuf data buffer
     */
    void sendImpl(const std::vector<uint8_t> &sendbuf);

    /**
     * @brief Enables broadcast for this socket.
     */
    void enableBroadcastImpl();

    /**
     * @brief Enables non-blocking operation for this socket.
     */
    void enableNonBlockingImpl();

  private:
    /**
     * @brief Binds this socket to a specific device
     * (root privileges are required).
     * @param device device name to bind the socket to
     * @throws OperationNotPermitted in case this function is called without
     * root privileges
     */
    void bindToDevice(const std::string &device);

  private:
    int sock_;
    sockaddr_in dst_addr_;
};

}

#endif // RCDISCOVER_SOCKET_LINUX_H
