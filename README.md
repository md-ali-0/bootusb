# BootUSB

A cross-platform GUI application for creating bootable USB drives from ISO files. Supports Linux, Windows, and macOS.

## Features

- **Cross-Platform**: Works on Linux, Windows, and macOS
- **Multiple File Systems**: Support for FAT32, NTFS, and ext4
- **Bootloader Options**: Syslinux, GRUB, or no bootloader
- **Partition Schemes**: MBR and GPT support
- **Advanced Options**: Cluster size, format type, bad block checking
- **Persistent Storage**: For Linux live USB drives
- **Real-time Progress**: Visual progress tracking
- **Logging**: Detailed operation logs

## Screenshots

The application features a clean, modern interface with:
- Device selection with automatic USB detection
- ISO file browser
- Format options (File system & Target system)
- Partition scheme and volume label settings
- Advanced options in a separate tab
- Real-time progress tracking

## Requirements

### Linux
- Qt5 development packages
- libudev development package
- CMake 3.16 or later
- GCC with C++17 support

### Windows
- Qt5 for Windows
- CMake 3.16 or later
- MinGW-w64 or Visual Studio
- Windows 7 or later

### macOS
- Qt5 (via Homebrew)
- CMake 3.16 or later
- Xcode Command Line Tools
- macOS 10.14 or later

## Installation

### Linux

#### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install qt5-default qtbase5-dev libudev-dev cmake build-essential

# Clone and build
git clone https://github.com/yourusername/bootusb.git
cd bootusb
./build.sh
```

#### Fedora/RHEL
```bash
# Install dependencies
sudo dnf install qt5-qtbase-devel systemd-devel cmake gcc-c++

# Clone and build
git clone https://github.com/yourusername/bootusb.git
cd bootusb
./build.sh
```

### Windows

1. Install Qt5 for Windows from [qt.io](https://www.qt.io/download)
2. Install CMake from [cmake.org](https://cmake.org/download/)
3. Install MinGW-w64 or Visual Studio
4. Clone the repository and run:
```cmd
git clone https://github.com/yourusername/bootusb.git
cd bootusb
build.bat
```

### macOS

```bash
# Install dependencies
brew install qt5 cmake

# Clone and build
git clone https://github.com/yourusername/bootusb.git
cd bootusb
./build.sh
```

## Building

### Quick Build
Use the provided build scripts:

- **Linux/macOS**: `./build.sh`
- **Windows**: `build.bat`

### Manual Build with CMake

```bash
mkdir build
cd build
cmake ..
make
```

### Build Options

- `--package`: Create release package after build
- `--clean`: Clean build directories

Example:
```bash
./build.sh --package  # Build and create release package
./build.sh --clean    # Clean build directories
```

## Usage

1. **Launch the application**
   - Linux: `./build-linux/bin/bootusb`
   - Windows: `build-windows\bin\BootUSB.exe`
   - macOS: `./build-macos/bin/BootUSB`

2. **Select a USB device**
   - Choose from the dropdown list
   - Click the refresh button to update the list

3. **Select an ISO file**
   - Click "Browse" to select your ISO file
   - Supported formats: .iso files

4. **Configure options**
   - **Main Tab**: File system, target system, partition scheme, volume label
   - **Advanced Tab**: Cluster size, format type, bad block checking, persistent storage

5. **Start the process**
   - Click "START" to begin creating the bootable USB
   - Monitor progress in the status section

## Advanced Features

### Persistent Storage (Linux ISOs)
Enable persistent storage to save changes between boots on Linux live USB drives.

### Bad Block Checking
Check for bad blocks before formatting for better reliability.

### Cluster Size
Optimize cluster size for your specific use case.

### Format Types
- **Quick Format**: Faster, only clears file system
- **Full Format**: Slower, performs complete disk check

## Troubleshooting

### Common Issues

1. **Permission Denied**
   - Run with sudo/administrator privileges
   - Ensure proper USB device permissions

2. **Qt Not Found**
   - Install Qt5 development packages
   - Set QTDIR environment variable

3. **USB Device Not Detected**
   - Ensure device is properly connected
   - Check if device is mounted (unmount first)

4. **Build Failures**
   - Ensure all dependencies are installed
   - Check CMake version (3.16+ required)
   - Verify C++17 compiler support

### Platform-Specific Notes

#### Linux
- Requires udev for USB device detection
- May need to run with sudo for device access
- Desktop integration via .desktop file

#### Windows
- Requires administrator privileges for device access
- Qt installation path must be in PATH or QTDIR
- MinGW-w64 recommended for compatibility

#### macOS
- Requires administrator privileges for device access
- Qt5 via Homebrew recommended
- May need to allow in Security & Privacy settings

## Development

### Project Structure
```
bootusb/
├── src/           # Source files
├── include/       # Header files
├── assets/        # Resources
├── CMakeLists.txt # CMake configuration
├── build.sh       # Linux/macOS build script
├── build.bat      # Windows build script
└── README.md      # This file
```

### Contributing
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test on multiple platforms
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Qt Framework for cross-platform GUI
- libudev for Linux USB device detection
- CMake for build system
- Open source community for inspiration and tools

## Support

For issues and questions:
- Create an issue on GitHub
- Check the troubleshooting section
- Review platform-specific requirements

## Version History

- **v1.0.0**: Initial release with cross-platform support
  - Basic USB boot creation
  - Multiple file system support
  - Advanced formatting options
  - Cross-platform build system

