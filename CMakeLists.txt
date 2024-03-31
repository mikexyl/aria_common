cmake_minimum_required(VERSION 3.16)
project(aria_common VERSION 1.0 DESCRIPTION "Aria Project, Aria Common Package" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Glog REQUIRED)
find_package(Gflags REQUIRED)
find_package(GTSAM REQUIRED)
find_package(GTSAM_UNSTABLE REQUIRED)

# find catkin
find_package(rclcpp QUIET)

# if catkin is found, add the catkin include directories
if(rclcpp_FOUND)
        find_package(rosbag2_cpp REQUIRED)
        find_package(rosbag2_storage REQUIRED)
        set(ROS_SRC
                src/rosbag_reader.cpp
                src/rosbag_writer.cpp
        )
        set(ROS_LIBS
                rosbag2_cpp::rosbag2_cpp
                rosbag2_storage::rosbag2_storage
        )
else()
        set(ROS_SRC "")
        set(rclcpp_INCLUDE_DIRS "")
        set(rclcpp_LIBRARIES "")
        set(ROS_LIBS "")
endif()

add_library(${PROJECT_NAME}
    src/dependency_tracker.cpp
    ${ROS_SRC}
)
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${GFLAGS_INCLUDE_DIR}
        ${GLOG_INCLUDE_DIR}
        ${GTSAM_INCLUDE_DIR}
        ${GTSAM_UNSTABLE_INCLUDE_DIR}
        ${rclcpp_INCLUDE_DIRS}
)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${GFLAGS_LIBRARY} ${GLOG_LIBRARY}
        ${GTSAM_LIBRARY} ${GTSAM_UNSTABLE_LIBRARY}
        ${rclcpp_LIBRARIES}
        ${ROS_LIBS}
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