/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 */

#ifndef RCDISCOVER_DISCOVER
#define RCDISCOVER_DISCOVER

#include "deviceinfo.h"

#ifdef WIN32
#include "socket_windows.h"
#else
#include "socket_linux.h"
#endif

namespace rcdiscover
{

class Discover
{
  public:
#ifdef WIN32
    typedef SocketWindows SocketType;
#else
    typedef SocketLinux SocketType;
#endif

  public:

    /**
      Initializes a socket ready for broadcasting requests.

      NOTE: Exceptions are thrown in case of severe network errors.
    */

    Discover();
    ~Discover();

    /**
      Broadcasts a discovery command request.
    */

    void broadcastRequest();

    /**
      Returns a discovery response. This method should be called until there is
      no further response.

      @param info    Info object that will be filled if there is an available
                     response.
      @param timeout Timeout in Milliseconds.
      @return        True if there was a valid response. In this case, the info
                     object contains valid information. False in case of a
                     timeout.
    */

    bool getResponse(std::vector<DeviceInfo> &info, int timeout_per_socket=1000);

  private:
    std::vector<SocketType> sockets_;
};

}

#endif
