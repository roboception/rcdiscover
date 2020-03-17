/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Raphael Schaller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "ping.h"

#include "socket_exception.h"
#include "utils.h"

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
    IcmpCloseHandle(h_icmp);
    throw std::runtime_error("Could not allocate memory");
  }

  DWORD result = IcmpSendEcho(h_icmp, ipaddr, data,
                              sizeof(data), nullptr,
                              reply_buffer, reply_size, 1000);

  IcmpCloseHandle(h_icmp);

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

bool checkReachabilityOfSensor(const DeviceInfo &info)
{
  const std::string command = "ping -c 1 -W 1 " + ip2string(info.getIP());

  FILE *in;
  if (!(in = popen(command.c_str(), "r")))
  {
    throw std::runtime_error("Could not execute ping command.");
  }

  const int exit_code = pclose(in);

  return exit_code == 0;
}

#endif

}