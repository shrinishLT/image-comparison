#include "Comparator/ImageHandler.h"   
#include "Comparator/ImageComparator.h"
#include "Comparator/ErrorPixelTransform.h"
#include "JsonUtils.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <csignal>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include <nlohmann/json.hpp>


// Signal handler for cleanup on termination
void signalHandler(int signum) {
    std::cerr << "Error: signal " << signum << " received." << std::endl;
    exit(signum);
}

// Parse color from string format "b,g,r"
cv::Vec3b parseColor(const std::string& colorStr) {
    std::stringstream ss(colorStr);
    int b, g, r;
    char delim;
    ss >> b >> delim >> g >> delim >> r;
    return cv::Vec3b(b, g, r);
}

// Parse bounding boxes from command line arguments
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

// Set error pixel transform function based on input
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
// Function to write image buffer to an output file
void writeImageBufferToFile(const std::vector<uchar>& imgBuffer, const std::string& outputFilePath) {
    // Decode the image buffer to create an OpenCV Mat
    cv::Mat img = cv::imdecode(imgBuffer, cv::IMREAD_COLOR);

    // Check if the image was successfully decoded
    if (img.empty()) {
        std::cerr << "Failed to decode the image buffer." << std::endl;
        return;
    }

    // Write the image to the output file
    if (!cv::imwrite(outputFilePath, img)) {
        std::cerr << "Failed to write the image to " << outputFilePath << std::endl;
    } else {
        std::cout << "Image successfully written to " << outputFilePath << std::endl;
    }
}

int main(int argc, char** argv) {
    Logger logger("root");

    if (argc < 4) {
        logger.handleError(__func__, "Usage: " + std::string(argv[0]) + " <baseImageURL> <compareImageURL> <functionType> [optional arguments]");
        // Exit the program
        return EXIT_FAILURE;
    }

    // Register signal handler
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);

    // Parse compulsory arguments
    std::string baseImageURL = argv[1];
    std::string compareImageURL = argv[2];
    std::string comparisonType = argv[3];

    // Default options
    cv::Vec3b mismatchColor = cv::Vec3b(255, 0, 255); // Default magenta color
    bool ignoreAntialiasing = true; // Default true
    bool ignoreColors = false; // Default false
    bool ignoreAlpha = false; // Default false
    double pixelThreshold = 0.5; // Default threshold value
    int highlightTransparency = 0; // Default transparent 
    std::vector<Box> boundingBoxes;
    std::vector<Box> ignoreBoxes;
    std::string transformType = "movement";

    // Process optional command line arguments
    for (int i = 4; i < argc; ++i) {
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
        }
    }

    // Load images
    ImageHandler handler1(baseImageURL);        
    ImageHandler handler2(compareImageURL);     
    auto img1 = handler1.loadImage(); // returns a pointer to cv::Mat
    auto img2 = handler2.loadImage(); // returns a pointer to cv::Mat

    // Set up comparator
    ImageComparator comparator;
    setErrorPixelTransformFunction(comparator, transformType);
    comparator.setMismatchPaintColor(mismatchColor);
    comparator.setIgnoreAntialiasing(ignoreAntialiasing);
    comparator.setIgnoreColors(ignoreColors);
    comparator.setIgnoreAlpha(ignoreAlpha);
    comparator.setPixelThreshold(pixelThreshold);
    comparator.setHighlightTransparency(highlightTransparency);
    comparator.setBoundingBoxes(boundingBoxes);
    comparator.setIgnoreBoxes(ignoreBoxes);

    // Perform comparison
    nlohmann::json resultJSON;

    std::string exactURL = "../test/URL/exact/output" + std::to_string(1) + ".png";
    std::string smartIgnoreBase = "../test/URL/smartIgnore/base/output" + std::to_string(1) + ".png";
    std::string smartIgnoreCompare = "../test/URL/smartIgnore/compare/output" + std::to_string(1) + ".png";

    if (comparisonType == "smartIgnore"){
        SmartIgnoreComparisonResult result = comparator.smartIgnoreComparison(img1, img2);
        writeImageBufferToFile(result.baseResultBuffer, smartIgnoreBase);
        writeImageBufferToFile(result.compareResultBuffer, smartIgnoreCompare);
        resultJSON = JsonUtils::to_json(result);
        logger.logJsonResponse("success", resultJSON.dump());
    }
    else if (comparisonType == "exact"){
        ExactComparisonResult result = comparator.exactComparison(img1,img2);
        writeImageBufferToFile(result.resultBuffer, exactURL);
        resultJSON = JsonUtils::to_json(result);
        logger.logJsonResponse("success", resultJSON.dump());
    }
    else {
        logger.handleError(__func__, "Invalid comparison type: " + comparisonType);
        return EXIT_FAILURE;
    }

    
    
    return 0;
}
