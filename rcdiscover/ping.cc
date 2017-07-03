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

#include "ping.h"

#include "socket_exception.h"

#ifdef WIN32

#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>

#else

#endif

namespace rcdiscover
{

#ifdef WIN32

bool checkReachabilityOfSensor(const DeviceInfo &info)
{
  char data[] = "data";

  ULONG ipaddr = htonl(info.getIP());

  HANDLE h_icmp = IcmpCreateFile();
  if (h_icmp == INVALID_HANDLE_VALUE)
  {
    throw SocketException("Unable to create ICMP socket", GetLastError());
  }

  DWORD reply_size = sizeof(ICMP_ECHO_REPLY) + sizeof(data);
  LPVOID reply_buffer = reinterpret_cast<void *>(malloc(reply_size));
  if (reply_buffer == nullptr)
  {
    throw std::runtime_error("Could not allocate memory");
  }

  DWORD result = IcmpSendEcho(h_icmp, ipaddr, data,
                              sizeof(data), nullptr,
                              reply_buffer, reply_size, 1000);

  if (result != 0 &&
      reinterpret_cast<ICMP_ECHO_REPLY *>(reply_buffer)->Status == IP_SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  }
}

#else

#endif

}
