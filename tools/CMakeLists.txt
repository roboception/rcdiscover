
# Author: Heiko Hirschmueller

project(tools CXX)

# build programs

add_executable(rcdiscover rcdiscover.cc)
target_link_libraries(rcdiscover rcdiscover_static)

# install tools

install(TARGETS rcdiscover COMPONENT bin DESTINATION bin)