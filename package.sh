#!/bin/bash

# BootUSB Package Generation Script
# Creates .deb, .exe, and .dmg packages for distribution

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
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

# Function to detect platform
detect_platform() {
    case "$(uname -s)" in
        Linux*)     echo "linux";;
        Darwin*)    echo "macos";;
        CYGWIN*|MINGW*|MSYS*) echo "windows";;
        *)          echo "unknown";;
    esac
}

# Function to create Linux .deb package
create_deb_package() {
    print_status "Creating Linux .deb package..."
    
    local version="1.0.0"
    local package_name="bootusb"
    local deb_dir="packages/deb"
    
    # Create package structure
    mkdir -p "$deb_dir/DEBIAN"
    mkdir -p "$deb_dir/usr/bin"
    mkdir -p "$deb_dir/usr/share/applications"
    mkdir -p "$deb_dir/usr/share/icons/hicolor/256x256/apps"
    mkdir -p "$deb_dir/usr/share/doc/$package_name"
    
    # Copy executable (use the correct name)
    cp "build-linux/bin/BootUSB" "$deb_dir/usr/bin/bootusb"
    chmod +x "$deb_dir/usr/bin/bootusb"
    
    # Create desktop file
    cat > "$deb_dir/usr/share/applications/bootusb.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=BootUSB
Comment=Create bootable USB drives from ISO files
Exec=/usr/bin/bootusb
Icon=bootusb
Terminal=false
Categories=System;Utility;
Keywords=usb;boot;iso;format;
StartupNotify=true
EOF
    
    # Create control file
    cat > "$deb_dir/DEBIAN/control" << EOF
Package: bootusb
Version: $version
Section: utils
Priority: optional
Architecture: amd64
Depends: libc6, libqt5core5a, libqt5widgets5, libudev1
Maintainer: BootUSB Team <support@bootusb.com>
Description: Cross-platform bootable USB creator
 BootUSB is a GUI application for creating bootable USB drives
 from ISO files. It supports multiple file systems, bootloaders,
 and advanced formatting options.
 .
 Features:
  - Multiple file system support (FAT32, NTFS, ext4)
  - Bootloader options (Syslinux, GRUB)
  - Partition scheme support (MBR, GPT)
  - Advanced formatting options
  - Cross-platform compatibility
EOF
    
    # Create postinst script
    cat > "$deb_dir/DEBIAN/postinst" << EOF
#!/bin/bash
set -e
update-desktop-database
EOF
    chmod +x "$deb_dir/DEBIAN/postinst"
    
    # Create copyright file
    cat > "$deb_dir/usr/share/doc/$package_name/copyright" << EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: BootUSB
Source: https://github.com/bootusb/bootusb

Files: *
Copyright: 2024 BootUSB Team
License: MIT
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 .
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 .
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
EOF
    
    # Build .deb package
    dpkg-deb --build "$deb_dir" "packages/bootusb_${version}_amd64.deb"
    
    print_success "Linux .deb package created: packages/bootusb_${version}_amd64.deb"
}

# Function to create Windows .exe installer
create_windows_package() {
    print_status "Creating Windows .exe package..."
    
    local version="1.0.0"
    local package_name="BootUSB"
    local exe_dir="packages/exe"
    
    # Create package structure
    mkdir -p "$exe_dir"
    
    # Copy executable and dependencies (use the correct name)
    cp "build-windows/bin/BootUSB.exe" "$exe_dir/"
    
    # Create NSIS installer script
    cat > "packages/installer.nsi" << EOF
!define PRODUCT_NAME "BootUSB"
!define PRODUCT_VERSION "$version"
!define PRODUCT_PUBLISHER "BootUSB Team"
!define PRODUCT_WEB_SITE "https://github.com/bootusb/bootusb"
!define PRODUCT_DIR_REGKEY "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\BootUSB.exe"
!define PRODUCT_UNINST_KEY "Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

!include "MUI2.nsh"

Name "\${PRODUCT_NAME} \${PRODUCT_VERSION}"
OutFile "packages/BootUSB_Setup_\${PRODUCT_VERSION}.exe"
InstallDir "\$PROGRAMFILES\\BootUSB"
InstallDirRegKey HKLM "\${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section "MainSection" SEC01
  SetOutPath "\$INSTDIR"
  SetOverwrite ifnewer
  File "packages/exe/BootUSB.exe"
  
  CreateDirectory "\$SMPROGRAMS\\BootUSB"
  CreateShortCut "\$SMPROGRAMS\\BootUSB\\BootUSB.lnk" "\$INSTDIR\\BootUSB.exe"
  CreateShortCut "\$DESKTOP\\BootUSB.lnk" "\$INSTDIR\\BootUSB.exe"
SectionEnd

Section -AdditionalIcons
  WriteIniStr "\$INSTDIR\\\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "\${PRODUCT_WEB_SITE}"
  CreateShortCut "\$SMPROGRAMS\\BootUSB\\Website.lnk" "\$INSTDIR\\\${PRODUCT_NAME}.url"
  CreateShortCut "\$SMPROGRAMS\\BootUSB\\Uninstall.lnk" "\$INSTDIR\\uninst.exe"
SectionEnd

Section -Post
  WriteUninstaller "\$INSTDIR\\uninst.exe"
  WriteRegStr HKLM "\${PRODUCT_DIR_REGKEY}" "" "\$INSTDIR\\BootUSB.exe"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "DisplayName" "\$(^Name)"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "UninstallString" "\$INSTDIR\\uninst.exe"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "DisplayIcon" "\$INSTDIR\\BootUSB.exe"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "DisplayVersion" "\${PRODUCT_VERSION}"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "URLInfoAbout" "\${PRODUCT_WEB_SITE}"
  WriteRegStr \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}" "Publisher" "\${PRODUCT_PUBLISHER}"
SectionEnd

Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "\$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove \$(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  Delete "\$INSTDIR\\\${PRODUCT_NAME}.url"
  Delete "\$INSTDIR\\uninst.exe"
  Delete "\$INSTDIR\\BootUSB.exe"

  Delete "\$SMPROGRAMS\\BootUSB\\Uninstall.lnk"
  Delete "\$SMPROGRAMS\\BootUSB\\Website.lnk"
  Delete "\$SMPROGRAMS\\BootUSB\\BootUSB.lnk"

  RMDir "\$SMPROGRAMS\\BootUSB"
  RMDir "\$INSTDIR"

  DeleteRegKey \${PRODUCT_UNINST_ROOT_KEY} "\${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "\${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
EOF
    
    # Check if NSIS is available
    if command -v makensis &> /dev/null; then
        makensis "packages/installer.nsi"
        print_success "Windows .exe installer created: packages/BootUSB_Setup_${version}.exe"
    else
        print_warning "NSIS not found. Creating simple .exe package..."
        cp "build-windows/bin/BootUSB.exe" "packages/BootUSB_${version}.exe"
        print_success "Windows .exe package created: packages/BootUSB_${version}.exe"
    fi
}

# Function to create macOS .dmg package
create_macos_package() {
    print_status "Creating macOS .dmg package..."
    
    local version="1.0.0"
    local package_name="BootUSB"
    local dmg_dir="packages/dmg"
    
    # Create package structure
    mkdir -p "$dmg_dir"
    
    # Copy application bundle or create one
    if [ -d "build-macos/bin/BootUSB.app" ]; then
        cp -r "build-macos/bin/BootUSB.app" "$dmg_dir/"
    else
        # Create simple app bundle
        mkdir -p "$dmg_dir/BootUSB.app/Contents/MacOS"
        mkdir -p "$dmg_dir/BootUSB.app/Contents/Resources"
        
        cp "build-macos/bin/BootUSB" "$dmg_dir/BootUSB.app/Contents/MacOS/"
        chmod +x "$dmg_dir/BootUSB.app/Contents/MacOS/BootUSB"
        
        # Create Info.plist
        cat > "$dmg_dir/BootUSB.app/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>BootUSB</string>
    <key>CFBundleIdentifier</key>
    <string>com.bootusb.app</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>BootUSB</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>$version</string>
    <key>CFBundleVersion</key>
    <string>$version</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.14</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
EOF
    fi
    
    # Create Applications symlink
    ln -sf /Applications "$dmg_dir/Applications"
    
    # Create .dmg file
    if command -v hdiutil &> /dev/null; then
        hdiutil create -volname "BootUSB" -srcfolder "$dmg_dir" -ov -format UDZO "packages/BootUSB_${version}.dmg"
        print_success "macOS .dmg package created: packages/BootUSB_${version}.dmg"
    else
        print_warning "hdiutil not found. Creating simple package..."
        tar -czf "packages/BootUSB_${version}_macos.tar.gz" -C "$dmg_dir" .
        print_success "macOS package created: packages/BootUSB_${version}_macos.tar.gz"
    fi
}

# Main function
main() {
    print_status "BootUSB Package Generation Script"
    print_status "================================="
    
    # Create packages directory
    mkdir -p packages
    
    # Detect platform
    local platform=$(detect_platform)
    print_status "Detected platform: $platform"
    
    # Build the application first
    print_status "Building application..."
    ./build.sh
    
    # Create packages based on platform
    case $platform in
        "linux")
            create_deb_package
            ;;
        "windows")
            create_windows_package
            ;;
        "macos")
            create_macos_package
            ;;
        *)
            print_error "Unsupported platform: $platform"
            exit 1
            ;;
    esac
    
    print_success "Package generation completed!"
    print_status "Packages are located in the 'packages/' directory"
}

# Handle command line arguments
case "$1" in
    "--help"|"-h")
        echo "BootUSB Package Generation Script"
        echo "Usage: $0 [OPTIONS]"
        echo ""
        echo "Options:"
        echo "  --help, -h     Show this help message"
        echo "  --clean        Clean package directories"
        echo ""
        echo "This script creates platform-specific packages:"
        echo "  Linux:   .deb package"
        echo "  Windows: .exe installer"
        echo "  macOS:   .dmg package"
        ;;
        
    "--clean")
        print_status "Cleaning package directories..."
        rm -rf packages
        print_success "Clean completed!"
        ;;
        
    *)
        main
        ;;
esac
