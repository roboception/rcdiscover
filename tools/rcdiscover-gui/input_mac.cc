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

#include "input_mac.h"

#include <sstream>

InputMAC::InputMAC(int x, int y, int w, int h, const char *label) : Fl_Input(x, y, w, h, label)
{
  cb=0;
  user=0;

  tooltip("Format example: 01:23:45:67:89:ab");
}

uint64_t InputMAC::getMAC()
{
  std::istringstream in(value());

  // read formated

  uint64_t v[6]={256, 256, 256, 256, 256, 256};
  char s[5]={' ', ' ', ' ', ' ', ' '};
  in >> std::hex >> v[0] >> s[0] >> v[1] >> s[1] >> v[2] >> s[2] >> v[3] >> s[3] >> v[4] >>
    s[4] >> v[5];

  // all separators must be ':'

  for (int i=0; i<5; i++)
  {
    if (s[i] != ':') return 0;
  }

  // its invalid if there is unparsed characters left

  if (!in.eof())
  {
    return 0;
  }

  // check validity of individual numbers

  for (int i=0; i<6; i++)
  {
    if (v[i] > 255) return 0;
  }

  // return MAC as integer

  return ((v[0]<<40) | (v[1]<<32) | (v[2]<<24) | (v[3]<<16) | (v[4]<<8) | v[5]);
}

int InputMAC::handle(int event)
{
  if (event == FL_KEYBOARD && !(Fl::event_key() == FL_Tab))
  {
    char ascii=Fl::event_text()[0];

    if (std::isprint(ascii) && !isxdigit(ascii) && ascii != ':')
    {
      return 1; // ignore all printable characters except accepted ones
    }
  }

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
