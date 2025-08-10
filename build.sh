#!/bin/bash

# BootUSB Cross-Platform Build Script
# Supports Linux, Windows (with MinGW), and macOS

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to detect platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "macos";;
        CYGWIN*|MINGW*|MSYS*) echo "windows";;
        *)          echo "unknown";;
    esac
}

# Function to check dependencies
check_dependencies() {
    local platform=$1
    
    print_status "Checking dependencies for $platform..."
    
    case $platform in
        "linux")
            # Check for Qt5
            if ! pkg-config --exists Qt5Core Qt5Widgets; then
                print_error "Qt5 not found. Please install Qt5 development packages."
                print_status "Ubuntu/Debian: sudo apt-get install qt5-default qtbase5-dev"
                print_status "Fedora: sudo dnf install qt5-qtbase-devel"
                exit 1
            fi
            
            # Check for libudev
            if ! pkg-config --exists libudev; then
                print_error "libudev not found. Please install udev development packages."
                print_status "Ubuntu/Debian: sudo apt-get install libudev-dev"
                print_status "Fedora: sudo dnf install systemd-devel"
                exit 1
            fi
            
            # Check for CMake
            if ! command -v cmake &> /dev/null; then
                print_error "CMake not found. Please install CMake."
                print_status "Ubuntu/Debian: sudo apt-get install cmake"
                print_status "Fedora: sudo dnf install cmake"
                exit 1
            fi
            ;;
            
        "macos")
            # Check for Qt5
            if ! brew list qt5 &> /dev/null; then
                print_error "Qt5 not found. Please install Qt5 using Homebrew."
                print_status "brew install qt5"
                exit 1
            fi
            
            # Check for CMake
            if ! command -v cmake &> /dev/null; then
                print_error "CMake not found. Please install CMake."
                print_status "brew install cmake"
                exit 1
            fi
            ;;
            
        "windows")
            # Check for Qt5 (assuming Qt is installed in default location)
            if [ ! -d "/c/Qt" ] && [ ! -d "/mingw64/qt5" ]; then
                print_warning "Qt5 not found in standard locations."
                print_status "Please ensure Qt5 is installed and QTDIR is set."
            fi
            
            # Check for CMake
            if ! command -v cmake &> /dev/null; then
                print_error "CMake not found. Please install CMake."
                print_status "Download from: https://cmake.org/download/"
                exit 1
            fi
            ;;
    esac
    
    print_success "Dependencies check passed for $platform"
}

# Function to build for Linux
build_linux() {
    print_status "Building for Linux..."
    
    mkdir -p build-linux
    cd build-linux
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local
    
    make -j$(nproc)
    
    print_success "Linux build completed successfully!"
    print_status "Executable location: build-linux/bin/bootusb"
    
    cd ..
}

# Function to build for macOS
build_macos() {
    print_status "Building for macOS..."
    
    mkdir -p build-macos
    cd build-macos
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_PREFIX_PATH="$(brew --prefix qt5)"
    
    make -j$(sysctl -n hw.ncpu)
    
    print_success "macOS build completed successfully!"
    print_status "Executable location: build-macos/bin/BootUSB"
    
    cd ..
}

# Function to build for Windows
build_windows() {
    print_status "Building for Windows..."
    
    mkdir -p build-windows
    cd build-windows
    
    # Try to find Qt installation
    local qt_path=""
    if [ -d "/c/Qt" ]; then
        qt_path="/c/Qt"
    elif [ -d "/mingw64/qt5" ]; then
        qt_path="/mingw64/qt5"
    fi
    
    if [ -n "$qt_path" ]; then
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local \
            -DCMAKE_PREFIX_PATH="$qt_path"
    else
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local
    fi
    
    make -j$(nproc)
    
    print_success "Windows build completed successfully!"
    print_status "Executable location: build-windows/bin/BootUSB.exe"
    
    cd ..
}

# Function to create release package
create_release_package() {
    local platform=$1
    
    print_status "Creating release package for $platform..."
    
    case $platform in
        "linux")
            mkdir -p release/linux
            cp build-linux/bin/bootusb release/linux/
            cp bootusb.desktop.in release/linux/
            cp README.md release/linux/ 2>/dev/null || true
            cp CHANGELOG.md release/linux/ 2>/dev/null || true
            ;;
            
        "macos")
            mkdir -p release/macos
            cp -r build-macos/bin/BootUSB.app release/macos/ 2>/dev/null || cp build-macos/bin/BootUSB release/macos/
            cp README.md release/macos/ 2>/dev/null || true
            cp CHANGELOG.md release/macos/ 2>/dev/null || true
            ;;
            
        "windows")
            mkdir -p release/windows
            cp build-windows/bin/BootUSB.exe release/windows/
            cp README.md release/windows/ 2>/dev/null || true
            cp CHANGELOG.md release/windows/ 2>/dev/null || true
            ;;
    esac
    
    print_success "Release package created for $platform"
}

# Main script
main() {
    print_status "BootUSB Cross-Platform Build Script"
    print_status "=================================="
    
    # Detect platform
    local platform=$(detect_platform)
    print_status "Detected platform: $platform"
    
    if [ "$platform" = "unknown" ]; then
        print_error "Unsupported platform detected."
        exit 1
    fi
    
    # Check dependencies
    check_dependencies $platform
    
    # Build based on platform
    case $platform in
        "linux")
            build_linux
            if [ "$1" = "--package" ]; then
                create_release_package linux
            fi
            ;;
            
        "macos")
            build_macos
            if [ "$1" = "--package" ]; then
                create_release_package macos
            fi
            ;;
            
        "windows")
            build_windows
            if [ "$1" = "--package" ]; then
                create_release_package windows
            fi
            ;;
    esac
    
    print_success "Build process completed successfully!"
}

# Handle command line arguments
case "$1" in
    "--help"|"-h")
        echo "BootUSB Cross-Platform Build Script"
        echo "Usage: $0 [OPTIONS]"
        echo ""
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --package      Create release package after build"
        echo "  --clean        Clean build directories"
        echo ""
        echo "Examples:"
        echo "  $0              # Build for current platform"
        echo "  $0 --package    # Build and create release package"
        echo "  $0 --clean      # Clean build directories"
        ;;
        
    "--clean")
        print_status "Cleaning build directories..."
        rm -rf build-linux build-macos build-windows release
        print_success "Clean completed!"
        ;;
        
    *)
        main "$1"
        ;;
esac
