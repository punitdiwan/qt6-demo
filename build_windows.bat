@echo off
REM Native Windows build script for HelloQt6
REM Requirements: Qt6, CMake, and a C++ compiler (MSVC or MinGW) in PATH
REM
REM For MSVC, run this from a "Developer Command Prompt for VS".
REM For MinGW, ensure Qt's MinGW bin folder is in PATH.
REM
REM Usage:
REM   build_windows.bat [Release|Debug]
REM   build_windows.bat          <- defaults to Release

setlocal

set BUILD_TYPE=%1
if "%BUILD_TYPE%"=="" set BUILD_TYPE=Release

set BUILD_DIR=build-windows

echo [build] Configuration : %BUILD_TYPE%
echo [build] Output dir    : %BUILD_DIR%
echo.

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -B "%BUILD_DIR%" -DCMAKE_BUILD_TYPE=%BUILD_TYPE%
if errorlevel 1 (
    echo [error] CMake configuration failed.
    exit /b 1
)

cmake --build "%BUILD_DIR%" --config %BUILD_TYPE% --parallel
if errorlevel 1 (
    echo [error] Build failed.
    exit /b 1
)

echo.
echo [done] Binary: %BUILD_DIR%\%BUILD_TYPE%\HelloQt6.exe
echo        (Qt DLLs copied automatically by windeployqt if found)
endlocal
