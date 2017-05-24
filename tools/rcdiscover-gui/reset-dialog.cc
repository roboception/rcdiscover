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

#include "reset-dialog.h"

#include "event_ids.h"
#include "../utils.h"

#include <wx/dialog.h>
#include <wx/panel.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/dataview.h>
#include <wx/msgdlg.h>
#include <wx/valgen.h>

#ifdef WIN32
#include "rcdiscover/wol_windows.h"
typedef rcdiscover::WOL_Windows WOL;
#else
#include "rcdiscover/wol_linux.h"
typedef rcdiscover::WOL_Linux WOL;
#endif

ResetDialog::ResetDialog(wxWindow *parent, wxWindowID id,
            const wxString &title,
            const wxPoint &pos,
            long style,
            const wxString &name) :
  wxDialog(parent, id, title, pos, wxSize(450,200), style, name),
  sensors_(nullptr),
  mac_{nullptr},
  ip_checkbox_(nullptr),
  ip_{nullptr},
  sensor_list_(nullptr)
{
  auto *panel = new wxPanel(this, -1);
  auto *vbox = new wxBoxSizer(wxVERTICAL);

  auto *grid = new wxFlexGridSizer(4, 2, 10, 25);

  auto *sensors_text = new wxStaticText(panel, wxID_ANY, "rc_visard");
  grid->Add(sensors_text);

  auto *sensors_box = new wxBoxSizer(wxHORIZONTAL);
  sensors_ = new wxChoice(panel, ID_Sensor_Combobox);
  sensors_box->Add(sensors_, 1);
  grid->Add(sensors_box, 1, wxEXPAND);

  auto *mac_text = new wxStaticText(panel, wxID_ANY, "MAC address");
  grid->Add(mac_text);

  auto *mac_box = new wxBoxSizer(wxHORIZONTAL);
  int i = 0;
  for (auto& m : mac_)
  {
    if (i > 0)
    {
      mac_box->Add(new wxStaticText(panel, ID_MAC_Textbox, ":"));
    }
    m = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(35, -1));
    mac_box->Add(m, 1);
    ++i;
  }
  grid->Add(mac_box, 1, wxEXPAND);

//      auto *ip_text_box = new wxBoxSizer(wxVERTICAL);
  ip_checkbox_ = new wxCheckBox(panel, ID_IP_Checkbox, "IP address");


//      ip_text_box->Add(ip_text);
//      broadcast_ = new wxCheckBox(panel, wxID_ANY, "Broadcast");
//      ip_text_box->Add(broadcast_);
  grid->Add(ip_checkbox_);

  auto *ip_box = new wxBoxSizer(wxHORIZONTAL);
  i = 0;
  for (auto &ip : ip_)
  {
    if (i > 0)
    {
      ip_box->Add(new wxStaticText(panel, ID_IP_Textbox, "."));
    }
    ip = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(30, -1));
    ip->Disable();
    ip_box->Add(ip, 1);
    ++i;
  }
  ip_box->AddStretchSpacer();
  grid->Add(ip_box, 0, wxEXPAND);

  vbox->Add(grid, 0, wxALL | wxEXPAND, 15);

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  auto *reset_param_button = new wxButton(panel, ID_Reset_Params, "Reset Parameters");
  button_box->Add(reset_param_button, 0, wxEXPAND);
  auto *reset_gige_button = new wxButton(panel, ID_Reset_GigE, "Reset GigE");
  button_box->Add(reset_gige_button, 0, wxEXPAND);
  auto *reset_all_button = new wxButton(panel, ID_Reset_All, "Reset All");
  button_box->Add(reset_all_button, 0, wxEXPAND);
  auto *switch_part_button = new wxButton(panel, ID_Switch_Partition, "Switch Partitions");
  button_box->Add(switch_part_button, 0, wxEXPAND);

  vbox->Add(button_box, 0, wxLEFT | wxRIGHT | wxBOTTOM | wxEXPAND, 15);

  panel->SetSizer(vbox);
  Centre();

  Connect(ID_Sensor_Combobox,
          wxEVT_CHOICE,
          wxCommandEventHandler(ResetDialog::onSensorSelected));
  Connect(ID_IP_Checkbox,
          wxEVT_CHECKBOX,
          wxCommandEventHandler(ResetDialog::onIpCheckboxChanged));
  Connect(ID_Reset_Params,
          wxEVT_BUTTON,
          wxCommandEventHandler(ResetDialog::onResetButton));
  Connect(ID_Reset_GigE,
          wxEVT_BUTTON,
          wxCommandEventHandler(ResetDialog::onResetButton));
  Connect(ID_Reset_All,
          wxEVT_BUTTON,
          wxCommandEventHandler(ResetDialog::onResetButton));
  Connect(ID_Switch_Partition,
          wxEVT_BUTTON,
          wxCommandEventHandler(ResetDialog::onResetButton));
}

void ResetDialog::setDiscoveredSensors(const wxDataViewListModel *sensor_list)
{
  sensor_list_ = sensor_list;

  if (sensor_list != nullptr)
  {
    sensors_->Clear();

    sensors_->Append("<Custom>");

    const auto rows = sensor_list->GetCount();
    for (typename std::decay<decltype(rows)>::type i = 0; i < rows; ++i)
    {
      wxVariant hostname{};
      wxVariant mac{};
      sensor_list->GetValueByRow(hostname, i, 0);
      sensor_list->GetValueByRow(mac, i, 3);
      const auto s = wxString::Format("%s - %s",
                                      hostname.GetString(),
                                      mac.GetString());
      sensors_->Append(s);
    }
  }

  clear();
}

void ResetDialog::setActiveSensor(const size_t row)
{
  sensors_->Select(row + 1);
  fillMacAndIp();
}

void ResetDialog::onSensorSelected(wxCommandEvent& event)
{
  if (sensors_->GetSelection() != wxNOT_FOUND)
  {
    if (sensors_->GetSelection() == 0)
    {
      clear();
    }
    else
    {
      fillMacAndIp();
    }
  }
}

void ResetDialog::onIpCheckboxChanged(wxCommandEvent&)
{
  const bool checked = ip_checkbox_->IsChecked();

  for (auto& ip : ip_)
  {
    ip->Enable(checked);
  }
}

void ResetDialog::onResetButton(wxCommandEvent& event)
{
  try
  {
    std::string func_name("");
    uint8_t func_id(0);

    switch(event.GetId())
    {
      case ID_Reset_Params:
        {
          func_name = "reset parameters";
          func_id = 0xAA;
        }
        break;

      case ID_Reset_GigE:
        {
          func_name = "reset GigE";
          func_id = 0xBB;
        }
        break;

      case ID_Reset_All:
        {
          func_name = "reset all";
          func_id = 0xFF;
        }
        break;

      case ID_Switch_Partition:
        {
          func_name = "switch partition";
          func_id = 0xCC;
        }
        break;

      default:
        throw std::runtime_error("Unknown event ID");
    }

    std::array<uint8_t, 6> mac;
    std::string mac_string;

    for (uint8_t i = 0; i < 6; ++i)
    {
      const auto s = mac_[i]->GetValue().ToStdString();

      try
      {
        const auto v = std::stoul(s, nullptr, 16);
        if (v < 0x00 || v > 0xff)
        {
          throw std::invalid_argument("");
        }
        mac[i] = static_cast<uint8_t>(v);
      }
      catch(const std::invalid_argument&)
      {
        wxMessageBox("Each MAC address segment must contain a hex value ranging from 0x00 to 0xff.",
                     "Error", wxOK | wxICON_ERROR);
        return;
      }

      if (i > 0)
      {
        mac_string += ":";
      }
      mac_string += s;
    }

    try
    {
      WOL wol(mac);

      if (ip_checkbox_->IsChecked())
      {
        std::array<uint8_t, 4> ip;
        for (uint8_t i = 0; i < 4; ++i)
        {
          try
          {
            const auto v = std::stoul(ip_[i]->GetValue().ToStdString(), nullptr, 10);
            if (v < 0 || v > 255)
            {
              throw std::invalid_argument("");
            }
            ip[i] = static_cast<uint8_t>(v);
          }
          catch(const std::invalid_argument&)
          {
            wxMessageBox("Each IP address segment must contain a decimal value ranging from 0 to 255.",
                         "Error", wxOK | wxICON_ERROR);
            return;
          }
        }
        wol.enableUDP(ip, 9);
      }
      else
      {
        wol.enableUDP(9);
      }

      std::ostringstream oss;
      oss << "Are you sure to " << func_name << " of rc_visard with MAC-address " << mac_string << "?";
      const int answer = wxMessageBox(oss.str(), "", wxYES_NO);

      if (answer == wxYES)
      {
        wol.send({0xEE, 0xEE, 0xEE, func_id});
      }
    }
    catch(const rcdiscover::WOLException& ex)
    {
      wxMessageBox(ex.what(), "An error occurred", wxOK | wxICON_ERROR);
    }
  }
  catch(const rcdiscover::OperationNotPermitted&)
  {
    wxMessageBox("rc_discovery probably requires root/admin privileges for this operation.",
                 "Operation not permitted",
                 wxOK | wxICON_ERROR);
  }
}

void ResetDialog::clear()
{
  sensors_->SetSelection(0);
  ip_checkbox_->SetValue(false);

  for (uint8_t i = 0; i < 6; ++i)
  {
    mac_[i]->Clear();
    mac_[i]->SetEditable(true);
  }
  for (uint8_t i = 0; i < 4; ++i)
  {
    ip_[i]->Clear();
    ip_[i]->SetEditable(true);
  }

  wxCommandEvent evt;
  onIpCheckboxChanged(evt);
}

void ResetDialog::fillMacAndIp()
{
  const int row = sensors_->GetSelection() - 1;

  wxVariant mac_string{};
  wxVariant ip_string{};
  sensor_list_->GetValueByRow(mac_string, row, 3);
  sensor_list_->GetValueByRow(ip_string, row, 2);

  const auto mac = split<6>(mac_string.GetString().ToStdString(), ':');
  const auto ip = split<4>(ip_string.GetString().ToStdString(), '.');

  for (uint8_t i = 0; i < 6; ++i)
  {
    mac_[i]->ChangeValue(mac[i]);
    mac_[i]->SetEditable(false);
  }
  for (uint8_t i = 0; i < 4; ++i)
  {
    ip_[i]->ChangeValue(ip[i]);
    ip_[i]->SetEditable(false);
  }

  ip_checkbox_->SetValue(true);
  wxCommandEvent evt;
  onIpCheckboxChanged(evt);
}

BEGIN_EVENT_TABLE(ResetDialog, wxDialog)
END_EVENT_TABLE()
