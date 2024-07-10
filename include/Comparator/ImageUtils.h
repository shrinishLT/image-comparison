#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include <vector>

class ImageUtils {
public:
    static bool antialiased(const cv::Mat& img1, const cv::Mat& img2, int x1, int y1, int x2, int y2); // rewrite this correctly
    static bool hasManySiblings(const cv::Mat& img, int x1, int y1, int width, int height);
    static double colorDelta(const cv::Mat& img1, const cv::Mat& img2, int k, int m, bool yOnly = false);
    static bool isWithinThreshold(int pixelThreshold, cv::Vec4b pixel1, cv::Vec4b pixel2);
    static bool doesIntensityMatch(cv::Vec4b pixel1, cv::Vec4b pixel2);
    static void drawGrayPixel(const cv::Vec4b& imgPixel, double alpha, cv::Vec4b& outputPixel);

private:
    static double rgb2y(double r, double g, double b);
    static double rgb2i(double r, double g, double b);
    static double rgb2q(double r, double g, double b);
    static void blendRGB(double& r, double& g, double& b, double a);
    // static void applyMedianBlur(const cv::Mat& img, cv::Mat& blurredImg, int k, int m);
    // static void applyGaussianBlur(const cv::Mat& img, cv::Mat& blurredImg, int k, int m);;
    static void drawPixel(cv::Vec4b& pixel, double r, double g, double b);
    static double getBrightness(double r, double g, double b);
    static double blend(double c, double a);
    static const double maxEuclideanDistanceBetweenPixels;
};

#endif // IMAGEUTILS_H
