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

    set(rcdiscover-gui-src
        rcdiscover-gui.cc
        rcdiscover-gui/event_ids.cc
        rcdiscover-gui/discover-frame.cc
        rcdiscover-gui/discover-thread.cc
        rcdiscover-gui/reset-dialog.cc)

    if (WIN32)
	if(MINGW)
            set(CMAKE_RC_COMPILER_INIT windres)
	    ENABLE_LANGUAGE(RC)
	    SET(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> -i <SOURCE> -o <OBJECT> <INCLUDES>")
	endif(MINGW)

        add_executable(rcdiscover-gui ${rcdiscover-gui-src} rcdiscover-gui/resources/resources.rc)

	# avoid console window opening in background
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows -static-libgcc -static-libstdc++")
    else (WIN32)
        add_executable(rcdiscover-gui ${rcdiscover-gui-src})
    endif(WIN32)

    target_link_libraries(rcdiscover-gui rcdiscover_static ${wxWidgets_LIBRARIES})

    if (WIN32)
        target_link_libraries(rcdiscover-gui iphlpapi.lib ws2_32.lib)
    endif (WIN32)
else(wxWidgets_FOUND)
    MESSAGE(WARN "wxWidgets not found! Not building GUI application")
endif(wxWidgets_FOUND)



# install tools

install(TARGETS rcdiscover COMPONENT bin DESTINATION bin)
