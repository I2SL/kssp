This library is an implementation of the [Cineshooter API](https://support.kesslercrane.com/hc/en-us/articles/9569257629083-Cineshooter-API), written to control a Kessler Second Shooter Pro. See below for installation instructions, usage, and notes of missing features/known issues.

# Installation
1) Install cmake `sudo apt install cmake`
2) Install gcc-11 
   * `sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test`
   * `sudo apt install gcc-11`
3) Install g++-11 `sudo apt install g++-11`
4) Install libx11-dev `sudo apt install libx11-dev`
5) Install conan `pip install conan`
   * This step requires Python  (`sudo apt install python3`) and pip (`sudo apt install pip`).
6) Generate Conan profile `conan profile detect --force`
7) Ensure profile (located at `~/.conan2/profiles`) matches 
   ```
    [settings]
    arch=x86_64
    build_type=Debug
    compiler=gcc
    compiler.cppstd=gnu14
    compiler.libcxx=libstdc++11
    compiler.version=11
    os=Linux
    ```
8) `cd` to working directory
9) `conan install . -pr:b=default --output-folder=build --build=missing`
10) `cd build`
11) `cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..`
12) `sudo make install`

To include the library in another project, include the following in `CMakeLists.txt`:
```cmake
find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
link_directories(${X11_LIBRARIES})

find_package(fmt 9.1.0 REQUIRED)
include_directories(${fmt_INCLUDE_DIR})
link_directories(${fmt_LIBRARIES})

find_package(Boost 1.81.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARIES})

find_package(PkgConfig)
pkg_check_modules(KESSLER REQUIRED kessler)
```
Then, link the `kessler`, `Boost`, `fmt`, and `X11` libraries:
```cmake
add_executable(example main.cpp)
target_link_libraries(example ${Boost_LIBRARIES} ${fmt_LIBRARIES} ${X11_LIBRARIES} ${KESSLER_LIBRARIES})
```
If using Conan, the `conanfile.txt` file in the root directory should include
```
[requires]
boost/1.81.0
fmt/9.1.0

[generators]
CMakeDeps
CMakeToolchain
```
The project can then be built by running
```
conan install . -pr:b=default --output-folder=build --build=missing
cd build
cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake ..
make
```
See the `examples` directory for executables built using CMake and Conan.

# Building Examples 
Using the same Conan prerequisites as above:
1) `cd` into the `examples` folder
2) `conan install . -pr:b=default --output-folder=build --build=missing`
3) `cd build`
4) `cmake -DCMAKE_C_COMPILER=gcc-11 -DCMAKE_CXX_COMPILER=g++-11 -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug ..`
5) `make`

# Usage
TODO: write an example and explanation
## Headers
The most useful include paths are:
* `kessler/stage.h`
* `kessler/tools/calibrator.h`
* `kessler/tools/pointer_utils.h`
  Note that `calibrator.h` includes both `stage.h` and `pointer_utils.h`.

# Notes

## Float data type
This implementation uses the `boost` endian library, which does not yet include load and store functions for `float` types. All values that should be floats are read in as `int32` and reinterpreted as floats through `memcpy`.

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
2) The provided examples crash with a end of file exception when used with firmware `1.0.1.16`. This library was built and tested using firmware `1.0.1.5`.

## Examples
The `examples` directory contains executables related to running the stage.
### Executables
TODO: add descriptions of each
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