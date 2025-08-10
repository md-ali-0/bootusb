# BootUSB Package Distribution Guide

This guide explains where to find the different package files (.deb, .exe, .dmg) and how to use them for distribution.

## 📦 **Package Locations**

### **Linux (.deb package)**
```bash
# Location after running package script:
packages/bootusb_1.0.0_amd64.deb

# Install on Ubuntu/Debian:
sudo dpkg -i packages/bootusb_1.0.0_amd64.deb
sudo apt-get install -f  # Fix any dependency issues

# Or install using apt:
sudo apt install ./packages/bootusb_1.0.0_amd64.deb
```

### **Windows (.exe installer)**
```cmd
# Location after running package script:
packages/BootUSB_Setup_1.0.0.exe  # With NSIS installer
packages/BootUSB_1.0.0.exe        # Simple executable

# Install:
# Double-click the .exe file and follow the installer
```

### **macOS (.dmg package)**
```bash
# Location after running package script:
packages/BootUSB_1.0.0.dmg

# Install:
# Double-click the .dmg file and drag BootUSB to Applications
```

## 🚀 **How to Generate Packages**

### **Quick Package Generation**
```bash
# Generate package for current platform:
./package.sh

# Clean package directories:
./package.sh --clean

# Show help:
./package.sh --help
```

### **Platform-Specific Package Generation**

#### **Linux (.deb)**
```bash
# On Linux system:
./package.sh
# Creates: packages/bootusb_1.0.0_amd64.deb
```

#### **Windows (.exe)**
```cmd
# On Windows system:
build.bat --package
# Creates: release\windows\BootUSB.exe

# Or use the package script (if available):
package.sh
# Creates: packages/BootUSB_Setup_1.0.0.exe
```

#### **macOS (.dmg)**
```bash
# On macOS system:
./package.sh
# Creates: packages/BootUSB_1.0.0.dmg
```

## 📁 **Package Structure**

### **Linux .deb Package Contents**
```
bootusb_1.0.0_amd64.deb
├── /usr/bin/bootusb                    # Executable
├── /usr/share/applications/bootusb.desktop  # Desktop entry
├── /usr/share/doc/bootusb/copyright    # License
└── DEBIAN/
    ├── control                         # Package metadata
    └── postinst                       # Post-install script
```

### **Windows .exe Installer Contents**
```
BootUSB_Setup_1.0.0.exe
├── BootUSB.exe                        # Main executable
├── Start Menu shortcuts
├── Desktop shortcut
└── Uninstaller
```

### **macOS .dmg Package Contents**
```
BootUSB_1.0.0.dmg
├── BootUSB.app/                       # Application bundle
│   ├── Contents/
│   │   ├── MacOS/BootUSB             # Executable
│   │   ├── Resources/                # Resources
│   │   └── Info.plist                # App metadata
└── Applications/                      # Symlink to Applications folder
```

## 🔧 **Package Requirements**

### **Linux .deb Dependencies**
- libc6
- libqt5core5a
- libqt5widgets5
- libudev1

### **Windows Requirements**
- Windows 7 or later
- Qt5 runtime libraries (included in installer)

### **macOS Requirements**
- macOS 10.14 or later
- Qt5 runtime libraries (included in app bundle)

## 📋 **Distribution Checklist**

### **Before Creating Packages**
- [ ] Update version number in CMakeLists.txt
- [ ] Test application on target platform
- [ ] Update changelog and documentation
- [ ] Ensure all dependencies are included

### **Package Testing**
- [ ] Test installation on clean system
- [ ] Verify all features work correctly
- [ ] Check uninstall process
- [ ] Test on different OS versions

### **Distribution Files**
- [ ] .deb package for Linux
- [ ] .exe installer for Windows
- [ ] .dmg package for macOS
- [ ] README.md with installation instructions
- [ ] CHANGELOG.md with version history
- [ ] LICENSE file

## 🛠 **Advanced Package Customization**

### **Customizing .deb Package**
Edit the `create_deb_package()` function in `package.sh`:
```bash
# Change package name, version, dependencies
local version="1.0.0"
local package_name="bootusb"
```

### **Customizing Windows Installer**
Edit the NSIS script in `package.sh`:
```nsi
!define PRODUCT_NAME "BootUSB"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "BootUSB Team"
```

### **Customizing macOS Package**
Edit the `create_macos_package()` function in `package.sh`:
```bash
# Change app bundle settings
local version="1.0.0"
local package_name="BootUSB"
```

## 📊 **Package Sizes**

Typical package sizes:
- **Linux .deb**: ~45 KB
- **Windows .exe**: ~150 KB (with installer)
- **macOS .dmg**: ~200 KB (with app bundle)

## 🔍 **Troubleshooting**

### **Common Issues**

#### **Linux .deb Installation Fails**
```bash
# Check dependencies:
dpkg -I packages/bootusb_1.0.0_amd64.deb

# Fix broken packages:
sudo apt-get install -f
```

#### **Windows .exe Won't Run**
- Ensure Windows Defender allows the application
- Run as Administrator if needed
- Check if Qt5 runtime is installed

#### **macOS .dmg Won't Open**
- Right-click and select "Open" to bypass Gatekeeper
- Go to System Preferences > Security & Privacy to allow

### **Package Generation Errors**
```bash
# Clean and rebuild:
./package.sh --clean
./build.sh --clean
./package.sh
```

## 📈 **Version Management**

### **Updating Version**
1. Update version in `CMakeLists.txt`:
```cmake
project(BootUSB VERSION 1.0.1 LANGUAGES CXX)
```

2. Update package scripts:
```bash
local version="1.0.1"
```

3. Regenerate packages:
```bash
./package.sh --clean
./package.sh
```

## 🎯 **Distribution Channels**

### **GitHub Releases**
- Upload packages to GitHub Releases
- Include installation instructions
- Add release notes

### **Package Repositories**
- **Linux**: PPA, COPR, AUR
- **Windows**: Chocolatey, Scoop
- **macOS**: Homebrew, MacPorts

### **Direct Distribution**
- Website downloads
- Email distribution
- USB/Network sharing

## 📞 **Support**

For package-related issues:
- Check the troubleshooting section
- Review platform-specific requirements
- Create an issue on GitHub
- Contact the development team
