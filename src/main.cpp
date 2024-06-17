#include "ImageComparator.h"
#include <iostream>
#include <sstream>

cv::Vec3b parseColor(const std::string& colorStr) {
    std::stringstream ss(colorStr);
    int b, g, r;
    char delim;

    ss >> b >> delim >> g >> delim >> r;

    return cv::Vec3b(b, g, r);
}

int main(int argc, char** argv) {
     // Directories and paths
    std::string baseImageDir = "../images/base";
    std::string compareImageDir = "../images/compare";
    std::string outputDir = "../output/standard";

    // Default options
    cv::Vec3b mismatchColor = cv::Vec3b(0, 0, 255); // Default red color
    bool ignoreAntialiasing = true; // Default true
    bool ignoreColors = false; // Default false
    // std::string transformType = "flat"; // Default transform type

    
    // Loop through each image and compare
    for (int i =1; i < 5; ++i) {
        std::string baseImagePath = baseImageDir + "/base" + std::to_string(i) + ".png";
        std::string compareImagePath = compareImageDir + "/compare" + std::to_string(i) + ".png";
        std::string outputPath = outputDir + "/output" + std::to_string(i) + ".png";

        ImageComparator comparator(baseImagePath, compareImagePath);
        comparator.setMismatchPaintColor(mismatchColor);
        comparator.setIgnoreAntialiasing(ignoreAntialiasing);
        comparator.setIgnoreColors(ignoreColors);
        // setErrorPixelTransformFunction(comparator, transformType);
        comparator.exactComparison(outputPath);

        std::cout << "Output image: " << outputPath << "\n";
    }

    return 0;
}
