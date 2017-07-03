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

  public:
    explicit WOL_Windows(uint64_t hardware_addr, uint16_t port);
    explicit WOL_Windows(std::array<uint8_t, 6> hardware_addr, uint16_t port);

  protected:
    void sendUdpImpl(const std::array<uint8_t, 4> *password) const;

  private:
    const ULONG broadcast_addr_;
};

}

#endif // WOL_LINUX_H
