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
#include "force-ip-dialog.h"

#include "event-ids.h"

#include "rcdiscover/force_ip.h"

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
  SensorCommandDialog(help_ctrl, parent, id, "Force temporary IP address", 3,
                      pos, style, name)
{
  auto *const panel = getPanel();
  auto *const vbox = getVerticalBox();
  auto *const grid = getGrid();

  auto *ip_text = new wxStaticText(panel, wxID_ANY, "IP address");
  grid->Add(ip_text);
  auto *ip_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(ip_box, ip_);
  grid->Add(ip_box);

  auto *subnet_text = new wxStaticText(panel, wxID_ANY, "Subnet mask");
  grid->Add(subnet_text);
  auto *subnet_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(subnet_box, subnet_);
  grid->Add(subnet_box);

  auto *gateway_text = new wxStaticText(panel, wxID_ANY, "Default gateway");
  grid->Add(gateway_text);
  auto *gateway_box = new wxBoxSizer(wxHORIZONTAL);
  addIpToBoxSizer(gateway_box, gateway_);
  grid->Add(gateway_box);

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  auto *set_ip_button = new wxButton(panel, ID_Force_IP,
                                     "Set IP address");
  button_box->Add(set_ip_button, 1);

  button_box->AddStretchSpacer(2);

  int w, h;
  set_ip_button->GetSize(&w, &h);
  auto *help_button = new wxContextHelpButton(panel, ID_Help_Force_IP,
                                              wxDefaultPosition, wxSize(h,h));
  button_box->Add(help_button, 0);

  vbox->Add(button_box, 0, wxLEFT | wxRIGHT | wxBOTTOM, 15);

  Connect(ID_Force_IP,
          wxEVT_BUTTON,
          wxCommandEventHandler(ForceIpDialog::onForceIpButton));
  Connect(ID_Help_Force_IP,
          wxEVT_BUTTON,
          wxCommandEventHandler(ForceIpDialog::onHelpButton));

  getPanel()->Fit();
  this->Fit();
  Centre();
}

void ForceIpDialog::clear()
{
  SensorCommandDialog::clear();


}

void ForceIpDialog::addIpToBoxSizer(wxBoxSizer *sizer,
                                    std::array<wxTextCtrl *, 4> &ip)
{
  bool first = true;
  for (auto &i : ip)
  {
    if (!first)
    {
      sizer->Add(new wxStaticText(getPanel(), ID_IP_Textbox, "."));
    }
    i = new wxTextCtrl(getPanel(), wxID_ANY, wxEmptyString, wxDefaultPosition,
                       wxSize(45, -1));
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

void ForceIpDialog::onForceIpButton(wxCommandEvent &event)
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
    oss << "Are you sure to set the IP address of rc_visard with MAC-address "
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
  }
  catch(const std::runtime_error &ex)
  {
    wxMessageBox(ex.what(), "Error", wxOK | wxICON_ERROR);
  }
}

void ForceIpDialog::onHelpButton(wxCommandEvent &event)
{
  displayHelp("forceip");
}

BEGIN_EVENT_TABLE(ForceIpDialog, SensorCommandDialog)
END_EVENT_TABLE()
