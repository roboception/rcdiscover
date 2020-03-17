/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
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

#ifndef EVENT_IDS_H
#define EVENT_IDS_H

#include <wx/defs.h>
#include <wx/event.h>

wxDECLARE_EVENT(wxEVT_COMMAND_DISCOVERY_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DISCOVERY_ERROR, wxThreadEvent);

enum
{
  ID_DiscoverButton = wxID_HIGHEST + 1,
  ID_FilterTextInput,
  ID_ResetButton,
  ID_ForceIpButton,
  ID_ReconnectButton,
  ID_DataViewListCtrl,
  ID_OpenWebGUI,
  ID_CopyName,
  ID_CopyManufacturer,
  ID_CopyModel,
  ID_CopySerial,
  ID_CopyIP,
  ID_CopyMac,
  ID_Reset_Params,
  ID_Reset_GigE,
  ID_Reset_All,
  ID_Switch_Partition,
  ID_Force_IP,
  ID_Clear_IP_Form,
  ID_Reconnect,
  ID_Help,
  ID_Help_Discovery,
  ID_Help_Reset,
  ID_Help_Force_IP,
  ID_Help_Reconnect,
  ID_OnlyRcCheckbox,
  ID_ForceIp_IpChanged,
  ID_ForceIp_SubnetChanged,
  ID_ForceIp_GatewayChanged,

  ID_Sensor_Combobox,
  ID_MAC_Textbox,
  ID_IP_Checkbox,
  ID_IP_Textbox,
  ID_Subnet_Textbox,
  ID_Gateway_Textbox
};

#endif // EVENT_IDS_H
