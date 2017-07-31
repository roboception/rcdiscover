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

/**
 * @brief Dialog for Magic Packets reset of rc_visard.
 */

class ResetDialog : public wxDialog
{
  public:
    ResetDialog() = default;

    ResetDialog(wxWindow *parent, wxWindowID id,
                const wxPoint &pos = wxDefaultPosition,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString &name = wxDialogNameStr);

    virtual ~ResetDialog() = default;

    /**
     * @brief Set list of discovered rc_visards to provide a drop down menu
     * to the user.
     * @param sensor_list list of rc_visards
     */
    void setDiscoveredSensors(const wxDataViewListModel *sensor_list);

    /**
     * @brief Select a specific rc_visard of the list set by
     * setDiscoveredSensors.
     * @param row row of rc_visard list
     */
    void setActiveSensor(const unsigned int row);

  private:
    /**
     * @brief Event handler for selection of an rc_visard from drop down menu.
     * @param event event
     */
    void onSensorSelected(wxCommandEvent &event);

    /**
     * @brief Event handler for click on one of the four reset buttons.
     * @param event event
     */
    void onResetButton(wxCommandEvent &event);

    /**
     * @brief Event handler for help button.
     */
    void onHelpButton(wxCommandEvent &);

    /**
     * @brief Reset and clear all fields.
     */
    void clear();

    /**
     * @brief Fill MAC address according to selected rc_visard
     */
    void fillMac();

    wxDECLARE_EVENT_TABLE();

  private:
    wxChoice *sensors_;
    std::array<wxTextCtrl *, 6> mac_;

    const wxDataViewListModel *sensor_list_;

    wxHtmlHelpController *help_ctrl_;
};

#endif // RESETDIALOG_H
