/*
 * Roboception GmbH
 * Munich, Germany
 * www.roboception.com
 *
 * Copyright (c) 2024 Roboception GmbH
 * All rights reserved
 *
 * Author: Heiko Hirschmueller
 */

#include "reset_window.h"
#include "help_window.h"

#include "layout.h"
#include "label.h"

#include "rcdiscover/wol.h"
#include "rcdiscover/wol_exception.h"
#include "rcdiscover/operation_not_permitted.h"

#include <FL/fl_ask.H>

#include <sstream>

namespace
{

void changingDeviceCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->isChangingDevice();
}

void macCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->update();
}

void resetParametersCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->isResetParameters();
}

void resetNetworkCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->isResetNetwork();
}

void resetAllCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->isResetAll();
}

void switchPartitionsCb(Fl_Widget *, void *user_data)
{
  ResetWindow *win=reinterpret_cast<ResetWindow *>(user_data);
  win->isSwitchPartitions();
}

void helpCb(Fl_Widget *, void *)
{
  HelpWindow::showWindow("reset");
}

}

namespace
{

static ResetWindow *win=0;

}

ResetWindow *ResetWindow::showWindow()
{
  if (!win)
  {
    win=new ResetWindow();
  }

  win->show();

  return win;
}

void ResetWindow::hideWindow()
{
  if (win)
  {
    win->hide();
  }
}

void ResetWindow::updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
  const std::string &sel_mac)
{
  device->update(list, sel_mac);
  isChangingDevice();
  update();
}

void ResetWindow::update()
{
  std::string v=device->getMAC();

  if (v.size() == 0)
  {
    mac->activate();
  }
  else
  {
    mac->deactivate();
  }

  if (mac->isValid())
  {
    reset_parameters->activate();
    reset_network->activate();
    reset_all->activate();
    switch_partitions->activate();
  }
  else
  {
    reset_parameters->deactivate();
    reset_network->deactivate();
    reset_all->deactivate();
    switch_partitions->deactivate();
  }
}

void ResetWindow::isChangingDevice()
{
  mac->value(device->getMAC().c_str());
  update();
}

namespace
{

void sendUDPRequest(const char *mac_string, uint64_t mac_value, const char *func_name,
  uint8_t func_id)
{
  if (mac_value != 0)
  {
    try
    {
      std::array<uint8_t, 6> mac;

      mac[0]=static_cast<uint8_t>(mac_value>>40);
      mac[1]=static_cast<uint8_t>(mac_value>>32);
      mac[2]=static_cast<uint8_t>(mac_value>>24);
      mac[3]=static_cast<uint8_t>(mac_value>>16);
      mac[4]=static_cast<uint8_t>(mac_value>>8);
      mac[5]=static_cast<uint8_t>(mac_value);

      rcdiscover::WOL wol(mac, 9);

      std::ostringstream reset_check_str;
      reset_check_str << "Are you sure to " << func_name <<
             " of rc_visard with MAC-address " << mac_string << "?";
      int answer=fl_choice_n("%s", "No", "Yes", 0, reset_check_str.str().c_str());

      while (answer == 1)
      {
        wol.send({{0xEE, 0xEE, 0xEE, func_id}});
        answer=fl_choice_n("Please check whether rc_visard's LED turned white and whether rc_visard is rebooting.",
          "Close", "Try again", 0);
      }
    }
    catch (const rcdiscover::OperationNotPermitted&)
    {
      fl_alert("rcdiscover probably requires root/admin privileges for this operation.");
    }
    catch (const std::runtime_error& ex)
    {
      fl_alert("%s", ex.what());
    }
  }
}

}

void ResetWindow::isResetParameters()
{
  sendUDPRequest(mac->value(), mac->getMAC(), "reset parameters", 0xAA);
  hide();
}

void ResetWindow::isResetNetwork()
{
  sendUDPRequest(mac->value(), mac->getMAC(), "reset network parameters", 0xBB);
  hide();
}

void ResetWindow::isResetAll()
{
  sendUDPRequest(mac->value(), mac->getMAC(), "reset all", 0xFF);
  hide();
}

void ResetWindow::isSwitchPartitions()
{
  sendUDPRequest(mac->value(), mac->getMAC(), "switch partition", 0xCC);
  hide();
}

ResetWindow::ResetWindow() : Fl_Double_Window(628, 124, "Reset rc_visard")
{
  int width=628-2*GAP_SIZE;
  int row_height=28;

  new Label(ADD_BELOW_XY, 120, row_height, "rc_visard");
  device=new DeviceChoice(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  device->add("<Custom>");
  device->value(0);
  device->callback(changingDeviceCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "MAC address");
  mac=new InputMAC(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  mac->setChangeCallback(macCb, this);

  reset_parameters=new Button(ADD_BELOW_XY, 160, row_height, "Reset parameters");
  reset_parameters->callback(resetParametersCb, this);

  reset_network=new Button(ADD_RIGHT_XY, 140, row_height, "Reset network");
  reset_network->callback(resetNetworkCb, this);

  reset_all=new Button(ADD_RIGHT_XY, 100, row_height, "Reset all");
  reset_all->callback(resetAllCb, this);

  switch_partitions=new Button(ADD_RIGHT_XY, 140, row_height, "Switch partitions");
  switch_partitions->callback(switchPartitionsCb, this);

  help=new Button(ADD_RIGHT_XY, row_height, row_height, "?");
  help->callback(helpCb, this);

  checkGroupSize(__func__);
  end();

  size_range(w(), h(), w(), h());

  update();
}
