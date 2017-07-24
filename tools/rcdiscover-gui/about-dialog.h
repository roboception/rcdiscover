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

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

#include <wx/dialog.h>

/**
 * @brief About dialog.
 */
class AboutDialog :
    public wxDialog
{
  public:
    AboutDialog(wxWindow *parent, wxWindowID id,
                const wxPoint &pos = wxDefaultPosition,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString &name = wxDialogNameStr);

    virtual ~AboutDialog() = default;
};

#endif // ABOUT_DIALOG_H
