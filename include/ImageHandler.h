#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <opencv2/opencv.hpp>
#include <string>

class ImageHandler {
public:
    ImageHandler(const std::string& imagePath);
    ~ImageHandler();

    cv::Mat loadImage();

private:
    std::string imagePath;  // can be path or URL 
    cv::Mat image;  // image loaded from path or URL

    bool isURL(const std::string& path);
    cv::Mat downloadImage(const std::string& url);
};

#endif // IMAGEHANDLER_H
