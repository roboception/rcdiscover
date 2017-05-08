# Author: Heiko Hirschmueller

project(tools CXX)

# build programs

add_executable(rcdiscover rcdiscover.cc)
target_link_libraries(rcdiscover rcdiscover_static)

if (WIN32)
  target_link_libraries(rcdiscover iphlpapi.lib ws2_32.lib)
endif ()

# install tools

install(TARGETS rcdiscover COMPONENT bin DESTINATION bin)
