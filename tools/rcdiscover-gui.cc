/*
 * rcdiscover - the network discovery tool for Roboception devices
 *
 * Copyright (c) 2017 Roboception GmbH
 * All rights reserved
 *
 * Author: Raphael Schaller
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "rcdiscover-gui/discover-frame.h"
#include "rcdiscover-gui/resources.h"

#include <sstream>

#include "wx/app.h"
#include "wx/msgdlg.h"

class RcDiscoverApp : public wxApp
{
  public:
    RcDiscoverApp() :
      frame_(nullptr)
    { }

    virtual ~RcDiscoverApp() = default;

    virtual bool OnInit() override
    {
      if (!wxApp::OnInit())
      {
        return false;
      }

      SetAppName("rcdiscover");
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

      registerResources();

      frame_ = new DiscoverFrame("rcdiscover", wxPoint(50,50));
      frame_->Show(true);
      return true;
    }

    virtual int OnExit() override
    {
#ifdef WIN32
      ::WSACleanup();
#endif

      return wxApp::OnExit();
    }

    virtual bool OnExceptionInMainLoop() override
    {
      try
      {
        throw;
      }
      catch (const std::exception &ex)
      {
        std::string error_msg = "Caught exception of type ";
        error_msg += typeid(ex).name();
        error_msg += ": ";
        error_msg += ex.what();
        wxMessageBox(error_msg, "Error", wxOK | wxICON_ERROR);
      }

      return wxApp::OnExceptionInMainLoop();
    }

  private:
    wxWindow *frame_;
};

wxIMPLEMENT_APP(RcDiscoverApp);
