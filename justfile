set shell := ["bash", "-c"]

build := "Release"

# Recipe for building in Release mode
build:
    # Run CMake with the Release preset
    cmake --preset config-{{build}} 
    # Source ROS and build
    source /opt/ros/humble/setup.sh && cmake --build --preset build-{{build}}
