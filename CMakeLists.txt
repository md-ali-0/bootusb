cmake_minimum_required(VERSION 3.16)
project(BootUSB VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Find Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set(PLATFORM_NAME "Windows")
    set(EXECUTABLE_NAME "BootUSB.exe")
    
    # Set Windows subsystem
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    
    # Windows-specific compiler flags
    add_compile_definitions(WIN32_LEAN_AND_MEAN)
    add_compile_definitions(_WIN32_WINNT=0x0601)  # Windows 7 or later
    
    # Find Windows-specific libraries
    find_library(USER32_LIB user32)
    find_library(SHELL32_LIB shell32)
    find_library(ADVAPI32_LIB advapi32)
    
elseif(APPLE)
    # macOS-specific settings
    set(PLATFORM_NAME "macOS")
    set(EXECUTABLE_NAME "BootUSB")
    
    # macOS-specific compiler flags
    add_compile_definitions(__APPLE__)
    
    # Set minimum macOS version
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.14")
    
    # Find macOS-specific libraries
    find_library(IOKIT_FRAMEWORK IOKit)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    
else()
    # Linux-specific settings
    set(PLATFORM_NAME "Linux")
    set(EXECUTABLE_NAME "bootusb")
    
    # Linux-specific compiler flags
    add_compile_definitions(__linux__)
    
    # Find Linux-specific libraries
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(UDEV REQUIRED libudev)
    
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/gui.cpp
    src/usb_detect.cpp
    src/format.cpp
    src/write_iso.cpp
    src/bootloader.cpp
)

# Header files
set(HEADERS
    include/gui.h
    include/usb_detect.h
    include/format.h
    include/write_iso.h
    include/bootloader.h
)

# Create executable
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

# Set target properties
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    OUTPUT_NAME "BootUSB"
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Include directories
target_include_directories(${EXECUTABLE_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link Qt libraries
target_link_libraries(${EXECUTABLE_NAME} PRIVATE
    Qt5::Core
    Qt5::Widgets
)

# Platform-specific linking
if(WIN32)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${USER32_LIB}
        ${SHELL32_LIB}
        ${ADVAPI32_LIB}
    )
    
    # Windows-specific compiler flags
    target_compile_options(${EXECUTABLE_NAME} PRIVATE /W4)
    
elseif(APPLE)
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${IOKIT_FRAMEWORK}
        ${COREFOUNDATION_FRAMEWORK}
    )
    
    # macOS-specific compiler flags
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra)
    
else()
    target_link_libraries(${EXECUTABLE_NAME} PRIVATE
        ${UDEV_LIBRARIES}
    )
    
    target_include_directories(${EXECUTABLE_NAME} PRIVATE
        ${UDEV_INCLUDE_DIRS}
    )
    
    target_compile_options(${EXECUTABLE_NAME} PRIVATE
        ${UDEV_CFLAGS_OTHER}
    )
    
    # Linux-specific compiler flags
    target_compile_options(${EXECUTABLE_NAME} PRIVATE -Wall -Wextra)
    
endif()

# Enable automoc for Qt
set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC OFF
    AUTORCC OFF
)

# Install rules
install(TARGETS ${EXECUTABLE_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

# Create desktop file for Linux
if(UNIX AND NOT APPLE)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/bootusb.desktop.in
        ${CMAKE_CURRENT_BINARY_DIR}/bootusb.desktop
        @ONLY
    )
    
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bootusb.desktop
        DESTINATION share/applications
    )
endif()

# Create Info.plist for macOS
if(APPLE)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
    )
    
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
        @ONLY
    )
endif()

# Print configuration summary
message(STATUS "BootUSB Configuration:")
message(STATUS "  Platform: ${PLATFORM_NAME}")
message(STATUS "  Executable: ${EXECUTABLE_NAME}")
message(STATUS "  Version: ${PROJECT_VERSION}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")