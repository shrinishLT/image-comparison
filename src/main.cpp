#include "ImageComparator.h"
#include <iostream>
#include <sstream>

cv::Vec3b parseColor(const std::string& colorStr) {
    std::stringstream ss(colorStr);
    int b, g, r;
    char delim;

    ss >> b >> delim >> g >> delim >> r;

    return cv::Vec3b(b, g, r);
}

int main(int argc, char** argv) {
    if (argc < 4 || argc > 7) {
        std::cerr << "Usage: " << argv[0] << " <base_image> <compare_image> <output_image> [<mismatch_color>] [<ignore_antialiasing>] [<ignore_colors>]" << std::endl;
        std::cerr << "Mismatch color format: B,G,R (e.g., 0,0,255 for red)" << std::endl;
        return -1;
    }

    std::string imgPath1 = argv[1];
    std::string imgPath2 = argv[2];
    std::string outputPath = argv[3];

    ImageComparator comparator(imgPath1, imgPath2);

    if (argc >= 5) {
        cv::Vec3b mismatchColor = parseColor(argv[4]);
        comparator.setMismatchPaintColor(mismatchColor);
    }

    if (argc >= 6) {
        bool ignoreAntialiasing = std::stoi(argv[5]);
        comparator.setIgnoreAntialiasing(ignoreAntialiasing);
    }

    if (argc == 7) {
        bool ignoreColors = std::stoi(argv[6]);
        comparator.setIgnoreColors(ignoreColors);
    }

    comparator.exactComparison(outputPath);

    return 0;
}
