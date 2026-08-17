#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Program started." << std::endl;

    cv::Mat frame(2, 3, CV_8UC3, cv::Scalar(0, 0, 0));
    frame.at<cv::Vec3b>(0, 0)[2] = 255;
    cv::Vec3b pixel = frame.at<cv::Vec3b>(0, 0);
    std::cout << static_cast<int>(pixel[0]) << std::endl;
    std::cout << static_cast<int>(pixel[1]) << std::endl;
    std::cout << static_cast<int>(pixel[2]) << std::endl;
    std::cout << frame.empty() << std::endl;
    std::cout << frame.channels() << std::endl;
    std::cout << frame.rows << std::endl;
    std::cout << frame.cols << std::endl;

    return 0;
}