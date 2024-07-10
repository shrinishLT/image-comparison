// #include "Comparator/ImageHandler.h"
// #include "Comparator/ImageComparator.h"
// #include "Comparator/ErrorPixelTransform.h"
// #include <sstream>
// #include <chrono>

// // cv::Vec3b parseColor(const std::string& colorStr) {
// //     std::stringstream ss(colorStr);
// //     int b, g, r;
// //     char delim;

// //     ss >> b >> delim >> g >> delim >> r;
// //     return cv::Vec3b(b, g, r);
// // }



// // Function to write image buffer to an output file
// void writeImageBufferToFile(const std::vector<uchar>& imgBuffer, const std::string& outputFilePath) {
//     // Decode the image buffer to create an OpenCV Mat
//     cv::Mat img = cv::imdecode(imgBuffer, cv::IMREAD_COLOR);

//     // Check if the image was successfully decoded
//     if (img.empty()) {
//         std::cerr << "Failed to decode the image buffer." << std::endl;
//         return;
//     }

//     // Write the image to the output file
//     if (!cv::imwrite(outputFilePath, img)) {
//         std::cerr << "Failed to write the image to " << outputFilePath << std::endl;
//     } else {
//         std::cout << "Image successfully written to " << outputFilePath << std::endl;
//     }
// }


// int main(){
//     // Do nothing here, just to make the project buildable
//     /// call  the methods here for testing over here
//     std::vector<std::string> baseUrls, compareUrls;
//     baseUrls.push_back("https://automation-dotlapse-artefact.lambdatest.com/org-454893/1ca4cf1f-6476-47f2-8350-e897ca83e5de/6e031122-a889-40c2-85ca-4c0ae3283e95/compared-screenshots/000001a8-1e55-418a-9552-f9d4f5ab688f.png");
//     baseUrls.push_back("https://automation-dotlapse-artefact.lambdatest.com/org-454893/1ca4cf1f-6476-47f2-8350-e897ca83e5de/2a52dd38-de94-4e3e-ba34-8f456cf21b96/compared-screenshots/0000023d-6083-4bed-bd17-b4a980dfc208.png");

//     compareUrls.push_back("https://automation-dotlapse-artefact.lambdatest.com/org-454893/1ca4cf1f-6476-47f2-8350-e897ca83e5de/f2a902b8-3a08-4520-8bcf-79d22a0f6bbb/base-screenshots/global-components-page-header-house-scenarios--ind-has-gain_safari_1280x1024.png");
//     compareUrls.push_back("https://automation-dotlapse-artefact.lambdatest.com/org-454893/1ca4cf1f-6476-47f2-8350-e897ca83e5de/236d2bbd-8c5d-4717-9ebf-eae7567c8458/compared-screenshots/00000374-2a22-4ca7-bd04-49f03832144a.png");



//     ImageComparator comparator;

//     // parse the input and set options and the function to be called 
//     // initialize the comparator with correct values 

//     for(int i=1;i<27;i++){
//         ImageHandler handler1(baseUrls[i]);        
//         ImageHandler handler2(compareUrls[i]);     
//         auto img1 = handler1.loadImage();            // returns a pointer to cv::<Mat>
//         auto img2 = handler2.loadImage();

//         std::string base = "../test/base" + std::to_string(i) + ".png";
//         std::string compare = "../test/compare" + std::to_string(i) + ".png";

        
//         SmartIgnoreComparisonResult result = comparator.smartIgnoreComparison(img1, img2);

//         std::cout << "Base mismatch percentage: " << result.baseMismatchPercentage << std::endl;
//         std::cout << "Compare mismatch percentage: " << result.compareMismatchPercentage << std::endl;

        
//         // use image buffer to write image to output path from buffer
//         std::string baseOutputPath =  "../test/smartignore/base/basediff"  + std::to_string(i) + ".png";
//         writeImageBufferToFile(result.baseResultBuffer, baseOutputPath);

//         std::string compareOutputPath =  "../test/smartignore/compare/comparediff"  + std::to_string(i) + ".png";
//         writeImageBufferToFile(result.compareResultBuffer, compareOutputPath);

//         ExactComparisonResult result2 = comparator.exactComparison(img1, img2);

//         std::string exactComparePath = "../test/exact/diff" + std::to_string(i) + ".png";
//         writeImageBufferToFile(result2.resultBuffer, exactComparePath);
//     }

//     return 0;
// }
