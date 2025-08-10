#!/bin/bash

# BootUSB Installation Script
# This script installs dependencies and builds the BootUSB application

set -e

echo "BootUSB Installation Script"
echo "=========================="

# Check if running as root
if [[ $EUID -eq 0 ]]; then
   echo "This script should not be run as root. Please run as a regular user."
   exit 1
fi

# Update package list
echo "Updating package list..."
sudo apt update

# Install dependencies
echo "Installing dependencies..."
sudo apt install -y build-essential cmake pkg-config libudev-dev qtbase5-dev qttools5-dev-tools syslinux grub2-common

# Build the application
echo "Building BootUSB..."
qmake bootusb.pro
make -j$(nproc)

# Make executable
chmod +x bootusb

echo ""
echo "Build completed successfully!"
echo "To run BootUSB: sudo ./bootusb"
echo ""
echo "WARNING: This tool performs destructive operations on USB devices."
echo "Always double-check device selection and test in a VM first." 