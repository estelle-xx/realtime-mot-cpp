#include <opencv2/opencv.hpp>
#include <string>

class VideoSource {
public:
    bool openCamera();
    bool openVideo(const std::string& vidio_path);
    bool read(cv::Mat& frame);
    void release();

private:
    cv::VideoCapture cap_;
};