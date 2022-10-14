cmake_minimum_required(VERSION 3.22)
project(libkessler)

set(CMAKE_CXX_STANDARD 14)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
add_library(libkessler libkessler/stage.cpp)
target_link_libraries(libkessler wsock32 ws2_32)
target_link_libraries(libkessler ${CONAN_LIBS})

add_executable(driver tests/driver.cpp)
target_link_libraries(driver libkessler)

add_executable(libkessler-test libkessler/stage.test.cpp)
target_link_libraries(libkessler-test libkessler ${CONAN_LIBS})
