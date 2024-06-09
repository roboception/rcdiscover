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
  set(FLTK_SKIP_OPENGL ON)
  set(FLTK_SKIP_FORMS ON)
  set(FLTK_SKIP_IMAGES ON)
  set(FLTK_SKIP_FLUID ON)
  find_package(FLTK REQUIRED)

  include_directories(${FLTK_INCLUDE_DIR})

  set(rcdiscover-gui-src
    rcdiscover-gui/discover_window.cc
    rcdiscover-gui/device_list.cc
    rcdiscover-gui/reset_window.cc
    rcdiscover-gui/set_tmp_ip_window.cc
    rcdiscover-gui/reconnect_window.cc
    rcdiscover-gui/help_window.cc
    rcdiscover-gui/about_dialog.cc
    rcdiscover-gui/device_choice.cc
    rcdiscover-gui/input_mac.cc
    rcdiscover-gui/input_ip.cc
    rcdiscover-gui/input_filter.cc
    rcdiscover-gui/logo.cc
    rcdiscover-gui.cc)

  if (WIN32)
    add_executable(rcdiscover-gui WIN32 ${rcdiscover-gui-src})
  else (WIN32)
    add_executable(rcdiscover-gui ${rcdiscover-gui-src})
  endif (WIN32)

  target_link_libraries(rcdiscover-gui ${PROJECT_NAMESPACE}::rcdiscover_static ${FLTK_LIBRARIES})

  if (WIN32)
    target_link_libraries(rcdiscover-gui gdiplus)
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
