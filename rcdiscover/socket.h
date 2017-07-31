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

#include <vector>
#include <cstdint>

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
    Socket() = default;

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;

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
};

}
