cmake_minimum_required(VERSION 3.22)
project(kssp VERSION 1.0.3 DESCRIPTION "Library to drive the Kessler Second Shooter Pro")

set(CMAKE_CXX_STANDARD 20)

include(GNUInstallDirs)

add_library(kssp SHARED
        kssp/src/stage.cpp
        kssp/src/responses/device_info.cpp
        kssp/src/responses/device_guid.cpp
        kssp/src/responses/motor_info.cpp
        kssp/src/responses/network_info.cpp
        kssp/src/responses/led_status.cpp
        kssp/src/notifications/motor_status.cpp
        kssp/src/notifications/playback_status.cpp
        kssp/src/notifications/error_status.cpp
        kssp/src/notifications/motor_position.cpp
        kssp/src/notifications/motor_calibrated.cpp
        kssp/src/notifications/aux_input_status.cpp
        kssp/src/notifications/unsupported_message.cpp)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(fmt 9.1.0 REQUIRED)
include_directories(${fmt_INCLUDE_DIR})
link_directories(${fmt_LIBRARIES})

find_package(Boost 1.81.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARIES})

target_link_libraries(kssp ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES})

configure_file(kssp.pc.in kssp.pc @ONLY)
target_include_directories(kssp PRIVATE include)

install(TARGETS kssp
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(DIRECTORY kssp/include/. DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/kssp)

install(FILES ${CMAKE_BINARY_DIR}/kssp.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)