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

  // interpret command line parameters

  bool printheader=true;
  bool iponly=false;
  bool serialonly=false;
  std::string fname;
  std::string fserial;
  std::string fmac;

  int i=1;
  while (i < argc)
  {
    std::string p=argv[i++];

    if (p == "-iponly" || p == "--iponly")
    {
      iponly=true;
      printheader=false;
    }
    else if (p == "-serialonly" || p == "--serialonly")
    {
      serialonly=true;
      printheader=false;
    }
    else if (p == "-f" && i < argc)
    {
      p=argv[i++];

      if (p.compare(0, 5, "name=") == 0)
      {
        fname=p.substr(5);
      }
      else if (p.compare(0, 7, "serial=") == 0)
      {
        fserial=p.substr(7);
      }
      else if (p.compare(0, 4, "mac=") == 0)
      {
        fmac=p.substr(4);

        // ensure correct format of mac address

        std::array<uint8_t, 6> amac=string2mac(fmac);
        uint64_t mac=0;

        for (int i=0; i<6; i++)
        {
          mac<<=8;
          mac|=amac[i];
        }

        fmac=mac2string(mac);
      }
      else
      {
        std::cerr << "Unknown option for parameter -f: " << p << std::endl;
        return 1;
      }

      printheader=false;
    }
    else if (p == "--version")
    {
      std::cout << PACKAGE_VERSION << std::endl;
      return 0;
    }
    else
    {
      std::cout << argv[0] << " <parameters>" << std::endl;
      std::cout << std::endl;
      std::cout << "-h                 Shows this help and exits." << std::endl;
      std::cout << "-f name=<name>     Filter by name" << std::endl;
      std::cout << "-f serial=<serial> Filter by serial number" << std::endl;
      std::cout << "-f mac=<mac>       Filter by MAC address" << std::endl;
      std::cout << "--iponly           Shows only the IP addresses of discoverd sensors" << std::endl;
      std::cout << "--serialonly       Shows only the serial number of discovered sensors" << std::endl;
      std::cout << "--version          Print version." << std::endl;
      return 0;
    }
  }

  // broadcast discover request

  rcdiscover::Discover discover;
  discover.broadcastRequest();

  std::vector<rcdiscover::DeviceInfo> infos;

  // print header line

  if (printheader)
  {
    std::cout << "User name\tSerial number\tIP\t\tMAC" << std::endl;
  }

  // get all responses, sort them and remove multiple entries

  while (discover.getResponse(infos, 100)) { }

  std::sort(infos.begin(), infos.end());
  const auto it = std::unique(infos.begin(), infos.end());
  infos.erase(it, infos.end());

  // go through all valid entries

  for (rcdiscover::DeviceInfo &info : infos)
  {
    if (!info.isValid())
    {
      continue;
    }

    // get name of sensor with fall back to model name

    std::string name=info.getUserName();

    if (name.size() == 0)
    {
      name=info.getModelName();
    }

    // filter as requested

    if (fname.size() > 0 && fname.compare(name) != 0) continue;
    if (fserial.size() > 0 && fserial.compare(info.getSerialNumber()) != 0) continue;
    if (fmac.size() > 0 && fmac.compare(mac2string(info.getMAC())) != 0) continue;

    // print information about the device

    if (iponly)
    {
      std::cout << ip2string(info.getIP()) << std::endl;
    }
    else if (serialonly)
    {
      std::cout << info.getSerialNumber() << std::endl;
    }
    else
    {
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

#ifdef WIN32
  ::WSACleanup();
#endif

  return 0;
}
