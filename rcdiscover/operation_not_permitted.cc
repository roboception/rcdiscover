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

#include "operation_not_permitted.h"

namespace rcdiscover
{

OperationNotPermitted::OperationNotPermitted() :
  std::runtime_error("Operation not permitted")
{ }

}
