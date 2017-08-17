/*
 * rcdiscover - the network discovery tool for rc_visard
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
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

#include "rcdiscover/discover.h"
#include "rcdiscover/deviceinfo.h"
#include "rcdiscover/utils.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

#ifdef WIN32
#include <winsock2.h>
#endif

int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  bool iponly=false;

  if (argc > 1 && std::strcmp(argv[1], "-iponly") == 0)
  {
    iponly=true;
  }

  rcdiscover::Discover discover;
  discover.broadcastRequest();

  std::vector<rcdiscover::DeviceInfo> infos;

  if (!iponly)
  {
    std::cout << "User name\tSerial number\tIP\t\tMAC" << std::endl;

    while (discover.getResponse(infos, 100)) { }

    std::sort(infos.begin(), infos.end());
    const auto it = std::unique(infos.begin(), infos.end());
    infos.erase(it, infos.end());

		for (rcdiscover::DeviceInfo &info : infos)
    {
			if (!info.isValid())
			{
			  continue;
			}

      std::string name=info.getUserName();

      if (name.size() == 0)
      {
        name=info.getModelName();
      }

      std::cout << name << "\t";
      std::cout << info.getSerialNumber() << "\t";
      std::cout << ip2string(info.getIP()) << "\t";
      std::cout << mac2string(info.getMAC());

      if (info.getModelName() != "rc_visard")
      {
        std::cout << "\t[other GEV device]";
      }

      std::cout << std::endl;
    }
  }
  else
  {
    while (discover.getResponse(infos, 100)) {}

    std::sort(infos.begin(), infos.end());
    const auto it = std::unique(infos.begin(), infos.end());
    infos.erase(it, infos.end());

		for (rcdiscover::DeviceInfo &info : infos)
    {
			if (!info.isValid())
			{
			  continue;
			}

      std::cout << ip2string(info.getIP()) << std::endl;
    }
  }

#ifdef WIN32
  ::WSACleanup();
#endif

  return 0;
}