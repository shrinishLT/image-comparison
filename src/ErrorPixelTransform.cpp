#include "ErrorPixelTransform.h"
#include <cmath>

namespace ErrorPixelTransform {

void flat(cv::Vec4b& px, const cv::Vec4b&, const cv::Vec4b&, const cv::Vec4b& mismatchColor) {
    px[0] = mismatchColor[0];
    px[1] = mismatchColor[1];
    px[2] = mismatchColor[2];
    px[3] = mismatchColor[3];
}

void movement(cv::Vec4b& px, const cv::Vec4b&, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor) {
    px[0] = static_cast<uchar>((comparePixel[0] * (mismatchColor[0] / 255.0) + mismatchColor[0]) / 2);
    px[1] = static_cast<uchar>((comparePixel[1] * (mismatchColor[1] / 255.0) + mismatchColor[1]) / 2);
    px[2] = static_cast<uchar>((comparePixel[2] * (mismatchColor[2] / 255.0) + mismatchColor[2]) / 2);
    px[3] = comparePixel[3];
}

void flatDifferenceIntensity(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor) {
    double intensity = std::sqrt(
        std::pow(static_cast<double>(basePixel[0]) - comparePixel[0], 2.0) +
        std::pow(static_cast<double>(basePixel[1]) - comparePixel[1], 2.0) +
        std::pow(static_cast<double>(basePixel[2]) - comparePixel[2], 2.0)
    );
    px[0] = mismatchColor[0];
    px[1] = mismatchColor[1];
    px[2] = mismatchColor[2];
    px[3] = static_cast<uchar>(intensity);
}

void movementDifferenceIntensity(cv::Vec4b& px, const cv::Vec4b& basePixel, const cv::Vec4b& comparePixel, const cv::Vec4b& mismatchColor) {
    double intensity = std::sqrt(
        std::pow(static_cast<double>(basePixel[0]) - comparePixel[0], 2.0) +
        std::pow(static_cast<double>(basePixel[1]) - comparePixel[1], 2.0) +
        std::pow(static_cast<double>(basePixel[2]) - comparePixel[2], 2.0)
    );
    double ratio = (intensity / 255.0) * 0.8;

    px[0] = static_cast<uchar>((1 - ratio) * (comparePixel[0] * (mismatchColor[0] / 255.0)) + ratio * mismatchColor[0]);
    px[1] = static_cast<uchar>((1 - ratio) * (comparePixel[1] * (mismatchColor[1] / 255.0)) + ratio * mismatchColor[1]);
    px[2] = static_cast<uchar>((1 - ratio) * (comparePixel[2] * (mismatchColor[2] / 255.0)) + ratio * mismatchColor[2]);
    px[3] = comparePixel[3];
}

void diffOnly(cv::Vec4b& px, const cv::Vec4b&, const cv::Vec4b& comparePixel, const cv::Vec4b&) {
    px[0] = comparePixel[0];
    px[1] = comparePixel[1];
    px[2] = comparePixel[2];
    px[3] = comparePixel[3];
}

} // namespace ErrorPixelTransform
