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

    template<typename T>
    const T &getHandle() const
    {
      return getDerived().getHandleImpl();
    }

    void bind(const sockaddr_in& addr)
    {
      getDerived().bindImpl(addr);
    }

    void send(const std::vector<uint8_t>& sendbuf)
    {
      getDerived().sendImpl(sendbuf);
    }

    void enableBroadcast()
    {
      getDerived().enableBroadcastImpl();
    }

    void enableNonBlocking()
    {
      getDerived().enableNonBlockingImpl();
    }
};

}
