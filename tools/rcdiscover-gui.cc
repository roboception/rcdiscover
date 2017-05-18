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

#include "rcdiscover/discover.h"
#include "rcdiscover/deviceinfo.h"
#include "utils.h"

#include <vector>
#include <algorithm>

#include <wx/wx.h> // wxWidgets core
#include <wx/dataview.h> // wxDataViewListCtrl
#include <wx/mstream.h> // wxMemoryInputStream
#include <wx/animate.h> // wxAnimation

#include "resources/logo_128.xpm"
#include "resources/logo_32_rotate.h"

wxDEFINE_EVENT(wxEVT_COMMAND_DISCOVERY_COMPLETED, wxThreadEvent);

enum
{
  ID_DiscoverButton = wxID_HIGHEST + 1,
  ID_DataViewListCtrl
};

class DiscoverThread : public wxThread
{
  public:
    DiscoverThread(wxFrame *parent) :
      parent_(parent)
    { }

  protected:
    virtual ExitCode Entry() override
    {
      ExitCode exit_code = (ExitCode)0;

      std::vector<wxVector<wxVariant>> device_list;

      try
      {
#ifdef WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

        rcdiscover::Discover discover;
        discover.broadcastRequest();

        rcdiscover::DeviceInfo info;

        while (discover.getResponse(info))
        {
          std::string name=info.getUserName();

          if (name.size() == 0)
          {
            name="rc_visard";
          }

          wxVector<wxVariant> data;
          data.push_back(wxVariant(name));
          data.push_back(wxVariant(info.getSerialNumber()));
          data.push_back(wxVariant(ip2string(info.getIP())));
          data.push_back(wxVariant(mac2string(info.getMAC())));

          device_list.push_back(std::move(data));
        }

        std::sort(device_list.begin(), device_list.end(),
                  [](const wxVector<wxVariant>& lhs, const wxVector<wxVariant>& rhs) {
          for (size_t i = 0; i < lhs.size(); ++i)
          {
            if (lhs[i].GetString() != rhs[i].GetString())
            {
              return lhs[i].GetString() < rhs[i].GetString();
            }
          }
          return false;
        });

        auto it = std::unique(device_list.begin(), device_list.end(),
                              [](const wxVector<wxVariant>& lhs, const wxVector<wxVariant>& rhs){
          for (size_t i = 0; i < lhs.size(); ++i)
          {
            if (lhs[i] != rhs[i])
            {
              return false;
            }
          }
          return true;
        });
        device_list.erase(it, device_list.end());

#ifdef WIN32
        ::WSACleanup();
#endif
      }
      catch(const std::exception& ex)
      {
        std::cerr << ex.what() << std::endl;
        device_list.clear();
        exit_code = (ExitCode)1;
      }

      wxThreadEvent event(wxEVT_COMMAND_DISCOVERY_COMPLETED);
      event.SetPayload(device_list);
      parent_->GetEventHandler()->AddPendingEvent(event);

      return exit_code;
    }

  private:
    wxFrame *parent_;
};

class RcDiscoverFrame : public wxFrame
{
  public:
    RcDiscoverFrame(const wxString& title,
                    const wxPoint& pos) :
      wxFrame(NULL, wxID_ANY, title, pos, wxSize(550,350)),
      device_list_(nullptr),
      discover_button_(nullptr)
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
      discover_button_ = new wxButton(panel, ID_DiscoverButton, "Start Discovery");
      button_box->Add(discover_button_, 1);

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
              wxCommandEventHandler(RcDiscoverFrame::onDiscoverButton));
      Connect(wxID_ANY,
              wxEVT_COMMAND_DISCOVERY_COMPLETED,
              wxThreadEventHandler(RcDiscoverFrame::onDiscoveryCompleted));
      Connect(ID_DataViewListCtrl,
              wxEVT_DATAVIEW_ITEM_ACTIVATED,
              wxDataViewEventHandler(RcDiscoverFrame::onDeviceDoubleClick));
    }

  private:
    void onDiscoverButton(wxCommandEvent&)
    {
      discover_button_->Disable();
      spinner_ctrl_->Play();

      auto *thread = new DiscoverThread(this);
      if (thread->Run() != wxTHREAD_NO_ERROR)
      {
        std::cerr << "Could not spawn thread" << std::endl;
        delete thread;
        thread = nullptr;
        discover_button_->Enable();
      }
    }

    void onDiscoveryCompleted(wxThreadEvent& event)
    {
      device_list_->DeleteAllItems();

      std::vector<wxVector<wxVariant>> data =
          event.GetPayload<std::vector<wxVector<wxVariant>>>();
      for(const auto& d : data)
      {
        device_list_->AppendItem(d);
      }

      spinner_ctrl_->Stop();
      discover_button_->Enable();
    }

    void onDeviceDoubleClick(wxDataViewEvent& event)
    {
      const auto item = event.GetItem();
      const auto row = device_list_->ItemToRow(item);
      const auto ip = device_list_->GetTextValue(row, 2);
      wxLaunchDefaultBrowser("http://" + ip + "/");
    }

    void onExit(wxCommandEvent& event)
    {
      Close(true);
    }

    void onAbout(wxCommandEvent&)
    {
      wxMessageBox("This is rc_discover", "About rc_discover",
                   wxOK | wxICON_INFORMATION);
    }

    wxDECLARE_EVENT_TABLE();

  private:
    wxDataViewListCtrl *device_list_;
    wxButton *discover_button_;
    wxAnimation spinner_;
    wxAnimationCtrl *spinner_ctrl_;
};

wxBEGIN_EVENT_TABLE(RcDiscoverFrame, wxFrame)
  EVT_MENU(wxID_EXIT, RcDiscoverFrame::onExit)
  EVT_MENU(wxID_ABOUT, RcDiscoverFrame::onAbout)
wxEND_EVENT_TABLE()

class RcDiscoverApp : public wxApp
{
  public:
    virtual bool OnInit() override
    {
      auto *frame = new RcDiscoverFrame("rc_discover", wxPoint(50,50));
      frame->Show(true);
      return true;
    }
};

wxIMPLEMENT_APP(RcDiscoverApp);
