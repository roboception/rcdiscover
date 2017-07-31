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

/**
 * @brief Exception representing an invalid socket operation.
 */
class SocketException : public std::runtime_error
{
  public:
    /**
     * @brief Constructor.
     * @param msg error message
     * @param errnum error code
     */
    SocketException(const std::string& msg, int errnum);
    virtual ~SocketException() = default;

    /**
     * @brief Returns error message.
     * @return error message
     */
    virtual const char*
    what() const noexcept override;

    /**
     * @brief Returns error code.
     * @return error code
     */
    int get_error_code() const noexcept;

  private:
    const int errnum_;
    const std::string msg_;
};

/**
 * @brief Exception representing a Network Unreachable error (code 101 on Unix).
 */
class NetworkUnreachableException : public SocketException
{
  public:
    /**
     * @brief Constructor.
     * @param msg error message
     * @param errnum error code
     */
    NetworkUnreachableException(const std::string &msg, int errnum);
    virtual ~NetworkUnreachableException() = default;
};

}
