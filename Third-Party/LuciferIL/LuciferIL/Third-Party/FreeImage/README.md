# FreeImage-CMake
[![Linux](https://github.com/Max-ChenFei/FreeImage-CMake/actions/workflows/cmake_build_unittest_linux.yml/badge.svg?branch=main&event=push)](https://github.com/Max-ChenFei/FreeImage-CMake/actions/workflows/cmake_build_unittest_linux.yml)
[![Windows](https://github.com/Max-ChenFei/FreeImage-CMake/actions/workflows/cmake_build_unittest_Windows.yml/badge.svg?branch=main&event=push)](https://github.com/Max-ChenFei/FreeImage-CMake/actions/workflows/cmake_build_unittest_Windows.yml)

This repository is a fork of the FreeImage that supports cmake building.
[FreeImage](https://freeimage.sourceforge.io/) is an Open Source library project for developers who would like to support popular graphics image formats like PNG, BMP, JPEG, TIFF and others as needed by today's multimedia applications. FreeImage is easy to use, fast, multithreading safe, compatible with all 32-bit or 64-bit versions of Windows, and cross-platform (works both with Linux and Mac OS X).

[CMake](https://cmake.org/) is a cross-platform, open source tool for defining build processes that run on multiple platforms. 

## Building

### Windows
set <build_type> Debug or Release
```
cd <path-to-source-directory>
# CMake discourages in-source build, so create a separate build directory
cmake -E make_directory build
cd build
# Generate a buildsystem for Windows
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_CONFIGURATION_TYPES:STRING=<build_type> 
# Build using MSBuild
msbuild -m -p:"Configuration=<build_type>;Platform=x64" ALL_BUILD.vcxproj
# Test
ctest -C <build_type>
```
### Linux
set <build_type> Debug or Release
```
cd <path-to-source-directory>
cmake -E make_directory build
cd build
cmake .. -G "Unix Makefiles" -DCMAKE_CONFIGURATION_TYPES:STRING=<build_type> 
cmake --build . --config <build_type>
ctest -C <build_type>
```

## Development
I use Visual Studio for C++ cross platform development on Windows and Linux[1]. The [**CMakeSettings.json**](https://docs.microsoft.com/en-us/cpp/build/cmakesettings-reference?view=msvc-160) file contains information that Visual Studio uses for IntelliSense and to construct the command-line arguments that it passes to cmake.exe for a specified configuration and compiler environment.  Here the local machine is Windows and the remote Linux server is connected in Visual Studio for building[2].

## Reference

[1] [CMake projects in Visual Studio](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio?view=msvc-160) and [Build Linux projects with CMake in Visual studio](https://docs.microsoft.com/en-us/cpp/linux/cmake-linux-project?view=msvc-160).

[2] [Connect to your target Linux system in Visual studio](https://docs.microsoft.com/en-us/cpp/linux/connect-to-your-remote-linux-computer?view=msvc-160)
