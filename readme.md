# Installation
1) Install cmake `sudo apt install cmake`
2) Install g++ `sudo apt install g++`
3) Install libx11-dev `sudo apt install libx11-dev`
4) Install conan `pip install conan`
   * This step requires Python  (`sudo apt install python3`) and pip (`sudo apt install pip`).
5) Generate Conan profile `conan profile new default --detect`
6) Ensure profile (located at `~/.conan/profiles`) matches 
   ```
      [settings]
      os=Linux
      os_build=Linux
      arch=x86_64
      arch_build=x86_64
      compiler=gcc
      compiler.version=11
      compiler.libcxx=libstdc++
      build_type=Debug
      [options]
      [build_requires]
      [env]
    ```
7) `cd` to working directory
8) `mkdir build && cd build`
9) `conan install .. -s -pr:b=default --build=missing`
10) `cmake ..`
11) `sudo make install`
12) To include the library in another project, include the following in `CMakeLists.txt`:
    ```cmake
    find_package(PkgConfig)
    pkg_check_modules(KESSLER REQUIRED kessler)
    ```
   

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