cmake_minimum_required(VERSION 3.22)
project(kessler-examples)

set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(PkgConfig)
pkg_check_modules (KESSLER REQUIRED kessler)

add_executable(basic_pointer basic_pointer.cpp)
target_link_libraries(basic_pointer ${CONAN_LIBS} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(pointer pointer.cpp)
target_link_libraries(pointer ${CONAN_LIBS} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(wasd_calibrator wasd_calibrator.cpp)
target_link_libraries(wasd_calibrator ${CONAN_LIBS} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(wasd_controller wasd_controller.cpp)
target_link_libraries(wasd_controller ${CONAN_LIBS} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})