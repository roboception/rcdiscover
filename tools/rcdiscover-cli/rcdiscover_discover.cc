/*
 * rcdiscover - the network discovery tool for rc_visard
 *
 * Copyright (c) 2018 Roboception GmbH
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

#include "rcdiscover_discover.h"

#include "cli_utils.h"

#include "rcdiscover/discover.h"
#include "rcdiscover/deviceinfo.h"
#include "rcdiscover/utils.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <cstring>

static void printHelp(std::ostream &os, const std::string &command)
{
  os << command << " [<args>]\n";
  os << '\n';
  os << "-h, --help         Show this help and exit\n";
  os << "-f name=<name>     Filter by name\n";
  os << "-f serial=<serial> Filter by serial number\n";
  os << "-f mac=<mac>       Filter by MAC address\n";
  os << "-f iface=<mac>     Filter by interface name\n";
  os << "--iponly           Show only the IP addresses of discovered sensors\n";
  os << "--serialonly       Show only the serial number of discovered sensors\n";
}

int runDiscover(const std::string &command, int argc, char **argv)
{
  // interpret command line parameters

  bool printheader = true;
  bool iponly = false;
  bool serialonly = false;
  DeviceFilter device_filter;

  int i = 0;
  while (i < argc)
  {
    std::string p = argv[i++];

    if (p == "-iponly" || p == "--iponly")
    {
      iponly = true;
      printheader = false;
    }
    else if (p == "-serialonly" || p == "--serialonly")
    {
      serialonly = true;
      printheader = false;
    }
    else if (p == "-f")
    {
      try
      {
        i += parseFilterArguments(argc - i, argv + i, device_filter);
      }
      catch (const std::invalid_argument &ex)
      {
        std::cerr << ex.what() << std::endl;
        return 1;
      }

      printheader = false;
    }
    else if (p == "-h" || p == "--help")
    {
      printHelp(std::cout, command);
      return 0;
    }
    else
    {
      std::cerr << "Invalid argument: " << p << '\n';
      printHelp(std::cerr, command);
      return 1;
    }
  }

  // broadcast discover request

  rcdiscover::Discover discover;
  discover.broadcastRequest();

  std::vector<rcdiscover::DeviceInfo> infos;

  // get all responses, sort them and remove multiple entries

  while (discover.getResponse(infos, 100))
  {}

  std::sort(infos.begin(), infos.end());
  const auto it = std::unique(infos.begin(), infos.end(),
                              [](const rcdiscover::DeviceInfo &lhs,
                                 const rcdiscover::DeviceInfo &rhs)
                              {
                                return lhs.getMAC() == rhs.getMAC() &&
                                       lhs.getIfaceName() == rhs.getIfaceName();
                              });
  infos.erase(it, infos.end());

  // go through all valid entries

  std::vector<rcdiscover::DeviceInfo> filtered_infos;
  for (rcdiscover::DeviceInfo &info : infos)
  {
    if (!info.isValid())
    {
      continue;
    }

    // filter as requested
    if (!filterDevice(info, device_filter)) continue;

    filtered_infos.push_back(info);
  }

  printDeviceTable(std::cout, filtered_infos, printheader, iponly, serialonly);

  return 0;
}
