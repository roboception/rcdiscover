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

#include "reconnect_window.h"
#include "help_window.h"

#include "layout.h"
#include "label.h"

#include "rcdiscover/force_ip.h"

#include <FL/fl_ask.H>

#include <stdexcept>

namespace
{

void changingDeviceCb(Fl_Widget *, void *user_data)
{
  ReconnectWindow *win=reinterpret_cast<ReconnectWindow *>(user_data);
  win->isChangingDevice();
}

void macCb(Fl_Widget *, void *user_data)
{
  ReconnectWindow *win=reinterpret_cast<ReconnectWindow *>(user_data);
  win->update();
}

void reconnectCb(Fl_Widget *, void *user_data)
{
  ReconnectWindow *win=reinterpret_cast<ReconnectWindow *>(user_data);
  win->isReconnect();
}

void helpCb(Fl_Widget *, void *)
{
  HelpWindow::showWindow("reconnect");
}

}

namespace
{

static ReconnectWindow *win=0;

}

ReconnectWindow *ReconnectWindow::showWindow()
{
  if (!win)
  {
    win=new ReconnectWindow();
  }

  win->show();

  return win;
}

void ReconnectWindow::hideWindow()
{
  if (win)
  {
    win->hide();
  }
}

void ReconnectWindow::updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
  const std::string &sel_mac)
{
  device->update(list, sel_mac);
  isChangingDevice();
  update();
}

void ReconnectWindow::update()
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
    reconnect->activate();
  }
  else
  {
    reconnect->deactivate();
  }
}

void ReconnectWindow::isChangingDevice()
{
  mac->value(device->getMAC().c_str());
  update();
}

void ReconnectWindow::isReconnect()
{
  if (mac->isValid())
  {
    try
    {
      rcdiscover::ForceIP force_ip;

      if (fl_choice("Are you sure to reconnect device with MAC-address %s?",
        "No", "Yes", 0, mac->value()) == 1)
      {
        force_ip.sendCommand(mac->getMAC(), 0, 0, 0);
      }

      hide();
    }
    catch(const std::runtime_error &ex)
    {
      fl_alert("%s", ex.what());
    }
  }
}

ReconnectWindow::ReconnectWindow() : Fl_Double_Window(400, 124, "Reconnect device")
{
  int width=400-2*GAP_SIZE;
  int row_height=28;

  new Label(ADD_BELOW_XY, 120, row_height, "rc_visard");
  device=new DeviceChoice(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  device->add("<Custom>");
  device->callback(changingDeviceCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "MAC address");
  mac=new InputMAC(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  mac->setChangeCallback(macCb, this);

  reconnect=new Button(ADD_BELOW_XY, 160, row_height, "Reconnect");
  reconnect->callback(reconnectCb, this);

  help=new Button(width+GAP_SIZE-row_height, addRightY(), row_height, row_height, "?");
  help->callback(helpCb, this);

  checkGroupSize(__func__);
  end();

  size_range(w(), h(), w(), h());

  update();
}
