cmake_minimum_required(VERSION 3.22)
project(kessler)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(
        kessler
        kessler/stage.cpp
        kessler/stage.hpp
        kessler/utils.hpp
        kessler/calibrator.hpp
        kessler/consts.hpp
        kessler/pointer_utils.hpp
        kessler/responses/device_info.cpp
        kessler/responses/device_guid.cpp
        kessler/responses/motor_info.cpp
        kessler/responses/network_info.cpp
        kessler/responses/network_info.h
        kessler/responses/led_status.cpp
        kessler/responses/led_status.h
        kessler/notifications/motor_status.cpp
        kessler/notifications/motor_status.h
        kessler/notifications/playback_status.cpp
        kessler/notifications/playback_status.h
        kessler/notifications/error_status.cpp
        kessler/notifications/error_status.h
        kessler/notifications/motor_position.cpp
        kessler/notifications/motor_position.h
        kessler/notifications/motor_calibrated.cpp
        kessler/notifications/motor_calibrated.h
        kessler/notifications/aux_input_status.cpp
        kessler/notifications/aux_input_status.h
        kessler/notifications/unsupported_message.cpp
        kessler/notifications/unsupported_message.h)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

target_link_libraries(kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(basic_pointer tests/basic_pointer.cpp)
target_link_libraries(basic_pointer kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(pointer tests/pointer.cpp)
target_link_libraries(pointer kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(wasd_calibrator tests/wasd_calibrator.cpp)
target_link_libraries(wasd_calibrator kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(wasd_controller tests/wasd_controller.cpp)
target_link_libraries(wasd_controller kessler ${CONAN_LIBS} ${X11_LIBRARIES})

add_executable(libkessler-test kessler/stage.test.cpp)
target_link_libraries(libkessler-test kessler ${CONAN_LIBS} ${X11_LIBRARIES})


