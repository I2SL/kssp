cmake_minimum_required(VERSION 3.22)
project(kessler)

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

set_target_properties(kessler PROPERTIES
        PUBLIC_HEADER kessler/include/stage.h kessler/include/tools/calibrator.h kessler/include/tools/pointer_utils.h)

configure_file(kessler.pc.in kessler.pc @ONLY)
target_include_directories(kessler PRIVATE .)

install(TARGETS kessler
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/kessler.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

add_executable(basic_pointer tests/basic_pointer.cpp)
target_link_libraries(basic_pointer kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(pointer tests/pointer.cpp)
target_link_libraries(pointer kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(wasd_calibrator tests/wasd_calibrator.cpp)
target_link_libraries(wasd_calibrator kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(wasd_controller tests/wasd_controller.cpp)
target_link_libraries(wasd_controller kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(libkessler-test kessler/src/stage.test.cpp)
target_link_libraries(libkessler-test kessler ${CONAN_LIBS} ${X11_LIBRARIES})


