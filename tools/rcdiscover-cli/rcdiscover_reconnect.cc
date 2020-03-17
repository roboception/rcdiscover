/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2018 Roboception GmbH
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

#include "rcdiscover_reconnect.h"

#include "cli_utils.h"

#include <rcdiscover/discover.h>
#include <rcdiscover/force_ip.h>

#include <stdexcept>
#include <iostream>
#include <algorithm>

static void printHelp(std::ostream &os, const std::string &command)
{
  os << "Usage: ";
  os << command << " [<args>]\n";
  os << '\n';
  os << "-h, --help         Show this help and exit\n";
  os << "-f name=<name>     Filter by name\n";
  os << "-f serial=<serial> Filter by serial number\n";
  os << "-f mac=<mac>       Filter by MAC address\n";
  os << "-f iface=<mac>     Filter by interface name\n";
  os << "-f model=<model>   Filter by model name\n";
  os << "-y                 Assume 'yes' for all queries\n";
}

int runReconnect(const std::string &command, int argc, char **argv)
{
  DeviceFilter device_filter{};
  bool yes = false;

  int i=0;
  while (i < argc)
  {
    std::string p=argv[i++];

    if (p == "-y")
    {
      yes = true;
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
        printHelp(std::cerr, command);
        return 1;
      }
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

  if (device_filter.mac.empty() &&
      device_filter.name.empty() &&
      device_filter.serial.empty())
  {
    std::cerr << "No filter set" << std::endl;
    printHelp(std::cerr, command);
    return 1;
  }

  const auto devices = discoverWithFilter(device_filter);

  if (devices.empty())
  {
    std::cout << "No device found" << std::endl;
    return 0;
  }

  std::cout << "Reconnecting the following devices:\n";
  printDeviceTable(std::cout, devices, true, false, false);

  if (!yes)
  {
    std::cout << "Are you sure? [y/N] ";
    std::string answer;
    std::getline(std::cin, answer);
    if (answer != "y" && answer != "Y")
    {
      std::cout << "Cancel" << std::endl;
      return 0;
    }
  }

  for (const auto &device : devices)
  {
    rcdiscover::ForceIP force_ip;
    force_ip.sendCommand(device.getMAC(), 0, 0, 0);
  }

  std::cout << "Done" << std::endl;

  return 0;
}
