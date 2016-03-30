link_directories(/usr/local/lib)

find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY_RAW NAMES usb)
# Fix the issue that libusb requires -1.0 on the end but CMake doesn't include it
string(REPLACE "b." "b-1.0." LIBUSB_LIBRARY ${LIBUSB_LIBRARY_RAW})
include_directories(${LIBUSB_INCLUDE_DIR} ${PS3_INCLUDE})

add_executable( SmartHeadTracker main.cpp markertracking.cpp ${PS3_SOURCES})
set_property(TARGET SmartHeadTracker PROPERTY CXX_STANDARD 11)
set_property(TARGET SmartHeadTracker PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries( SmartHeadTracker ${OpenCV_LIBS} ${LIBUSB_LIBRARY})
