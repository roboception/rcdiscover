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

#ifndef RC_SET_TMP_IP_WINDOW_H
#define RC_SET_TMP_IP_WINDOW_H

#include "device_choice.h"
#include "input_mac.h"
#include "input_ip.h"
#include "button.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

#include <vector>
#include <utility>

class SetTmpIPWindow : public Fl_Double_Window
{
  public:

    static SetTmpIPWindow *showWindow();
    static void hideWindow();

    void updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
      const std::string &sel_mac);

    void update();

    void isChangingDevice();
    void doIP();
    void doSubnetMask();
    void isSet();
    void isClear();

  private:

    SetTmpIPWindow();

    DeviceChoice *device;
    InputMAC *mac;
    InputIP *ip;
    InputIP *subnet_mask;
    InputIP *default_gateway;
    Button *set_ip;
    Button *clear_form;
    Button *help;
};

#endif
