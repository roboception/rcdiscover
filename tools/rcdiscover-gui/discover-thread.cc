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

// placed here to make sure to include winsock2.h before windows.h
#include "rcdiscover/discover.h"
#include "rcdiscover/ping.h"

#include "discover-thread.h"

#include "event-ids.h"
#include "rcdiscover/utils.h"

#include <vector>
#include <algorithm>
#include <future>

#include <wx/window.h>

wxThread::ExitCode DiscoverThread::Entry()
{
  std::vector<wxVector<wxVariant>> device_list;

  try
  {
    rcdiscover::Discover discover;
    discover.broadcastRequest();

    std::vector<rcdiscover::DeviceInfo> infos;

    while (discover.getResponse(infos, 100)) { }

    std::vector<std::future<bool>> reachable;
    for (rcdiscover::DeviceInfo &info : infos)
    {
      if (!info.isValid())
      {
        continue;
      }
      reachable.push_back(std::async(std::launch::async, [&info]
      {
        return checkReachabilityOfSensor(info);
      }));
    }

    int i = 0;
    for (rcdiscover::DeviceInfo &info : infos)
    {
      if (!info.isValid())
      {
        continue;
      }

      std::string name=info.getUserName();

      if (name.size() == 0)
      {
        name = "rc_visard";
      }

      wxVector<wxVariant> data;
      data.push_back(wxVariant(name));
      data.push_back(wxVariant(info.getSerialNumber()));
      data.push_back(wxVariant(ip2string(info.getIP())));
      data.push_back(wxVariant(mac2string(info.getMAC())));
      data.push_back(wxVariant(reachable[i].get() ? L"\u2713" : L"\u2717"));

      device_list.push_back(std::move(data));

      ++i;
    }

    std::sort(device_list.begin(), device_list.end(),
              [](const wxVector<wxVariant>& lhs, const wxVector<wxVariant>& rhs) {
      for (size_t i = 0; i < lhs.size(); ++i)
      {
        if (lhs[i].GetString() != rhs[i].GetString())
        {
          return lhs[i].GetString() < rhs[i].GetString();
        }
      }
      return false;
    });

    auto it = std::unique(device_list.begin(), device_list.end(),
                          [](const wxVector<wxVariant>& lhs, const wxVector<wxVariant>& rhs){
      for (size_t i = 0; i < lhs.size(); ++i)
      {
        if (lhs[i] != rhs[i])
        {
          return false;
        }
      }
      return true;
    });

    device_list.erase(it, device_list.end());
  }
  catch(const std::exception& ex)
  {
    wxThreadEvent event(wxEVT_COMMAND_DISCOVERY_ERROR);
    event.SetString(ex.what());
    parent_->GetEventHandler()->QueueEvent(event.Clone());

    return ExitCode(1);
  }

  wxThreadEvent event(wxEVT_COMMAND_DISCOVERY_COMPLETED);
  event.SetPayload(device_list);
  parent_->GetEventHandler()->QueueEvent(event.Clone());

  return ExitCode(0);
}
