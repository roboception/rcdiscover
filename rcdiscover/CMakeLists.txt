
# Author: Heiko Hirschmueller

project(rcdiscover CXX)

add_definitions(-DHAVE_PCAP)

set(rcdiscover_src
  deviceinfo.cc
  discover.cc
  wol.cc
  wol_linux.cc
  operation_not_permitted.cc
  wol_exception.cc
)

add_library(rcdiscover_static STATIC ${rcdiscover_src})
