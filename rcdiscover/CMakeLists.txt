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

project(rcdiscover CXX)
include(GNUInstallDirs)

if (UNIX)
  find_package( Threads REQUIRED)
endif ()

add_definitions(-DHAVE_PCAP)

set(rcdiscover_src
        deviceinfo.cc
        discover.cc
        force_ip.cc
        operation_not_permitted.cc
        wol_exception.cc
        socket_exception.cc
        ping.cc
        wol.cc
        gige_request_counter.cc
        )
set(rcdiscover_hh
        deviceinfo.h
        discover.h
        force_ip.h
        operation_not_permitted.h
        wol_exception.h
        socket_exception.h
        socket.h
        ping.h
        wol.h
        gige_request_counter.h
        utils.h)

if (WIN32)
  set(rcdiscover_src ${rcdiscover_src} socket_windows.cc)
  set(rcdiscover_hh ${rcdiscover_hh} socket_windows.h)
else (WIN32)
  set(rcdiscover_src ${rcdiscover_src} socket_linux.cc)
  set(rcdiscover_hh ${rcdiscover_hh} socket_linux.h)
endif (WIN32)

# Static library
add_library(rc_discover_static STATIC ${rcdiscover_src})
add_library(${PROJECT_NAMESPACE}::rcdiscover_static ALIAS rc_discover_static)

target_include_directories(rc_discover_static
        PUBLIC
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

# Install shared library
add_library(rc_discover SHARED ${rcdiscover_src})
add_library(${PROJECT_NAMESPACE}::rc_discover_shared ALIAS rc_discover)
target_include_directories(rc_discover
    PUBLIC
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
      $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
      $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>)

set_target_properties(rc_discover PROPERTIES SOVERSION ${abiversion})
target_compile_options(rc_discover PRIVATE $<$<CXX_COMPILER_ID:GNU>:-Wall>)
if (UNIX)
  target_link_libraries (rc_discover ${CMAKE_THREAD_LIBS_INIT})
endif(UNIX)


install(TARGETS rc_discover
        EXPORT PROJECTTargets
        COMPONENT bin
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${rcdiscover_hh} COMPONENT dev DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rc_discover)
