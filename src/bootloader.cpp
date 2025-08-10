#include "bootloader.h"
#include <cstdlib>
#include <iostream>

bool install_syslinux(const std::string &usb_device) {
    // Basic wrapper. syslinux expects a FAT partition; typically you install to the partition, e.g. /dev/sdb1
    std::string cmd = "sudo syslinux " + usb_device;
    int rc = system(cmd.c_str());
    return rc == 0;
}

bool install_grub(const std::string &usb_device, const std::string &mount_point) {
    std::string cmd = "sudo grub-install --target=i386-pc --boot-directory=" + mount_point + "/boot " + usb_device;
    int rc = system(cmd.c_str());
    return rc == 0;
}