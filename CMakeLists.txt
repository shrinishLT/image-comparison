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

# Include the vcpkg toolchain file if it is defined
if(DEFINED CMAKE_TOOLCHAIN_FILE)
  include(${CMAKE_TOOLCHAIN_FILE})
endif()

# Find required packages
find_package(OpenCV REQUIRED)
find_package(CURL REQUIRED)
find_package(GTest REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Include directories
include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    include
)

# Source files
set(SOURCES
    src/main.cpp
    src/Logger.cpp
    src/JsonUtils.cpp

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

# Link main executable with required libraries
target_link_libraries(image_comparison
    ${OpenCV_LIBS}
    ${CURL_LIBRARIES}
    nlohmann_json::nlohmann_json
)

# Add tests
add_test(NAME run_tests COMMAND run_tests)
