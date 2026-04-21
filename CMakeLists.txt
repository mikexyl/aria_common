cmake_minimum_required(VERSION 3.16)
project(aria_common VERSION 1.0 DESCRIPTION "Aria Project, Aria Common Package" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# find gtsam
find_package(GTSAM REQUIRED)

# find spdlog
find_package(spdlog REQUIRED)

# find opencv
find_package(OpenCV REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(TBB REQUIRED)
find_package(g2o QUIET)
if (g2o_FOUND)
        # set a marcros to enable g2o
        add_definitions(-DUSE_G2O)
else()
        message(WARNING "G2O not found, disabling g2o")
endif()

find_package(yaml-cpp REQUIRED)

set(INCLUDES
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${spdlog_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
        ${TBB_INCLUDE_DIRS}
)

if(g2o_FOUND)
set(LIBS
        gtsam
        spdlog::spdlog
        ${OpenCV_LIBS}
        Eigen3::Eigen
        g2o::core
        g2o::stuff
        g2o::opengl_helper
        g2o::types_sim3
        g2o::types_sba
        g2o::types_slam3d
        ${TBB_LIBRARIES}
        yaml-cpp
)
else()
set(LIBS
        gtsam
        spdlog::spdlog
        ${OpenCV_LIBS}
        Eigen3::Eigen
        TBB::tbb TBB::tbbmalloc
        yaml-cpp
)
endif()

add_library(${PROJECT_NAME} SHARED
        src/logging.cpp
        src/benchmark.cpp
)
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${INCLUDES}
)
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${LIBS}
)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}Targets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(DIRECTORY include/aria_common
        DESTINATION include)

install(EXPORT ${PROJECT_NAME}Targets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION lib/cmake/${PROJECT_NAME}
)

export (TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Targets.cmake)

# Configure and install the package configuration file
include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME}
)
