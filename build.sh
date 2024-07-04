# Create the build directory if it doesn't exist
mkdir -p build

# Compile the shared library -> add all the source files that should be compiled
g++ -shared -o build/imageComparisonInterface.so -fPIC -std=c++17 -I./include -I./include/Comparator \
    -I/usr/local/include/opencv4 -I/usr/local/include/spdlog -I/usr/local/include \
    src/interface.cpp src/comparator/ImageHandler.cpp src/comparator/ImageComparator.cpp \
    src/comparator/ErrorPixelTransform.cpp src/comparator/ImageUtils.cpp \
    -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lcurl -lspdlog

# Check if the compilation was successful
if [ $? -eq 0 ]; then
  echo "Shared library built successfully."
else
  echo "Failed to build shared library."
  exit 1
fi

## can be run if you are on a linux machine