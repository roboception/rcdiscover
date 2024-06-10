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

#ifndef DEVICE_LIST_H
#define DEVICE_LIST_H

#include <FL/Fl.H>
#include <FL/Fl_Table_Row.H>

#include <vector>
#include <set>
#include <string>

class DeviceList : public Fl_Table_Row
{
  public:

    DeviceList(int x, int y, int w, int h);

    void clear();

    void add(const char *name, const char *manufacturer, const char *model, const char *sn,
      const char *ip, const char *mac, const char *interface, bool reachable);

    void setSelectionChangeCallback(Fl_Callback* _cb, void* p) { cb=_cb; user=p; }

    int getSelectedRow();
    bool isReachableRCDeviceSelected();
    bool isRCVisardSelected();
    std::string getSelectedMAC();
    std::vector<std::pair<std::string, std::string> > getCurrentNameMACList(bool only_rc_visard);

    std::string getCell(int r, int c);
    void openWebGUI(int r);

    void filterRCDevices(bool filter_rc);
    void filter(const char *filter_value);

    void getSorting(int &sort_col, bool &sort_down);
    void setSorting(int sort_col, bool sort_down);

  protected:

    int handle(int event);
    void draw_cell(TableContext context, int ROW, int COL, int X, int Y, int W, int H);

  private:

    void notifySelectionChange();

    void sortColumn(int c);

    bool addDeviceIndex(size_t i);
    void updateDeviceIndices();

    std::set<std::string> previous_mac_list;
    bool new_discovery;

    struct DeviceListData
    {
      std::string item[8];
      std::set<std::string> interface_list;
      bool reachable;
      bool new_discovery;
    };

    std::vector<DeviceListData> device; // all discovered devices
    std::vector<size_t> index; // indices of visible devices in sorted order

    Fl_Callback *cb;
    void *user;

    int sort_col;
    bool sort_down;
    bool filter_rc;
    std::string filter_value;
};

#endif
