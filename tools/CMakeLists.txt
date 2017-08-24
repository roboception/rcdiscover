# rcdiscover - the network discovery tool for rc_visard
#
# Copyright (c) 2017 Roboception GmbH
# All rights reserved
#
# Author: Heiko Hirschmueller
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# 3. Neither the name of the copyright holder nor the names of its contributors
# may be used to endorse or promote products derived from this software without
# specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

project(tools CXX)

# build programs

add_executable(rcdiscover rcdiscover.cc)
target_link_libraries(rcdiscover rcdiscover_static)

if (WIN32)
  target_link_libraries(rcdiscover iphlpapi.lib ws2_32.lib)
  set_target_properties(rcdiscover PROPERTIES LINK_FLAGS -mconsole)
endif (WIN32)

if(wxWidgets_FOUND)
  message(STATUS ${wxWidgets_INCLUDE_DIRS})
  include_directories(${wxWidgets_INCLUDE_DIRS})

  set(rcdiscover-gui-src
    rcdiscover-gui.cc
    rcdiscover-gui/event-ids.cc
    rcdiscover-gui/discover-frame.cc
    rcdiscover-gui/discover-thread.cc
    rcdiscover-gui/reset-dialog.cc
    rcdiscover-gui/about-dialog.cc
    rcdiscover-gui/resources.cc)

  if (WIN32)
    if(MINGW)
      set(CMAKE_RC_COMPILER_INIT windres)
      ENABLE_LANGUAGE(RC)
      SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT> <INCLUDES>")
    endif(MINGW)

    add_executable(rcdiscover-gui
      ${rcdiscover-gui-src}
      rcdiscover-gui/resources/resources.rc)

  else (WIN32)
    add_executable(rcdiscover-gui ${rcdiscover-gui-src})
  endif (WIN32)

  target_link_libraries(rcdiscover-gui rcdiscover_static ${wxWidgets_LIBRARIES})

  if (WIN32)
    target_link_libraries(rcdiscover-gui iphlpapi.lib ws2_32.lib)
    set_target_properties(rcdiscover-gui PROPERTIES LINK_FLAGS -mwindows)
  endif (WIN32)

  install(TARGETS rcdiscover rcdiscover-gui COMPONENT bin DESTINATION bin)

  if (UNIX AND NOT APPLE)
    if (NOT CMAKE_INSTALL_DATADIR)
      set(CMAKE_INSTALL_DATADIR /usr/share)
    endif ()
    install(FILES rcdiscover-gui/resources/rcdiscover-gui.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES rcdiscover-gui/resources/logo_32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_64.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_128.png DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_32.png DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps RENAME rcdiscover-gui.png)
  endif ()

else(wxWidgets_FOUND)
  MESSAGE(WARNING "wxWidgets not found! Not building GUI application")

  install(TARGETS rcdiscover COMPONENT bin DESTINATION bin)
endif(wxWidgets_FOUND)
