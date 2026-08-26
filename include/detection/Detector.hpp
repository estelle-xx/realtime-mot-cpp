#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "detection/Detection.hpp"

class Detector {
    public:
        std::vector<Detection> detect(const cv::Mat& frame);
};