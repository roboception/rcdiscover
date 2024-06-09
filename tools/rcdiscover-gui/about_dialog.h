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

#ifndef RC_ABOUT_DIALOG_H
#define RC_ABOUT_DIALOG_H

#include "button.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Multiline_Output.H>

class AboutDialog : public Fl_Double_Window
{
  public:

    AboutDialog();

  private:

    Fl_Multiline_Output *out;
    Button *close;
};

#endif
