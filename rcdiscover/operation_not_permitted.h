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

#ifndef OPERATION_NOT_PERMITTED_H
#define OPERATION_NOT_PERMITTED_H

#include <stdexcept>

namespace rcdiscover
{

/**
 * @brief Exception representing an "operation not permitted" error.
 */
class OperationNotPermitted : public std::runtime_error
{
  public:
    OperationNotPermitted();
};

}

#endif // OPERATION_NOT_PERMITTED_H
