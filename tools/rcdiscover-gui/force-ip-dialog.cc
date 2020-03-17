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
#include "rcdiscover/force_ip.h"

#include "force-ip-dialog.h"

#include "event-ids.h"

#include <sstream>

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/cshelp.h>
#include <wx/msgdlg.h>

ForceIpDialog::ForceIpDialog(wxHtmlHelpController *help_ctrl,
                             wxWindow *parent, wxWindowID id,
                             const wxPoint &pos,
                             long style,
                             const wxString &name) :
  SensorCommandDialog(help_ctrl, parent, id, "Set temporary IP address", 3,
                      pos, style, name)
{
  auto *const panel = getPanel();
  auto *const vbox = getVerticalBox();
  auto *const grid = getGrid();

  auto *ip_text = new wxStaticText(panel, wxID_ANY, "IP address");
  grid->Add(ip_text);
  auto *ip_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(ip_box, ip_, ID_ForceIp_IpChanged);
  grid->Add(ip_box);

  auto *subnet_text = new wxStaticText(panel, wxID_ANY, "Subnet mask");
  grid->Add(subnet_text);
  auto *subnet_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(subnet_box, subnet_, ID_ForceIp_SubnetChanged);
  grid->Add(subnet_box);

  auto *gateway_text = new wxStaticText(panel, wxID_ANY, "Default gateway");
  grid->Add(gateway_text);
  auto *gateway_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(gateway_box, gateway_, ID_ForceIp_GatewayChanged);
  grid->Add(gateway_box);

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  auto *set_ip_button = new wxButton(panel, ID_Force_IP,
                                     "Set temporary IP address");
  button_box->Add(set_ip_button, 1);
  auto *clear_button = new wxButton(panel, ID_Clear_IP_Form,
                                     "Clear form");
  button_box->Add(clear_button, 0);

  button_box->AddSpacer(20);

  int w, h;
  set_ip_button->GetSize(&w, &h);
  auto *help_button = new wxContextHelpButton(panel, ID_Help_Force_IP,
                                              wxDefaultPosition, wxSize(h,h));
  button_box->Add(help_button, 0);

  vbox->Add(button_box, 0, wxLEFT | wxRIGHT | wxBOTTOM, 15);
  vbox->Fit(this);

  Connect(ID_Force_IP,
          wxEVT_BUTTON,
          wxCommandEventHandler(ForceIpDialog::onForceIpButton));
  Connect(ID_Clear_IP_Form,
          wxEVT_BUTTON,
          wxCommandEventHandler(ForceIpDialog::onClearButton));
  Connect(ID_Help_Force_IP,
          wxEVT_BUTTON,
          wxCommandEventHandler(ForceIpDialog::onHelpButton));

  Centre();
}

void ForceIpDialog::onClearButton(wxCommandEvent &)
{
  for (auto &x : ip_)
  {
    x->ChangeValue("");
  }
  for (auto &x : subnet_)
  {
    x->ChangeValue("");
  }
  for (auto &x : gateway_)
  {
    x->ChangeValue("");
  }

  for (auto &x : changed_by_user_)
  {
    x.second = false;
  }
}

void ForceIpDialog::addIpToBoxSizer(wxBoxSizer *sizer,
                                    std::array<wxTextCtrl *, 4> &ip,
                                    int id)
{
  bool first = true;
  for (auto &i : ip)
  {
    if (!first)
    {
      sizer->Add(new wxStaticText(getPanel(), ID_IP_Textbox, "."));
    }
    i = new wxTextCtrl(getPanel(), id, wxEmptyString, wxDefaultPosition,
                       wxSize(45, -1));
    changed_by_user_.emplace(i, false);
    Connect(id, wxEVT_TEXT, wxCommandEventHandler(ForceIpDialog::onIpChanged));
    sizer->Add(i, 1);
    first = false;
  }
}

uint32_t ForceIpDialog::parseIp(const std::array<wxTextCtrl *, 4> &ip)
{
  std::uint32_t result{};

  for (std::uint8_t i = 0; i < 4; ++i)
  {
    const auto s = ip[i]->GetValue().ToStdString();

    try
    {
      const auto v = std::stoul(s, nullptr, 10);
      if (v > 255)
      {
        throw std::invalid_argument("");
      }
      result |= (static_cast<std::uint32_t>(v) << ((4 - 1 - i) * 8));
    }
    catch(const std::invalid_argument &)
    {
      throw std::runtime_error(
            std::string("Each ip address, subnet and gateway segment must ") +
                        "contain a decimal value ranging from 0 to 255.");
    }
  }

  return result;
}

void ForceIpDialog::changeTextCtrlIfNotChangedByUser(wxTextCtrl *ctrl,
                                                     const std::string &v)
{
  if (ctrl->HasFocus())
  {
    return;
  }

  if (ctrl->GetValue().empty())
  {
    changed_by_user_[ctrl] = false;
  }

  if (!changed_by_user_[ctrl])
  {
    ctrl->ChangeValue(v);
    ctrl->SetBackgroundColour(wxColour(240, 240, 240));
  }
}

void ForceIpDialog::onIpChanged(wxCommandEvent &event)
{
  auto *const text_ctrl =
      dynamic_cast<wxTextCtrl *>(event.GetEventObject());
  if (text_ctrl != nullptr)
  {
    if (!text_ctrl->GetValue().empty())
    {
      changed_by_user_[text_ctrl] = true;
      text_ctrl->SetBackgroundColour(wxColor(255, 255, 255));
    }
  }

  if (event.GetId() == ID_ForceIp_IpChanged ||
      event.GetId() == ID_ForceIp_SubnetChanged)
  {
    try
    {
      const auto ip = parseIp(ip_);

      // 10.0.0.0/8 addresses
      if (static_cast<std::uint8_t>(ip >> 24) == 10)
      {
        changeTextCtrlIfNotChangedByUser(subnet_[0], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[1], "0");
        changeTextCtrlIfNotChangedByUser(subnet_[2], "0");
        changeTextCtrlIfNotChangedByUser(subnet_[3], "0");
      }

      // 172.16.0.0/12 addresses
      if (static_cast<std::uint8_t>(ip >> 24) == 172 &&
          (static_cast<std::uint8_t>(ip >> 16) & 16) != 0)
      {
        changeTextCtrlIfNotChangedByUser(subnet_[0], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[1], "240");
        changeTextCtrlIfNotChangedByUser(subnet_[2], "0");
        changeTextCtrlIfNotChangedByUser(subnet_[3], "0");
      }

      // 192.168.0.0/16 addresses
      if (static_cast<std::uint8_t>(ip >> 24) == 192
          && static_cast<std::uint8_t>(ip >> 16) == 168)
      {
        changeTextCtrlIfNotChangedByUser(subnet_[0], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[1], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[2], "0");
        changeTextCtrlIfNotChangedByUser(subnet_[3], "0");
      }

      // 169.254.0.0/16 addresses
      if (static_cast<std::uint8_t>(ip >> 24) == 169
          && static_cast<std::uint8_t>(ip >> 16) == 254)
      {
        changeTextCtrlIfNotChangedByUser(subnet_[0], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[1], "255");
        changeTextCtrlIfNotChangedByUser(subnet_[2], "0");
        changeTextCtrlIfNotChangedByUser(subnet_[3], "0");
      }

      const auto subnet = parseIp(subnet_);

      const auto predicted_gateway = (ip & subnet) | 0x1;

      changeTextCtrlIfNotChangedByUser(gateway_[0],
          std::to_string(static_cast<std::uint8_t>(predicted_gateway >> 24)));
      changeTextCtrlIfNotChangedByUser(gateway_[1],
          std::to_string(static_cast<std::uint8_t>(predicted_gateway >> 16)));
      changeTextCtrlIfNotChangedByUser(gateway_[2],
          std::to_string(static_cast<std::uint8_t>(predicted_gateway >> 8)));
      changeTextCtrlIfNotChangedByUser(gateway_[3],
          std::to_string(static_cast<std::uint8_t>(predicted_gateway >> 0)));
    }
    catch(const std::runtime_error &)
    { }
  }
}

void ForceIpDialog::onForceIpButton(wxCommandEvent &)
{
  try
  {
    std::array<uint8_t, 6> mac = getMac();
    std::string mac_string = getMacString();

    const auto ip = parseIp(ip_);
    const auto subnet = parseIp(subnet_);
    const auto gateway = parseIp(gateway_);

    if ((ip & subnet) != (gateway & subnet))
    {
      std::ostringstream oss;
      oss << "IP address and gateway appear to be in different subnets. " <<
             "Are you sure to proceed?";
      const int answer = wxMessageBox(oss.str(), "", wxYES_NO);
      if (answer == wxNO)
      {
        return;
      }
    }

    rcdiscover::ForceIP force_ip;

    std::ostringstream oss;
    oss << "Are you sure to set the IP address of the device with MAC-address "
        << mac_string << "?";
    const int answer = wxMessageBox(oss.str(), "", wxYES_NO);

    if (answer == wxYES)
    {
      std::uint64_t m = 0;
      m |= static_cast<std::uint64_t>(mac[0]) << 40;
      m |= static_cast<std::uint64_t>(mac[1]) << 32;
      m |= static_cast<std::uint64_t>(mac[2]) << 24;
      m |= static_cast<std::uint64_t>(mac[3]) << 16;
      m |= static_cast<std::uint64_t>(mac[4]) << 8;
      m |= static_cast<std::uint64_t>(mac[5]) << 0;
      force_ip.sendCommand(m, ip, subnet, gateway);
    }

    Hide();
  }
  catch(const std::runtime_error &ex)
  {
    wxMessageBox(ex.what(), "Error", wxOK | wxICON_ERROR);
  }
}

void ForceIpDialog::onHelpButton(wxCommandEvent &)
{
  displayHelp("forceip");
}

BEGIN_EVENT_TABLE(ForceIpDialog, SensorCommandDialog)
END_EVENT_TABLE()
