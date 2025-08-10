@echo off
setlocal enabledelayedexpansion

REM BootUSB Windows Build Script
REM This script builds BootUSB for Windows using CMake and MinGW

echo [INFO] BootUSB Windows Build Script
echo [INFO] =============================

REM Check if CMake is available
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Please install CMake from https://cmake.org/download/
    exit /b 1
)

REM Check if Qt is available
if not exist "C:\Qt" (
    if not exist "C:\Qt5" (
        echo [WARNING] Qt not found in standard locations.
        echo [INFO] Please ensure Qt5 is installed and QTDIR is set.
    )
)

REM Create build directory
if not exist "build-windows" mkdir build-windows
cd build-windows

REM Configure with CMake
echo [INFO] Configuring with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    exit /b 1
)

REM Build the project
echo [INFO] Building BootUSB...
cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [SUCCESS] Windows build completed successfully!
echo [INFO] Executable location: build-windows\bin\BootUSB.exe

cd ..

REM Create release package if requested
if "%1"=="--package" (
    echo [INFO] Creating release package...
    if not exist "release\windows" mkdir release\windows
    copy "build-windows\bin\BootUSB.exe" "release\windows\"
    if exist "README.md" copy "README.md" "release\windows\"
    if exist "CHANGELOG.md" copy "CHANGELOG.md" "release\windows\"
    echo [SUCCESS] Release package created for Windows
)

echo [SUCCESS] Build process completed successfully!
pause
