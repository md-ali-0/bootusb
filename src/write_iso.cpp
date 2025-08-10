#include "write_iso.h"
#include "bootloader.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <algorithm>

bool write_iso_to_usb(const std::string &iso_path, const std::string &usb_path, std::function<void(size_t, size_t)> progress_callback) {
    return write_iso_to_usb_advanced(iso_path, usb_path, progress_callback, 4 * 1024 * 1024, false);
}

bool write_iso_to_usb_advanced(const std::string &iso_path, const std::string &usb_path, 
                              std::function<void(size_t, size_t)> progress_callback,
                              size_t buffer_size, bool verify_write) {
    // Open iso file
    int ifd = open(iso_path.c_str(), O_RDONLY);
    if (ifd < 0) { 
        std::cerr << "Error opening ISO file: " << strerror(errno) << std::endl;
        return false; 
    }

    struct stat st;
    if (fstat(ifd, &st) != 0) { 
        std::cerr << "Error getting ISO file stats: " << strerror(errno) << std::endl;
        close(ifd); 
        return false; 
    }
    size_t total = (size_t)st.st_size;

    // Open usb device
    int ofd = open(usb_path.c_str(), O_WRONLY | O_SYNC);
    if (ofd < 0) { 
        std::cerr << "Error opening USB device: " << strerror(errno) << std::endl;
        close(ifd); 
        return false; 
    }

    // Use provided buffer size or default to 4MB
    const size_t BUF = buffer_size > 0 ? buffer_size : 4 * 1024 * 1024;
    std::vector<char> buf(BUF);
    size_t written = 0;
    ssize_t r;
    
    std::cout << "Writing ISO to USB with " << (BUF / (1024*1024)) << "MB buffer..." << std::endl;
    
    while ((r = read(ifd, buf.data(), BUF)) > 0) {
        ssize_t w = write(ofd, buf.data(), r);
        if (w < 0) { 
            std::cerr << "Error writing to USB: " << strerror(errno) << std::endl;
            close(ifd); 
            close(ofd); 
            return false; 
        }
        written += (size_t)w;
        if (progress_callback) progress_callback(written, total);
    }
    if (r < 0) { 
        std::cerr << "Error reading ISO: " << strerror(errno) << std::endl;
        close(ifd); 
        close(ofd); 
        return false; 
    }

    fsync(ofd);
    close(ifd);
    close(ofd);
    
    // Verify write if requested
    if (verify_write) {
        std::cout << "Verifying write..." << std::endl;
        if (!verify_iso_write(iso_path, usb_path, progress_callback)) {
            std::cerr << "Write verification failed!" << std::endl;
            return false;
        }
        std::cout << "Write verification successful!" << std::endl;
    }
    
    return true;
}

bool verify_iso_write(const std::string &iso_path, const std::string &usb_path, 
                     std::function<void(size_t, size_t)> progress_callback) {
    // Open both files for verification
    int ifd = open(iso_path.c_str(), O_RDONLY);
    if (ifd < 0) return false;

    int ofd = open(usb_path.c_str(), O_RDONLY);
    if (ofd < 0) {
        close(ifd);
        return false;
    }

    struct stat st;
    if (fstat(ifd, &st) != 0) {
        close(ifd);
        close(ofd);
        return false;
    }
    size_t total = (size_t)st.st_size;

    const size_t BUF = 1024 * 1024; // 1MB buffer for verification
    std::vector<char> iso_buf(BUF);
    std::vector<char> usb_buf(BUF);
    size_t verified = 0;
    ssize_t r1, r2;

    while ((r1 = read(ifd, iso_buf.data(), BUF)) > 0) {
        r2 = read(ofd, usb_buf.data(), r1);
        if (r2 != r1) {
            close(ifd);
            close(ofd);
            return false;
        }

        if (memcmp(iso_buf.data(), usb_buf.data(), r1) != 0) {
            close(ifd);
            close(ofd);
            return false;
        }

        verified += r1;
        if (progress_callback) progress_callback(verified, total);
    }

    close(ifd);
    close(ofd);
    return true;
}

bool create_persistent_storage(const std::string &usb_path, size_t size_gb) {
    (void)size_gb; // Suppress unused parameter warning
    // This is a simplified implementation
    // In a real implementation, you would:
    // 1. Create a new partition
    // 2. Format it with ext4
    // 3. Create a casper-rw file or partition
    
    std::string cmd = "sudo parted -s " + usb_path + " mkpart primary ext4 100% 100%";
    int rc = system(cmd.c_str());
    if (rc != 0) return false;
    
    // Get the partition number (assuming it's the last partition)
    cmd = "sudo parted -s " + usb_path + " print | tail -1 | awk '{print $1}'";
    // This would need proper parsing in a real implementation
    
    return true;
}

bool install_bootloader_advanced(const std::string &usb_path, const std::string &bootloader_type) {
    if (bootloader_type == "None") {
        return true;
    } else if (bootloader_type == "Syslinux") {
        return install_syslinux(usb_path);
    } else if (bootloader_type == "GRUB") {
        // Mount the USB and install GRUB
        std::string mount_point = "/tmp/bootusb_mount";
        std::string cmd = "sudo mkdir -p " + mount_point + " && sudo mount " + usb_path + "1 " + mount_point;
        int rc = system(cmd.c_str());
        if (rc != 0) return false;
        
        cmd = "sudo grub-install --target=i386-pc --boot-directory=" + mount_point + "/boot " + usb_path;
        rc = system(cmd.c_str());
        
        system(("sudo umount " + mount_point).c_str());
        return rc == 0;
    } else if (bootloader_type == "Auto-detect") {
        // Try to detect the ISO type and install appropriate bootloader
        // This would require analyzing the ISO file structure
        return install_syslinux(usb_path); // Default fallback
    }
    
    return false;
}
