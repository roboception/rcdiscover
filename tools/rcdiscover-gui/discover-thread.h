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

#ifndef DISCOVERTHREAD_H
#define DISCOVERTHREAD_H

#include "event_ids.h"
#include "rcdiscover/discover.h"
#include "../utils.h"

#include <vector>
#include <algorithm>
#include <iostream>

#include <wx/thread.h>

class wxWindow;

class DiscoverThread : public wxThread
{
  public:
    DiscoverThread(wxWindow *parent) :
      parent_(parent)
    { }

  protected:
    virtual ExitCode Entry() override;

  private:
    wxWindow *parent_;
};

#endif // DISCOVERTHREAD_H
