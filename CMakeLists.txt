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

cmake_minimum_required (VERSION 3.1)

project(rcdiscover)

include(cmake/project_version.cmake)
include(GNUInstallDirs)

# For CMake Version >= 3.0: issue FATAL_ERROR if link dependency contains
# double-colons but is not an imported target.
if (POLICY CMP0028)
  cmake_policy(SET CMP0028 NEW)
endif ()

enable_testing()


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

if (CMAKE_VERSION VERSION_LESS "3.1.0")
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

option(BUILD_RCDISCOVER_CLI "build rcdiscover commandline tool" ON)
option(BUILD_RCDISCOVER_SHARED_LIB "build rcdiscover shared library" ON)
option(BUILD_RCDISCOVER_GUI "build rcdiscover-gui GUI tool" OFF)

if (WIN32)
  set(BUILD_RCDISCOVER_SHARED_LIB OFF CACHE BOOL "Override option" FORCE)
endif ()

# - Build individual parts -
add_subdirectory(rcdiscover)
add_subdirectory(tools)

# - Define information for packaging -
if (BUILD_RCDISCOVER_SHARED_LIB)
  set(PROJECT_LIBRARIES rcdiscover)
  install(FILES package.xml COMPONENT bin DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME_LOWER})
endif()

include(cmake/configure_link_libs.cmake)


if (UNIX)
  # install example sysctl config to disable reverse path filtering
  install(FILES debian/50-rcdiscover-rpfilter.conf COMPONENT bin DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/rcdiscover)
endif ()

include(cmake/package_debian.cmake)
