#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <opencv2/opencv.hpp>
#include <vector>

class ImageUtils {
public:
    static bool antialiased(const cv::Mat& img1, int x1, int y1, int width, int height, const cv::Mat& img2);
    static bool hasManySiblings(const cv::Mat& img, int x1, int y1, int width, int height);
    static double colorDelta(const cv::Mat& img1, const cv::Mat& img2, int k, int m, bool yOnly = false);

private:
    static double rgb2y(double r, double g, double b);
    static double rgb2i(double r, double g, double b);
    static double rgb2q(double r, double g, double b);
    static void blendRGB(double& r, double& g, double& b, double a);
};

#endif // IMAGEUTILS_H