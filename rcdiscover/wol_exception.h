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

#ifndef WOL_EXCEPTION_H
#define WOL_EXCEPTION_H

#include <stdexcept>

namespace rcdiscover
{

/**
 * @brief Exception representing an error during Magic Packet sending.
 */
class WOLException : public std::runtime_error
{
  public:
    WOLException(const std::string& msg, int errnum);

    virtual const char*
    what() const noexcept override;

  private:
    const int errnum_;
    const std::string msg_;
};

}

#endif // WOL_EXCEPTION_H
