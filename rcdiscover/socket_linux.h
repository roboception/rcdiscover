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

#ifndef SOCKET_LINUX_H
#define SOCKET_LINUX_H

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
     * @return the created socket
     */
    static SocketLinux create(in_addr_t dst_ip, uint16_t port);

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
     */
    SocketLinux(int domain, int type, int protocol,
                in_addr_t dst_ip, uint16_t port);
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
    const static in_addr_t broadcast_addr_;
    int sock_;
    sockaddr_in dst_addr_;
};

}

#endif // SOCKET_LINUX_H
