cmake_minimum_required(VERSION 3.10)
project(CN-ns3-project)

# Add your simulation program
add_executable(footballer-network-sim footballer-network-sim.cc)

# Link with ns-3 libraries
target_link_libraries(footballer-network-sim
  PRIVATE
    libns3.45-applications
    libns3.45-internet
    libns3.45-core
    libns3.45-network
    libns3.45-mobility
)
