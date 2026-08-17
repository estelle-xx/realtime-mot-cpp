#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Program started." << std::endl;

    cv::Mat frame(2, 3, CV_8UC3);
    std::cout << frame.empty() << std::endl;
    std::cout << frame.channels() << std::endl;
    std::cout << frame.rows << std::endl;
    std::cout << frame.cols << std::endl;

    return 0;
}