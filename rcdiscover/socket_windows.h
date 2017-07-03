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
    typedef SOCKET SocketType;

  public:
    static SocketWindows create();
    static std::vector<SocketWindows> createAndBindForAllInterfaces();

    SocketWindows(int domain, int type, int protocol);
    SocketWindows(SocketWindows &&other);
    SocketWindows &operator=(SocketWindows &&other);
    ~SocketWindows();

    static const ULONG &getBroadcastAddr();

  protected:
    const SOCKET &getHandleImpl() const;
    void bindImpl(const sockaddr_in &addr);
    void sendtoImpl(const std::vector<uint8_t> &sendbuf, const sockaddr_in &addr);
    void enableBroadcastImpl();
    void enableNonBlockingImpl();

  private:
    const static ULONG broadcast_addr_;
    SOCKET sock_;
};

}
