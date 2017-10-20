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
#ifndef FORCE_IP_DIALOG_H
#define FORCE_IP_DIALOG_H

#include "sensor-command-dialog.h"

#include <map>

/**
 * @brief Dialog for sending FORCEIP_CMD to camera.
 */
class ForceIpDialog : public SensorCommandDialog
{
  public:
    ForceIpDialog() = default;

    ForceIpDialog(wxHtmlHelpController *help_ctrl,
                  wxWindow *parent, wxWindowID id,
                  const wxPoint &pos = wxDefaultPosition,
                  long style = wxDEFAULT_DIALOG_STYLE,
                  const wxString &name = wxDialogNameStr);

    virtual ~ForceIpDialog() = default;

  private:
    void addIpToBoxSizer(wxBoxSizer *sizer,
                         std::array<wxTextCtrl *, 4> &ip, int id);
    static std::uint32_t parseIp(const std::array<wxTextCtrl *, 4> &ip);

    void changeTextCtrlIfNotChangedByUser(wxTextCtrl *ctrl,
                                          const std::string &v);

  private:
    void onClearButton(wxCommandEvent &event);

    void onForceIpButton(wxCommandEvent &event);
    void onHelpButton(wxCommandEvent &event);

    void onIpChanged(wxCommandEvent &event);

  private:
    std::array<wxTextCtrl *, 4> ip_;
    std::array<wxTextCtrl *, 4> subnet_;
    std::array<wxTextCtrl *, 4> gateway_;

    std::map<const wxTextCtrl *, bool> changed_by_user_;

    wxDECLARE_EVENT_TABLE();
};

#endif // FORCE_IP_DIALOG_H
