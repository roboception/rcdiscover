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

class SocketLinux : public Socket<SocketLinux>
{
  friend class Socket<SocketLinux>;

  public:
    typedef int SocketType;

  public:
    static SocketLinux create(in_addr_t dst_ip, uint16_t port);
    static std::vector<SocketLinux> createAndBindForAllInterfaces(uint16_t port);

    SocketLinux(int domain, int type, int protocol, in_addr_t dst_ip, uint16_t port);
    SocketLinux(SocketLinux &&other);
    SocketLinux &operator=(SocketLinux &&other);
    ~SocketLinux();

    const sockaddr_in& getDestSockAddr() const;

    static const in_addr_t &getBroadcastAddr();

  protected:
    const int &getHandleImpl() const;
    void bindImpl(const sockaddr_in &addr);
    void sendImpl(const std::vector<uint8_t> &sendbuf);
    void enableBroadcastImpl();
    void enableNonBlockingImpl();

  private:
    void bindToDevice(const std::string &device);

  private:
    const static in_addr_t broadcast_addr_;
    int sock_;
    sockaddr_in dst_addr_;
};

}

#endif // SOCKET_LINUX_H
