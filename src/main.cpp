#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    std::cout << "Program started." << std::endl;

    std::string pipeline = "icamerasrc buffer-count=7 ! videoconvert ! appsink";
    cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);
    std::cout << cap.isOpened() << std::endl;

    cv::Mat frame;

    while (true) {
        if (!cap.read(frame)){
            break;
        }
        
        cv::imshow("Camera", frame);

        int key = cv::waitKey(1);

        if (key == 27) {
            break;
        }
    }

    return 0;
}