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

namespace rcdiscover
{

class Discover
{
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

    bool getResponse(DeviceInfo &info, int timeout=1000);

  private:

    int sock;
};

}

#endif