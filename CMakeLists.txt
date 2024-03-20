cmake_minimum_required(VERSION 3.16)
project(aria_common VERSION 1.0 DESCRIPTION "Aria Project, Aria Common Package" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include_directories(include)

add_library(${PROJECT_NAME}
    src/dependency_tracker.cpp
)

install(TARGETS aria_common 
        EXPORT aria_commonTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY include/aria_common
        DESTINATION include)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/aria_common/aria_commonConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

export(TARGETS aria_common
       NAMESPACE aria::
       FILE "${CMAKE_CURRENT_BINARY_DIR}/aria_common/aria_commonTargets.cmake")

configure_file(cmake/aria_commonConfig.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/aria_common/aria_commonConfig.cmake"
    COPYONLY)

set(ConfigPackageLocation lib/cmake/aria_common)
install(EXPORT aria_commonTargets
        FILE aria_commonTargets.cmake
        NAMESPACE aria_common::
        DESTINATION ${ConfigPackageLocation})

install(FILES
        cmake/aria_commonConfig.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/aria_common/aria_commonConfigVersion.cmake"
        DESTINATION ${ConfigPackageLocation})