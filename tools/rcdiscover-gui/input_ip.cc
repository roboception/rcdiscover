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

#include "input_ip.h"

#include <sstream>

InputIP::InputIP(int x, int y, int w, int h, const char *label) : Fl_Input(x, y, w, h, label)
{
  cb=0;
  user=0;

  tooltip("Format example: 10.123.4.5");
}

uint32_t InputIP::getIP()
{
  std::istringstream in(value());

  // read formated

  uint32_t v[4]={256, 256, 256, 256};
  char s[3]={' ', ' ', ' '};
  in >> v[0] >> s[0] >> v[1] >> s[1] >> v[2] >> s[2] >> v[3];

  // all separators must be '.'

  for (int i=0; i<3; i++)
  {
    if (s[i] != '.') return 0;
  }

  // its invalid if there is unparsed characters left

  if (!in.eof())
  {
    return 0;
  }

  // check validity of individual numbers

  for (int i=0; i<4; i++)
  {
    if (v[i] > 255) return 0;
  }

  // return IP as integer

  return ((v[0]<<24) | (v[1]<<16) | (v[2]<<8) | v[3]);
}

void InputIP::setIP(uint32_t v)
{
  std::ostringstream out;

  out << ((v>>24)&0xff) << '.' << ((v>>16)&0xff) << '.' << ((v>>8)&0xff) << '.' << (v&0xff);

  value(out.str().c_str());
}

int InputIP::handle(int event)
{
  if (event == FL_KEYBOARD && !(Fl::event_key() == FL_Tab))
  {
    char ascii=Fl::event_text()[0];

    if (std::isprint(ascii) && !isdigit(ascii) && ascii != '.')
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
