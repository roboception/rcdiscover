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

#include "input_filter.h"

#include <sstream>

InputFilter::InputFilter(int x, int y, int w, int h, const char *label) : Fl_Input(x, y, w, h, label)
{
  cb=0;
  user=0;
}

int InputFilter::handle(int event)
{
  std::string v=value();

  // handle event in superclass

  int ret=Fl_Input::handle(event);

  // call callback if value has changed

  if (cb && v != value())
  {
    cb(this, user);
  }

  return ret;
}
