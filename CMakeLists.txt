cmake_minimum_required(VERSION 3.10)
project(ImageComparison)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build type to Release for optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Add optimization flags
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Find OpenCV
find_package(OpenCV REQUIRED)

# Include directories
include_directories(${OpenCV_INCLUDE_DIRS} include)

# Source files
set(SOURCES
    src/ImageComparator.cpp
    src/ImageUtils.cpp
    src/ErrorPixelTransform.cpp
    src/main.cpp
)

# Add executable
add_executable(image_comparison ${SOURCES})

# Link libraries
target_link_libraries(image_comparison ${OpenCV_LIBS})
