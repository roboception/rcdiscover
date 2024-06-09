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

#ifndef RC_INPUT_IP_H
#define RC_INPUT_IP_H

#include <FL/Fl.H>
#include <FL/Fl_Input.H>

#include <cstdint>

class InputIP : public Fl_Input
{
  public:

    InputIP(int x, int y, int w, int h, const char *label);

    void setChangeCallback(Fl_Callback* _cb, void* p) { cb=_cb; user=p; }

    uint32_t getIP();
    void setIP(uint32_t v);

    bool isValid() { return getIP() != 0; }

    int handle(int event);

  private:

    Fl_Callback *cb;
    void *user;
};

#endif
