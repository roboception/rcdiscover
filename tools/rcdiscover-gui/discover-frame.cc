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

#include "discover-frame.h"

#include "event_ids.h"
#include "discover-thread.h"
#include "reset-dialog.h"

#include "rcdiscover/wol.h"

#include <memory>

#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/button.h>
#include <wx/animate.h>
#include <wx/mstream.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/clipbrd.h>
#include <wx/dc.h>
#include <wx/msgdlg.h>

#include "resources/logo_128.xpm"
#include "resources/logo_32_rotate.h"



DiscoverFrame::DiscoverFrame(const wxString& title,
                const wxPoint& pos) :
  wxFrame(NULL, wxID_ANY, title, pos, wxSize(550,350)),
  device_list_(nullptr),
  discover_button_(nullptr),
  reset_button_(nullptr),
  reset_dialog_(nullptr),
  menu_event_item_(nullptr)
{
  wxIcon icon_128(logo_128_xpm);
  SetIcon(icon_128);

  wxMemoryInputStream gif_stream(logo_32_rotate_gif, sizeof(logo_32_rotate_gif));
  spinner_.Load(gif_stream, wxANIMATION_TYPE_GIF);

  wxMenu *menuFile = new wxMenu();
  menuFile->Append(wxID_EXIT);

  wxMenu *menuHelp = new wxMenu();
  menuHelp->Append(wxID_ABOUT);

  wxMenuBar *menuBar = new wxMenuBar();
  menuBar->Append(menuFile, "&File");
  menuBar->Append(menuHelp, "&Help");

  SetMenuBar(menuBar);
  CreateStatusBar();

  auto *panel = new wxPanel(this, -1);

  auto *vbox = new wxBoxSizer(wxVERTICAL);

  auto *button_box = new wxBoxSizer(wxHORIZONTAL);
  discover_button_ = new wxButton(panel, ID_DiscoverButton, "Rerun Discovery");
  button_box->Add(discover_button_, 1);
  reset_button_ = new wxButton(panel, ID_ResetButton, "Reset rc_visard");
  button_box->Add(reset_button_, 1);

  button_box->Add(-1, 0, wxEXPAND);

  spinner_ctrl_ = new wxAnimationCtrl(panel, wxID_ANY, spinner_, wxPoint(-1,-1), wxSize(32,32));
  button_box->Add(spinner_ctrl_, 0);

  vbox->Add(button_box, 0, wxALL, 10);

  auto *data_box = new wxBoxSizer(wxHORIZONTAL);

  device_list_ = new wxDataViewListCtrl(panel,
                                        ID_DataViewListCtrl,
                                        wxPoint(-1,-1),
                                        wxSize(-1,-1));
  device_list_->AppendTextColumn("Hostname",
                                 wxDATAVIEW_CELL_INERT,
                                 100, wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
  device_list_->AppendTextColumn("Serial Number",
                                 wxDATAVIEW_CELL_INERT,
                                 150, wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
  device_list_->AppendTextColumn("IP Address",
                                 wxDATAVIEW_CELL_INERT,
                                 100, wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);
  device_list_->AppendTextColumn("MAC Address",
                                 wxDATAVIEW_CELL_INERT,
                                 130, wxALIGN_LEFT,
                                 wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_SORTABLE);

  device_list_->SetToolTip("Double-click row to open WebGUI in browser.");

  data_box->Add(device_list_, 1, wxEXPAND);

  vbox->Add(data_box, 1, wxLEFT | wxRIGHT | wxEXPAND, 10);

  panel->SetSizer(vbox);
  Centre();

  Connect(ID_DiscoverButton,
          wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(DiscoverFrame::onDiscoverButton));
  Connect(wxID_ANY,
          wxEVT_COMMAND_DISCOVERY_COMPLETED,
          wxThreadEventHandler(DiscoverFrame::onDiscoveryCompleted));
  Connect(wxID_ANY,
          wxEVT_COMMAND_DISCOVERY_ERROR,
          wxThreadEventHandler(DiscoverFrame::onDiscoveryError));
  Connect(ID_ResetButton,
          wxEVT_COMMAND_BUTTON_CLICKED,
          wxCommandEventHandler(DiscoverFrame::onResetButton));
  Connect(ID_DataViewListCtrl,
          wxEVT_DATAVIEW_ITEM_ACTIVATED,
          wxDataViewEventHandler(DiscoverFrame::onDeviceDoubleClick));
  Connect(ID_DataViewListCtrl,
          wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
          wxDataViewEventHandler(DiscoverFrame::onDataViewContextMenu));
  Connect(ID_OpenWebGUI,
          wxEVT_MENU,
          wxMenuEventHandler(DiscoverFrame::onOpenWebGUI));
  Connect(wxCOPY,
          wxEVT_MENU,
          wxMenuEventHandler(DiscoverFrame::onCopy));
  Connect(ID_ResetButton,
          wxEVT_MENU,
          wxMenuEventHandler(DiscoverFrame::onResetContextMenu));
  Connect(wxID_EXIT,
          wxEVT_MENU,
          wxCommandEventHandler(DiscoverFrame::onExit));
  Connect(wxID_ABOUT,
          wxEVT_MENU,
          wxCommandEventHandler(DiscoverFrame::onAbout));

  reset_dialog_ = new ResetDialog(panel, wxID_ANY, "Reset rc_visard");

  // start discovery on startup
  wxCommandEvent evt;
  onDiscoverButton(evt);
}

void DiscoverFrame::setBusy()
{
  discover_button_->Disable();
  reset_button_->Disable();
  spinner_ctrl_->Play();
}

void DiscoverFrame::clearBusy()
{
  discover_button_->Enable();
  reset_button_->Enable();
  spinner_ctrl_->Stop();
}

void DiscoverFrame::onDiscoverButton(wxCommandEvent&)
{
  setBusy();

  auto *thread = new DiscoverThread(this);
  if (thread->Run() != wxTHREAD_NO_ERROR)
  {
    std::cerr << "Could not spawn thread" << std::endl;
    delete thread;
    thread = nullptr;

    clearBusy();
  }
}

void DiscoverFrame::onDiscoveryCompleted(wxThreadEvent& event)
{
  device_list_->DeleteAllItems();

  std::vector<wxVector<wxVariant>> data =
      event.GetPayload<std::vector<wxVector<wxVariant>>>();
  for(const auto& d : data)
  {
    device_list_->AppendItem(d);
  }

  clearBusy();
}

void DiscoverFrame::onDiscoveryError(wxThreadEvent& event)
{
  wxString error = event.GetPayload<wxString>();
  std::ostringstream oss;
  oss << "An error occurred during discovery: " << error;
  wxMessageBox(oss.str(), "Error", wxOK | wxICON_ERROR);
}

void DiscoverFrame::onResetButton(wxCommandEvent& event)
{
  openResetDialog(device_list_->GetSelectedRow());
}

void DiscoverFrame::onDeviceDoubleClick(wxDataViewEvent& event)
{
  const auto item = event.GetItem();
  const auto row = device_list_->ItemToRow(item);

  const auto ip_wxstring = device_list_->GetTextValue(row, 2);
  wxLaunchDefaultBrowser("http://" + ip_wxstring + "/");
}

void DiscoverFrame::onDataViewContextMenu(wxDataViewEvent& event)
{
  menu_event_item_.reset(new std::pair<int, int>(
                           device_list_->ItemToRow(event.GetItem()),
                           event.GetColumn()));

  if (menu_event_item_->first < 0)
  {
    return;
  }

  wxMenu menu;

  if (menu_event_item_->second >= 0)
  {
    menu.Append(wxCOPY, "&Copy");
    menu.AppendSeparator();
  }
  menu.Append(ID_OpenWebGUI, "Open &WebGUI");
  menu.AppendSeparator();
  menu.Append(ID_ResetButton, "Reset");

  PopupMenu(&menu);
}

void DiscoverFrame::onCopy(wxMenuEvent& event)
{
  if (!menu_event_item_ ||
      menu_event_item_->first < 0 ||
      menu_event_item_->second < 0)
  {
    return;
  }

  const auto row = menu_event_item_->first;
  const auto cell = device_list_->GetTextValue(row, menu_event_item_->second);

  if (wxTheClipboard->Open())
  {
    wxTheClipboard->SetData(new wxTextDataObject(cell));
    wxTheClipboard->Close();
  }
}

void DiscoverFrame::onOpenWebGUI(wxMenuEvent& event)
{
  if (!menu_event_item_ ||
      menu_event_item_->first < 0)
  {
    return;
  }

  const auto ip_wxstring = device_list_->GetTextValue(menu_event_item_->first, 2);
  wxLaunchDefaultBrowser("http://" + ip_wxstring + "/");
}

void DiscoverFrame::onResetContextMenu(wxMenuEvent&)
{
  if (!menu_event_item_ ||
      menu_event_item_->first < 0)
  {
    return;
  }

  openResetDialog(menu_event_item_->first);
}

void DiscoverFrame::onExit(wxCommandEvent& event)
{
  Close(true);
}

void DiscoverFrame::onAbout(wxCommandEvent&)
{
  wxMessageBox("This is rc_discover", "About rc_discover",
               wxOK | wxICON_INFORMATION);
}

void DiscoverFrame::openResetDialog(const int row)
{
  reset_dialog_->setDiscoveredSensors(device_list_->GetStore());

  if (row != wxNOT_FOUND)
  {
    reset_dialog_->setActiveSensor(row);
  }

  reset_dialog_->ShowModal();
  reset_dialog_->setDiscoveredSensors(nullptr);
}

BEGIN_EVENT_TABLE(DiscoverFrame, wxFrame)
END_EVENT_TABLE()
