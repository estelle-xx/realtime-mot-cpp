#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

#include "core/VideoSource.hpp"
#include "detection/Detection.hpp"
#include "detection/Detector.hpp"
#include "tracking/Tracker.hpp"

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

int main(int argc, char* argv[]) {
    std::cout << "Program started." << std::endl;

    Detector detector("models/yolo11n.onnx");

    VideoSource source;

    Tracker tracker;

    bool opened = false;

    if (argc == 1) {
        opened = source.openCamera();
    }
    else if (argc == 2) {
        opened = source.openVideo(argv[1]);
    }
    else {
        std::cerr << "Usage: " << argv[0] << " [video_path]" << std::endl;

        return 1;
    }

    if (!opened) {
        std::cerr << "Failed to open video source." << std::endl;
        return 1;
    }

    cv::Mat frame;
    
    while (true) {
        if (!source.read(frame)){
            break;
        }
        std::vector<Detection> detections = detector.detect(frame);

        std::vector<Detection> tracking_detections;

        for (const Detection& detection : detections) {
            if (isTrackingTarget(detection.class_id)) {
                tracking_detections.push_back(detection);
            }
        }

        tracker.update(tracking_detections);

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

        for (const Track& track : tracker.getTracks()) {
            if (track.missed_frames > 0) {
                continue;
            }
            std::string track_label = "ID " + std::to_string(track.track_id);

            cv::putText(
                frame,
                track_label,
                cv::Point(
                    static_cast<int>(track.bbox.x),
                    static_cast<int>(track.bbox.y) - 25
                ),

                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                cv::Scalar(0, 255, 0),
                2
            );

            for (int i = 1; i < static_cast<int>(track.trajectory.size()); ++i) {
                cv::line(
                    frame,
                    track.trajectory[i -1],
                    track.trajectory[i],
                    cv::Scalar(0, 255, 255),
                    2
                );
            }
        }
        cv::imshow("Camera", frame);

        int key = cv::waitKey(1);

        if (key == 27) {
            break;
        }
    }

    source.release();
    cv::destroyAllWindows();

    return 0;
}