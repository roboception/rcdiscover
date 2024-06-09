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

#include "help_window.h"

#include "layout.h"

#include "resources/help.h"

#include <vector>
#include <cstring>

namespace
{

static HelpWindow *win=0;

}

void HelpWindow::showWindow(const char *target)
{
  if (!win)
  {
    win=new HelpWindow();
  }

  if (target)
  {
    win->help->topline(target);
  }

  win->show();
}

void HelpWindow::hideWindow()
{
  if (win)
  {
    win->hide();
  }
}

HelpWindow::HelpWindow() : Fl_Double_Window(640, 480, "rcdiscover Help")
{
  int width=640-2*GAP_SIZE;
  int height=480-2*GAP_SIZE;

  help=new Fl_Help_View(ADD_RIGHT_XY, width, height, 0);

  std::vector<char> content(help_html_len+1);
  memcpy(content.data(), help_html, help_html_len);
  content[help_html_len]='\0';

  help->value(content.data());

  checkGroupSize(__func__);
  end();
}
