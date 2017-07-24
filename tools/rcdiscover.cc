/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
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
