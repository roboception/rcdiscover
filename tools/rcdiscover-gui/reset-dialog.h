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

#ifndef RESETDIALOG_H
#define RESETDIALOG_H

#include <array>

#include <wx/dialog.h>

class wxChoice;
class wxCheckBox;
class wxTextCtrl;
class wxDataViewListModel;
class wxHtmlHelpController;

class ResetDialog : public wxDialog
{
  public:
    ResetDialog() :
      wxDialog()
    { }

    ResetDialog(wxWindow *parent, wxWindowID id,
                const wxPoint &pos = wxDefaultPosition,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString &name = wxDialogNameStr);

    void setDiscoveredSensors(const wxDataViewListModel *sensor_list);
    void setActiveSensor(const size_t row);

  private:
    void onSensorSelected(wxCommandEvent& event);
    // void onIpCheckboxChanged(wxCommandEvent&);
    void onResetButton(wxCommandEvent& event);
    void onHelpButton(wxCommandEvent& event);

    void clear();
    void fillMacAndIp();

    wxDECLARE_EVENT_TABLE();

  private:
    wxChoice *sensors_;
    std::array<wxTextCtrl *, 6> mac_;
    // wxCheckBox *ip_checkbox_;
    // std::array<wxTextCtrl *, 4> ip_;

    const wxDataViewListModel *sensor_list_;

    wxHtmlHelpController *help_ctrl_;
};

#endif // RESETDIALOG_H
