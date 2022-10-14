# Prerequisites
1) [Install CMake](https://cmake.org/install/)
2) [Install Choco](https://chocolatey.org/install)
3) `choco install mingw`
4) Install Conan [3]
5) Generate Conan profile `conan profile new default --detect`
6) Verify compiler version in Canon profile
7) `mkdir build`
8) `cd build`
9) `conan install .. --build`

# References
1) https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p1204r0.html#tests-unit
2) https://medium.com/swlh/cpm-an-awesome-dependency-manager-for-c-with-cmake-3c53f4376766
3) https://blog.jetbrains.com/clion/2019/05/getting-started-with-the-conan-clion-plugin/