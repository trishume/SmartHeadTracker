link_directories(/usr/local/lib)

find_path(LIBUSB_INCLUDE_DIR NAMES libusb.h PATH_SUFFIXES "include" "libusb" "libusb-1.0")
find_library(LIBUSB_LIBRARY_RAW NAMES usb)
# Fix the issue that libusb requires -1.0 on the end but CMake doesn't include it
string(REPLACE "b." "b-1.0." LIBUSB_LIBRARY ${LIBUSB_LIBRARY_RAW})

find_path(ZMQ_INCLUDE_DIR NAMES zmq.h HINTS /usr/local/Cellar/zeromq/4.0.5_2/include)
find_library(ZMQ_LIBRARY NAMES zmq HINTS /usr/local/Cellar/zeromq/4.0.5_2/lib)

find_path(HALIDE_INCLUDE_DIR Halide.h HINTS /usr/local/Cellar/halide/2015.10.22/include)
find_library(HALIDE_LIBRARY Halide HINTS /usr/local/Cellar/halide/2015.10.22/lib)

include_directories(${LIBUSB_INCLUDE_DIR} ${ZMQ_INCLUDE_DIR} ${PS3_INCLUDE} ${HALIDE_INCLUDE_DIR})

add_executable( SmartHeadTracker main.cpp markertracking.cpp server.cpp halideFuncs.cpp ${PS3_SOURCES})
set_property(TARGET SmartHeadTracker PROPERTY CXX_STANDARD 11)
set_property(TARGET SmartHeadTracker PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries( SmartHeadTracker ${OpenCV_LIBS} ${LIBUSB_LIBRARY} ${ZMQ_LIBRARY} ${HALIDE_LIBRARY})
