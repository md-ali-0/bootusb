#ifndef FORMAT_H
#define FORMAT_H

#include <string>

// device: e.g. /dev/sdb1 or /dev/sdb
// fs_type: "vfat", "ntfs", "ext4"
bool format_usb(const std::string &device, const std::string &fs_type = "vfat");

#endif // FORMAT_H