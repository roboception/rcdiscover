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

#include "about-dialog.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include "resources/lgpl.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static const std::string rc_copyright = "Copyright 2017 Roboception GmbH.";
static const std::string wx_license = "This product contains code from the \
LGPLed library wxWidgets (http://www.wxwidgets.org/).";

AboutDialog::AboutDialog(wxWindow *parent, wxWindowID id,
                         const wxPoint &pos,
                         long style,
                         const wxString &name) :
  wxDialog(parent, id, "About rc_discover", pos, wxSize(420, 300), style, name)
{
  auto *panel = new wxPanel(this);
  auto *vbox = new wxBoxSizer(wxVERTICAL);

  // title
  auto *title_box = new wxBoxSizer(wxHORIZONTAL);
  auto *title = new wxStaticText(panel, wxID_ANY, "rc_discover "
                                 TOSTRING(PACKAGE_VERSION_MAJOR) "."
                                 TOSTRING(PACKAGE_VERSION_MINOR) "."
                                 TOSTRING(PACKAGE_VERSION_PATCH),
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
