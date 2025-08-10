#ifndef WRITE_ISO_H
#define WRITE_ISO_H

#include <string>
#include <functional>

// iso_path: path to .iso file
// usb_path: raw block device, e.g. /dev/sdb
// progress_callback: optional lambda receiving bytes_written, total_bytes
bool write_iso_to_usb(const std::string &iso_path, const std::string &usb_path, std::function<void(size_t, size_t)> progress_callback = nullptr);

// Advanced version with configurable buffer size and verification
bool write_iso_to_usb_advanced(const std::string &iso_path, const std::string &usb_path, 
                              std::function<void(size_t, size_t)> progress_callback,
                              size_t buffer_size, bool verify_write);

// Verify that the write was successful by comparing ISO and USB contents
bool verify_iso_write(const std::string &iso_path, const std::string &usb_path, 
                     std::function<void(size_t, size_t)> progress_callback);

// Create persistent storage partition for live USB
bool create_persistent_storage(const std::string &usb_path, size_t size_gb);

// Advanced bootloader installation with type selection
bool install_bootloader_advanced(const std::string &usb_path, const std::string &bootloader_type);

#endif // WRITE_ISO_H