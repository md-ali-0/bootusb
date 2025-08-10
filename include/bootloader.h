#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include <string>

bool install_syslinux(const std::string &usb_device);
bool install_grub(const std::string &usb_device, const std::string &mount_point);

#endif // BOOTLOADER_H