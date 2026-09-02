#include <iostream>
#include <opencv2/opencv.hpp>
#include "core/VideoSource.hpp"
#include <vector>
#include "detection/Detection.hpp"
#include "detection/Detector.hpp"
#include <string>
#include <sstream>
#include <iomanip>

std::string getClassName(int class_id) {
    switch (class_id) {
        case 0: return "person";
        case 1: return "bicycle";
        case 2: return "car";
        case 3: return "motorcycle";
        case 5: return "bus";
        case 7: return "truck";
        default: return "other";
    }
}

bool isTrackingTarget(int class_id) {
        switch (class_id) {
            case 0:  // person
            case 1:  // bicycle
            case 2:  // car
            case 3:  // motorcycle
            case 5:  // bus
            case 7:  // truck
                return true;

            default:
                return false;
        }
    }

int main() {
    std::cout << "Program started." << std::endl;

    Detector detector("models/yolo11n.onnx");

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
        std::vector<Detection> detections = detector.detect(frame);

        for (const Detection& detection : detections) {
            if (!isTrackingTarget(detection.class_id)) {
                continue;
            }

            cv::rectangle(
                frame,
                detection.bbox,
                cv::Scalar(0, 255, 0),
                2
            );

            std::ostringstream label_stream;

            label_stream << getClassName(detection.class_id) << " " << std::fixed << std::setprecision(2) << detection.confidence;

            std::string label = label_stream.str();

            cv::putText(
                frame,
                label,
                cv::Point(
                    static_cast<int>(detection.bbox.x),
                    static_cast<int>(detection.bbox.y) - 5
                ),
                cv:: FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 255, 0),
                1
            );
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