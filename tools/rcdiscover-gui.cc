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

#include "rcdiscover-gui/discover_window.h"
#include "rcdiscover-gui/resources/logo_128.xpm"

#include <FL/Fl.H>
#include <FL/Fl_Pixmap.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/fl_ask.H>

#include <string>
#include <map>
#include <fstream>

#ifdef WIN32
#include <direct.h>
#include <winsock2.h>
#endif

class WSA
{
  public:
    WSA()
    {
#ifdef WIN32
      WSADATA wsaData;
      WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif
    }

    ~WSA()
    {
#ifdef WIN32
      ::WSACleanup();
#endif
    }
};

int main(int argc, char *argv[])
{
  WSA wsa;
  int ret=0;

  // load configuration

  std::string name;

  int width=1180;
  int height=394;
  int only_rc=1;
  std::string filter;

  try
  {
#ifdef WIN32
    const char *dir=getenv("USERPROFILE");

    if (dir && dir[0] != '\0')
    {
      name=dir;
      if (name[name.size()-1] != '\\')
      {
        name.append("\\");
      }

      name.append("\\AppData");
      _mkdir(name.c_str());
      name.append("\\Local");
      _mkdir(name.c_str());
      name.append("\\rcdiscover");
      _mkdir(name.c_str());
      name.append("\\rcdiscover.txt");
    }
#else
    const char *dir=getenv("HOME");

    if (dir && dir[0] != '\0')
    {
      name=dir;
      if (name[name.size()-1] != '/')
      {
        name.append("/");
      }

      name.append(".config");
      mkdir(name.c_str(), 0777);
      name.append("/rcdiscover");
    }
#endif

    if (name.size() > 0)
    {
      std::ifstream in(name);

      std::string key;
      std::string value;
      while (in >> key >> value)
      {
        if (value.size() > 0)
        {
          if (key == "width") width=std::stoi(value);
          if (key == "height") height=std::stoi(value);
          if (key == "only_rc") only_rc=std::stoi(value);
          if (key == "filter") filter=value;
        }
      }

      in.close();
    }
  }
  catch (...)
  {
    // allowed to fail
  }

  try
  {
    // some theme settings

    Fl::background(248, 248, 248);
//    Fl::visible_focus(false);

    // create main window

    DiscoverWindow *window=new DiscoverWindow(width, height, only_rc, filter);

    // set icon

    Fl_Pixmap icon_xpm(logo_128_xpm);
    Fl_RGB_Image icon_rgb(&icon_xpm, FL_GRAY);
    window->icon(&icon_rgb);
    window->xclass("rcdiscover");

    // show

    Fl::lock();
    window->show(argc, argv);
    window->doDiscover();

    ret=Fl::run();
    Fl::unlock();

    // store configuration

    try
    {
      std::ofstream out(name);

      out << "width " << window->w() << std::endl;
      out << "height " << window->h() << std::endl;
      out << "only_rc " << window->getOnlyRCValue() << std::endl;
      out << "filter " << window->getFilterValue() << std::endl;

      out.close();
    }
    catch (...)
    {
      // allowed to fail
    }

    // clean up resources

    delete window;
  }
  catch (const std::exception &ex)
  {
    fl_alert("%s", ex.what());
    ret=1;
  }

  return ret;
}
