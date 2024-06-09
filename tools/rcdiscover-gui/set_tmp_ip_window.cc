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

#include "set_tmp_ip_window.h"
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
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->isChangingDevice();
}

void ipCb(Fl_Widget *, void *user_data)
{
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->doIP();
}

void subnetMaskCb(Fl_Widget *, void *user_data)
{
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->doSubnetMask();
}

void updateCb(Fl_Widget *, void *user_data)
{
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->update();
}

void setCb(Fl_Widget *, void *user_data)
{
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->isSet();
}

void clearCb(Fl_Widget *, void *user_data)
{
  SetTmpIPWindow *win=reinterpret_cast<SetTmpIPWindow *>(user_data);
  win->isClear();
}

void helpCb(Fl_Widget *, void *)
{
  HelpWindow::showWindow("forceip");
}

}

namespace
{

static SetTmpIPWindow *win=0;

}

SetTmpIPWindow *SetTmpIPWindow::showWindow()
{
  if (!win)
  {
    win=new SetTmpIPWindow();
  }

  win->show();

  return win;
}

void SetTmpIPWindow::hideWindow()
{
  if (win)
  {
    win->hide();
  }
}

void SetTmpIPWindow::updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
  const std::string &sel_mac)
{
  device->update(list, sel_mac);
  isChangingDevice();
  update();
}

void SetTmpIPWindow::update()
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

  if (mac->isValid() && ip->isValid() && subnet_mask->isValid() && default_gateway->isValid())
  {
    set_ip->activate();
  }
  else
  {
    set_ip->deactivate();
  }
}

void SetTmpIPWindow::isChangingDevice()
{
  mac->value(device->getMAC().c_str());
  update();
}

void SetTmpIPWindow::doIP()
{
  if (ip->isValid() && !subnet_mask->isValid())
  {
    uint32_t v=ip->getIP();

    if ((v>>24) == 10) // 10.0.0.0/8 addresses
    {
      subnet_mask->value("255.0.0.0");
    }

    if ((v>>24) == 172 && (static_cast<uint8_t>(v>>16)&16) != 0) // 172.16.0.0/12 addresses
    {
      subnet_mask->value("255.240.0.0");
    }

    if ((v>>24) == 192 && static_cast<uint8_t>(v>>16) == 168) // 192.168.0.0/16 addresses
    {
      subnet_mask->value("255.255.0.0");
    }

    if ((v>>24) == 169 && static_cast<uint8_t>(v>>16) == 254) // 169.254.0.0/16 addresses
    {
      subnet_mask->value("255.255.0.0");
    }
  }

  doSubnetMask();
}

void SetTmpIPWindow::doSubnetMask()
{
  if (ip->isValid() && subnet_mask->isValid())
  {
    default_gateway->setIP((ip->getIP() & subnet_mask->getIP()) | 0x1);
  }

  update();
}

void SetTmpIPWindow::isSet()
{
  if (mac->isValid() && ip->isValid() && subnet_mask->isValid() && default_gateway->isValid())
  {
    try
    {
      if ((ip->getIP() & subnet_mask->getIP()) != (default_gateway->getIP() & subnet_mask->getIP()))
      {
        if (fl_choice("IP address and gateway appear to be in different subnets. ",
          "Cancel", "Proceed", 0) != 1)
        {
          return;
        }
      }

      rcdiscover::ForceIP force_ip;

      if (fl_choice("Are you sure to set the IP address of the device with MAC-address %s?",
        "No", "Yes", 0, mac->value()) == 1)
      {
        force_ip.sendCommand(mac->getMAC(), ip->getIP(), subnet_mask->getIP(),
          default_gateway->getIP());

        hide();
      }
    }
    catch (const std::runtime_error &ex)
    {
      fl_alert("%s", ex.what());
    }
  }
}

void SetTmpIPWindow::isClear()
{
  ip->value("");
  subnet_mask->value("");
  default_gateway->value("");

  update();
}

SetTmpIPWindow::SetTmpIPWindow() : Fl_Double_Window(480, 238, "Set temporary IP address")
{
  int width=480-2*GAP_SIZE;
  int row_height=28;

  new Label(ADD_BELOW_XY, 120, row_height, "rc_visard");
  device=new DeviceChoice(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  device->add("<Custom>");
  device->callback(changingDeviceCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "MAC address");
  mac=new InputMAC(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  mac->setChangeCallback(updateCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "IP address");
  ip=new InputIP(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  ip->setChangeCallback(ipCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "Subnet mask");
  subnet_mask=new InputIP(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  subnet_mask->setChangeCallback(subnetMaskCb, this);

  new Label(ADD_BELOW_XY, 120, row_height, "Default gateway");
  default_gateway=new InputIP(ADD_RIGHT_XY, width-GAP_SIZE-120, row_height, 0);
  default_gateway->setChangeCallback(updateCb, this);

  set_ip=new Button(ADD_BELOW_XY, 200, row_height, "Set temporary IP address");
  set_ip->callback(setCb, this);

  clear_form=new Button(ADD_RIGHT_XY, 100, row_height, "Clear form");
  clear_form->callback(clearCb, this);

  help=new Button(width+GAP_SIZE-row_height, addRightY(), row_height, row_height, "?");
  help->callback(helpCb, this);

  checkGroupSize(__func__);
  end();

  size_range(w(), h(), w(), h());
}
