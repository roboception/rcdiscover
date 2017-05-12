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
#include "utils.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifdef WIN32
#include <winsock2.h>
#endif

int main(int argc, char *argv[])
{
#ifdef WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

  std::cout << "User name\tSerial number\tIP\t\tMAC" << std::endl;

  rcdiscover::Discover discover;
  discover.broadcastRequest();

  rcdiscover::DeviceInfo info;
  while (discover.getResponse(info))
  {
    std::string name=info.getUserName();

    if (name.size() == 0)
    {
      name="rc_visard";
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

#ifdef WIN32
  ::WSACleanup();
#endif

  return 0;
}
