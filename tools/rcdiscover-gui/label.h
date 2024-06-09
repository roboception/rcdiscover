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

#ifndef RC_LABEL_H
#define RC_LABEL_H

#include <FL/Fl.H>
#include <FL/Fl_Output.H>

class Label : public Fl_Output
{
  public:

    Label(int x, int y, int w, int h, const char *label) : Fl_Output(x, y, w, h)
    {
      box(FL_FLAT_BOX);
      color(FL_BACKGROUND_COLOR);
      clear_visible_focus();
      value(label);
    }
};

#endif
