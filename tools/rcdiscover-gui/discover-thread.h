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

#include <wx/thread.h>

class wxWindow;

/**
 * @brief Thread in which the discovery of rc_visards is run.
 */
class DiscoverThread : public wxThread
{
  public:
    DiscoverThread(wxWindow *parent) :
      parent_(parent)
    { }

    virtual ~DiscoverThread() = default;

  protected:
    virtual ExitCode Entry() override;

  private:
    wxWindow *parent_;
};

#endif // DISCOVERTHREAD_H
