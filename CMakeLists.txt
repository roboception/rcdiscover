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

cmake_minimum_required (VERSION 2.8.12)

project(rcdiscover)
include(cmake/project_version.cmake)

enable_testing()

# - Find manditory packages -

find_package(Threads REQUIRED)

set(wxWidgets_USE_STATIC ON)
set(wxWidgets_USE_UNICODE ON)
find_package(wxWidgets COMPONENTS html adv core base)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

# - Configuration for code optimization -

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RELEASE CACHE STRING "Build type: DEBUG or RELEASE" FORCE)
endif ()

# - Standard definitions -

if (CMAKE_COMPILER_IS_GNUCC)
  add_definitions(-Wall)
  add_definitions(-Wextra)
  add_definitions(-Wpedantic)
  add_definitions(-Wconversion)
  add_definitions(-Werror)
endif ()

if (CMAKE_MAJOR_VERSION VERSION_LESS "3.1.0")
  add_definitions(-std=c++11)
else ()
  set(CMAKE_CXX_STANDARD 11)
endif ()

if (WIN32)
  add_definitions(-DWIN32)
  set(CMAKE_C_FLAGS "-mtune=generic -mno-abm")
  set(CMAKE_CXX_FLAGS "-mtune=generic -mno-abm")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0")
  set(CMAKE_CXX_FLAGS_RELEASE "-O2")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -static-libstdc++ -static")
endif ()

# - Build individual parts -

add_subdirectory(rcdiscover)

# by default install commandline and gui tools
option(INSTALL_RCDISCOVER_CLI "install rcdiscover commandline tool" ON)
option(INSTALL_RCDISCOVER_GUI "install rcdiscover-gui GUI tool" ON)

if (UNIX)
  # workaround to build two debian packages: rcdiscover and rcdiscover-gui:
  # - building package without extra cmake options builds one debian package with everything
  # - building package with -DRCDISCOVER_CLI_ONLY_DEB=ON builds rcdiscover-cli package only with cmdline tool
  # - building package with -DRCDISCOVER_GUI_ONLY_DEB=ON will build the rcdiscover-gui deb which recommends rcdiscover-cli
  option(RCDISCOVER_CLI_ONLY_DEB "build debian package 'rcdiscover' with only cmdline tool" OFF)
  option(RCDISCOVER_GUI_ONLY_DEB "build debian package 'rcdiscover-gui' with only gui tool" OFF)

  if (RCDISCOVER_CLI_ONLY_DEB AND RCDISCOVER_GUI_ONLY_DEB)
    message(FATAL_ERROR "Can't enable RCDISCOVER_CLI_ONLY_DEB and RCDISCOVER_GUI_ONLY_DEB at the same time!")
  endif ()

  if (RCDISCOVER_CLI_ONLY_DEB)
    # build the rcdiscover debian package
    # force installing of only the cmdline tool
    set(INSTALL_RCDISCOVER_CLI ON CACHE BOOL "Override option" FORCE)
    set(INSTALL_RCDISCOVER_GUI OFF CACHE BOOL "Override option" FORCE)
  endif()

  if (RCDISCOVER_GUI_ONLY_DEB)
    # build rcdiscover-gui debian package
    # force installing of only gui tool
    set(INSTALL_RCDISCOVER_CLI OFF CACHE BOOL "Override option" FORCE)
    set(INSTALL_RCDISCOVER_GUI ON CACHE BOOL "Override option" FORCE)
  endif ()
endif ()

add_subdirectory(tools)

# add a target to generate API documentation with Doxygen

add_subdirectory(doc)

# - Define information for packaging -

if (RCDISCOVER_GUI_ONLY_DEB)
  set(CPACK_PACKAGE_NAME "rcdiscover-gui")
  set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "rcdiscover-cli")
  set(CPACK_DEBIAN_PACKAGE_CONFLICTS "rcdiscover")
endif ()

if (RCDISCOVER_CLI_ONLY_DEB)
  set(CPACK_PACKAGE_NAME "rcdiscover-cli")
  set(CPACK_DEBIAN_PACKAGE_CONFLICTS "rcdiscover")
endif ()


  # don't install rpfilter sysctl for gui package (would conflict with cli package)
if (UNIX AND NOT RCDISCOVER_GUI_ONLY_DEB)
  # install example sysctl config to disable reverse path filtering
  install(FILES debian/50-rcdiscover-rpfilter.conf DESTINATION ${CMAKE_INSTALL_PREFIX}/share/rcdiscover)
  # ask if this config should be applied when installing this as debian package
  set(CONFIG_FILE "${PROJECT_BINARY_DIR}/config")
  set(POSTINST_FILE "${PROJECT_BINARY_DIR}/postinst")
  set(POSTRM_FILE "${PROJECT_BINARY_DIR}/postrm")
  set(TEMPLATES_FILE "${PROJECT_BINARY_DIR}/templates")
  configure_file(${PROJECT_SOURCE_DIR}/debian/config.in ${CONFIG_FILE})
  configure_file(${PROJECT_SOURCE_DIR}/debian/postinst.in ${POSTINST_FILE})
  configure_file(${PROJECT_SOURCE_DIR}/debian/postrm.in ${POSTRM_FILE})
  configure_file(${PROJECT_SOURCE_DIR}/debian/templates.in ${TEMPLATES_FILE})
  execute_process(COMMAND chmod 755 "${CONFIG_FILE}")
  execute_process(COMMAND chmod 755 "${POSTINST_FILE}")
  execute_process(COMMAND chmod 755 "${POSTRM_FILE}")
  execute_process(COMMAND chmod 644 "${TEMPLATES_FILE}")
  set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${POSTINST_FILE};${POSTRM_FILE};${TEMPLATES_FILE};${CONFIG_FILE}")

  # optionally specify dependencies of the debian package here (comma separated!)
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "debconf")
endif ()

include(cmake/package_debian.cmake)
