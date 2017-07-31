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

#include "resources/help.hhp.h"
#include "resources/help.htm.h"
#include "resources/help.hhc.h"
#include "resources/help.hhk.h"

#include <wx/fs_mem.h>

void registerResources()
{
  wxFileSystem::AddHandler(new wxMemoryFSHandler());

  wxMemoryFSHandler::AddFile("help.hhp", help_hhp, sizeof(help_hhp));
  wxMemoryFSHandler::AddFile("help.htm", help_htm, sizeof(help_htm));
  wxMemoryFSHandler::AddFile("help.hhc", help_hhc, sizeof(help_hhc));
  wxMemoryFSHandler::AddFile("help.hhk", help_hhk, sizeof(help_hhk));
}
