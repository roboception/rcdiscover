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

#ifndef DISCOVERFRAME_H
#define DISCOVERFRAME_H

#include <memory>
#include <vector>

#include <wx/frame.h>
#include <wx/animate.h>

class wxDataViewListCtrl;
class wxButton;
class wxDataViewEvent;
class wxPanel;
class wxHtmlHelpController;

class ResetDialog;
class ForceIpDialog;
class ReconnectDialog;
class AboutDialog;

/**
 * @brief Main window in which the table of discovered devices is displayed.
 */
class DiscoverFrame : public wxFrame
{
  public:
    /**
     * @brief Constructor.
     * @param title title of the window
     * @param pos position of the window
     */
    DiscoverFrame(const wxString& title,
                    const wxPoint& pos);

    virtual ~DiscoverFrame() = default;

    enum COLUMNS
    {
      NAME = 0,
      MANUFACTURER = 1,
      MODEL = 2,
      SERIAL = 3,
      IP = 4,
      MAC = 5,
      IFACE = 6,
      REACHABLE = 7,

      NUM_COLUMNS = 8
    };

  private:
    /**
     * @brief Let spinner rotate.
     */
    void setBusy();
    /**
     * @brief Stop spinner rotation.
     */
    void clearBusy();

    /**
     * @brief Event handler for Discovery button click.
     */
    void onDiscoverButton(wxCommandEvent &);

    /**
     * @brief Event handler for completed device discovery.
     * @param event event
     */
    void onDiscoveryCompleted(wxThreadEvent &event);

    /**
     * @brief Event handler for help button.
     */
    void onHelpDiscovery(wxCommandEvent &);

    /**
     * @brief Event handler for erroneous device discovery.
     * @param event event
     */
    void onDiscoveryError(wxThreadEvent &event);

    /**
     * @brief Event handler for Reset button click.
     */
    void onResetButton(wxCommandEvent &);

    /**
     * @brief Event handler for ForceIP button click.
     */
    void onForceIpButton(wxCommandEvent &);

    /**
     * @brief Event handler for Reconnect button click.
     */
    void onReconnectButton(wxCommandEvent &);

    /**
     * @brief Event handler for double click on an device.
     * @param event event
     */
    void onDeviceDoubleClick(wxDataViewEvent &event);

    /**
     * @brief Event handler for right mouse button click on device.
     * @param event event
     */
    void onDataViewContextMenu(wxDataViewEvent &event);

    /**
     * @brief Event handler for "copy" context menu item.
     */
    void onCopy(wxMenuEvent &);

    /**
     * @brief Event handler for "open web gui" context menu item.
     */
    void onOpenWebGUI(wxMenuEvent &);

    /**
     * @brief Event handler for "reset" context menu item.
     */
    void onResetContextMenu(wxMenuEvent &);

    /**
     * @brief Event handler for "force ip" context menu item.
     */
    void onForceIpContextMenu(wxMenuEvent &);

    /**
     * @brief Event handler for "reconnect" context menu item.
     */
    void onReconnectContextMenu(wxMenuEvent &);

    /**
     * @brief Event handler for exit command.
     */
    void onExit(wxCommandEvent &);

    /**
     * @brief Event handler for "help" item in window menu.
     */
    void onHelp(wxCommandEvent &);

    /**
     * @brief Event handler for "about" item in window menu.
     */
    void onAbout(wxCommandEvent &);

    /**
     * @brief Open device reset dialog.
     * @param row row of currently selected device in the table
     */
    void openResetDialog(int row);

    /**
     * @brief Open Force IP dialog.
     * @param row row of currently selected device in the table
     */
    void openForceIpDialog(int row);

    /**
     * @brief Open Reconnect dialog.
     * @param row row of currently selected device in the table
     */
    void openReconnectDialog(int row);

    /**
     * @brief Event handler for change of the "only RC cameras" checkbox.
     * @param evt event
     */
    void onOnlyRcCheckbox(wxCommandEvent &evt);

    /**
     * @brief Event handler for change of the filter text box.
     * @param evt event
     */
    void onFilterTextChange(wxCommandEvent &evt);

    /**
     * @brief Updates the device table.
     * @param d discovered cameras
     */
    void updateDeviceList(const std::vector<wxVector<wxVariant>> &d);

    /**
     * @brief Open WebGUI for device in specific row.
     */
    void openWebGUI(int row);

    wxDECLARE_EVENT_TABLE();

  private:
    wxDataViewListCtrl *device_list_;
    wxButton *discover_button_;
    wxTextCtrl *filter_input_;
    wxButton *reset_button_;
    wxButton *force_ip_button_;
    wxButton *reconnect_button_;
    ResetDialog *reset_dialog_;
    ForceIpDialog *force_ip_dialog_;
    ReconnectDialog *reconnect_dialog_;
    AboutDialog *about_dialog_;
    wxAnimation spinner_;
    wxAnimationCtrl *spinner_ctrl_;
    wxHtmlHelpController *help_ctrl_;
    std::unique_ptr<std::pair<int, int>> menu_event_item_;
    bool only_rc_sensors_;
    std::string filter_text_;
    std::vector<wxVector<wxVariant>> last_data_;
};

#endif // DISCOVERFRAME_H
