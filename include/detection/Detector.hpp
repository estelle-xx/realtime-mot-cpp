#pragma once

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

#include <string>
#include <vector>

#include "detection/Detection.hpp"

class Detector {
public:
    Detector(const std::string& model_path);

    std::vector<Detection> detect(const cv::Mat& frame);

private:
    cv::dnn::Net net_;
    };