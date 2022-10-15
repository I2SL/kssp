# Prerequisites
1) [Install Choco](https://chocolatey.org/install)
2) `choco install mingw`
3) Ensure mingw is added to system path
4) `choco install cmake.install --installargs '"ADD_CMAKE_TO_PATH=System"'`
5) Ensure cmake is added system path
6) [Install Conan](https://conan.io/)
7) Generate Conan profile `conan profile new default --detect`
8) Verify compiler version in Canon profile (`~/.conan/profiles/`)
9) `cd cmake-build-debug`
10) `conan install .. --build=missing`
    * **Note:** If this step throws errors, try deleting everything in `~/.conan/data/` folder and run again.

# References
1) https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html#tests-unit
2) https://medium.com/swlh/cpm-an-awesome-dependency-manager-for-c-with-cmake-3c53f4376766
3) https://blog.jetbrains.com/clion/2019/05/getting-started-with-the-conan-clion-plugin/
4) https://support.kesslercrane.com/hc/en-us/articles/210577103-HOW-TO-CineDrive-API-Instructions