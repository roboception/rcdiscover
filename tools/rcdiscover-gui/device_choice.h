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

#ifndef RC_DEVICE_CHOICE_H
#define RC_DEVICE_CHOICE_H

#include <FL/Fl.H>
#include <FL/Fl_Choice.H>

#include <vector>
#include <utility>
#include <string>

class DeviceChoice : public Fl_Choice
{
  public:

    DeviceChoice(int x, int y, int w, int h, const char *label);

    std::string getMAC();

    void update(const std::vector<std::pair<std::string, std::string> > &list,
      const std::string &sel_mac);
};

#endif
