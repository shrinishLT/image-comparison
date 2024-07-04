cmake_minimum_required(VERSION 3.10)
project(ImageComparison)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set build type to Release for optimization
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
endif()

# Add optimization flags for Release build
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Set the target architecture for macOS
set(CMAKE_OSX_ARCHITECTURES "x86_64")  # Or "arm64" for Apple Silicon

# Set the path to vcpkg toolchain file
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "/Users/shrinishv/Documents/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")
endif()

# Include the vcpkg toolchain file
include(${CMAKE_TOOLCHAIN_FILE})

# Find required packages
find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${SPDLOG_INCLUDE_DIRS}
    /Users/shrinishv/Documents/vcpkg/installed/x64-osx/include  # Explicit path to spdlog
    include
)

# Source files
set(SOURCES
    src/interface.cpp

    src/comparator/ImageHandler.cpp
    src/comparator/ImageComparator.cpp
    src/comparator/ImageUtils.cpp
    src/comparator/ErrorPixelTransform.cpp
)

# Google Test
enable_testing()

# Add test executable
add_executable(run_tests
    tests/main_test.cpp
    tests/comparator/test_image_comparator.cpp
    tests/comparator/test_image_handler.cpp
    tests/comparator/test_image_utils.cpp
    tests/comparator/test_image_error_pixel_transform.cpp
)

# Link Google Test libraries
target_link_libraries(run_tests GTest::GTest GTest::Main pthread)

# Add main executable
add_executable(image_comparison ${SOURCES})

# Add shared library
add_library(interface SHARED ${SOURCES})

# Link shared library with required libraries
target_link_libraries(interface
    ${OpenCV_LIBS}
    ${CURL_LIBRARIES}
    spdlog::spdlog
)


# Link main executable with required libraries
target_link_libraries(image_comparison
    ${OpenCV_LIBS}
    ${CURL_LIBRARIES}
    spdlog::spdlog
)

# Add tests
add_test(NAME run_tests COMMAND run_tests)
