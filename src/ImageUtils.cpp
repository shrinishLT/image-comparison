#include "ImageUtils.h"
#include <cmath>
#include <algorithm>

const double ImageUtils::maxEuclideanDistanceBetweenPixels = std::sqrt(4.0 * 255.0 * 255.0);

bool ImageUtils::antialiased(const cv::Mat& img1, const cv::Mat& img2, int x1, int y1, int x2, int y2) {
    // for 1st pixel 
    // int width = img1.cols;
    // int height = img1.rows;
    // int x0 = std::max(x1 - 1, 0);
    // int y0 = std::max(y1 - 1, 0);
    // int x2 = std::min(x1 + 1, width - 1);
    // int y2 = std::min(y1 + 1, height - 1);
    // int zeroes = int(x1 == x0 || x1 == x2 || y1 == y0 || y1 == y2);
    // double minDelta = 0.0, maxDelta = 0.0;
    // int min_x = 0, min_y = 0, max_x = 0, max_y = 0;

    // for (int x = x0; x <= x2; ++x) {
    //     for (int y = y0; y <= y2; ++y) {
    //         if (x == x1 && y == y1) continue;
    //         double delta = colorDelta(img1, img1, (y1 * width + x1) * 4, (y * width + x) * 4, true);
    //         if (delta == 0) {
    //             zeroes++;
    //             if (zeroes > 2) return false;
    //         } else if (delta < minDelta) {
    //             minDelta = delta;
    //             min_x = x;
    //             min_y = y;
    //         } else if (delta > maxDelta) {
    //             maxDelta = delta;
    //             max_x = x;
    //             max_y = y;
    //         }
    //     }
    // }

    // if (minDelta == 0 || maxDelta == 0) return false;

    // return (hasManySiblings(img1, min_x, min_y, width, height) && hasManySiblings(img2, min_x, min_y, width, height)) ||
    //        (hasManySiblings(img1, max_x, max_y, width, height) && hasManySiblings(img2, max_x, max_y, width, height));
    return true;
}

bool ImageUtils::hasManySiblings(const cv::Mat& img, int x1, int y1, int width, int height) {
    int x0 = std::max(x1 - 1, 0);
    int y0 = std::max(y1 - 1, 0);
    int x2 = std::min(x1 + 1, width - 1);
    int y2 = std::min(y1 + 1, height - 1);
    int pos = (y1 * width + x1) * 4;
    int zeroes = int(x1 == x0 || x1 == x2 || y1 == y0 || y1 == y2);

    for (int x = x0; x <= x2; ++x) {
        for (int y = y0; y <= y2; ++y) {
            if (x == x1 && y == y1) continue;

            int pos2 = (y * width + x) * 4;
            if (std::equal(img.ptr<unsigned char>(0) + pos, img.ptr<unsigned char>(0) + pos + 4, img.ptr<unsigned char>(0) + pos2)) {
                zeroes++;
            }

            if (zeroes > 2) return true;
        }
    }

    return false;
}

double ImageUtils::colorDelta(const cv::Mat& img1, const cv::Mat& img2, int k, int m, bool yOnly) {
    auto getChannel = [](const cv::Mat& img, int pos, int channel) -> unsigned char {
        return img.ptr<unsigned char>(0)[pos + channel];
    };

    double r1 = getChannel(img1, k, 0);
    double g1 = getChannel(img1, k, 1);
    double b1 = getChannel(img1, k, 2);
    double a1 = getChannel(img1, k, 3);
    double r2 = getChannel(img2, m, 0);
    double g2 = getChannel(img2, m, 1);
    double b2 = getChannel(img2, m, 2);
    double a2 = getChannel(img2, m, 3);

    if (a1 == a2 && r1 == r2 && g1 == g1 && b1 == b2) return 0.0;

    if (a1 < 255) blendRGB(r1, g1, b1, a1 / 255.0);
    if (a2 < 255) blendRGB(r2, g2, b2, a2 / 255.0);

    double y = rgb2y(r1, g1, b1) - rgb2y(r2, g2, b2);

    if (yOnly) return y;

    double i = rgb2i(r1, g1, b1) - rgb2i(r2, g2, b2);
    double q = rgb2q(r1, g1, b1) - rgb2q(r2, g2, b2);

    return 0.5053 * y * y + 0.299 * i * i + 0.1957 * q * q;
}

double ImageUtils::rgb2y(double r, double g, double b) {
    return r * 0.29889531 + g * 0.58662247 + b * 0.11448223;
}

double ImageUtils::rgb2i(double r, double g, double b) {
    return r * 0.59597799 - g * 0.27417610 - b * 0.32180189;
}

double ImageUtils::rgb2q(double r, double g, double b) {
    return r * 0.21147017 - g * 0.52261711 + b * 0.31114694;
}

void ImageUtils::blendRGB(double& r, double& g, double& b, double a) {
    r = r * a + (1 - a) * 255;
    g = g * a + (1 - a) * 255;
    b = b * a + (1 - a) * 255;
}

bool ImageUtils::isWithinThreshold(int pixelThreshold, cv::Vec4b pixel1, cv::Vec4b pixel2){
    // Calculate Euclidean distance between the pixels
    double distance = std::sqrt(
        std::pow(static_cast<double>(pixel1[0]) - pixel2[0], 2.0) +
        std::pow(static_cast<double>(pixel1[1]) - pixel2[1], 2.0) +
        std::pow(static_cast<double>(pixel1[2]) - pixel2[2], 2.0) +
        std::pow(static_cast<double>(pixel1[3]) - pixel2[3], 2.0)
    );

    if (distance / maxEuclideanDistanceBetweenPixels < pixelThreshold) {
        return true;
    }

    return false;
}

bool ImageUtils::doesIntensityMatch(cv::Vec4b pixel1, cv::Vec4b pixel2) {
    double intensity1 = 0.2989 * pixel1[2] + 0.5870 * pixel1[1] + 0.1140 * pixel1[0];
    double intensity2 = 0.2989 * pixel2[2] + 0.5870 * pixel2[1] + 0.1140 * pixel2[0];
    if (std::abs(intensity1 - intensity2) < 1e-5) {
        return true;
    }

    return false;
}
