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

#include "about_dialog.h"
#include "layout.h"

#include "resources/logo_128.xpm"

#include <FL/Fl_Box.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>

#include <sstream>

namespace
{

void closeCb(Fl_Widget *, void *user_data)
{
  AboutDialog *about=reinterpret_cast<AboutDialog *>(user_data);
  about->hide();
}

}

AboutDialog::AboutDialog() :
  Fl_Double_Window(600, 238, "About rcdiscover")
{
  set_modal();

  int width=600-2*GAP_SIZE;

  Fl_Pixmap logo_xpm(logo_128_xpm);
  Fl_RGB_Image logo_rgb(&logo_xpm, FL_GRAY);

  Fl_Box *box=new Fl_Box(ADD_RIGHT_XY, 100, 180);
  box->box(FL_NO_BOX);
  box->image(logo_rgb.copy(64, 64));

  out=new Fl_Multiline_Output(ADD_RIGHT_XY, width-110, 180);
  out->labeltype(FL_NO_LABEL);
  out->box(FL_NO_BOX);
  out->textsize(static_cast<int>(1.2*out->textsize()));
  out->clear_visible_focus();

  std::ostringstream msg;
  msg << "Tool for the discovery of GigE Vision devices." << "\n";
  msg << "\n";
  msg << "Version: " << PACKAGE_VERSION << "\n";
  msg << "\n";
  msg << "Copyright (c) 2017-2023 Roboception GmbH\n";
  msg << "The source code of rcdiscover is available\n";
  msg << "under the 3-clause BSD license.\n";
  msg << "\n";
  msg << "This program is based on FLTK.\n";

  out->value(msg.str().c_str());

  close=new Button(ADD_BELOW_XY, width, 28, "Close");
  close->callback(closeCb, this);

  checkGroupSize(__func__);
  end();

  size_range(w(), h(), w(), h());
}
