#include "ImageComparator.h"
#include "ErrorPixelTransform.h"
#include <iostream>
#include <sstream>

cv::Vec3b parseColor(const std::string& colorStr) {
    std::stringstream ss(colorStr);
    int b, g, r;
    char delim;

    ss >> b >> delim >> g >> delim >> r;

    return cv::Vec3b(b, g, r);
}

void setErrorPixelTransformFunction(ImageComparator& comparator, const std::string& transformType) {
    if (transformType == "flat") {
        comparator.setErrorPixelTransform(ErrorPixelTransform::flat);
    } else if (transformType == "movement") {
        comparator.setErrorPixelTransform(ErrorPixelTransform::movement);
    } else if (transformType == "flatDifferenceIntensity") {
        comparator.setErrorPixelTransform(ErrorPixelTransform::flatDifferenceIntensity);
    } else if (transformType == "movementDifferenceIntensity") {
        comparator.setErrorPixelTransform(ErrorPixelTransform::movementDifferenceIntensity);
    } else if (transformType == "diffOnly") {
        comparator.setErrorPixelTransform(ErrorPixelTransform::diffOnly);
    } else {
        std::cerr << "Unknown transform type: " << transformType << std::endl;
    }
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
    bool ignoreAlpha = false; // Default false
    double pixelThreshold = 0.1; // Default threshold value
    std::string transformType = "flat"; // Default transform type

    // Process command line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--mismatchColor" && i + 1 < argc) {
            mismatchColor = parseColor(argv[++i]);
        } else if (std::string(argv[i]) == "--ignoreAntialiasing" && i + 1 < argc) {
            ignoreAntialiasing = std::stoi(argv[++i]) != 0;
        } else if (std::string(argv[i]) == "--ignoreColors" && i + 1 < argc) {
            ignoreColors = std::stoi(argv[++i]) != 0;
        } else if (std::string(argv[i]) == "--ignoreAlpha" && i + 1 < argc) {
            ignoreAlpha = std::stoi(argv[++i]) != 0;
        } else if (std::string(argv[i]) == "--pixelThreshold" && i + 1 < argc) {
            pixelThreshold = std::stod(argv[++i]);
        } else if (std::string(argv[i]) == "--transformType" && i + 1 < argc) {
            transformType = argv[++i];
        }
    }

    // Loop through each image and compare
    for (int i = 1; i < 5; ++i) {
        std::string baseImagePath = baseImageDir + "/base" + std::to_string(i) + ".png";
        std::string compareImagePath = compareImageDir + "/compare" + std::to_string(i) + ".png";
        std::string outputPath = outputDir + "/output" + std::to_string(i) + ".png";

        ImageComparator comparator(baseImagePath, compareImagePath);
        comparator.setMismatchPaintColor(mismatchColor);
        comparator.setIgnoreAntialiasing(ignoreAntialiasing);
        comparator.setIgnoreColors(ignoreColors);
        comparator.setIgnoreAlpha(ignoreAlpha);
        comparator.setPixelThreshold(pixelThreshold);
        setErrorPixelTransformFunction(comparator, transformType);
        comparator.exactComparison(outputPath);

        std::cout << "Output image: " << outputPath << "\n";
    }

    return 0;
}
