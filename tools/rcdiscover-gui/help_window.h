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

#ifndef RC_HELP_WINDOW_H
#define RC_HELP_WINDOW_H

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Help_View.H>

class HelpWindow : public Fl_Double_Window
{
  public:

    static void showWindow(const char *target=0);
    static void hideWindow();

  private:

    HelpWindow();

    Fl_Help_View *help;
};

#endif
