#include "format.h"
#include <cstdlib>
#include <iostream>

bool format_usb(const std::string &device, const std::string &fs_type) {
    std::string cmd;
    if (fs_type == "vfat") {
        // Use parted to create msdos partition table + single primary partition, then mkfs.vfat
        cmd = "sudo parted -s " + device + " mklabel msdos mkpart primary fat32 1MiB 100% && sudo mkfs.vfat -F32 " + device + "1";
    } else if (fs_type == "ntfs") {
        cmd = "sudo parted -s " + device + " mklabel msdos mkpart primary ntfs 1MiB 100% && sudo mkfs.ntfs -f " + device + "1";
    } else if (fs_type == "ext4") {
        cmd = "sudo parted -s " + device + " mklabel msdos mkpart primary ext4 1MiB 100% && sudo mkfs.ext4 -F " + device + "1";
    } else {
        std::cerr << "Unsupported filesystem type: " << fs_type << std::endl;
        return false;
    }
    int rc = system(cmd.c_str());
    return rc == 0;
}
