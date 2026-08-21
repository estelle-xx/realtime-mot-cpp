#include <iostream>
#include <opencv2/opencv.hpp>
#include "core/VideoSource.hpp"

int main() {
    std::cout << "Program started." << std::endl;

    VideoSource camera;
    if(!camera.open()) {
        std::cerr << "Failed to open camera." << std::endl;
        return 1;
    }

    cv::Mat frame;

    while (true) {
        if (!camera.read(frame)){
            break;
        }

        cv::imshow("Camera", frame);

        int key = cv::waitKey(1);

        if (key == 27) {
            break;
        }
    }

    camera.release();
    cv::destroyAllWindows();

    return 0;
}