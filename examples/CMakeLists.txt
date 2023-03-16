cmake_minimum_required(VERSION 3.22)
project(kessler-examples)

set(CMAKE_CXX_STANDARD 20)

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(fmt 9.1.0 REQUIRED)
include_directories(${fmt_INCLUDE_DIR})
link_directories(${fmt_LIBRARIES})

find_package(Boost 1.81.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARIES})

find_package(nlohmann_json 3.11.2 REQUIRED)
include_directories(${nlohmann_json_INCLUDE_DIR})
link_directories(${nlohmann_json_LIBRARIES})

find_package(PkgConfig)
pkg_check_modules (KESSLER REQUIRED kessler)

add_executable(basic_pointer basic_pointer.cpp)
target_link_libraries(basic_pointer ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(pointer pointer.cpp)
target_link_libraries(pointer ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(wasd_controller wasd_controller.cpp)
target_link_libraries(wasd_controller ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})

add_executable(calibrator calibrator.cpp)
target_link_libraries(calibrator ${nlohmann_json_LIBRARIES} ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})