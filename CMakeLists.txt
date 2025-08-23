cmake_minimum_required(VERSION 3.10)
project(CN-ns3-project)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include OpenCV directories
include_directories(${OpenCV_INCLUDE_DIRS})

# Add your simulation program
add_executable(footballer-network-sim 
    footballer-network-sim.cc
    image_classifier.cpp
)

# Link with ns-3 CMake targets and OpenCV
target_link_libraries(footballer-network-sim
  PRIVATE
    ns3::core
    ns3::network
    ns3::internet
    ns3::applications
    ns3::mobility
    ns3::netanim
    ns3::flow-monitor
    ns3::point-to-point
    ${OpenCV_LIBS}
)

# Print OpenCV information
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")