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
    static SocketWindows create(ULONG dst_ip, uint16_t port);
    static std::vector<SocketWindows> createAndBindForAllInterfaces(
      uint16_t port);

    SocketWindows(int domain, int type, int protocol,
                  ULONG dst_ip, uint16_t port);
    SocketWindows(SocketWindows &&other);
    SocketWindows &operator=(SocketWindows &&other);
    ~SocketWindows();

    static const ULONG &getBroadcastAddr();

  protected:
    const SOCKET &getHandleImpl() const;
    void bindImpl(const sockaddr_in &addr);
    void sendImpl(const std::vector<uint8_t> &sendbuf);
    void enableBroadcastImpl();
    void enableNonBlockingImpl();

  private:
    const static ULONG broadcast_addr_;
    SOCKET sock_;
    sockaddr_in dst_addr_;
};

}
