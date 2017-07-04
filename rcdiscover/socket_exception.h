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

#include <stdexcept>

namespace rcdiscover
{

class SocketException : public std::runtime_error
{
  public:
    SocketException(const std::string& msg, int errnum);

    virtual const char*
    what() const noexcept override;

    int get_error_code() const noexcept;

  private:
    const int errnum_;
    const std::string msg_;
};

}
