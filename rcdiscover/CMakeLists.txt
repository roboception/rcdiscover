
# Author: Heiko Hirschmueller

project(rcdiscover CXX)

set(rcdiscover_src
  deviceinfo.cc
  discover.cc
)

add_library(rcdiscover_static STATIC ${rcdiscover_src})