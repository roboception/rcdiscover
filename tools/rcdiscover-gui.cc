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
#include "rcdiscover/wol_exception.h"

#ifdef WIN32
#include "rcdiscover/wol_windows.h"
#else
#include "rcdiscover/wol_linux.h"
#endif

#include "utils.h"

#include <vector>
#include <algorithm>

#include <wx/wx.h> // wxWidgets core
#include <wx/dataview.h> // wxDataViewListCtrl
#include <wx/mstream.h> // wxMemoryInputStream
#include <wx/animate.h> // wxAnimation
#include <wx/clipbrd.h> // clipboard

#include "resources/logo_128.xpm"
#include "resources/logo_32_rotate.h"

wxDEFINE_EVENT(wxEVT_COMMAND_DISCOVERY_COMPLETED, wxThreadEvent);

#ifdef WIN32
typedef rcdiscover::WOL_Windows WOL;
#else
typedef rcdiscover::WOL_Linux WOL;
#endif

enum
{
  ID_DiscoverButton = wxID_HIGHEST + 1,
  ID_ResetButton,
  ID_DataViewListCtrl,
  ID_OpenWebGUI,
  ID_Reset_Params,
  ID_Reset_GigE,
  ID_Reset_All,
  ID_Switch_Partition
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
      discover_button_(nullptr),
      reset_button_(nullptr),
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
              wxCommandEventHandler(RcDiscoverFrame::onDiscoverButton));
      Connect(wxID_ANY,
              wxEVT_COMMAND_DISCOVERY_COMPLETED,
              wxThreadEventHandler(RcDiscoverFrame::onDiscoveryCompleted));
      Connect(ID_DataViewListCtrl,
              wxEVT_DATAVIEW_ITEM_ACTIVATED,
              wxDataViewEventHandler(RcDiscoverFrame::onDeviceDoubleClick));
      Connect(ID_DataViewListCtrl,
              wxEVT_DATAVIEW_ITEM_CONTEXT_MENU,
              wxDataViewEventHandler(RcDiscoverFrame::onDataViewContextMenu));
      Connect(ID_OpenWebGUI,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onOpenWebGUI));
      Connect(wxCOPY,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onCopy));
      Connect(ID_Reset_Params,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onReset));
      Connect(ID_Reset_GigE,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onReset));
      Connect(ID_Reset_All,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onReset));
      Connect(ID_Switch_Partition,
              wxEVT_MENU,
              wxMenuEventHandler(RcDiscoverFrame::onReset));

      // start discovery on startup
      wxCommandEvent evt;
      onDiscoverButton(evt);
    }

  private:
    void setBusy()
    {
      discover_button_->Disable();
      reset_button_->Disable();
      spinner_ctrl_->Play();
    }

    void clearBusy()
    {
      discover_button_->Enable();
      reset_button_->Enable();
      spinner_ctrl_->Stop();
    }

    void onDiscoverButton(wxCommandEvent&)
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

    void onDiscoveryCompleted(wxThreadEvent& event)
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

    void onDeviceDoubleClick(wxDataViewEvent& event)
    {
      const auto item = event.GetItem();
      const auto row = device_list_->ItemToRow(item);

      const auto ip_wxstring = device_list_->GetTextValue(row, 2);
      wxLaunchDefaultBrowser("http://" + ip_wxstring + "/");
    }

    void onDataViewContextMenu(wxDataViewEvent& event)
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
      menu.Append(ID_Reset_Params, "Reset &parameters");
      menu.Append(ID_Reset_GigE, "Reset &GigE");
      menu.Append(ID_Reset_All, "Reset &all");
      menu.Append(ID_Switch_Partition, "&Switch partition");

      PopupMenu(&menu);
    }

    void onCopy(wxMenuEvent& event)
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

    void onOpenWebGUI(wxMenuEvent& event)
    {
      if (!menu_event_item_ ||
          menu_event_item_->first < 0)
      {
        return;
      }

      const auto ip_wxstring = device_list_->GetTextValue(menu_event_item_->first, 2);
      wxLaunchDefaultBrowser("http://" + ip_wxstring + "/");
    }

    void onReset(wxMenuEvent& event)
    {
      if (!menu_event_item_ ||
          menu_event_item_->first < 0)
      {
        return;
      }

      const auto ip_wxstring = device_list_->GetTextValue(menu_event_item_->first, 2);

      std::array<uint8_t, 4> ip;
      {
        std::string ip_string(ip_wxstring.ToStdString());
        std::stringstream ip_stream(ip_string);
        std::string ip_segment;

        int i = 0;
        while(std::getline(ip_stream, ip_segment, '.'))
        {
          ip[i] = std::stoi(ip_segment);
          ++i;
        }
      }

      const auto mac_wxstring = device_list_->GetTextValue(menu_event_item_->first, 3);

      std::array<uint8_t, 6> mac;
      {
        std::string mac_string(mac_wxstring.ToStdString());
        std::stringstream mac_stream(mac_string);
        std::string mac_segment;

        int i = 0;
        while(std::getline(mac_stream, mac_segment, ':'))
        {
          mac[i] = std::stoul(mac_segment, nullptr, 16);
          ++i;
        }
      }

      try
      {
        std::string func_name("");
        uint8_t func_id(0);

        switch(event.GetId())
        {
          case ID_Reset_Params:
            {
              func_name = "reset parameters";
              func_id = 0xAA;
            }
            break;

          case ID_Reset_GigE:
            {
              func_name = "reset GigE";
              func_id = 0xBB;
            }
            break;

          case ID_Reset_All:
            {
              func_name = "reset all";
              func_id = 0xFF;
            }
            break;

          case ID_Switch_Partition:
            {
              func_name = "reset partition";
              func_id = 0xCC;
            }
            break;

          default:
            throw std::runtime_error("Unknown event ID");
        }

        std::ostringstream oss;
        oss << "Are you sure to " << func_name << " of rc_visard with MAC-address " << mac_wxstring << "?";
        const int answer = wxMessageBox(oss.str(), "", wxYES_NO);

        if (answer == wxYES)
        {
          try
          {
            WOL wol(mac);
            wol.enableUDP(ip, 9);
            wol.send({0xEE, 0xEE, 0xEE, func_id});
          }
          catch(const rcdiscover::WOLException& ex)
          {
            wxMessageBox(ex.what(), "An error occurred", wxOK | wxICON_ERROR);
          }
        }
      }
      catch(const rcdiscover::OperationNotPermitted&)
      {
        wxMessageBox("rc_discovery probably requires root/admin privileges for this operation.",
                     "Operation not permitted",
                     wxOK | wxICON_ERROR);
      }
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
    wxButton *reset_button_;
    wxAnimation spinner_;
    wxAnimationCtrl *spinner_ctrl_;
    std::unique_ptr<std::pair<int, int>> menu_event_item_;
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
#ifdef WIN32
      ::WSADATA wsaData;
      int result;
      if ((result = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
      {
        std::ostringstream oss;
        oss << "WSAStartup failed: " << result;
        wxMessageBox(oss.str(), "Error", wxOK | wxICON_ERROR);
      }
#endif

      auto *frame = new RcDiscoverFrame("rc_discover", wxPoint(50,50));
      frame->Show(true);
      return true;
    }

    virtual int OnExit() override
    {
#ifdef WIN32
      ::WSACleanup();
#endif

      return 0;
    }
};

wxIMPLEMENT_APP(RcDiscoverApp);
