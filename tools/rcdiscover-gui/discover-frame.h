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

#ifndef DISCOVERFRAME_H
#define DISCOVERFRAME_H

#include <memory>

#include <wx/frame.h>
#include <wx/animate.h>

class wxDataViewListCtrl;
class wxButton;
class wxDataViewEvent;
class wxPanel;
class ResetDialog;
class AboutDialog;
class wxHtmlHelpController;

/**
 * @brief Main window in which the table of discovered rc_visards is displayed.
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
     * @brief Event handler for completed rc_visard discovery.
     * @param event event
     */
    void onDiscoveryCompleted(wxThreadEvent &event);

    /**
     * @brief Event handler for help button.
     */
    void onHelpDiscovery(wxCommandEvent &);

    /**
     * @brief Event handler for erroneous rc_visard discovery.
     * @param event event
     */
    void onDiscoveryError(wxThreadEvent &event);

    /**
     * @brief Event handler for Reset button click.
     */
    void onResetButton(wxCommandEvent &);

    /**
     * @brief Event handler for double click on an rc_visard.
     * @param event event
     */
    void onDeviceDoubleClick(wxDataViewEvent &event);

    /**
     * @brief Event handler for right mouse button click on rc_visard.
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
     * @brief Open rc_visard reset dialog.
     * @param row row of currently selected rc_visard in the table
     */
    void openResetDialog(int row);

    wxDECLARE_EVENT_TABLE();

  private:
    wxDataViewListCtrl *device_list_;
    wxButton *discover_button_;
    wxButton *reset_button_;
    ResetDialog *reset_dialog_;
    AboutDialog *about_dialog_;
    wxAnimation spinner_;
    wxAnimationCtrl *spinner_ctrl_;
    wxHtmlHelpController *help_ctrl_;
    std::unique_ptr<std::pair<int, int>> menu_event_item_;
};

#endif // DISCOVERFRAME_H
