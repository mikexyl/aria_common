# Get the directory containing this file.
get_filename_component(aria_common_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

# Define the location of the headers and libraries.
set(aria_common_INCLUDE_DIRS "${aria_common_CMAKE_DIR}/../../../include")
set(aria_common_LIBRARIES "${aria_common_CMAKE_DIR}/../../../lib/libaria_common.so") # Adjust for static lib if necessary

# Include directories
include_directories(${aria_common_INCLUDE_DIRS})

# Optionally, you can check for dependencies of aria_common here.
# find_package(AnotherLibrary REQUIRED)

# If aria_common depends on other libraries, and those libraries
# are not part of the standard link path, add link directories like so:
# link_directories(${aria_common_CMAKE_DIR}/../../../lib)

# Make the above variables available to the finder.
set(aria_common_FOUND TRUE)
