# Prerequisites
1) [Install Choco](https://chocolatey.org/install)
2) `choco install cmake.install --installargs '"ADD_CMAKE_TO_PATH=System"'`
3) Ensure cmake is added system path
4) [Install Visual Studio 2017 Build Tools](https://visualstudio.microsoft.com/vs/older-downloads/)
5) [Install Conan](https://conan.io/)
6) Generate Conan profile `conan profile new default --detect`
7) Check profile matches
    ```
    [settings]
    os=Windows
    os_build=Windows
    arch=x86_64
    arch_build=x86_64
    compiler=Visual Studio
    compiler.version=15
    compiler.runtime=MD
    build_type=Release
    [options]
    [build_requires]
    [env]
    ```
8) `cd cmake-build-release`
9) `conan install .. -s build_type=Release -s compiler="Visual Studio" -s compiler.runtime=MD -pr:b=default --build=missing`
   * **Note:** If this step throws errors, try deleting everything in `~/.conan/data/` folder and run again.

# Notes
## Reinstalling packages
Sometimes packages need to be reinstalled when switching between machines. Try this if the IDE cannot find files or there are build errors:
1) Clear cache: `conan remove '*' -s -b -f`
2) Remove packages: `conan remove '*'`
3) Navigate to build folder: `cd cmake-build-release`
4) Install packages: `conan install .. -s build_type=Release -s compiler="Visual Studio" -s compiler.runtime=MD -pr:b=default --build=missing`

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


# References
1) https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html#tests-unit
2) https://medium.com/swlh/cpm-an-awesome-dependency-manager-for-c-with-cmake-3c53f4376766
3) https://blog.jetbrains.com/clion/2019/05/getting-started-with-the-conan-clion-plugin/
4) https://support.kesslercrane.com/hc/en-us/articles/9569257629083-Cineshooter-API
5) https://gist.github.com/vladon/8b487e41cb3b49e172db