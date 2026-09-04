#pragma once

#include <opencv2/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <vector>

struct Track {
    int track_id;
    cv::Rect2f bbox;
    cv::Rect2f predicted_bbox;
    int class_id;

    int age;
    int missed_frames;

    std::vector<cv::Point2f> trajectory;

    cv::KalmanFilter kalman_filter;
};