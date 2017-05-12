# Author: Heiko Hirschmueller

project(tools CXX)

# build programs

add_executable(rcdiscover rcdiscover.cc)
target_link_libraries(rcdiscover rcdiscover_static)

if (WIN32)
  target_link_libraries(rcdiscover iphlpapi.lib ws2_32.lib)
endif (WIN32)

if(wxWidgets_FOUND)
    include_directories(${wxWidgets_INCLUDE_DIRS})
    add_executable(rcdiscover-gui rcdiscover-gui.cc)
    target_link_libraries(rcdiscover-gui rcdiscover_static ${wxWidgets_LIBRARIES})

    if (WIN32)
        target_link_libraries(rcdiscover-gui iphlpapi.lib ws2_32.lib)
    endif (WIN32)
else(wxWidgets_FOUND)
    MESSAGE(WARN "wxWidgets not found! Not building GUI application")
endif(wxWidgets_FOUND)



# install tools

install(TARGETS rcdiscover COMPONENT bin DESTINATION bin)
