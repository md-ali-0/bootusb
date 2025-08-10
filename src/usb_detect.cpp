#include "usb_detect.h"
#include <libudev.h>
#include <iostream>
#include <sstream>

static std::string human_readable_size(unsigned long long sectors) {
    // sectors are usually 512 bytes
    unsigned long long bytes = sectors * 512ULL;
    const char* suf[] = {"B","KB","MB","GB","TB"};
    int i = 0;
    double dbl = (double)bytes;
    while (dbl >= 1024.0 && i < 4) { dbl /= 1024.0; ++i; }
    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f %s", dbl, suf[i]);
    return std::string(buf);
}

std::vector<USBDevice> list_usb_devices() {
    std::vector<USBDevice> devices;
    struct udev *udev = udev_new();
    if (!udev) return devices;

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    udev_enumerate_add_match_subsystem(enumerate, "block");
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices_list = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    udev_list_entry_foreach(entry, devices_list) {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *dev = udev_device_new_from_syspath(udev, path);
        if (!dev) continue;

        const char *devnode = udev_device_get_devnode(dev);
        // only consider whole devices (not partitions) - check DEVTYPE == disk
        const char *devtype = udev_device_get_devtype(dev);
        if (!devnode || !devtype || std::string(devtype) != "disk") {
            udev_device_unref(dev);
            continue;
        }

        // check if removable by looking for parent usb device
        struct udev_device *parent = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (!parent) {
            udev_device_unref(dev);
            continue;
        }

        const char *model = udev_device_get_sysattr_value(parent, "product");
        const char *size_sectors = udev_device_get_sysattr_value(dev, "size");
        USBDevice d;
        d.devnode = devnode ? devnode : "";
        d.model = model ? model : "USB Disk";
        if (size_sectors) {
            unsigned long long s = strtoull(size_sectors, nullptr, 10);
            d.size = human_readable_size(s);
        } else d.size = "Unknown";

        devices.push_back(d);
        udev_device_unref(dev);
    }

    udev_enumerate_unref(enumerate);
    udev_unref(udev);
    return devices;
}