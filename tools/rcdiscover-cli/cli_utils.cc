/*
 * rcdiscover - the network discovery tool for rc_visard
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

#include "cli_utils.h"

#include <rcdiscover/utils.h>
#include <rcdiscover/discover.h>

#include <stdexcept>
#include <array>

int parseFilterArguments(int argc, char **argv, DeviceFilter &filter)
{
  if (argc == 0)
  {
    throw std::invalid_argument("-f expects an option");
  }

  const std::string p = argv[0];

  if (p.compare(0, 5, "name=") == 0)
  {
    filter.name = p.substr(5);
  }
  else if (p.compare(0, 7, "serial=") == 0)
  {
    filter.serial = p.substr(7);
  }
  else if (p.compare(0, 4, "mac=") == 0)
  {
    filter.mac = p.substr(4);
  }
  else
  {
    throw std::invalid_argument("Unknown option for parameter -f: " + p);
  }

  return 1;
}

bool filterDevice(const rcdiscover::DeviceInfo &device_info,
                  const DeviceFilter &filter)
{
  const std::string &fname = filter.name;
  const std::string &fserial = filter.serial;
  const std::string &fmac = filter.mac;
  const std::string &name = device_info.getUserName().empty() ? device_info.getModelName() : device_info.getUserName();
  if (!fname.empty() && !wildcardMatch(name.begin(), name.end(), fname.begin(), fname.end())) return false;
  const auto &serial = device_info.getSerialNumber();
  if (!fserial.empty() && !wildcardMatch(serial.begin(), serial.end(), fserial.begin(), fserial.end())) return false;
  const auto &mac = mac2string(device_info.getMAC());
  if (!fmac.empty() && !wildcardMatch(mac.begin(), mac.end(), fmac.begin(), fmac.end())) return false;
  return true;
}

std::vector<rcdiscover::DeviceInfo> discoverWithFilter(
    const DeviceFilter &filter)
{
  rcdiscover::Discover discover;
  discover.broadcastRequest();

  std::vector<rcdiscover::DeviceInfo> infos;
  while (discover.getResponse(infos, 100)) { }

  std::sort(infos.begin(), infos.end());
  infos.erase(std::unique(infos.begin(), infos.end()), infos.end());

  std::vector<rcdiscover::DeviceInfo> filtered_devices;
  for (const auto &info : infos)
  {
    if (filterDevice(info, filter))
    {
      filtered_devices.push_back(info);
    }
  }
  return filtered_devices;
}

void printTable(std::ostream &oss,
                const std::vector<std::vector<std::string>> &to_be_printed)
{
  std::size_t max_columns = 0;
  for (const auto &row : to_be_printed)
  {
    max_columns = std::max(max_columns, row.size());
  }

  std::vector<std::size_t> column_width(max_columns, 0);
  for (const auto &row : to_be_printed)
  {
    for (std::size_t col = 0; col < row.size(); ++col)
    {
      column_width[col] = std::max(column_width[col], row[col].size());
    }
  }

  for (const auto &row : to_be_printed)
  {
    for (std::size_t col = 0; col < row.size(); ++col)
    {
      std::string s = row[col];
      if (col < row.size() - 1)
      {
        s.append(column_width[col] - s.length(), ' ');
        s += '\t';
      }
      oss << s;
    }
    oss << '\n';
  }
}

void printDeviceTable(std::ostream &oss,
                      const std::vector<rcdiscover::DeviceInfo> &devices,
                      bool print_header,
                      bool iponly, bool serialonly)
{
  std::vector<std::vector<std::string>> to_be_printed;

  if (print_header)
  {
    to_be_printed.push_back({"Name", "Serial Number", "IP", "MAC", "Model"});
  }

  for (const auto &info : devices)
  {
    to_be_printed.emplace_back();
    auto &print = to_be_printed.back();

    if (iponly)
    {
      print.push_back(ip2string(info.getIP()));
    }
    else if (serialonly)
    {
      print.push_back(info.getSerialNumber());
    }
    else
    {
      const std::string &name = info.getUserName().empty()
                                ? info.getModelName()
                                : info.getUserName();

      print.push_back(name);
      print.push_back(info.getSerialNumber());
      print.push_back(ip2string(info.getIP()));
      print.push_back(mac2string(info.getMAC()));
      print.push_back(info.getModelName());
    }
  }

  printTable(oss, to_be_printed);
}
