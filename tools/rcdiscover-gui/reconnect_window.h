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

#ifndef RC_RECONNECT_WINDOW_H
#define RC_RECONNECT_WINDOW_H

#include "device_choice.h"
#include "input_mac.h"
#include "button.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include <vector>
#include <utility>

class ReconnectWindow : public Fl_Double_Window
{
  public:

    static ReconnectWindow *showWindow();
    static void hideWindow();

    void updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
      const std::string &sel_mac);

    void update();

    void isChangingDevice();
    void isReconnect();

  private:

    ReconnectWindow();

    DeviceChoice *device;
    InputMAC *mac;
    Button *reconnect;
    Button *help;
};

#endif
