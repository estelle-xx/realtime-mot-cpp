#include <iostream>
#include <opencv2/opencv.hpp>
#include "core/VideoSource.hpp"
#include <vector>
#include "detection/Detection.hpp"
#include "detection/Detector.hpp"

int main() {
    std::cout << "Program started." << std::endl;

    Detector detector("models/yolo11n.onnx");

    VideoSource camera;
    
    if(!camera.open()) {
        std::cerr << "Failed to open camera." << std::endl;
        return 1;
    }

    cv::Mat frame;
    std::vector<Detection> detections;

    Detection detection;
    Detection detection2;

    detection.bbox = cv::Rect2f(100.0f, 200.0f, 80.0f, 160.0f);
    detection.confidence = 0.92f;
    detection.class_id = 0;

    detection2.bbox = cv::Rect2f(300.0f, 150.0f, 100.0f, 200.0f);
    detection2.confidence = 0.87f;
    detection2.class_id = 0;

    detections.push_back(detection);
    detections.push_back(detection2);

    std::cout << "detections size: " << detections.size() << std::endl;
    
    for (const Detection& detection : detections) {
        std::cout << "confidence: " << detection.confidence << std::endl;
    }
    
    while (true) {
        if (!camera.read(frame)){
            break;
        }
        std::vector<Detection> detections = detector.detect(frame);
        std::cout << "detections size: " << detections.size() << std::endl;
        for (const Detection& detection : detections) {
            cv::rectangle(
                frame,
                detection.bbox,
                cv::Scalar(0, 255, 0),
                2
            );

            std::string label = std::to_string(detection.confidence);

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