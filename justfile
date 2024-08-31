set shell := ["bash", "-c"]

# Recipe for building in Release mode
build-Release:
    # Run CMake with the Release preset
    cmake --preset config-Release
    # Source ROS and build
    source /opt/ros/humble/setup.sh && cmake --build --preset build-Release

# Recipe for building in Debug mode
build-Debug:
    # Run CMake with the Debug preset
    cmake --preset config-Debug
    # Source ROS and build
    . /opt/ros/humble/setup.sh && cmake --build --preset build-Debug
