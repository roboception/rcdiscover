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

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifdef WIN32
#include <winsock2.h>
#endif

namespace
{

std::string mac2string(const uint64_t mac)
{
  std::ostringstream out;

  out << std::hex << std::setfill('0');
  out << std::setw(2) << ((mac>>40)&0xff) << ':' << std::setw(2) << ((mac>>32)&0xff) << ':'
      << std::setw(2) << ((mac>>24)&0xff) << ':' << std::setw(2) << ((mac>>16)&0xff) << ':'
      << std::setw(2) << ((mac>>8)&0xff) << ':' << std::setw(2) << (mac&0xff);

  return out.str();
}

std::string ip2string(const uint32_t ip)
{
  std::ostringstream out;

  out << ((ip>>24)&0xff) << '.' << ((ip>>16)&0xff) << '.'
      << ((ip>>8)&0xff) << '.' << (ip&0xff);

  return out.str();
}

}

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
