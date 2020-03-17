# rcdiscover - the network discovery tool for Roboception devices
#
# Copyright (c) 2019 Roboception GmbH
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

#
# rcdiscover commandline program
#
if (BUILD_RCDISCOVER_CLI)
  add_executable(rcdiscover-cli
    rcdiscover.cc
    rcdiscover-cli/rcdiscover_discover.cc
    rcdiscover-cli/rcdiscover_reconnect.cc
    rcdiscover-cli/rcdiscover_force_ip.cc
    rcdiscover-cli/rcdiscover_reset.cc
    rcdiscover-cli/cli_utils.cc)
  target_link_libraries(rcdiscover-cli ${PROJECT_NAMESPACE}::rcdiscover_static)

  if (WIN32)
    target_link_libraries(rcdiscover-cli iphlpapi.lib ws2_32.lib)
    set_target_properties(rcdiscover-cli PROPERTIES LINK_FLAGS -mconsole)
  endif(WIN32)
  set_target_properties(rcdiscover-cli PROPERTIES OUTPUT_NAME "rcdiscover")
  install(TARGETS rcdiscover-cli COMPONENT bin DESTINATION bin)
endif ()


#
# rcdiscover-gui program
#
if(BUILD_RCDISCOVER_GUI)
  # - Find manditory packages -
  set(wxWidgets_USE_STATIC ON)
  set(wxWidgets_USE_UNICODE ON)
  find_package(wxWidgets REQUIRED COMPONENTS html adv core base)
  include(${wxWidgets_USE_FILE})

  set(rcdiscover-gui-src
    rcdiscover-gui.cc
    rcdiscover-gui/event-ids.cc
    rcdiscover-gui/discover-frame.cc
    rcdiscover-gui/discover-thread.cc
    rcdiscover-gui/sensor-command-dialog.cc
    rcdiscover-gui/reset-dialog.cc
    rcdiscover-gui/force-ip-dialog.cc
    rcdiscover-gui/reconnect-dialog.cc
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

  target_include_directories(rcdiscover-gui
    PRIVATE
      ${wxWidgets_INCLUDE_DIRS}
  )
  target_link_libraries(rcdiscover-gui ${PROJECT_NAMESPACE}::rcdiscover_static ${wxWidgets_LIBRARIES})

  if (WIN32)
    target_link_libraries(rcdiscover-gui iphlpapi.lib ws2_32.lib uxtheme)
    set_target_properties(rcdiscover-gui PROPERTIES LINK_FLAGS -mwindows)
  endif (WIN32)

  install(TARGETS rcdiscover-gui COMPONENT gui DESTINATION bin)

  if (UNIX AND NOT APPLE)
    if (NOT CMAKE_INSTALL_DATADIR)
      set(CMAKE_INSTALL_DATADIR ${CMAKE_INSTALL_PREFIX}/share)
    endif ()
    install(FILES rcdiscover-gui/resources/rcdiscover-gui.desktop
            COMPONENT gui
            DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
    install(FILES rcdiscover-gui/resources/logo_32.png
            COMPONENT gui
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/32x32/apps
            RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_64.png
            COMPONENT gui
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/64x64/apps
            RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_128.png
            COMPONENT gui
            DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/128x128/apps
            RENAME rcdiscover-gui.png)
    install(FILES rcdiscover-gui/resources/logo_32.png
            COMPONENT gui
            DESTINATION ${CMAKE_INSTALL_DATADIR}/pixmaps
            RENAME rcdiscover-gui.png)
  endif ()
endif(BUILD_RCDISCOVER_GUI)
