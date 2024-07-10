#include "Comparator/ImageHandler.h"
#include <opencv2/opencv.hpp>
#include <curl/curl.h>
#include <sstream>
#include <stdexcept>
#include <exception>

ImageHandler::ImageHandler(const std::string& imagePath)
    : imagePath(imagePath), logger("ImageHandler") {
    logger.logInternalInfo(__func__, "ImageHandler created with path: " + imagePath);
}

ImageHandler::~ImageHandler() {
    logger.logInternalInfo(__func__, "ImageHandler destroyed");
}

bool ImageHandler::isURL(const std::string& path) {
    return path.find("http://") == 0 || path.find("https://") == 0;
}

size_t ImageHandler::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::shared_ptr<cv::Mat> ImageHandler::downloadImage(const std::string& url) {
    logger.logInternalInfo(__func__, "Attempting to download image from URL: " + url);
    
    CURL* curl = nullptr;
    CURLcode res;
    std::string readBuffer;

    try {
        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
            res = curl_easy_perform(curl);
            curl_easy_cleanup(curl);

            if (res == CURLE_OK) {
                std::vector<uchar> data(readBuffer.begin(), readBuffer.end());
                auto img = std::make_shared<cv::Mat>(cv::imdecode(data, cv::IMREAD_UNCHANGED));
                if (img->empty()) {
                    logger.handleError(__func__, "Downloaded data is not a valid image.");
                }
                logger.logInternalInfo(__func__, "Image downloaded and decoded successfully.");
                return img;
            } else {
                logger.handleError(__func__, "Failed to download image from URL: " + url);
            }
        }
        logger.handleError(__func__, "CURL initialization failed.");
    } catch (const std::exception& e) {
        if (curl) {
            curl_easy_cleanup(curl);
        }
        logger.handleError(__func__, std::string("Exception occurred: ") + e.what());
    }

    return nullptr;  // This line will never be reached, added to avoid compiler warnings
}

std::shared_ptr<cv::Mat> ImageHandler::loadImage() {
    logger.logInternalInfo(__func__, "Loading image from: " + imagePath);

    try {
        if (isURL(imagePath)) {
            return downloadImage(imagePath);
        } else {
            auto img = std::make_shared<cv::Mat>(cv::imread(imagePath, cv::IMREAD_UNCHANGED));
            if (img->empty()) {
                logger.handleError(__func__, "Failed to load image from path: " + imagePath);
            }
            logger.logInternalInfo(__func__, "Image loaded successfully from path.");
            return img;
        }
    } catch (const std::exception& e) {
        logger.handleError(__func__, std::string("Exception occurred: ") + e.what());
    }

    return nullptr;  // This line will never be reached, added to avoid compiler warnings
}
