/*
 * rcdiscover - the network discovery tool for rc_visard
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

// placed here to make sure to include winsock2.h before windows.h
#include "rcdiscover/wol.h"

#include "reset-dialog.h"

#include "event-ids.h"
#include "resources.h"

#include "rcdiscover/wol_exception.h"
#include "rcdiscover/operation_not_permitted.h"

#include <thread>
#include <sstream>

#include <wx/dialog.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/valgen.h>
#include <wx/msgdlg.h>
#include <wx/panel.h>
#include <wx/cshelp.h>

ResetDialog::ResetDialog(wxHtmlHelpController *help_ctrl,
                         wxWindow *parent, wxWindowID id,
                         const wxPoint &pos,
                         long style,
                         const wxString &name) :
  SensorCommandDialog(help_ctrl, parent, id, "Reset rc_visard", 0, pos,
                      style, name)
{
  const auto panel = getPanel();
  const auto vbox = getVerticalBox();

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  auto *reset_param_button = new wxButton(panel, ID_Reset_Params,
                                          "Reset Parameters", wxDefaultPosition,
                                          wxDefaultSize, wxBU_EXACTFIT);
  button_box->Add(reset_param_button, 0, wxEXPAND);
  auto *reset_gige_button = new wxButton(panel, ID_Reset_GigE,
                                         "Reset Network", wxDefaultPosition,
                                         wxDefaultSize, wxBU_EXACTFIT);
  button_box->Add(reset_gige_button, 0, wxEXPAND);
  auto *reset_all_button = new wxButton(panel, ID_Reset_All,
                                        "Reset All", wxDefaultPosition,
                                        wxDefaultSize, wxBU_EXACTFIT);
  button_box->Add(reset_all_button, 0, wxEXPAND);
  auto *switch_part_button = new wxButton(panel, ID_Switch_Partition,
                                          "Switch Partitions",
                                          wxDefaultPosition, wxDefaultSize,
                                          wxBU_EXACTFIT);
  button_box->Add(switch_part_button, 0, wxEXPAND);

  button_box->AddSpacer(20);
  int w, h;
  switch_part_button->GetSize(&w, &h);
  auto *help_button = new wxContextHelpButton(panel, ID_Help_Reset,
                                              wxDefaultPosition, wxSize(h,h));
  button_box->Add(help_button, 0, wxEXPAND);

  vbox->Add(button_box, 0, wxLEFT | wxRIGHT | wxBOTTOM, 15);
  vbox->Fit(this);

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
  Connect(ID_Help_Reset,
          wxEVT_BUTTON,
          wxCommandEventHandler(ResetDialog::onHelpButton));

  Centre();
}

void ResetDialog::onResetButton(wxCommandEvent &event)
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
          func_name = "reset network parameters";
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

    try
    {
      std::array<uint8_t, 6> mac = getMac();
      std::string mac_string = getMacString();

      rcdiscover::WOL wol(mac, 9);

      std::ostringstream reset_check_str;
      reset_check_str << "Are you sure to " << func_name <<
             " of rc_visard with MAC-address " << mac_string << "?";
      const int reset_check_answer =
          wxMessageBox(reset_check_str.str(), "", wxYES_NO);

      if (reset_check_answer == wxYES)
      {
        bool try_again = false;
        do
        {
          wol.send({{0xEE, 0xEE, 0xEE, func_id}});

          auto sent_dialog = new wxMessageDialog(
              this,
              "Please check whether rc_visard's LED turned white and whether rc_visard is rebooting.",
              "Command sent", wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxCENTRE);
          sent_dialog->SetOKCancelLabels("Try again", "Done");
          const int sent_answer = sent_dialog->ShowModal();
          try_again = sent_answer == wxID_OK;
        }
        while (try_again);
      }

      Hide();
    }
    catch(const std::runtime_error& ex)
    {
      wxMessageBox(ex.what(), "Error", wxOK | wxICON_ERROR);
    }
  }
  catch(const rcdiscover::OperationNotPermitted&)
  {
    wxMessageBox(std::string("rcdiscover probably requires root/admin ") +
                 "privileges for this operation.",
                 "Operation not permitted",
                 wxOK | wxICON_ERROR);
  }
}

void ResetDialog::onHelpButton(wxCommandEvent &)
{
  displayHelp("reset");
}

BEGIN_EVENT_TABLE(ResetDialog, SensorCommandDialog)
END_EVENT_TABLE()
