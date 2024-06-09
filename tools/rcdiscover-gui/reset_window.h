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

#ifndef RC_RESET_WINDOW_H
#define RC_RESET_WINDOW_H

#include "device_choice.h"
#include "input_mac.h"
#include "button.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>

class ResetWindow : public Fl_Double_Window
{
  public:

    static ResetWindow *showWindow();
    static void hideWindow();

    void updateDevices(const std::vector<std::pair<std::string, std::string> > &list,
      const std::string &sel_mac);

    void update();

    void isChangingDevice();
    void isResetParameters();
    void isResetNetwork();
    void isResetAll();
    void isSwitchPartitions();

  private:

    ResetWindow();

    DeviceChoice *device;
    InputMAC *mac;
    Button *reset_parameters;
    Button *reset_network;
    Button *reset_all;
    Button *switch_partitions;
    Button *help;
};

#endif
