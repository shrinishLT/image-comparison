#include "ImageComparator.h"
#include "ImageUtils.h"
#include <opencv2/opencv.hpp>
#include <iostream>

ImageComparator::ImageComparator(const std::string& imgPath1, const std::string& imgPath2)
    : imgPath1(imgPath1), imgPath2(imgPath2), mismatchPaintColor(cv::Vec3b(0, 0, 255)), ignoreAntialiasing(true), ignoreColors(false), ignoreAlpha(false) {} // Default values

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

bool ImageComparator::comparePixels(const cv::Mat& img1, const cv::Mat& img2, int x, int y, int width, int height) const {
    try {
        if (x >= width || y >= height) {
            std::cerr << "Pixel access out of bounds at (" << x << ", " << y << ")" << std::endl;
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
            if (pixel1 == pixel2) {
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

        if (baseImg.cols != compareImg.cols) {
            std::cerr << "Images do not have the same width!" << std::endl;
            return;
        }

        // Convert images to 4 channels if they don't have enough channels
        if (baseImg.channels() < 4) {
            cv::cvtColor(baseImg, baseImg, cv::COLOR_BGR2BGRA);
        }
        if (compareImg.channels() < 4) {
            cv::cvtColor(compareImg, compareImg, cv::COLOR_BGR2BGRA);
        }

        int maxHeight = std::max(baseImg.rows, compareImg.rows);
        cv::Mat resultImg = (baseImg.rows >= compareImg.rows) ? baseImg.clone() : compareImg.clone();
        cv::resize(resultImg, resultImg, cv::Size(resultImg.cols, maxHeight));

        int mismatchedPixels = 0;
        for (int y = 0; y < std::min(baseImg.rows, compareImg.rows); ++y) {
            for (int x = 0; x < baseImg.cols; ++x) {
                if (comparePixels(baseImg, compareImg, x, y, baseImg.cols, baseImg.rows)) {
                    if (x >= resultImg.cols || y >= resultImg.rows) {
                        std::cerr << "Attempted to access pixel out of bounds at (" << x << ", " << y << ")" << std::endl;
                        continue;
                    }
                    resultImg.at<cv::Vec4b>(y, x) = cv::Vec4b(mismatchPaintColor[0], mismatchPaintColor[1], mismatchPaintColor[2], 255); // Paint the pixel with the custom color
                    ++mismatchedPixels;
                }
            }
        }

        double mismatchPercentage = 100.0 * mismatchedPixels / (baseImg.rows * baseImg.cols);
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
