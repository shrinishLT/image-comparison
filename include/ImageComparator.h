#ifndef IMAGECOMPARATOR_H
#define IMAGECOMPARATOR_H

#include <opencv2/opencv.hpp>
#include <string>

class ImageComparator {
public:
    ImageComparator(const std::string& imgPath1, const std::string& imgPath2);

    void setMismatchPaintColor(const cv::Vec3b& color);
    void setIgnoreAntialiasing(bool value);
    void setIgnoreColors(bool value);
    void setIgnoreAlpha(bool value); // New method to set ignoreAlpha flag
    void setErrorPixelTransform(void (*transformFunc)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&)); // Set the error pixel transform function
    void exactComparison(const std::string& outputPath) const;

private:
    std::string imgPath1;
    std::string imgPath2;
    cv::Vec3b mismatchPaintColor;
    bool ignoreAntialiasing;
    bool ignoreColors;
    bool ignoreAlpha; // New flag to ignore alpha channel
    void (*errorPixelTransform)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&); // Error pixel transform function

    bool comparePixels(const cv::Mat& img1, const cv::Mat& img2, int x, int y, int width, int height) const;
};

#endif // IMAGECOMPARATOR_H
