cmake_minimum_required(VERSION 3.15)
project(VideoColorQuantizer)

# Use C++17 or newer
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable warnings and optimization flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

# Find OpenCL
find_package(OpenCL REQUIRED)

# Find Boost libraries with program options
cmake_policy(SET CMP0167 NEW)
FIND_PACKAGE(Boost COMPONENTS program_options REQUIRED)
# print out the boost include directories
MESSAGE( STATUS "Boost_INCLUDE_DIRS: " ${Boost_INCLUDE_DIRS} )


# Optionally, find FFmpeg libraries manually or through pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(AVFORMAT REQUIRED libavformat)
pkg_check_modules(AVCODEC REQUIRED libavcodec)
pkg_check_modules(AVUTIL REQUIRED libavutil)
pkg_check_modules(SWSCALE REQUIRED libswscale)

# Define include directories
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${OpenCL_INCLUDE_DIRS}
  ${AVFORMAT_INCLUDE_DIRS}
  ${AVCODEC_INCLUDE_DIRS}
  ${AVUTIL_INCLUDE_DIRS}
  ${SWSCALE_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src
)

# Add your source files
file(GLOB_RECURSE SOURCES
  "src/*.cpp"
  "src/*.hpp"
)

# Add kernel files
file(GLOB_RECURSE KERNELS
  "src/kernels/*.cl"
)
file(GLOB KERNELS "kernels/*.cl")
foreach(KERNEL ${KERNELS})
  configure_file(${KERNEL} ${CMAKE_BINARY_DIR} COPYONLY)
endforeach()

# Create the executable
add_executable(video_quantizer ${SOURCES})

# Link to required libraries
target_link_libraries(video_quantizer
  ${Boost_LIBRARIES}
  ${Boost_PROGRAM_OPTIONS_LIBRARY}
  ${OpenCL_LIBRARIES}
  ${AVFORMAT_LIBRARIES}
  ${AVCODEC_LIBRARIES}
  ${AVUTIL_LIBRARIES}
  ${SWSCALE_LIBRARIES}
)
