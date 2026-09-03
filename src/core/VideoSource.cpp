#include "core/VideoSource.hpp"
#include <string>

bool VideoSource::openCamera() {
    std::string pipeline =
        "icamerasrc buffer-count=7 ! videoconvert ! "
        "appsink max-buffers=1 drop=true sync=false";

    return cap_.open(pipeline, cv::CAP_GSTREAMER);
}

bool VideoSource::openVideo(const std::string& video_path) {
    return cap_.open(video_path);
}

bool VideoSource::read(cv::Mat& frame) {
    return cap_.read(frame);
}

void VideoSource::release() {
    cap_.release();
}