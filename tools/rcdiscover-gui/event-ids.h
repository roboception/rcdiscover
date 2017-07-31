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

#ifndef EVENT_IDS_H
#define EVENT_IDS_H

#include <wx/defs.h>
#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_COMMAND_DISCOVERY_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DISCOVERY_ERROR, wxThreadEvent);

enum
{
  ID_DiscoverButton = wxID_HIGHEST + 1,
  ID_ResetButton,
  ID_DataViewListCtrl,
  ID_OpenWebGUI,
  ID_Reset_Params,
  ID_Reset_GigE,
  ID_Reset_All,
  ID_Switch_Partition,
  ID_Help,
  ID_Help_Discovery,
  ID_Help_Reset,

  ID_Sensor_Combobox,
  ID_MAC_Textbox,
  ID_IP_Textbox,
  ID_IP_Checkbox
};

#endif // EVENT_IDS_H
