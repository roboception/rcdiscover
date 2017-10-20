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
#ifndef RECONNECTDIALOG_H
#define RECONNECTDIALOG_H

#include "sensor-command-dialog.h"

/**
 * @brief Dialog for sending FORCEIP_CMD with IP set to 0 to camera.
 */
class ReconnectDialog : public SensorCommandDialog
{
  public:
    ReconnectDialog() = default;

    ReconnectDialog(wxHtmlHelpController *help_ctrl,
                    wxWindow *parent, wxWindowID id,
                    const wxPoint &pos = wxDefaultPosition,
                    long style = wxDEFAULT_DIALOG_STYLE,
                    const wxString &name = wxDialogNameStr);

    virtual ~ReconnectDialog() = default;

  private:
    void onReconnectButton(wxCommandEvent &event);
    void onHelpButton(wxCommandEvent &event);

  private:
    wxDECLARE_EVENT_TABLE();
};

#endif // RECONNECTDIALOG_H
