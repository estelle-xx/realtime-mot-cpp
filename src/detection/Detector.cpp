#include "detection/Detector.hpp"

Detector::Detector(const std::string& model_path) {
    net_ = cv::dnn::readNetFromONNX(model_path);
}

std::vector<Detection> Detector::detect(const cv::Mat& frame) {
    std::vector<Detection> detections;

    Detection detection;
    detection.bbox = cv::Rect2f(100.0f, 200.0f, 80.0f, 160.0f);
    detection.confidence = 0.92f;
    detection.class_id = 0;

    Detection detection2;   
    detection2.bbox = cv::Rect2f(300.0f, 150.0f, 100.0f, 200.0f);
    detection2.confidence = 0.87f;
    detection2.class_id = 0;

    detections.push_back(detection);
    detections.push_back(detection2);

    return detections;
}