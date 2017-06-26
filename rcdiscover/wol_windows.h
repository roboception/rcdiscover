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

#ifndef WOL_LINUX_H
#define WOL_LINUX_H

#include "wol.h"

#include <winsock2.h>

namespace rcdiscover
{

class WOL_Windows : public WOL<WOL_Windows>
{
  friend class WOL<WOL_Windows>;

  protected:
    // RAII wrapper for socket
    class Socket
    {
      public:
        static Socket socketUDP();
        static Socket socketRaw();

      public:
        Socket(int domain, int type, int protocol);
        Socket(Socket&& other);

        Socket& operator=(Socket&& other);

        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

        ~Socket();

        const SOCKET &getHandle() const;

        void bind(const sockaddr_in& addr);
        void sendto(const std::vector<uint8_t>& sendbuf,
                    const sockaddr_in& addr);
        void enableBroadcast();

      private:
        SOCKET sock_;
    };

  public:
    explicit WOL_Windows(uint64_t hardware_addr);
    explicit WOL_Windows(std::array<uint8_t, 6> hardware_addr);

  protected:
    std::vector<uint32_t> getBroadcastIPs() const;
};

}

#endif // WOL_LINUX_H
