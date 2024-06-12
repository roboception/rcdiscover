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

#include "discover_window.h"

#include "menu_bar.h"
#include "help_window.h"
#include "about_dialog.h"
#include "reset_window.h"
#include "set_tmp_ip_window.h"
#include "reconnect_window.h"
#include "label.h"
#include "layout.h"

#include "rcdiscover/discover.h"
#include "rcdiscover/ping.h"

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Menu_Item.H>

#ifdef WIN32
#include <Windows.h>
#undef min
#undef max
#endif

#include <sstream>
#include <iomanip>
#include <chrono>
#include <iostream>

namespace
{

void quitCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doClose();
}

void helpCb(Fl_Widget *, void *)
{
  HelpWindow::showWindow("discovery");
}

void aboutCb(Fl_Widget *, void *)
{
  AboutDialog about;
  about.show();
  while (about.shown()) Fl::wait();
}

void discoverCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doDiscover();
}

void onlyRCCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doOnlyRC();
}

void filterCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doFilter();
}

template <int n> void copyToClipboardCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doCopyToClipboard(n);
}

void openWebGUICb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doOpenWebGUI();
}

void listCb(Fl_Widget *, void *user_data)
{
  if (Fl::event_button() == FL_RIGHT_MOUSE && Fl::event() == FL_PUSH)
  {
    DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
    win->doOpenContextMenu();
  }
}

void listSelectionChangeCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->update();
}

void resetCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doReset();
}

void setTmpIPCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doSetTmpIP();
}

void reconnectCb(Fl_Widget *, void *user_data)
{
  DiscoverWindow *win=reinterpret_cast<DiscoverWindow *>(user_data);
  win->doReconnect();
}

}

DiscoverWindow::DiscoverWindow(int ww, int hh, int _only_rc, const std::string &_filter) :
  Fl_Double_Window(1180, 394, "rcdiscover")
{
  running=false;
  discover_thread=0;

  int width=1180-2*GAP_SIZE;
  int row_height=28;

  menu_bar=new MenuBar(0, 0, width+2*GAP_SIZE, row_height);
  menu_bar->add("File/Quit", FL_CTRL+'q', quitCb, this);
  menu_bar->add("Help/Help", FL_F+1, helpCb, this);
  menu_bar->add("Help/About", 0, aboutCb, this);

  {
    int xc=addBelowX();
    int yc=addBelowY()+5;

    Fl_Group *group=new Fl_Group(xc, yc, width, row_height);

    discover=new Button(xc, yc, 160, row_height, "Rerun discovery");
    discover->callback(discoverCb, this);

    Label *divider=new Label(addRightX()+20, addRightY(), 25, row_height, "|");
    divider->textcolor(FL_INACTIVE_COLOR);

    only_rc=new Fl_Check_Button(ADD_RIGHT_XY, 150, row_height, "Only rc_...devices");
    only_rc->value(_only_rc);
    only_rc->callback(onlyRCCb, this);

    filter=new InputFilter(addRightX()+40, addRightY(), 150, row_height, "Filter");
    filter->value(_filter.c_str());
    filter->setChangeCallback(filterCb, this);

    Fl_Group *empty=new Fl_Group(ADD_RIGHT_XY, width-5*GAP_SIZE-(160+45+150+190+row_height), row_height);
    empty->end();
    group->resizable(empty);

    logo=new Logo(ADD_RIGHT_XY, row_height, row_height);

    group->end();
  }

  {
    int xc=addBelowX();
    int yc=addBelowY()+5;

    // invisible context menu "button" at the same area as device list
    context_menu=new Fl_Menu_Button(xc, yc, 0, 0);
    context_menu->type(Fl_Menu_Button::POPUP3);

    context_menu->add("Copy name", 0, copyToClipboardCb<0>, this);
    context_menu->add("Copy manufacturer", 0, copyToClipboardCb<1>, this);
    context_menu->add("Copy model", 0, copyToClipboardCb<2>, this);
    context_menu->add("Copy serial number", 0, copyToClipboardCb<3>, this);
    context_menu->add("Copy IP address", 0, copyToClipboardCb<4>, this);
    context_menu->add("Copy MAC address", 0, copyToClipboardCb<5>, this);
    context_menu->add("Copy interface(s)", 0, copyToClipboardCb<6>, this, FL_MENU_DIVIDER);
    context_menu->add("Open WebGUI", 0, openWebGUICb, this, FL_MENU_DIVIDER);
    context_menu->add("Reset rc_visard", 0, resetCb, this);
    context_menu->add("Set temporary IP address", 0, setTmpIPCb, this);
    context_menu->add("Reconnect device", 0, reconnectCb, this);

    openwebgui_index=context_menu->find_index("Open WebGUI");
    reset_index=context_menu->find_index("Reset rc_visard");

    // device list
    list=new DeviceList(xc, yc, width, 260);
    list->callback(listCb, this);
    list->setSelectionChangeCallback(listSelectionChangeCb, this);

    resizable(list);
  }

  {
    int xc=addBelowX();
    int yc=addBelowY();

    Fl_Group *group=new Fl_Group(addBelowX(), addBelowY()+5, width, row_height);

    reset=new Button(xc, yc, 180, row_height, "Reset rc_visard");
    reset->callback(resetCb, this);

    set_tmp_ip=new Button(ADD_RIGHT_XY, 200, row_height, "Set temporary IP address");
    set_tmp_ip->callback(setTmpIPCb, this);

    reconnect=new Button(ADD_RIGHT_XY, 180, row_height, "Reconnect device");
    reconnect->callback(reconnectCb, this);

    Fl_Group *empty=new Fl_Group(ADD_RIGHT_XY, width-4*GAP_SIZE-(180+200+180+30), row_height);
    empty->end();
    group->resizable(empty);

    help=new Button(ADD_RIGHT_XY, row_height, row_height, "?");
    help->callback(helpCb, this);

    group->end();
  }

  callback(quitCb, this);

//  checkGroupSize(__func__);
  end();

  size_range(std::min(w(), 640), std::min(h(), 200));

  list->filterRCDevices(_only_rc != 0);
  list->filter(_filter.c_str());

  update();

  size(ww, hh);
}

DiscoverWindow::~DiscoverWindow()
{
  running=false;

  if (discover_thread)
  {
    discover_thread->join();
    delete discover_thread;
  }
}

void DiscoverWindow::doDiscover()
{
  running=false;

  if (discover_thread)
  {
    discover_thread->join();
    delete discover_thread;
    discover_thread=0;
  }

  running=true;
  discover_thread=new std::thread(&DiscoverWindow::discoverThread, this);

  update();
}

void DiscoverWindow::doOnlyRC()
{
  list->filterRCDevices(only_rc->value() != 0);
  update();
}

void DiscoverWindow::doFilter()
{
  list->filter(filter->value());
  update();
}

void DiscoverWindow::doOpenContextMenu()
{
  if (list->callback_context() == Fl_Table::CONTEXT_CELL)
  {
    // right click is used to open context menu

    list->select_row(list->callback_row());
    update();

    // enable or disable some menu items depending on selection

    if (list->isReachableRCDeviceSelected())
    {
      // const cast is ok since menu item has been created dynamically
      const_cast<Fl_Menu_Item *>(&context_menu->menu()[openwebgui_index])->activate();
    }
    else
    {
      const_cast<Fl_Menu_Item *>(&context_menu->menu()[openwebgui_index])->deactivate();
    }

    if (list->isRCVisardSelected())
    {
      // const cast is ok since menu item has been created dynamically
      const_cast<Fl_Menu_Item *>(&context_menu->menu()[reset_index])->activate();
    }
    else
    {
      const_cast<Fl_Menu_Item *>(&context_menu->menu()[reset_index])->deactivate();
    }

    // show context menu at current mouse pointer position

    context_menu->position(Fl::event_x(), Fl::event_y());
    context_menu->show();
    context_menu->popup();
  }
  else
  {
    list->select_row(0, 0);
  }
}

void DiscoverWindow::doCopyToClipboard(int i)
{
  int r=list->getSelectedRow();

  if (r >= 0)
  {
    std::string value=list->getCell(r, i);
    Fl::copy(value.c_str(), static_cast<int>(value.size()), 1);
  }
}

void DiscoverWindow::doOpenWebGUI()
{
  int r=list->getSelectedRow();
  list->openWebGUI(r);
}

void DiscoverWindow::doReset()
{
  std::string mac=list->getSelectedMAC();

  if (list->isRCVisardSelected() || mac.size() == 0)
  {
    ResetWindow *win=ResetWindow::showWindow();
    win->updateDevices(list->getCurrentNameMACList(true), mac);
  }
}

void DiscoverWindow::doSetTmpIP()
{
  SetTmpIPWindow *win=SetTmpIPWindow::showWindow();
  win->updateDevices(list->getCurrentNameMACList(false), list->getSelectedMAC());
}

void DiscoverWindow::doReconnect()
{
  ReconnectWindow *win=ReconnectWindow::showWindow();
  win->updateDevices(list->getCurrentNameMACList(false), list->getSelectedMAC());
}

void DiscoverWindow::doClose()
{
  if (!running)
  {
    hide();
  }

  HelpWindow::hideWindow();
  ResetWindow::hideWindow();
  SetTmpIPWindow::hideWindow();
  ReconnectWindow::hideWindow();
}

void DiscoverWindow::update()
{
  if (list->isRCVisardSelected() || list->getSelectedMAC().size() == 0)
  {
    reset->activate();
  }
  else
  {
    reset->deactivate();
  }

  if (running)
  {
    discover->deactivate();
    logo->startSpinning();
  }
  else
  {
    discover->activate();
    logo->stopSpinning();
  }

  redraw();
}

void DiscoverWindow::discoverThread()
{
  try
  {
    // clear list and update to deactive discover button

    Fl::lock();
    list->clear();
    update();
    Fl::unlock();
    Fl::awake();

    // broadcast discovery request

    rcdiscover::Discover discover;
    discover.broadcastRequest();

    // collecting answers

    std::chrono::steady_clock::time_point tstart=std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point tend=tstart;

    std::vector<rcdiscover::DeviceInfo> info;

    while (running && (discover.getResponse(info, 100) ||
      std::chrono::duration<double, std::milli>(tend-tstart).count() < 1000))
    {
      // add answers immediately to table

      Fl::lock();

      for (size_t k=0; k<info.size(); k++)
      {
        if (info[k].isValid())
        {
          std::ostringstream ip, mac;

          ip << ((info[k].getIP()>>24)&0xff) << '.' << ((info[k].getIP()>>16)&0xff) << '.' <<
            ((info[k].getIP()>>8)&0xff) << '.'  << (info[k].getIP()&0xff);

          mac << std::hex << std::setw(2) << std::setfill('0') <<
            std::setw(2) << std::setfill('0') << ((info[k].getMAC()>>40)&0xff) << ':' <<
            std::setw(2) << std::setfill('0') << ((info[k].getMAC()>>32)&0xff) << ':' <<
            std::setw(2) << std::setfill('0') << ((info[k].getMAC()>>24)&0xff) << ':' <<
            std::setw(2) << std::setfill('0') << ((info[k].getMAC()>>16)&0xff) << ':' <<
            std::setw(2) << std::setfill('0') << ((info[k].getMAC()>>8)&0xff) << ':' <<
            std::setw(2) << std::setfill('0') << (info[k].getMAC()&0xff);

          list->add(info[k].getUserName().c_str(),
            info[k].getManufacturerName().c_str(),
            info[k].getModelName().c_str(),
            info[k].getSerialNumber().c_str(),
            ip.str().c_str(),
            mac.str().c_str(),
            info[k].getIfaceName().c_str(),
            checkReachabilityOfSensor(info[k]));
        }
      }

      update();
      Fl::unlock();
      Fl::awake();

      info.clear();
      tend=std::chrono::steady_clock::now();
    }
  }
  catch (const std::exception &ex)
  {
    // this should never happen, but goes to std error instead of opening an
    // error dialog, because we are working in a background thread

    std::cerr << "Exception in discover thread: " << ex.what() << std::endl;
  }
  catch (...)
  {
    // this should never happen, but goes to std error instead of opening an
    // error dialog, because we are working in a background thread

    std::cerr << "Unknown exception in discover thread" << std::endl;
  }

  // leaving discover thread and update to activate discover button

  Fl::lock();
  running=false;
  update();
  Fl::unlock();
  Fl::awake();
}
