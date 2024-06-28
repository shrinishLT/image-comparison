#include "ImageHandler.h"
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <curl/curl.h>
#include <sstream>

ImageHandler::ImageHandler(const std::string& imagePath)
    : imagePath(imagePath) {}

ImageHandler::~ImageHandler() {
    // Destructor
}

bool ImageHandler::isURL(const std::string& path) {
    return path.find("http://") == 0 || path.find("https://") == 0;
}

cv::Mat ImageHandler::downloadImage(const std::string& url) {
    CURL* curl;
    CURLcode res;
    std::vector<uchar> buffer;
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, [](void* data, size_t size, size_t nmemb, void* userp) -> size_t {
            std::vector<uchar>* buffer = (std::vector<uchar>*)userp;
            size_t totalSize = size * nmemb;
            buffer->insert(buffer->end(), (uchar*)data, (uchar*)data + totalSize);
            return totalSize;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        return cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
    }
    return cv::Mat();
}

cv::Mat ImageHandler::loadImage() {
    if (isURL(imagePath)) {
        image = downloadImage(imagePath);
    } else {
        image = cv::imread(imagePath, cv::IMREAD_UNCHANGED);
    }
    return image;
}
