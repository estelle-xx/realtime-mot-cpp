#include "core/VideoSource.hpp"
#include <string>

bool VideoSource::open() {
    std::string pipeline =
        "icamerasrc buffer-count=7 ! videoconvert ! "
        "appsink max-buffers=1 drop=true sync=false";

    return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

bool VideoSource::read(cv::Mat& frame) {
    return cap_.read(frame);
}

void VideoSource::release() {
    cap_.release();
}