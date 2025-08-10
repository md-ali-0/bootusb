# BootUSB (C++ / Qt)

A minimal, educational Rufus-like bootable USB creator for Linux written in C++ using Qt5 for GUI and libudev for USB detection. This project is intended as a starting point — it demonstrates device detection, ISO selection, raw write (safe wrapper around low-level write), formatting (via system tools), and bootloader installation (syslinux/grub wrapper).

**WARNING:** This tool performs destructive operations on block devices. Always test inside a VM and ensure you select the correct device. Use at your own risk.

## Features

- **USB Device Detection**: Automatically detects removable USB storage devices using libudev
- **ISO File Selection**: Browse and select ISO files for writing
- **Filesystem Formatting**: Support for FAT32, NTFS, and ext4 filesystems
- **Raw ISO Writing**: Direct block-level writing for bootable USB creation
- **Progress Tracking**: Real-time progress bar during write operations
- **Threaded Operations**: Non-blocking UI during write operations
- **Safety Confirmations**: Multiple confirmation dialogs before destructive operations

## Requirements
- Linux (Debian/Ubuntu tested)
- Qt5 (Widgets)
- libudev (libudev-dev)
- CMake (>=3.10) or qmake
- build-essential (g++, make)
- syslinux, grub-install available if you want bootloader install

Install dependencies (Debian/Ubuntu):

```bash
sudo apt update
sudo apt install -y build-essential cmake pkg-config libudev-dev qtbase5-dev qttools5-dev-tools syslinux grub2-common
```

## Build

### Using qmake (Recommended)
```bash
qmake bootusb.pro
make -j$(nproc)
```

### Using CMake
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Run

You will need root privileges to write to block devices. Example (careful):

```bash
sudo ./bootusb
```

Recommended: run inside a disposable VM and double-check device selection.

## Usage

1. **Launch the application** with sudo privileges
2. **Refresh devices** to detect connected USB storage devices
3. **Select an ISO file** using the file browser
4. **Choose filesystem type** (FAT32 recommended for most bootable USBs)
5. **Click "Write ISO to USB"** and confirm the operation
6. **Wait for completion** - the progress bar will show write progress

## Safety Features

- Only removable USB devices are detected and listed
- Multiple confirmation dialogs before destructive operations
- Clear device identification with model and size information
- Progress tracking to monitor write operations

## Technical Details

- **Device Detection**: Uses libudev to enumerate block devices and filter for USB removable storage
- **File Writing**: Direct block-level I/O using open(), read(), write() system calls
- **Formatting**: Wrapper around parted and mkfs utilities
- **Bootloader**: Basic wrappers for syslinux and grub-install
- **Threading**: QThread-based worker for non-blocking UI during operations

## Troubleshooting

- **No devices found**: Ensure USB storage device is connected and not mounted
- **Permission denied**: Run with sudo privileges
- **Format fails**: Ensure parted and mkfs utilities are installed
- **Write fails**: Check if device is mounted or in use by another process

## Development

The project structure:
```
bootusb/
├── include/          # Header files
├── src/             # Source files
├── bootusb.pro      # qmake project file
├── CMakeLists.txt   # CMake configuration
└── README.md        # This file
```

## License

This is an educational project. Use at your own risk.

