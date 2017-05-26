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

class DiscoverFrame : public wxFrame
{
  public:
    DiscoverFrame(const wxString& title,
                    const wxPoint& pos);

  private:
    void setBusy();
    void clearBusy();

    void onDiscoverButton(wxCommandEvent&);
    void onDiscoveryCompleted(wxThreadEvent& event);
    void onDiscoveryError(wxThreadEvent& event);

    void onResetButton(wxCommandEvent& event);

    void onDeviceDoubleClick(wxDataViewEvent& event);
    void onDataViewContextMenu(wxDataViewEvent& event);
    void onCopy(wxMenuEvent& event);
    void onOpenWebGUI(wxMenuEvent& event);
    void onResetContextMenu(wxMenuEvent& event);

    void onExit(wxCommandEvent& event);
    void onAbout(wxCommandEvent&);

    void openResetDialog(int row);

    wxDECLARE_EVENT_TABLE();

  private:
    wxDataViewListCtrl *device_list_;
    wxButton *discover_button_;
    wxButton *reset_button_;
    ResetDialog *reset_dialog_;
    wxAnimation spinner_;
    wxAnimationCtrl *spinner_ctrl_;
    std::unique_ptr<std::pair<int, int>> menu_event_item_;
};

#endif // DISCOVERFRAME_H