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

#include "wol_exception.h"
#include "operation_not_permitted.h"

namespace rcdiscover
{

class WOL_Linux : public WOL<WOL_Linux>
{
  friend class WOL<WOL_Linux>;

  public:
    WOL_Linux(uint64_t hardware_addr, uint16_t port);
    WOL_Linux(std::array<uint8_t, 6> hardware_addr, uint16_t port);

  protected:
    typedef SocketLinux SocketType;
};

}

#endif // WOL_LINUX_H
