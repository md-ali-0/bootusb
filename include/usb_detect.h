#ifndef USB_DETECT_H
#define USB_DETECT_H

#include <string>
#include <vector>

struct USBDevice {
    std::string devnode;  // /dev/sdb
    std::string model;
    std::string size;     // human readable size
};

std::vector<USBDevice> list_usb_devices();

#endif // USB_DETECT_H