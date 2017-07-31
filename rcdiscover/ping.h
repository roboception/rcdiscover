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

#include "deviceinfo.h"

namespace rcdiscover
{

/**
 * @brief Check whether an rc_visard is reachable via ICMP.
 * @param info DeviceInfo of rc_visard
 * @return whether the rc_visard is reachable
 */
bool checkReachabilityOfSensor(const DeviceInfo &info);

}
