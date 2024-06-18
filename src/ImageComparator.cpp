#include "ImageComparator.h"
#include "ImageUtils.h"
#include "ErrorPixelTransform.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

ImageComparator::ImageComparator(const std::string& imgPath1, const std::string& imgPath2)
    : imgPath1(imgPath1), imgPath2(imgPath2), mismatchPaintColor(cv::Vec3b(0, 0, 255)), ignoreAntialiasing(true), ignoreColors(false), ignoreAlpha(false), pixelThreshold(0.1), highlightTransparency(255), errorPixelTransform(ErrorPixelTransform::flat) {} // Default values

void ImageComparator::setMismatchPaintColor(const cv::Vec3b& color) {
    mismatchPaintColor = color;
}

void ImageComparator::setIgnoreAntialiasing(bool value) {
    ignoreAntialiasing = value;
}

void ImageComparator::setIgnoreColors(bool value) {
    ignoreColors = value;
}

void ImageComparator::setIgnoreAlpha(bool value) {
    ignoreAlpha = value;
}

void ImageComparator::setPixelThreshold(double value) {
    pixelThreshold = value;
}

void ImageComparator::setBoundingBoxes(const std::vector<Box>& boxes) {
    boundingBoxes = boxes;
    ignoreBoxes.clear(); // Clear ignoreBoxes if boundingBoxes are set
}

void ImageComparator::setIgnoreBoxes(const std::vector<Box>& boxes) {
    ignoreBoxes = boxes;
    boundingBoxes.clear(); // Clear boundingBoxes if ignoreBoxes are set
}

void ImageComparator::setHighlightTransparency(int value) {
    highlightTransparency = value;
}

void ImageComparator::setErrorPixelTransform(void (*transformFunc)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&)) {
    errorPixelTransform = transformFunc;
}

bool ImageComparator::isInBoundingBox(int x, int y) const {
    for (const auto& box : boundingBoxes) {
        if (x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
            return true;
        }
    }
    return false;
}

bool ImageComparator::isInIgnoreBox(int x, int y) const {
    for (const auto& box : ignoreBoxes) {
        if (x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
            return true;
        }
    }
    return false;
}

bool ImageComparator::comparePixels(const cv::Mat& img1, const cv::Mat& img2, int x, int y, int width, int height) const {
    try {
        if (x >= width || y >= height) {
            std::cerr << "Pixel access out of bounds at (" << x << ", " << y << ")" << std::endl;
            return false;
        }

        if (!boundingBoxes.empty() && !isInBoundingBox(x, y)) {
            return false;
        }

        if (!ignoreBoxes.empty() && isInIgnoreBox(x, y)) {
            return false;
        }

        cv::Vec4b pixel1 = img1.at<cv::Vec4b>(y, x);
        cv::Vec4b pixel2 = img2.at<cv::Vec4b>(y, x);

        if (ignoreAlpha) {
            pixel1[3] = 255; // Set alpha to 255 (opaque) for comparison
            pixel2[3] = 255;
        }

        if (ignoreColors) {
            double intensity1 = 0.2989 * pixel1[2] + 0.5870 * pixel1[1] + 0.1140 * pixel1[0];
            double intensity2 = 0.2989 * pixel2[2] + 0.5870 * pixel2[1] + 0.1140 * pixel2[0];
            if (std::abs(intensity1 - intensity2) < 1e-5) {
                return false;
            }
        } else {
            // Calculate Euclidean distance between the pixels
            double distance = std::sqrt(
                std::pow(static_cast<double>(pixel1[0]) - pixel2[0], 2.0) +
                std::pow(static_cast<double>(pixel1[1]) - pixel2[1], 2.0) +
                std::pow(static_cast<double>(pixel1[2]) - pixel2[2], 2.0) +
                std::pow(static_cast<double>(pixel1[3]) - pixel2[3], 2.0)
            );

            double maxDistance = std::sqrt(4.0 * 255.0 * 255.0);
            if (distance / maxDistance < pixelThreshold) {
                return false;
            }
        }

        if (ignoreAntialiasing && ImageUtils::antialiased(img1, x, y, width, height, img2)) {
            return false;
        }

        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception in comparePixels: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception in comparePixels: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown exception in comparePixels at (" << x << ", " << y << ")" << std::endl;
        return false;
    }
}

void ImageComparator::exactComparison(const std::string& outputPath) const {
    try {
        cv::Mat baseImg = cv::imread(imgPath1, cv::IMREAD_UNCHANGED);
        cv::Mat compareImg = cv::imread(imgPath2, cv::IMREAD_UNCHANGED);

        if (baseImg.empty() || compareImg.empty()) {
            std::cerr << "Could not open or find the images!" << std::endl;
            return;
        }

        int maxWidth = std::max(baseImg.cols, compareImg.cols);
        int maxHeight = std::max(baseImg.rows, compareImg.rows);

        // Ensure both images have 4 channels
        if (baseImg.channels() < 4) {
            cv::cvtColor(baseImg, baseImg, cv::COLOR_BGR2BGRA);
        }
        if (compareImg.channels() < 4) {
            cv::cvtColor(compareImg, compareImg, cv::COLOR_BGR2BGRA);
        }

        // Create a new canvas with the max dimensions and fill with a background color (e.g., black)
        cv::Mat canvas = cv::Mat::zeros(maxHeight, maxWidth, CV_8UC4);
        cv::Mat baseCanvas = canvas.clone();
        cv::Mat compareCanvas = canvas.clone();

        // Copy baseImg and compareImg into their respective canvases
        baseImg.copyTo(baseCanvas(cv::Rect(0, 0, baseImg.cols, baseImg.rows)));
        compareImg.copyTo(compareCanvas(cv::Rect(0, 0, compareImg.cols, compareImg.rows)));

        cv::Mat resultImg = baseCanvas.clone();

        // Convert mismatchPaintColor from Vec3b to Vec4b
        cv::Vec4b mismatchColor4b(mismatchPaintColor[0], mismatchPaintColor[1], mismatchPaintColor[2], highlightTransparency);

        int mismatchedPixels = 0;
        for (int y = 0; y < maxHeight; ++y) {
            for (int x = 0; x < maxWidth; ++x) {
                if (comparePixels(baseCanvas, compareCanvas, x, y, maxWidth, maxHeight)) {
                    errorPixelTransform(resultImg.at<cv::Vec4b>(y, x), baseCanvas.at<cv::Vec4b>(y, x), compareCanvas.at<cv::Vec4b>(y, x), mismatchColor4b); // Use the error pixel transform function
                    ++mismatchedPixels;
                }
            }
        }

        double mismatchPercentage = 100.0 * mismatchedPixels / (maxHeight * maxWidth);
        std::cout << "Mismatch Percentage: " << mismatchPercentage << "%" << std::endl;

        cv::imwrite(outputPath, resultImg);
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception in exactComparison: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception in exactComparison: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in exactComparison" << std::endl;
    }
}
