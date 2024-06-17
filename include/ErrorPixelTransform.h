#ifndef ERRORPIXELTRANSFORM_H
#define ERRORPIXELTRANSFORM_H

#include <opencv2/opencv.hpp>

// Define a structure for error pixel color
struct ErrorPixelColor {
    uchar red;
    uchar green;
    uchar blue;
    uchar alpha;
};

// Error pixel transform functions
namespace ErrorPixelTransform {
    void flat(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor);
    void movement(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor);
    void flatDifferenceIntensity(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor);
    void movementDifferenceIntensity(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor);
    void diffOnly(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor);
}

#endif // ERRORPIXELTRANSFORM_H
