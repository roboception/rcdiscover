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

#ifndef RC_INPUT_MAC_H
#define RC_INPUT_MAC_H

#include <FL/Fl.H>
#include <FL/Fl_Input.H>

#include <cstdint>

class InputMAC : public Fl_Input
{
  public:

    InputMAC(int x, int y, int w, int h, const char *label);

    void setChangeCallback(Fl_Callback* _cb, void* p) { cb=_cb; user=p; }

    uint64_t getMAC();

    bool isValid() { return getMAC() != 0; }

    int handle(int event);

  private:

    Fl_Callback *cb;
    void *user;
};

#endif
