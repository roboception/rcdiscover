
# Author: Heiko Hirschmueller

project(rcdiscover CXX)

add_definitions(-DHAVE_PCAP)

set(rcdiscover_src
  deviceinfo.cc
  discover.cc
  operation_not_permitted.cc
  wol_exception.cc
  socket_exception.cc
  ping.cc
  wol.cc
)

if (WIN32)
  set(rcdiscover_src ${rcdiscover_src} socket_windows.cc)
else (WIN32)
  set(rcdiscover_src ${rcdiscover_src} socket_linux.cc)
endif (WIN32)

add_library(rcdiscover_static STATIC ${rcdiscover_src})
