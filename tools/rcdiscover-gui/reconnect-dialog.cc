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

#include "reconnect-dialog.h"

#include "event-ids.h"

#include <sstream>

#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/button.h>
#include <wx/cshelp.h>
#include <wx/msgdlg.h>

ReconnectDialog::ReconnectDialog(
    wxHtmlHelpController *help_ctrl,
    wxWindow *parent, wxWindowID id,
    const wxPoint &pos, long style,
    const wxString &name) :
  SensorCommandDialog(help_ctrl, parent, id, "Reconnect rc_visard", 0,
                      pos, style, name)
{
  auto *const panel = getPanel();
  auto *const vbox = getVerticalBox();

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  auto *reconnect_button = new wxButton(panel, ID_Reconnect, "Reconnect");
  button_box->Add(reconnect_button, 1);

  button_box->AddSpacer(20);

  int w, h;
  reconnect_button->GetSize(&w, &h);
  auto *help_button = new wxContextHelpButton(panel, ID_Help_Reconnect,
                                              wxDefaultPosition, wxSize(h,h));
  button_box->Add(help_button, 0);

  vbox->Add(button_box, 0, wxLEFT | wxRIGHT | wxBOTTOM, 15);
  vbox->Fit(this);

  Connect(ID_Reconnect,
          wxEVT_BUTTON,
          wxCommandEventHandler(ReconnectDialog::onReconnectButton));
  Connect(ID_Help_Reconnect,
          wxEVT_BUTTON,
          wxCommandEventHandler(ReconnectDialog::onHelpButton));

  Centre();
}

void ReconnectDialog::onReconnectButton(wxCommandEvent &)
{
  try
  {
    std::array<uint8_t, 6> mac = getMac();
    std::string mac_string = getMacString();

    rcdiscover::ForceIP force_ip;

    std::ostringstream oss;
    oss << "Are you sure to reconnect rc_visard with MAC-address "
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
      force_ip.sendCommand(m, 0, 0, 0);
    }

    Hide();
  }
  catch(const std::runtime_error &ex)
  {
    wxMessageBox(ex.what(), "Error", wxOK | wxICON_ERROR);
  }
}

void ReconnectDialog::onHelpButton(wxCommandEvent &)
{
  displayHelp("reconnect");
}

BEGIN_EVENT_TABLE(ReconnectDialog, SensorCommandDialog)
END_EVENT_TABLE()
