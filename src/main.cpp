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

std::vector<Box> parseBoxes(int argc, char** argv, int startIndex) {
    std::vector<Box> boxes;
    for (int i = startIndex; i < argc; i += 4) {
        if (i + 3 < argc) {
            int top = std::stoi(argv[i]);
            int left = std::stoi(argv[i + 1]);
            int bottom = std::stoi(argv[i + 2]);
            int right = std::stoi(argv[i + 3]);
            boxes.emplace_back(Box{top, left, bottom, right});
        }
    }
    return boxes;
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
    std::string outputDisplacementDir = "../output/hide_diff";

    // Default options
    cv::Vec3b mismatchColor = cv::Vec3b(0, 0, 255); // Default red color
    bool ignoreAntialiasing = true; // Default true
    bool ignoreColors = false; // Default false
    bool ignoreAlpha =  false; // Default false
    double pixelThreshold = 0.1; // Default threshold value
    std::string transformType = "movement"; // Default transform type
    int highlightTransparency = 255; // Default opaque (for transparent use 128 (50% transparent))
    std::vector<Box> boundingBoxes;
    std::vector<Box> ignoreBoxes;
    bool useIgnoreDisplacements = false;

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
        } else if (std::string(argv[i]) == "--highlightTransparency" && i + 1 < argc) {
            highlightTransparency = std::stoi(argv[++i]);
        } else if (std::string(argv[i]) == "--boundingBoxes" && i + 4 < argc) {
            boundingBoxes = parseBoxes(argc, argv, i + 1);
            i += boundingBoxes.size() * 4;
        } else if (std::string(argv[i]) == "--ignoreBoxes" && i + 4 < argc) {
            ignoreBoxes = parseBoxes(argc, argv, i + 1);
            i += ignoreBoxes.size() * 4;
        } else if (std::string(argv[i]) == "--useIgnoreDisplacements") {
            useIgnoreDisplacements = true;
        }
    }

    // Loop through each image and compare
    for (int i = 1; i < 6; ++i) {
        std::string baseImagePath = baseImageDir + "/base" + std::to_string(i) + ".png";
        std::string compareImagePath = compareImageDir + "/compare" + std::to_string(i) + ".png";
        std::string outputPath = outputDisplacementDir + "/output" + std::to_string(i) + ".png";

        ImageComparator comparator(baseImagePath, compareImagePath);
        comparator.setMismatchPaintColor(mismatchColor);
        comparator.setIgnoreAntialiasing(ignoreAntialiasing);
        comparator.setIgnoreColors(ignoreColors);
        comparator.setIgnoreAlpha(ignoreAlpha);
        comparator.setPixelThreshold(pixelThreshold);
        comparator.setHighlightTransparency(highlightTransparency);
        setErrorPixelTransformFunction(comparator, transformType);

        if (!boundingBoxes.empty()) {
            comparator.setBoundingBoxes(boundingBoxes);
        } else if (!ignoreBoxes.empty()) {
            comparator.setIgnoreBoxes(ignoreBoxes);
        }
        std::cout << "ignore Starts" << std::endl;

        // comparator.exactComparison(outputPath); 
        comparator.ignoreDisplacementsComparison(outputPath);

        std::cout << "Output image: " << outputPath << "\n";
    }

    return 0;
}
