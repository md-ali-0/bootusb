cmake_minimum_required(VERSION 3.10)
project(BootUSB LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(UDEV REQUIRED libudev)

include_directories(${UDEV_INCLUDE_DIRS} include)

add_executable(bootusb
    src/main.cpp
    src/gui.cpp
    src/usb_detect.cpp
    src/format.cpp
    src/write_iso.cpp
    src/bootloader.cpp
)

target_link_libraries(bootusb
    Qt5::Widgets
    ${UDEV_LIBRARIES}
)