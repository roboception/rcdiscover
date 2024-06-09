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

#include "device_list.h"

#include <FL/fl_draw.H>
#include <FL/fl_ask.H>
#include <FL/filename.H>

#ifdef WIN32
#include <Windows.h>
#undef min
#undef max
#endif

#include <sstream>

DeviceList::DeviceList(int x, int y, int w, int h) : Fl_Table_Row(x, y, w, h)
{
  type(SELECT_SINGLE);

  cb=0;
  user=0;
  new_discovery=false;

  sort_col=0;
  sort_down=true;
  filter_rc=true;

  cols(8);
  col_header(1);
  col_header_height(28);
  col_resize_min(10);

  col_width(0, 110);
  col_width(1, 170);
  col_width(2, 180);
  col_width(3, 140);
  col_width(4, 130);
  col_width(5, 160);
  col_width(6, 160);
  col_width(7, 100);

  col_resize(1);

  rows(0);
  row_header(0);
  row_height_all(28);
  row_resize(0);

  end();
}

void DeviceList::clear()
{
  // store previous MAC addresses for finding out which devices are new

  previous_mac_list.clear();

  for (size_t i=0; i<device.size(); i++)
  {
    previous_mac_list.insert(device[i].item[5]);
  }

  new_discovery=false;

  // clear table

  device.clear();
  index.clear();
  rows(0);
}

void DeviceList::add(const char *name, const char *manufacturer, const char *model, const char *sn,
      const char *ip, const char *mac, const char *interface, bool reachable)
{
  // check if device with this mac address already exists

  int k=-1;
  for (size_t i=0; i<device.size(); i++)
  {
    if (device[i].item[5].compare(mac) == 0)
    {
      k=static_cast<int>(i);
      break;
    }
  }

  if (k >= 0)
  {
    // insert interface if it does not already exist

    if (device[k].interface_list.insert(interface).second)
    {
      // rebuild human readable item

      std::ostringstream out;

      bool first=true;
      for (const auto &it: device[k].interface_list)
      {
        if (!first) out << ", ";
        out << it;
        first=false;
      }

      device[k].item[6]=out.str();
      device[k].reachable|=reachable;

      if (device[k].reachable)
      {
        device[k].item[7]="\u2713";
      }
      else
      {
        device[k].item[7]="\u2717";
      }

      redraw();
    }
  }
  else
  {
    // add as new device

    DeviceListData data;
    data.item[0]=name;
    data.item[1]=manufacturer;
    data.item[2]=model;
    data.item[3]=sn;
    data.item[4]=ip;
    data.item[5]=mac;
    data.item[6]=interface;

    data.interface_list.insert(interface);

    if (reachable)
    {
      data.item[7]="\u2713";
    }
    else
    {
      data.item[7]="\u2717";
    }

    data.reachable=reachable;
    data.new_discovery=(previous_mac_list.size() > 0 && previous_mac_list.find(mac) == previous_mac_list.end());
    new_discovery=(new_discovery || data.new_discovery);

    device.push_back(data);

    if (addDeviceIndex(device.size()-1))
    {
      rows(static_cast<int>(index.size()));
    }
  }
}

int DeviceList::getSelectedRow()
{
  int sel=-1;
  for (size_t i=0; i<index.size(); i++)
  {
    if (row_selected(static_cast<int>(i)))
    {
      sel=static_cast<int>(i);
    }
  }

  return sel;
}

bool DeviceList::isReachableRCDeviceSelected()
{
  int sel=getSelectedRow();
  return (sel >= 0 && device[index[sel]].reachable &&
    device[index[sel]].item[2].compare(0, 3, "rc_") == 0);
}

bool DeviceList::isRCVisardSelected()
{
  int sel=getSelectedRow();
  return (sel >= 0 && device[index[sel]].item[2].compare(0, 9, "rc_visard") == 0);
}

std::string DeviceList::getSelectedMAC()
{
  int sel=getSelectedRow();

  if (sel >= 0)
  {
    return device[index[sel]].item[5];
  }

  return std::string();
}

std::vector<std::pair<std::string, std::string> > DeviceList::getCurrentNameMACList(bool only_rc_visard)
{
  std::vector<std::pair<std::string, std::string> > ret;

  for (size_t i=0; i<index.size(); i++)
  {
    if (!only_rc_visard || device[index[i]].item[2].compare(0, 9, "rc_visard") == 0)
    {
      ret.push_back(std::pair<std::string, std::string>(device[index[i]].item[0], device[index[i]].item[5]));
    }
  }

  return ret;
}

std::string DeviceList::getCell(int r, int c)
{
  if (r >= 0 && r < static_cast<int>(index.size()) && c >= 0 && c < 7)
  {
    return device[index[r]].item[c];
  }

  return std::string();
}

void DeviceList::openWebGUI(int r)
{
  // only for reachable rc devices

  if (device[index[r]].reachable && device[index[r]].item[2].compare(0, 3, "rc_") == 0)
  {
    // open Web GUI

    std::ostringstream out;

    out << "http://" << device[index[r]].item[4].c_str();

    char msg[160];
    if (fl_open_uri(out.str().c_str(), msg, 160) == 0)
    {
      msg[159]='\0';
      fl_alert("%s", msg);
    }
  }
}

void DeviceList::filterRCDevices(bool _filter_rc)
{
  filter_rc=_filter_rc;
  updateDeviceIndices();
  rows(static_cast<int>(index.size()));
  select_row(0, 0);
  notifySelectionChange();
  redraw();
}

void DeviceList::filter(const char *_filter_value)
{
  filter_value=_filter_value;
  updateDeviceIndices();
  rows(static_cast<int>(index.size()));
  select_row(0, 0);
  notifySelectionChange();
  redraw();
}

int DeviceList::handle(int event)
{
  // call handle of parent

  int ret=Fl_Table_Row::handle(event);

  if (Fl::event() == FL_PUSH)
  {
    if (new_discovery && callback_context() == CONTEXT_CELL)
    {
      // remove highlighting newly discovered devices on first click

      new_discovery=false;
      for (size_t i=0; i<device.size(); i++)
      {
        device[i].new_discovery=false;
      }

      redraw();
    }

    if (Fl::event_button() == FL_LEFT_MOUSE)
    {
      if (callback_context() == CONTEXT_COL_HEADER)
      {
        // click over header is used for sorting

        sortColumn(callback_col());
        ret=1;
      }

      if (Fl::event_clicks() == 1 && callback_context() == CONTEXT_CELL)
      {
        // double click on row opens the web gui

        openWebGUI(callback_row());
        ret=1;
      }
    }
    else if (Fl::event_button() == FL_MIDDLE_MOUSE)
    {
      if (callback_context() == CONTEXT_CELL)
      {
        // middle click on cell is used to copy cell content to selection buffer

        std::string value=getCell(callback_row(), callback_col());
        select_row(callback_row());

        Fl::copy(value.c_str(), static_cast<int>(value.size()), 0);
      }
      else
      {
        select_row(0, 0);
      }

      ret=1;
    }

    notifySelectionChange();
  }
  else if (Fl::event() == FL_RELEASE && Fl::event_button() == FL_LEFT_MOUSE)
  {
    notifySelectionChange();
  }

  return ret;
}

void DeviceList::draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H)
{
  switch (context)
  {
    case CONTEXT_STARTPAGE:
      fl_font(FL_HELVETICA, 16);
      break;

    case CONTEXT_RC_RESIZE:
      if (cols() > 0)
      {
        int cw=0;
        for (int i=0; i<cols()-1; i++)
        {
          cw+=col_width(i);
        }

        int last_col=cols()-1;
        if (cw+10 < tiw)
        {
          if (cw+col_width(last_col) != tiw)
          {
            col_width(last_col, tiw-cw);
          }
        }
      }
      break;

    case CONTEXT_COL_HEADER:
      {
        static const char *header[]={"Name", "Manufacturer", "Model", "Serial",
          "IP Address", "MAC Address", "Interface(s)", "Reachable"};

        fl_push_clip(X, Y, W, H);
        fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, row_header_color());
        fl_color(FL_INACTIVE_COLOR);

        if (sort_col == COL)
        {
          // show that this column is used for sorting and show direction

          std::ostringstream out;
          out << header[COL] << ' ';

          if (sort_down)
          {
            out << "@-22>";
          }
          else
          {
            out << "@-22<";
          }

          fl_draw(out.str().c_str(), X, Y, W, H, FL_ALIGN_LEFT);
        }
        else
        {
          fl_draw(header[COL], X, Y, W, H, FL_ALIGN_LEFT);
        }

        fl_pop_clip();
      }
      break;

    case CONTEXT_CELL:
      {
        fl_push_clip(X, Y, W, H);

        if (row_selected(ROW))
        {
          fl_color(FL_SELECTION_COLOR);
        }
        else if (device[index[ROW]].new_discovery)
        {
          fl_color(FL_DARK3);
        }
        else
        {
          fl_color(FL_WHITE);
        }

        fl_rectf(X, Y, W, H);

        if (row_selected(ROW))
        {
          fl_color(FL_WHITE);
        }
        else
        {
          fl_color(FL_GRAY0);
        }

        if (COL < 7)
        {
          fl_draw(device[index[ROW]].item[COL].c_str(), X, Y, W, H, FL_ALIGN_LEFT);
        }
        else
        {
          fl_draw(device[index[ROW]].item[COL].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
        }

        fl_color(color());
        fl_rect(X, Y, W, H);
        fl_pop_clip();
      }
      break;

    default:
      break;
  }
}

void DeviceList::notifySelectionChange()
{
  if (cb)
  {
    cb(this, user);
  }
}

void DeviceList::sortColumn(int c)
{
  if (sort_col == c)
  {
    // toggle sorting direction
    sort_down=!sort_down;
  }
  else
  {
    // sort new column
    sort_col=c;
    sort_down=true;
  }

  updateDeviceIndices();
  select_row(0, 0);
  redraw();
}

bool DeviceList::addDeviceIndex(size_t i)
{
  // check if device is visible according to filter criteria

  if (filter_rc && device[i].item[2].compare(0, 3, "rc_") != 0) // only rc devices
  {
    return false;
  }

  if (filter_value.size() > 0)
  {
    bool found=false;
    for (int k=0; k<7 && !found; k++)
    {
      found=(device[i].item[k].find(filter_value) != std::string::npos);
    }

    if (!found)
    {
      return false;
    }
  }

  // insert index according to sorting

  int dir=1;
  if (!sort_down)
  {
    dir=-1;
  }

  size_t k=0;
  while (k < index.size())
  {
    if (device[i].item[sort_col].compare(device[index[k]].item[sort_col])*dir < 0)
    {
      index.insert(index.begin()+k, i);
      break;
    }

    k++;
  }

  if (k >= index.size())
  {
    index.push_back(i);
  }

  return true;
}

void DeviceList::updateDeviceIndices()
{
  // rebuilding the list of indices

  index.clear();

  for (size_t i=0; i<device.size(); i++)
  {
    addDeviceIndex(i);
  }
}
