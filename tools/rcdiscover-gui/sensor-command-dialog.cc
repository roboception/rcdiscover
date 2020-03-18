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
#include "sensor-command-dialog.h"

#include "event-ids.h"
#include "resources.h"

#include "rcdiscover/utils.h"
#include "discover-frame.h"

#include <sstream>

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/combobox.h>
#include <wx/dataview.h>
#include <wx/html/helpctrl.h>
#include <wx/cshelp.h>

SensorCommandDialog::SensorCommandDialog(wxHtmlHelpController *help_ctrl,
                                         wxWindow *parent, wxWindowID id,
                                         std::string title,
                                         const int additional_grid_rows,
                                         const wxPoint &pos,
                                         long style,
                                         const wxString &name) :
  wxDialog(parent, id, std::move(title), pos, wxSize(-1,-1), style, name),
  sensors_(nullptr),
  mac_{{nullptr, nullptr, nullptr, nullptr, nullptr, nullptr}},
  sensor_list_(nullptr),
  help_ctrl_(help_ctrl)
{
  panel_ = new wxPanel(this, -1);
  vbox_ = new wxBoxSizer(wxVERTICAL);

  grid_ = new wxFlexGridSizer(2 + additional_grid_rows, 2, 10, 25);

  auto *sensors_text = new wxStaticText(panel_, wxID_ANY, "rc_visard");
  grid_->Add(sensors_text);

  auto *sensors_box = new wxBoxSizer(wxHORIZONTAL);
  sensors_ = new wxChoice(panel_, ID_Sensor_Combobox);
  sensors_box->Add(sensors_, 1);
  grid_->Add(sensors_box, 1, wxEXPAND);

  auto *mac_text = new wxStaticText(panel_, wxID_ANY, "MAC address");
  grid_->Add(mac_text);

  auto *mac_box = new wxBoxSizer(wxHORIZONTAL);
  int i = 0;
  for (auto& m : mac_)
  {
    if (i > 0)
    {
      mac_box->Add(new wxStaticText(panel_, ID_MAC_Textbox, ":"));
    }
    m = new wxTextCtrl(panel_, wxID_ANY, wxEmptyString,
                       wxDefaultPosition, wxSize(35, -1));
    mac_box->Add(m, 1);
    ++i;
  }
  grid_->Add(mac_box, 1, wxEXPAND);

  vbox_->Add(grid_, 0, wxALL | wxEXPAND, 15);

  panel_->SetSizer(vbox_);

  Connect(ID_Sensor_Combobox,
          wxEVT_CHOICE,
          wxCommandEventHandler(SensorCommandDialog::onSensorSelected));
}

void SensorCommandDialog::setDiscoveredSensors(
    const wxDataViewListModel *sensor_list,
    const std::vector<bool>& show)
{
  sensor_list_ = sensor_list;

  if (sensor_list != nullptr)
  {
    sensors_->Clear();

    sensors_->Append("<Custom>");

    const auto rows = sensor_list->GetCount();
    unsigned int sensors_row = 0;
    for (typename std::decay<decltype(rows)>::type i = 0; i < rows; ++i)
    {
      if (show.empty() || show[i])
      {
        wxVariant hostname{};
        wxVariant mac{};
        sensor_list->GetValueByRow(hostname, i, DiscoverFrame::NAME);
        sensor_list->GetValueByRow(mac, i, DiscoverFrame::MAC);
        const auto s = wxString::Format("%s - %s", hostname.GetString(), mac.GetString());
        sensors_->Append(s);
        row_map_.emplace(i, sensors_row + 1);
        row_map_inv_.emplace(sensors_row + 1, i);
        ++sensors_row;
      }
    }
  }

  clear();
}

void SensorCommandDialog::setActiveSensor(const unsigned int row)
{
  clear();

  const auto found = row_map_.find(static_cast<int>(row));
  if (found != row_map_.cend())
  {
    sensors_->Select(found->second);
    fillMac();
  }
  else
  {
    sensors_->Select(0);
  }
}

wxBoxSizer *SensorCommandDialog::getVerticalBox()
{
  return vbox_;
}

wxPanel *SensorCommandDialog::getPanel()
{
  return panel_;
}

wxFlexGridSizer *SensorCommandDialog::getGrid()
{
  return grid_;
}

std::array<uint8_t, 6> SensorCommandDialog::getMac() const
{
  std::array<uint8_t, 6> mac;
  for (uint8_t i = 0; i < 6; ++i)
  {
    const auto s = mac_[i]->GetValue().ToStdString();

    try
    {
      const auto v = std::stoul(s, nullptr, 16);
      if (v > 0xff)
      {
        throw std::invalid_argument("");
      }
      mac[i] = static_cast<uint8_t>(v);
    }
    catch(const std::invalid_argument&)
    {
      throw std::runtime_error(
            std::string("Each MAC address segment must contain ") +
            "a hex value ranging from 0x00 to 0xff.");
    }
  }
  return mac;
}

std::string SensorCommandDialog::getMacString() const
{
  const auto mac = getMac();

  std::ostringstream mac_string;
  bool first = true;
  for (const auto m : mac)
  {
    if (!first)
    {
      mac_string << ":";
    }
    mac_string << std::hex << std::setfill('0') << std::setw(2);
    mac_string << static_cast<unsigned int>(m);
    first = false;
  }
  return mac_string.str();
}

void SensorCommandDialog::displayHelp(const std::string &section)
{
  const std::string url = std::string("help.htm#") + section;
  help_ctrl_->Display(url);

  // need second call otherwise it does not jump to the section if the
  // help is displayed the first time
  help_ctrl_->Display(url);
}

void SensorCommandDialog::clear()
{
  clearMac();
}

void SensorCommandDialog::onSensorSelected(wxCommandEvent &)
{
  if (sensors_->GetSelection() != wxNOT_FOUND)
  {
    if (sensors_->GetSelection() == 0)
    {
      clearMac();
    }
    else
    {
      fillMac();
    }
  }
}

void SensorCommandDialog::fillMac()
{
  const int row = sensors_->GetSelection();

  if (row == wxNOT_FOUND)
  {
    return;
  }

  wxVariant mac_string{};
  sensor_list_->GetValueByRow(mac_string,
                              row_map_inv_.at(static_cast<unsigned int>(row)),
                              DiscoverFrame::MAC);

  const auto mac = split<6>(mac_string.GetString().ToStdString(), ':');

  for (uint8_t i = 0; i < 6; ++i)
  {
    mac_[i]->ChangeValue(mac[i]);
    mac_[i]->SetEditable(false);
  }
}

void SensorCommandDialog::clearMac()
{
  sensors_->SetSelection(0);

  for (uint8_t i = 0; i < 6; ++i)
  {
    mac_[i]->Clear();
    mac_[i]->SetEditable(true);
  }
}

BEGIN_EVENT_TABLE(SensorCommandDialog, wxDialog)
END_EVENT_TABLE()
