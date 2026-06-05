# Cross-compilation toolchain: Linux host → Windows x64 target (MinGW-w64)
#
# Prerequisites on Ubuntu/Debian:
#   sudo apt install gcc-mingw-w64-x86-64 g++-mingw-w64-x86-64 mingw-w64-tools
#
# You also need Qt6 built for MinGW. Install via the Qt online installer
# (select "MinGW 11.2.0 64-bit" under Qt 6.x) then point QT_ROOT below.
#
# Usage:
#   cmake -B build-win -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw64.cmake \
#         -DQT_ROOT=/path/to/Qt/6.x.x/mingw_64
#   cmake --build build-win --parallel

set(CMAKE_SYSTEM_NAME    Windows)
set(CMAKE_SYSTEM_VERSION 10)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

# MinGW compilers
set(CMAKE_C_COMPILER   x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER  x86_64-w64-mingw32-windres)

# Sysroot / find rules
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Point to your Qt6-for-Windows install (override with -DQT_ROOT=...)
if(DEFINED QT_ROOT)
    list(APPEND CMAKE_PREFIX_PATH "${QT_ROOT}")
    set(CMAKE_FIND_ROOT_PATH "${QT_ROOT}" ${CMAKE_FIND_ROOT_PATH})
endif()
