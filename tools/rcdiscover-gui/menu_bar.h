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

#ifndef RC_MENU_BAR_H
#define RC_MENU_BAR_H

#include <FL/Fl.H>
#include <FL/Fl_Menu_Bar.H>

class MenuBar : public Fl_Menu_Bar
{
  public:

    MenuBar(int x, int y, int w, int h) : Fl_Menu_Bar(x, y, w, h)
    {
      box(FL_THIN_UP_BOX);
      color(fl_lighter(FL_GRAY));
    }
};

#endif
