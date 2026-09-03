#pragma once

#include <opencv2/core.hpp>
#include <vector>

#include "tracking/Track.hpp"
#include "detection/Detection.hpp"

class Tracker {
public:
    float calculateIoU(
        const cv::Rect2f& box_a,
        const cv::Rect2f& box_b
    ) const;

    void update(const std::vector<Detection>& detections);

    const std::vector<Track>& getTracks() const;
    
private:
    std::vector<Track> tracks_;
    int next_track_id_ = 1;
};