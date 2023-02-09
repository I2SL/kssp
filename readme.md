# Prerequisites
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
      compiler.version=9
      compiler.libcxx=libstdc++11
      build_type=Debug
      [options]
      [build_requires]
      [env]
    ```
7) `cd cmake-build-debug`
8) `conan install .. -s -pr:b=default --build=missing`
   

# Notes

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
1) When running firmware `1.0.1.5`, the effective `beginPosition` is 0, and the effective `endPosition` is `endPosition` - `beginPosition`.

## Tests
The `tests` directory contains executables related to running the stage and tracking objects. This brings in packages that are not required for operating the stage, so eventually this will be split into a separate project.
### Executables
* `basic_pointer.cpp`
* `live_tracker.cpp`
* `pointer.cpp`
* `wasd_calibrator.cpp`
* `wasd_controller.cpp`

### Auxiliary Files
* `calibrator.hpp`
* `ebs_tracking_utils.hpp`
* `pointer_utils.hpp`

# References
1) https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html#tests-unit
2) https://medium.com/swlh/cpm-an-awesome-dependency-manager-for-c-with-cmake-3c53f4376766
3) https://blog.jetbrains.com/clion/2019/05/getting-started-with-the-conan-clion-plugin/
4) https://support.kesslercrane.com/hc/en-us/articles/9569257629083-Cineshooter-API
5) https://gist.github.com/vladon/8b487e41cb3b49e172db
6) http://ceres-solver.org/