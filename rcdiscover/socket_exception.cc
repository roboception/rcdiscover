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

#include "socket_exception.h"

namespace rcdiscover
{

SocketException::SocketException(const std::string& msg, const int errnum) :
  std::runtime_error(msg),
  errnum_(errnum),
  msg_(msg + " - " + std::to_string(errnum))
{ }

const char *SocketException::what() const noexcept
{
  return msg_.c_str();
}

int SocketException::get_error_code() const noexcept
{
  return errnum_;
}

NetworkUnreachableException::NetworkUnreachableException(
    const std::string &msg, int errnum) :
  SocketException(msg, errnum)
{ }

}
