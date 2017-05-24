/*
* Roboception GmbH
* Munich, Germany
* www.roboception.com
*
* Copyright (c) 2017 Roboception GmbH
* All rights reserved
*
* Author: Raphael Schaller
*/

#include "rcdiscover-gui/discover-frame.h"

#include <sstream>

#include "wx/app.h"
#include "wx/msgdlg.h"

class RcDiscoverApp : public wxApp
{
  public:
    virtual bool OnInit() override
    {
      SetAppName("rc_discover");
      SetVendorName("Roboception");

#ifdef WIN32
      ::WSADATA wsaData;
      int result;
      if ((result = ::WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
      {
        std::ostringstream oss;
        oss << "WSAStartup failed: " << result;
        wxMessageBox(oss.str(), "Error", wxOK | wxICON_ERROR);
      }
#endif

      auto *frame = new DiscoverFrame("rc_discover", wxPoint(50,50));
      frame->Show(true);
      return true;
    }

    virtual int OnExit() override
    {
#ifdef WIN32
      ::WSACleanup();
#endif

      return 0;
    }
};

wxIMPLEMENT_APP(RcDiscoverApp);
