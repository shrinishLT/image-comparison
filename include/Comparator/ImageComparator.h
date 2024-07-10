#ifndef IMAGECOMPARATOR_H
#define IMAGECOMPARATOR_H

#include <opencv2/opencv.hpp>
#include <memory>
#include <string>
#include <vector>
#include <utility> // For std::pair
#include "Logger.h"

struct Box {
    int top;
    int left;
    int bottom;
    int right;
};

struct ExactComparisonResult {
    std::vector<uchar> resultBuffer;
    double mismatchPercentage;
};

struct SmartIgnoreComparisonResult {
    std::vector<uchar> baseResultBuffer;
    std::vector<uchar> compareResultBuffer;
    double compareMismatchPercentage;
    double baseMismatchPercentage;
};

class ImageComparator {
public:
    ImageComparator();

    // Setters
    void setMismatchPaintColor(const cv::Vec3b& color);
    void setIgnoreAntialiasing(bool value);
    void setIgnoreColors(bool value);
    void setIgnoreAlpha(bool value);
    void setScaleToSameSize(bool value);
    void setPixelThreshold(double value);
    void setBoundingBoxes(const std::vector<Box>& boxes);
    void setIgnoreBoxes(const std::vector<Box>& boxes);
    void setHighlightTransparency(int value);
    void setErrorPixelTransform(void (*transformFunc)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&));
    ExactComparisonResult exactComparison(std::shared_ptr<cv::Mat> img1, std::shared_ptr<cv::Mat> img2) const;
    ExactComparisonResult ignoreDisplacementsComparison(std::shared_ptr<cv::Mat> img1, std::shared_ptr<cv::Mat> img2) const;
    SmartIgnoreComparisonResult smartIgnoreComparison(std::shared_ptr<cv::Mat> img1, std::shared_ptr<cv::Mat> img2) const;

private:
    cv::Vec3b mismatchPaintColor;
    bool ignoreAntialiasing;
    bool ignoreColors;
    bool ignoreAlpha;
    bool scaleToSameSize;
    double pixelThreshold;
    int highlightTransparency;
    std::vector<Box> boundingBoxes;
    std::vector<Box> ignoreBoxes;
    void (*errorPixelTransform)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&);

    Logger logger;

    mutable int canvasWidth;
    mutable int canvasHeight;
    mutable int mismatchedPixels;

    // General Methods for Image comparisons 
    bool comparePixels(const cv::Mat& img1, const cv::Mat& img2, int x1, int y1, int x2, int y2) const;
    bool isInBoundingBox(int x, int y) const;
    bool isInIgnoreBox(int x, int y) const;
    bool doesRowsMatch(int row1, int row2, const cv::Mat& img1, const cv::Mat& img2) const;

    // Methods for ignoreDisplacementsComparison
    std::pair<int, int> matchRows(int row1, int row2, int direction, const cv::Mat& img1, const cv::Mat& img2) const;
    int binarySearchOnHeight(int row1, int row2, int maxHeight, const cv::Mat& img1, const cv::Mat& img2) const;
    std::pair<int, int> getBestMatchingRectangle(int startRow1, int startRow2,int endRow1,int endRow2, const cv::Mat& img1, const cv::Mat& img2, const std::vector<int>& mismatchedRows) const;
    void paintMismatchedCells(const std::vector<int>& toMatchWith, const cv::Mat& img1, const cv::Mat& img2, cv::Mat& paintedImg) const;
    void markMatchedRows(std::vector<int>& toMatchWith, int startRow1, int height) const;
    bool doesRectangleMatch(int row1, int row2, int height, const cv::Mat& img1, const cv::Mat& img2) const;

};

#endif // IMAGECOMPARATOR_H