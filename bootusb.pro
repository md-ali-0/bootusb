QT += widgets
CONFIG += c++17

TARGET = bootusb
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/gui.cpp \
    src/usb_detect.cpp \
    src/format.cpp \
    src/write_iso.cpp \
    src/bootloader.cpp

HEADERS += \
    include/bootloader.h \
    include/format.h \
    include/usb_detect.h \
    include/write_iso.h \
    include/gui.h

INCLUDEPATH += include

LIBS += -ludev

# Ensure we link against the correct Qt libraries
QT += core gui widgets 