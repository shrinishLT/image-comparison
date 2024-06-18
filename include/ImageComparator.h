#ifndef IMAGECOMPARATOR_H
#define IMAGECOMPARATOR_H

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

struct Box {
    int top;
    int left;
    int bottom;
    int right;
};

class ImageComparator {
public:
    ImageComparator(const std::string& imgPath1, const std::string& imgPath2);

    void setMismatchPaintColor(const cv::Vec3b& color);
    void setIgnoreAntialiasing(bool value);
    void setIgnoreColors(bool value);
    void setIgnoreAlpha(bool value);
    void setPixelThreshold(double value);
    void setBoundingBoxes(const std::vector<Box>& boxes);
    void setIgnoreBoxes(const std::vector<Box>& boxes);
    void setErrorPixelTransform(void (*transformFunc)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&));
    void exactComparison(const std::string& outputPath) const;

private:
    std::string imgPath1;
    std::string imgPath2;
    cv::Vec3b mismatchPaintColor;
    bool ignoreAntialiasing;
    bool ignoreColors;
    bool ignoreAlpha;
    double pixelThreshold;
    std::vector<Box> boundingBoxes;
    std::vector<Box> ignoreBoxes;
    void (*errorPixelTransform)(cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b&);

    bool comparePixels(const cv::Mat& img1, const cv::Mat& img2, int x, int y, int width, int height) const;
    bool isInBoundingBox(int x, int y) const;
    bool isInIgnoreBox(int x, int y) const;
};

#endif // IMAGECOMPARATOR_H
