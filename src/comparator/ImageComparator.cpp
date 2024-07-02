#include "Comparator/ImageComparator.h"
#include "Comparator/ImageUtils.h"
#include "Comparator/ErrorPixelTransform.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <utility> // For std::pair
#include <chrono>
#include <functional>


ImageComparator::ImageComparator(const std::shared_ptr<cv::Mat>& img1, const std::shared_ptr<cv::Mat>& img2)
    : img1(img1), img2(img2), mismatchPaintColor(cv::Vec3b(0, 0, 255)), ignoreAntialiasing(true), ignoreColors(false), ignoreAlpha(false), pixelThreshold(0.1), highlightTransparency(255), errorPixelTransform(ErrorPixelTransform::flat), canvasWidth(0), canvasHeight(0), scaleToSameSize(false), mismatchedPixels(0), logger("ImageComparator") {} // Default values

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

void ImageComparator::setScaleToSameSize(bool value) { 
    scaleToSameSize = value;
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


// under construction 
bool ImageComparator::isInBoundingBox(int x, int y) const {
    for (const auto& box : boundingBoxes) {
        if (x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
            return true;
        }
    }
    return false;
}

// under construction
bool ImageComparator::isInIgnoreBox(int x, int y) const {
    for (const auto& box : ignoreBoxes) {
        if (x >= box.left && x <= box.right && y >= box.top && y <= box.bottom) {
            return true;
        }
    }
    return false;
}

// under construction
// returns true if difference is found
bool ImageComparator::comparePixels(const cv::Mat& img1, const cv::Mat& img2, int y1, int x1, int y2, int x2) const {
    try {
        if (x1 >= img1.cols  || y1 >= img1.rows || x2 >= img2.cols || y2 >= img2.rows) {
            // pixles dont exist in both cases so its a mismatch 
            return true;
        }
       
        if (!boundingBoxes.empty() && !isInBoundingBox(x1, y1)) {
            return false;
        }

        if (!ignoreBoxes.empty() && isInIgnoreBox(x1, y1)) {
            return false;
        }

        cv::Vec4b pixel1 = img1.at<cv::Vec4b>(y1, x1);
        cv::Vec4b pixel2 = img2.at<cv::Vec4b>(y2, x2);

        if (ignoreAlpha) {
            pixel1[3] = 255; // Set alpha to 255 (opaque) for comparison
            pixel2[3] = 255;
        }

        if (ignoreColors) {
            if(ImageUtils::doesIntensityMatch(pixel1,pixel2)){
                return false;
            }
        } else {
            // if(ImageUtils::isWithinThreshold(pixelThreshold, pixel1, pixel2)) {
            //     return false;
            // }
            double distance = std::sqrt(
            std::pow(static_cast<double>(pixel1[0]) - pixel2[0], 2.0) +
            std::pow(static_cast<double>(pixel1[1]) - pixel2[1], 2.0) +
            std::pow(static_cast<double>(pixel1[2]) - pixel2[2], 2.0) +
            std::pow(static_cast<double>(pixel1[3]) - pixel2[3], 2.0)
            );

            if (distance / std::sqrt(4.0 * 255.0 * 255.0) < pixelThreshold) {
                return false;
            }

        }

        if (ignoreAntialiasing && ImageUtils::antialiased(img1, img2, x1, y1, x2, y2) ) {
            return false;
        }

        return true;

    } catch (const cv::Exception& e) {
        BOOST_LOG_TRIVIAL(error) << "OpenCV exception in comparePixels: " << e.what();
        return false;
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Standard exception in comparePixels: " << e.what();
        return false;
    } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Unknown exception in comparePixels " << std::endl;
        return false;
    }
}

void ImageComparator::exactComparison(const std::string& outputPath) const {
     BOOST_LOG_NAMED_SCOPE(__func__); 
    try {
       

        if (img1->empty() || img2->empty()) {
            BOOST_LOG_TRIVIAL(error) << "Error reading images!";
            return;
        }

        canvasWidth = std::max(img1->cols, img2->cols);
        canvasHeight = std::max(img1->rows, img2->rows);

        if(scaleToSameSize){
            cv::resize(*img1, *img1, cv::Size(canvasWidth, canvasHeight));
            cv::resize(*img2, *img2, cv::Size(canvasWidth, canvasHeight));
        }

        // Ensure both images have 4 channels  --> This breaks comparisons in resemble.js 
        if (img1->channels() < 4) {
            cv::cvtColor(*img1, *img1, cv::COLOR_BGR2BGRA);
        }
        if (img2->channels() < 4) {
            cv::cvtColor(*img2, *img2, cv::COLOR_BGR2BGRA);
        }

        // Create a new canvas with the max dimensions and fill with a background color (e.g., black)
        cv::Mat resultCanvas = cv::Mat::zeros(canvasHeight, canvasWidth, CV_8UC4);

        // Copy the base image to the canvas
        img1->copyTo(resultCanvas(cv::Rect(0, 0, img1->cols, img1->rows)));


        // Convert mismatchPaintColor from Vec3b to Vec4b
        cv::Vec4b mismatchColor4b(mismatchPaintColor[0], mismatchPaintColor[1], mismatchPaintColor[2], highlightTransparency);
        mismatchedPixels = 0;

        for (int y = 0; y < canvasHeight; ++y) {
            for (int x = 0; x < canvasWidth; ++x) {
                if (comparePixels(*img1, *img2, y, x, y, x)) {
                    // what if this y,x doesnt exist for any of the images 
                    cv::Vec4b basePixel = cv::Vec4b(0, 0, 0, 0); 
                    cv::Vec4b comparePixel = cv::Vec4b(0, 0, 0, 0);

                    if( x < img1->cols && y < img1->rows){
                        basePixel =  basePixel = img1->at<cv::Vec4b>(y, x);
                    } 
                    if(x < img2->cols && y < img2->rows){
                        comparePixel = img2->at<cv::Vec4b>(y, x);
                    }
                    errorPixelTransform(resultCanvas.at<cv::Vec4b>(y, x),basePixel,comparePixel, mismatchColor4b);
                    ++mismatchedPixels;
                }
            }
        }

        double mismatchPercentage = 100.0 * mismatchedPixels / (canvasHeight * canvasWidth);

        BOOST_LOG_TRIVIAL(info) << "Mismatch Percentage: " << mismatchPercentage << "%";
        cv::imwrite(outputPath, resultCanvas);

    } catch (const cv::Exception& e) {
        BOOST_LOG_TRIVIAL(error) << "OpenCV exception in exactComparison: " << e.what();
        return;
    } catch (const std::exception& e) {
        BOOST_LOG_TRIVIAL(error) << "Standard exception in exactComparison: " << e.what();
        return;
    } catch (...) {
        BOOST_LOG_TRIVIAL(error) << "Unknown exception in exactComparison";
        return;
    }
}

void ImageComparator::ignoreDisplacementsComparison(const std::string& outputPath) const {
    try {

        if (img1->empty() || img2->empty()) {
            std::cerr << "Could not open or find the images!" << std::endl;
            return;
        }
        canvasWidth = std::max(img1->cols, img2->cols);
        canvasHeight = std::max(img1->rows, img2->rows);

        if(scaleToSameSize){
            cv::resize(*img1, *img1, cv::Size(canvasWidth, canvasHeight));
            cv::resize(*img2, *img2, cv::Size(canvasWidth, canvasHeight));
        }
      
        // Ensure both images have 4 channels
        if (img1->channels() < 4) {
            cv::cvtColor(*img1, *img1, cv::COLOR_BGR2BGRA);
        }
        if (img2->channels() < 4) {
            cv::cvtColor(*img2, *img2, cv::COLOR_BGR2BGRA);
        }

        // Create a new canvas with the max dimensions and fill with a background color (e.g., black)
        cv::Mat resultCanvas = cv::Mat::zeros(canvasHeight, canvasWidth, CV_8UC4);

        // Copy the base image to the canvas
        img1->copyTo(resultCanvas(cv::Rect(0, 0, img1->cols, img1->rows)));

        std::vector<int> mismatchedRows;
        std::vector<int> toMatchWith(canvasHeight, -1);

        int startRow1 = 0, startRow2 = 0;
        int endRow1 = img1->rows - 1, endRow2 = img2->rows - 1;
        mismatchedPixels = 0;
        int totalPixels = canvasWidth * canvasHeight;

        while (startRow1 < endRow1 && startRow2 < endRow2) {
            std::tie(startRow1, startRow2) = matchRows(startRow1, startRow2, 1, *img1, *img2);
            std::tie(endRow1, endRow2) = matchRows(endRow1, endRow2, -1, *img1, *img2);

            mismatchedRows.push_back(startRow2);
            // std::cout << startRow1 << " " << endRow1 << " " << startRow2 << " " << endRow2 << std::endl;

            auto [height, bestRow] = getBestMatchingRectangle(startRow1, startRow2, endRow1, endRow2, *img1, *img2, mismatchedRows) ;
    
            if (height < 3 ) {
                toMatchWith[startRow1] = startRow2;
                startRow1++;
                startRow2++;
            } else {
                // std::cout << "Matching rectangle found: " << height << " " << bestRow << std::endl;
                markMatchedRows(toMatchWith, startRow1, height);
                startRow1 += height;
                startRow2 = bestRow + height;
            }
        }

        paintMismatchedCells(toMatchWith, *img1, *img2, resultCanvas);
        cv::imwrite(outputPath, resultCanvas);

        double mismatchPercentage = 100.0 * mismatchedPixels / totalPixels;
        std::cout << mismatchedPixels << std::endl;
        std::cout << "Mismatch Percentage: " << mismatchPercentage << "%" << std::endl;

    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception in ignoreDisplacementsComparison: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Standard exception in ignoreDisplacementsComparison: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception in ignoreDisplacementsComparison" << std::endl;
    }
}

bool ImageComparator::doesRectangleMatch(int row1, int row2, int height, const cv::Mat& img1, const cv::Mat& img2) const {
    for (int row = 0; row < height; ++row) { 
        if (!doesRowsMatch(row1 + row, row2 + row, img1, img2)) {
            return false;
        }
    }
    return true;
}

int ImageComparator::binarySearchOnHeight(int row1, int row2, int maxHeight, const cv::Mat& img1, const cv::Mat& img2) const {
    int start = 0;
    int end = maxHeight;
    int ans = -1;
    while (start <= end) {
        int mid = (start + end) / 2;
        if (doesRectangleMatch(row1, row2, mid, img1, img2)) {
            start = mid + 1;
            ans = mid;
        } else {
            end = mid - 1;
        }
    }
    return ans;
}

std::pair<int, int> ImageComparator::getBestMatchingRectangle(int startRow1, int startRow2, int endRow1, int endRow2, const cv::Mat& img1, const cv::Mat& img2, const std::vector<int>& mismatchedRows) const {
    int bestHeight = 0;
    int bestRow = 0;

    for (int row2 : mismatchedRows) {
        if(std::abs(row2 - startRow1) > 200) continue; // skip if the row is too far (optimization + avoiding false positives)
        int maxHeight = std::min(endRow1 - startRow1 + 1, endRow2 - row2 + 1);
        int height = binarySearchOnHeight(startRow1, row2, maxHeight, img1, img2);

        if (height > bestHeight) {
            bestHeight = height;
            bestRow = row2;
        }
        // if(bestHeight > 200) return {bestHeight, bestRow};
    }

    for (int row2 = startRow2; row2 <= endRow2; ++row2) {
        if(std::abs(row2 - startRow1) > 200) continue;
        int maxHeight = std::min(endRow1 - startRow1 + 1, endRow2 - row2 + 1);
        int height = binarySearchOnHeight(startRow1, row2, maxHeight, img1, img2);

        if (height > bestHeight) {
            bestHeight = height;
            bestRow = row2;
        }
        // if(bestHeight > 200) return {bestHeight, bestRow};
    }
    
    if (bestHeight == 0) {
        return {-1, -1};
    }

    return {bestHeight, bestRow};
}

void ImageComparator::paintMismatchedCells(const std::vector<int>& toMatchWith, const cv::Mat& img1, const cv::Mat& img2, cv::Mat& paintedImg) const {
    for (size_t row1 = 0; row1 < toMatchWith.size(); ++row1) {
        int row2 = toMatchWith[row1];
        if (row2 == -1) {
            continue;
        }
        for (int col = 0; col < std::min(img1.cols,img2.cols); ++col) {
            if (row1 < img1.rows && row2 < img2.rows && comparePixels(img1, img2, static_cast<int>(row1), col, row2, col)) {
                mismatchedPixels++;
                cv::Vec4b mismatchColor4b(mismatchPaintColor[0], mismatchPaintColor[1], mismatchPaintColor[2], highlightTransparency);
                errorPixelTransform(paintedImg.at<cv::Vec4b>(row1, col), img1.at<cv::Vec4b>(row1, col), img2.at<cv::Vec4b>(row2, col), mismatchColor4b); // Use the error pixel transform function
            }
        }
    }
}

bool ImageComparator::doesRowsMatch(int row1, int row2, const cv::Mat& img1, const cv::Mat& img2) const {
    for (int col = 0; col < std::min(img1.cols, img2.cols); ++col) {
        if (comparePixels(img1, img2, row1, col, row2, col)) {
            return false;
        }
    }
    return true;
}

void ImageComparator::markMatchedRows(std::vector<int>& toMatchWith, int startRow1, int height) const {
    for (int row = startRow1; row < startRow1 + height; ++row) {
        toMatchWith[row] = -1;
    }
}

std::pair<int, int> ImageComparator::matchRows(int row1, int row2, int direction, const cv::Mat& img1, const cv::Mat& img2) const {
    while (row1 < img1.rows && row2 < img2.rows && row1 >= 0 && row2 >= 0 && doesRowsMatch(row1,row2,img1,img2)) {
        row1 += direction;
        row2 += direction;
    }
    return {row1, row2};
}

