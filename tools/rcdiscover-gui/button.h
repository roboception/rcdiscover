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

#ifndef RC_BUTTON_H
#define RC_BUTTON_H

#include <FL/Fl.H>
#include <FL/Fl_Button.H>

class Button : public Fl_Button
{
  public:

    Button(int x, int y, int w, int h, const char *label) : Fl_Button(x, y, w, h, label)
    {
      box(FL_GTK_UP_BOX);
      down_box(FL_GTK_DOWN_BOX);
      selection_color(FL_DARK2);
    }
};

#endif
