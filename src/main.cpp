#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Program started." << std::endl;

    std::string pipeline = "icamerasrc buffer-count=7 ! videoconvert ! appsink";
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    std::cout << cap.isOpened() << std::endl;

    cv::Mat frame;

    bool read_ok = cap.read(frame);

    std::cout << read_ok << std::endl;
    std::cout << frame.empty() << std::endl;
    std::cout << frame.rows << std::endl;
    std::cout << frame.cols << std::endl;
    std::cout << frame.channels() << std::endl;

    cv::imshow("Camera Frame", frame);
    cv::waitKey(0);

    return 0;
}