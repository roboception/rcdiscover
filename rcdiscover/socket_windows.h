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

#include "socket.h"

#include <winsock2.h>

namespace rcdiscover
{

class SocketWindows : public Socket<SocketWindows>
{
  friend class Socket<SocketWindows>;

  public:
    /**
     * @brief Type representing the native socket handle type.
     */
    typedef SOCKET SocketType;

  public:
    /**
     * @brief Create a new socket.
     * @param dst_ip destination IP address
     * @param port destination port
     * @return the created socket
     */
    static SocketWindows create(ULONG dst_ip, uint16_t port);

    /**
     * @brief Creates sockets for all interfaces and binds them to the
     * respective interface.
     * @param port destination port
     * @return vector of sockets
     */
    static std::vector<SocketWindows> createAndBindForAllInterfaces(
      uint16_t port);

    /**
     * @brief Constructor.
     * @param domain domain of socket()
     * @param type type of socket()
     * @param protocol protocol of socket()
     * @param dst_ip destination IP address
     * @param port destination port
     */
    SocketWindows(int domain, int type, int protocol,
                  ULONG dst_ip, uint16_t port);
    SocketWindows(SocketWindows &&other);
    SocketWindows &operator=(SocketWindows &&other);
    ~SocketWindows();

    /**
     * @brief Returns the broadcast address.
     * @return broadcast address
     */
    static const ULONG &getBroadcastAddr();

  protected:
    /**
     * @brief Returns the native socket handle.
     * @return native socket handle.
     */
    const SOCKET &getHandleImpl() const;

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
    const static ULONG broadcast_addr_;
    SOCKET sock_;
    sockaddr_in dst_addr_;
};

}
