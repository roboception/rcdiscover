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

#include "about-dialog.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "resources/lgpl.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static const std::string rc_copyright = "Copyright (c) 2017 Roboception GmbH";
static const std::string rc_license = "The source code of rcdiscover is available under the 3-clause BSD license.";
static const std::string wx_license = "This program contains code from the \
LGPLed library wxWidgets (http://www.wxwidgets.org/).";

AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id,
                         const wxPoint &pos,
                         long style,
                         const wxString &name) :
  wxDialog(parent, id, "About rcdiscover", pos, wxSize(420, 400), style, name)
{
  auto *panel = new wxPanel(this);
  auto *vbox = new wxBoxSizer(wxVERTICAL);

  // title
  auto *title_box = new wxBoxSizer(wxHORIZONTAL);
  auto *title = new wxStaticText(panel, wxID_ANY, "rcdiscover " PACKAGE_VERSION,
                                 wxDefaultPosition,
                                 wxDefaultSize,
                                 wxALIGN_CENTRE);
  title->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
                        wxFONTWEIGHT_BOLD));
  title_box->Add(title, 1, wxEXPAND | wxALL, 10);
  vbox->Add(title_box, 0, wxALIGN_CENTER);

  // Roboception copyright
  auto *copyright_box = new wxBoxSizer(wxHORIZONTAL);
  auto *copyright = new wxStaticText(panel, wxID_ANY, rc_copyright);
  copyright_box->Add(copyright, 1, wxEXPAND | wxALL, 10);
  vbox->Add(copyright_box, 0, wxALIGN_CENTER);

  // Roboception license
  auto *rclicense_box = new wxBoxSizer(wxHORIZONTAL);
  auto *rclicense = new wxStaticText(panel, wxID_ANY, rc_license);
  rclicense->Wrap(400);
  rclicense_box->Add(rclicense, 1, wxEXPAND | wxALL, 10);
  vbox->Add(rclicense_box, 0, wxALIGN_CENTER);

  // wxWidgets license
  auto *licenses_box = new wxBoxSizer(wxHORIZONTAL);
  auto *licenses = new wxStaticText(panel, wxID_ANY, wx_license);
  licenses->Wrap(400);
  licenses_box->Add(licenses, 1, wxEXPAND | wxALL, 10);
  vbox->Add(licenses_box, 0);

  auto *lgpl_box = new wxBoxSizer(wxHORIZONTAL);
  auto *lgpl = new wxTextCtrl(panel, wxID_ANY, wxEmptyString, wxDefaultPosition,
                            wxSize(400, -1), wxTE_MULTILINE | wxTE_READONLY);
  lgpl->SetFont(
      wxFont(7, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
  lgpl->WriteText(lgpl_text);
  lgpl_box->Add(lgpl, 1, wxEXPAND | wxALL, 10);
  vbox->Add(lgpl_box, 1);

  panel->SetSizer(vbox);
  Centre();
}
