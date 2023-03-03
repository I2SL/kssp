# Installation
1) Install cmake `sudo apt install cmake`
2) Install gcc-11 `sudo apt install gcc-11`
3) Install g++-11 `sudo apt install g++-11`
4) Install libx11-dev `sudo apt install libx11-dev`
5) Install conan `pip install conan`
   * This step requires Python  (`sudo apt install python3`) and pip (`sudo apt install pip`).
6) Generate Conan profile `conan profile new default --detect`
7) Ensure profile (located at `~/.conan/profiles`) matches 
   ```
      [settings]
      os=Linux
      os_build=Linux
      arch=x86_64
      arch_build=x86_64
      compiler=gcc
      compiler.version=11
      compiler.libcxx=libstdc++11
      build_type=Debug
      [options]
      [build_requires]
      [env]
    ```
8) `cd` to working directory
9) `mkdir build && cd build`
10) `conan install .. -s -pr:b=default --build=missing`
11) `cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 ..`
12) `sudo make install`

To include the library in another project, include the following in `CMakeLists.txt`:
```cmake
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(PkgConfig)
pkg_check_modules(KESSLER REQUIRED kessler)
```
Then, link the `kessler` and `X11` libraries:
```cmake
add_executable(example main.cpp)
target_link_libraries(example ${X11_LIBRARIES} ${KESSLER_LIBRARIES})
```
Note that the `kessler` library also requires `boost 1.81.0` and `fmt 9.1.0` to be linked as well. If using Conan, add
```cmake
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()
```
to `CMakeLists.txt`. Then, link the Conan libraries alongside the others:
```cmake
add_executable(example main.cpp)
target_link_libraries(example ${CONAN_LIBS} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})
```
The `conanfile.txt` in the root directory should include
```
[requires]
boost/1.81.0
fmt/9.1.0

[generators]
cmake
```
The project can then be built by running
```
mkdir build && cd build
conan install .. -s -pr:b=default --build=missing
cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 ..
make
```
See the `examples` directory for executables built using CMake and Conan.

# Building Examples 
Using the same Conan prerequisites as above:
1) `cd` into the `examples` folder
2) `mkdir build && cd build`
3) `conan install .. -s -pr:b=default --build=missing`
4) `cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 ..`
5) `make`

# Notes

## Headers
The most useful include paths are:
* `kessler/stage.h`
* `kessler/tools/calibrator.h`
* `kessler/tools/pointer_utils.h`
Note that `calibrator.h` includes both `stage.h` and `pointer_utils.h`.

## Float data type
This implementation uses the `boost` endian library, which does not yet include load and store functions for `float` types. All values that should be floats are read in as `int32` and reinterpreted as floats.

## Not Implemented
The following features are not yet implemented:

### Get/Set Messages
* **NetworkInfo**
  * `Set`
  * `Get` with `LimitedResponse = 1`parameter.
* **TrajectoryData**
* **SoftwareID**

### Action Messages
* **StartPlayback**
* **StopPlayback**
* **RapidToFirstKeyFrame**
* **TakePicture**
* **SetupPlayback**
* **BumpPositionSpeedAcceleration**

## Known Issues
1) When running firmware `1.0.1.5`, when issuing a position command the effective `beginPosition` is 0, and the effective `endPosition` is `endPosition` - `beginPosition`. The device will still return a position in between `beginPosition` and `endPosition` when it reaches the target.

## Examples
The `examples` directory contains executables related to running the stage.
### Executables
* `basic_pointer.cpp`
* `pointer.cpp`
* `wasd_calibrator.cpp`
* `wasd_controller.cpp`

# References
1) https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html#tests-unit
2) https://medium.com/swlh/cpm-an-awesome-dependency-manager-for-c-with-cmake-3c53f4376766
3) https://blog.jetbrains.com/clion/2019/05/getting-started-with-the-conan-clion-plugin/
4) https://support.kesslercrane.com/hc/en-us/articles/9569257629083-Cineshooter-API
5) https://gist.github.com/vladon/8b487e41cb3b49e172db
6) https://gist.github.com/Yousha/3830712334ac30a90eb6041b932b68d7