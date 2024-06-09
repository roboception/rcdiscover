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

#ifndef DISCOVER_WINDOW_H
#define DISCOVER_WINDOW_H

#include "menu_bar.h"
#include "device_list.h"
#include "button.h"
#include "input_filter.h"
#include "logo.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Menu_Button.H>

#include <vector>
#include <string>
#include <memory>
#include <atomic>
#include <thread>

class DiscoverWindow : public Fl_Double_Window
{
  public:

    DiscoverWindow(int ww, int hh, int _only_rc, const std::string &_filter);
    ~DiscoverWindow();

    int getOnlyRCValue() { return only_rc->value(); }
    const char *getFilterValue() { return filter->value(); }

    void doDiscover();
    void doOnlyRC();
    void doFilter();
    void doOpenContextMenu();
    void doCopyToClipboard(int i);
    void doOpenWebGUI();
    void doReset();
    void doSetTmpIP();
    void doReconnect();
    void doClose();

    void update();

  private:

    void discoverThread();

    std::atomic_bool running;
    std::thread *discover_thread;

    MenuBar *menu_bar;

    Button *discover;
    Fl_Check_Button *only_rc;
    InputFilter *filter;
    Logo *logo;
    DeviceList *list;
    Fl_Menu_Button *context_menu;
    int openwebgui_index;
    int reset_index;
    Button *reset;
    Button *set_tmp_ip;
    Button *reconnect;
    Button *help;
};

#endif
