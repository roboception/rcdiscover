
# Author: Heiko Hirschmueller

project(rcdiscover CXX)

add_definitions(-DHAVE_PCAP)

set(rcdiscover_src
  deviceinfo.cc
  discover.cc
  wol.cc
  operation_not_permitted.cc
  wol_exception.cc
)

if (WIN32)
  set(rcdiscover_src ${rcdiscover_src} wol_windows.cc)
else (WIN32)
  set(rcdiscover_src ${rcdiscover_src} wol_linux.cc)
endif (WIN32)

add_library(rcdiscover_static STATIC ${rcdiscover_src})
