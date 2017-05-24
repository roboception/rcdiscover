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

#include "wol_exception.h"

#include <stdexcept>

namespace rcdiscover
{

WOLException::WOLException(const std::string& msg, const int errnum) :
  std::runtime_error(msg),
  errnum_(errnum),
  msg_(msg + " - " + std::to_string(errnum))
{ }

const char *WOLException::what() const noexcept
{
  return msg_.c_str();
}

}
