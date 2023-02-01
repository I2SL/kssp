cmake_minimum_required(VERSION 3.22)
project(libkessler)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_library(
        libkessler libkessler/stage.cpp
        libkessler/responses/device_info.cpp
        libkessler/responses/device_guid.cpp
        libkessler/responses/motor_info.cpp
        libkessler/responses/network_info.cpp
        libkessler/responses/network_info.h
        libkessler/responses/led_status.cpp
        libkessler/responses/led_status.h
        libkessler/notifications/motor_status.cpp
        libkessler/notifications/motor_status.h
        libkessler/notifications/playback_status.cpp
        libkessler/notifications/playback_status.h
        libkessler/notifications/error_status.cpp
        libkessler/notifications/error_status.h
        libkessler/notifications/motor_position.cpp
        libkessler/notifications/motor_position.h
        libkessler/notifications/motor_calibrated.cpp
        libkessler/notifications/motor_calibrated.h
        libkessler/notifications/aux_input_status.cpp
        libkessler/notifications/aux_input_status.h
        libkessler/notifications/unsupported_message.cpp
        libkessler/notifications/unsupported_message.h)
target_link_libraries(libkessler wsock32 ws2_32)
target_link_libraries(libkessler ${CONAN_LIBS})

add_executable(basic_pointer tests/basic_pointer.cpp)
target_link_libraries(basic_pointer libkessler ${CONAN_LIBS})

add_executable(pointer tests/pointer.cpp)
target_link_libraries(pointer libkessler ${CONAN_LIBS})

add_executable(wasd_calibrator tests/wasd_calibrator.cpp)
target_link_libraries(wasd_calibrator libkessler ${CONAN_LIBS})

add_executable(wasd_controller tests/wasd_controller.cpp)
target_link_libraries(wasd_controller libkessler ${CONAN_LIBS})

add_executable(live_tracker tests/live_tracker.cpp)
target_link_libraries(live_tracker libkessler ${CONAN_LIBS})

add_executable(libkessler-test libkessler/stage.test.cpp)
target_link_libraries(libkessler-test libkessler ${CONAN_LIBS})


