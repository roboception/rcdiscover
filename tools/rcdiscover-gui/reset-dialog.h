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