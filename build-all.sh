#!/bin/bash

# BootUSB Cross-Platform Build Script for All Platforms
# Builds for Linux, Windows, and macOS simultaneously

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

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

print_platform() {
    echo -e "${PURPLE}[PLATFORM]${NC} $1"
}

print_step() {
    echo -e "${CYAN}[STEP]${NC} $1"
}

# Function to detect current platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "macos";;
        CYGWIN*|MINGW*|MSYS*) echo "windows";;
        *)          echo "unknown";;
    esac
}

# Function to check dependencies for all platforms
check_dependencies() {
    print_status "Checking dependencies for all platforms..."
    
    # Check for CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake not found. Please install CMake."
        exit 1
    fi
    
    # Check for Qt5
    if ! pkg-config --exists Qt5Core Qt5Widgets 2>/dev/null; then
        print_warning "Qt5 not found. Some builds may fail."
    fi
    
    # Check for cross-compilation tools
    if command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        print_success "MinGW cross-compiler found for Windows builds"
    else
        print_warning "MinGW cross-compiler not found. Windows builds will be skipped."
    fi
    
    if command -v x86_64-apple-darwin-g++ &> /dev/null; then
        print_success "macOS cross-compiler found"
    else
        print_warning "macOS cross-compiler not found. macOS builds will be skipped."
    fi
    
    print_success "Dependency check completed"
}

# Function to build for Linux
build_linux() {
    print_platform "Building for Linux..."
    
    mkdir -p build-linux
    cd build-linux
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_SYSTEM_NAME=Linux
    
    make -j$(nproc)
    
    print_success "Linux build completed!"
    print_status "Executable: build-linux/bin/BootUSB"
    
    cd ..
}

# Function to build for Windows (cross-compilation)
build_windows() {
    print_platform "Building for Windows..."
    
    # Check if MinGW is available
    if ! command -v x86_64-w64-mingw32-g++ &> /dev/null; then
        print_warning "MinGW cross-compiler not found. Skipping Windows build."
        return 0
    fi
    
    mkdir -p build-windows
    cd build-windows
    
    # Try to find Qt for Windows
    local qt_path=""
    if [ -d "/usr/x86_64-w64-mingw32/qt5" ]; then
        qt_path="/usr/x86_64-w64-mingw32/qt5"
    elif [ -d "/opt/mingw64/qt5" ]; then
        qt_path="/opt/mingw64/qt5"
    fi
    
    if [ -n "$qt_path" ]; then
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local \
            -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
            -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++ \
            -DCMAKE_PREFIX_PATH="$qt_path"
    else
        print_warning "Qt for Windows not found. Creating basic Windows build..."
        cmake .. \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX=/usr/local \
            -DCMAKE_SYSTEM_NAME=Windows \
            -DCMAKE_C_COMPILER=x86_64-w64-mingw32-gcc \
            -DCMAKE_CXX_COMPILER=x86_64-w64-mingw32-g++
    fi
    
    make -j$(nproc)
    
    print_success "Windows build completed!"
    print_status "Executable: build-windows/bin/BootUSB.exe"
    
    cd ..
}

# Function to build for macOS (cross-compilation)
build_macos() {
    print_platform "Building for macOS..."
    
    # Check if macOS cross-compiler is available
    if ! command -v x86_64-apple-darwin-g++ &> /dev/null; then
        print_warning "macOS cross-compiler not found. Skipping macOS build."
        return 0
    fi
    
    mkdir -p build-macos
    cd build-macos
    
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX=/usr/local \
        -DCMAKE_SYSTEM_NAME=Darwin \
        -DCMAKE_C_COMPILER=x86_64-apple-darwin-gcc \
        -DCMAKE_CXX_COMPILER=x86_64-apple-darwin-g++ \
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14
    
    make -j$(nproc)
    
    print_success "macOS build completed!"
    print_status "Executable: build-macos/bin/BootUSB"
    
    cd ..
}

# Function to create packages for all platforms
create_all_packages() {
    print_status "Creating packages for all platforms..."
    
    mkdir -p packages
    
    # Create Linux package
    if [ -f "build-linux/bin/BootUSB" ]; then
        print_platform "Creating Linux .deb package..."
        ./package.sh
    else
        print_warning "Linux build not found. Skipping Linux package."
    fi
    
    # Create Windows package
    if [ -f "build-windows/bin/BootUSB.exe" ]; then
        print_platform "Creating Windows .exe package..."
        # For Windows, we'll create a simple package since we're on Linux
        mkdir -p packages/windows
        cp build-windows/bin/BootUSB.exe packages/windows/
        print_success "Windows executable copied to packages/windows/BootUSB.exe"
    else
        print_warning "Windows build not found. Skipping Windows package."
    fi
    
    # Create macOS package
    if [ -f "build-macos/bin/BootUSB" ]; then
        print_platform "Creating macOS package..."
        mkdir -p packages/macos
        cp build-macos/bin/BootUSB packages/macos/
        print_success "macOS executable copied to packages/macos/BootUSB"
    else
        print_warning "macOS build not found. Skipping macOS package."
    fi
}

# Function to show build summary
show_summary() {
    print_status "Build Summary:"
    echo "=================="
    
    if [ -f "build-linux/bin/BootUSB" ]; then
        print_success "✓ Linux build: build-linux/bin/BootUSB"
        ls -lh build-linux/bin/BootUSB
    else
        print_error "✗ Linux build: Failed"
    fi
    
    if [ -f "build-windows/bin/BootUSB.exe" ]; then
        print_success "✓ Windows build: build-windows/bin/BootUSB.exe"
        ls -lh build-windows/bin/BootUSB.exe
    else
        print_warning "✗ Windows build: Skipped (no cross-compiler)"
    fi
    
    if [ -f "build-macos/bin/BootUSB" ]; then
        print_success "✓ macOS build: build-macos/bin/BootUSB"
        ls -lh build-macos/bin/BootUSB
    else
        print_warning "✗ macOS build: Skipped (no cross-compiler)"
    fi
    
    echo ""
    print_status "Packages created:"
    if [ -d "packages" ]; then
        find packages -type f -exec ls -lh {} \;
    fi
}

# Main function
main() {
    print_status "BootUSB Cross-Platform Build Script for All Platforms"
    print_status "====================================================="
    
    # Clean everything first
    print_step "Cleaning previous builds..."
    ./build.sh --clean 2>/dev/null || true
    ./package.sh --clean 2>/dev/null || true
    rm -rf build-linux build-windows build-macos packages
    
    # Check dependencies
    check_dependencies
    
    # Build for all platforms
    print_step "Building for all platforms..."
    
    # Build for current platform (Linux)
    build_linux
    
    # Build for Windows (cross-compilation)
    build_windows
    
    # Build for macOS (cross-compilation)
    build_macos
    
    # Create packages
    print_step "Creating packages..."
    create_all_packages
    
    # Show summary
    show_summary
    
    print_success "All builds completed!"
    print_status "Check the build directories for executables and packages directory for distribution files."
}

# Handle command line arguments
case "$1" in
    "--help"|"-h")
        echo "BootUSB Cross-Platform Build Script for All Platforms"
        echo "Usage: $0 [OPTIONS]"
        echo ""
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --clean        Clean all build directories"
        echo "  --linux-only   Build only for Linux"
        echo "  --windows-only Build only for Windows"
        echo "  --macos-only   Build only for macOS"
        echo ""
        echo "This script builds BootUSB for all platforms:"
        echo "  Linux:   Native build"
        echo "  Windows: Cross-compilation (requires MinGW)"
        echo "  macOS:   Cross-compilation (requires macOS SDK)"
        ;;
        
    "--clean")
        print_status "Cleaning all build directories..."
        rm -rf build-linux build-windows build-macos packages
        print_success "Clean completed!"
        ;;
        
    "--linux-only")
        print_status "Building only for Linux..."
        check_dependencies
        build_linux
        create_all_packages
        show_summary
        ;;
        
    "--windows-only")
        print_status "Building only for Windows..."
        check_dependencies
        build_windows
        create_all_packages
        show_summary
        ;;
        
    "--macos-only")
        print_status "Building only for macOS..."
        check_dependencies
        build_macos
        create_all_packages
        show_summary
        ;;
        
    *)
        main
        ;;
esac
