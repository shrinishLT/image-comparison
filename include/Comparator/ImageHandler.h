#ifndef IMAGEHANDLER_H
#define IMAGEHANDLER_H

#include <opencv2/opencv.hpp>
#include <string>
#include "Logger.h"  // Include logging

class ImageHandler {
public:
    explicit ImageHandler(const std::string& imagePath);
    ~ImageHandler();
    std::shared_ptr<cv::Mat> loadImage();

private:
    std::string imagePath;
    Logger logger;

    static bool isURL(const std::string& path);
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
    std::shared_ptr<cv::Mat> downloadImage(const std::string& url);
};


#endif // IMAGEHANDLER_H
