cmake_minimum_required(VERSION 3.22)
project(kessler VERSION 0.1.0 DESCRIPTION "Library to drive the Kessler Second Shooter Pro")

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
include(GNUInstallDirs)
conan_basic_setup()

add_library(kessler SHARED
        kessler/src/stage.cpp
        kessler/src/responses/device_info.cpp
        kessler/src/responses/device_guid.cpp
        kessler/src/responses/motor_info.cpp
        kessler/src/responses/network_info.cpp
        kessler/src/responses/led_status.cpp
        kessler/src/notifications/motor_status.cpp
        kessler/src/notifications/playback_status.cpp
        kessler/src/notifications/error_status.cpp
        kessler/src/notifications/motor_position.cpp
        kessler/src/notifications/motor_calibrated.cpp
        kessler/src/notifications/aux_input_status.cpp
        kessler/src/notifications/unsupported_message.cpp)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})
target_link_libraries(kessler ${CONAN_LIBS} ${X11_LIBRARIES})

configure_file(kessler.pc.in kessler.pc @ONLY)
target_include_directories(kessler PRIVATE include)

install(TARGETS kessler
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY kessler/include/. DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kessler)

install(FILES ${CMAKE_BINARY_DIR}/kessler.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)