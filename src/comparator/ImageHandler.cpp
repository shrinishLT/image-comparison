#include "Comparator/ImageHandler.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <curl/curl.h>
#include <sstream>
#include <Logger.h>

ImageHandler::ImageHandler(const std::string& imagePath)
    : imagePath(imagePath), logger("ImageHandler") {}

ImageHandler::~ImageHandler() {
    // Destructor
}

bool ImageHandler::isURL(const std::string& path) {
    return path.find("http://") == 0 || path.find("https://") == 0;
}

size_t ImageHandler::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

std::shared_ptr<cv::Mat> ImageHandler::downloadImage(const std::string& url) {
    BOOST_LOG_NAMED_SCOPE(__func__);    
    CURL* curl;
    CURLcode res;
    std::string readBuffer;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if(res == CURLE_OK) {
            std::vector<uchar> data(readBuffer.begin(), readBuffer.end());
            auto img = std::make_shared<cv::Mat>(cv::imdecode(data, cv::IMREAD_UNCHANGED));
            if(img->empty()) {
                BOOST_LOG_TRIVIAL(error) << "Downloaded data is not a valid image.";
                throw std::runtime_error("Downloaded data is not a valid image.");
            }
            return img;
        } else {
            BOOST_LOG_TRIVIAL(error) << "Failed to download image from URL.";
            throw std::runtime_error("Failed to download image from URL.");
        }
    }
    BOOST_LOG_TRIVIAL(error) << "CURL initialization failed.";
    throw std::runtime_error("CURL initialization failed.");
}

std::shared_ptr<cv::Mat> ImageHandler::loadImage() {
    if (isURL(imagePath)) {
        return downloadImage(imagePath);
    } else {
        auto img = std::make_shared<cv::Mat>(cv::imread(imagePath, cv::IMREAD_UNCHANGED));
        if(img->empty()) {
            BOOST_LOG_TRIVIAL(error) << "Failed to load image from path: " << imagePath;
            throw std::runtime_error("Failed to load image from path: " + imagePath);
        }
        return img;
    }
}