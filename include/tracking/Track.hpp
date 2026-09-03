#pragma once

#include <opencv2/core.hpp>
#include <vector>

struct Track {
    int track_id;
    cv::Rect2f bbox;
    int class_id;

    int age;
    int missed_frames;

    std::vector<cv::Point2f> trajectory;
};