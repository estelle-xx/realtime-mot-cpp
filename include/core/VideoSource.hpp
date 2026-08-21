#include <opencv2/opencv.hpp>

class VideoSource {
    public:
        bool open();
        bool read(cv::Mat& frame);
        void release();

    private:
        cv::VideoCapture cap_;
};