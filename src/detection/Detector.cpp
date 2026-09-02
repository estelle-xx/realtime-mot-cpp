#include "detection/Detector.hpp"
#include <algorithm>
#include <opencv2/imgproc.hpp>

Detector::Detector(const std::string& model_path) {
    net_ = cv::dnn::readNetFromONNX(model_path);
}

std::vector<Detection> Detector::detect(const cv::Mat& frame) {
    std::vector<Detection> detections;

    const int input_width = 640;
    const int input_height = 640;

    float scale = std::min(
        static_cast<float>(input_width) / frame.cols,
        static_cast<float>(input_height) / frame.rows
    );

    int resized_width = static_cast<int>(frame.cols * scale);
    int resized_height = static_cast<int>(frame.rows * scale);

    cv::Mat resized_frame;

    cv::resize(
        frame,
        resized_frame,
        cv::Size(resized_width, resized_height)
    );

    int pad_width = input_width - resized_width;
    int pad_height = input_height - resized_height;

    int pad_left = pad_width / 2;
    int pad_right = pad_width - pad_left;

    int pad_top = pad_height / 2;
    int pad_bottom = pad_height - pad_top;

    cv::Mat letterbox_frame;

    cv::copyMakeBorder(
        resized_frame,
        letterbox_frame,
        pad_top,
        pad_bottom,
        pad_left,
        pad_right,
        cv::BORDER_CONSTANT,
        cv::Scalar(114, 114, 114)
    );

    cv::Mat blob = cv::dnn::blobFromImage(
        letterbox_frame,
        1.0 / 255.0,
        cv::Size(640, 640),
        cv::Scalar(),
        true,
        false
    );

    net_.setInput(blob);

    std::vector<cv::Mat> outputs;
    net_.forward(outputs, net_.getUnconnectedOutLayersNames());

    const cv::Mat& output = outputs[0];

    cv::Mat output_2d = output.reshape(1, output.size[1]);
    cv::Mat predictions = output_2d.t();

    std::vector<cv::Rect> boxes;
    std::vector<float> scores;
    std::vector<int> class_ids;

    const float confidence_threshold = 0.5f;

    for (int i = 0; i < predictions.rows; ++i) {
        const cv::Mat prediction = predictions.row(i);

        cv::Mat class_scores = prediction.colRange(4, 84);

        double max_score;
        cv::Point max_class_location;

        cv::minMaxLoc(
            class_scores,
            nullptr,
            &max_score,
            nullptr,
            &max_class_location
        );

        if (max_score < confidence_threshold) {
            continue;
        }

        int class_id = max_class_location.x;

        float center_x = prediction.at<float>(0, 0);
        float center_y = prediction.at<float>(0, 1);
        float width = prediction.at<float>(0, 2);
        float height = prediction.at<float>(0, 3);

        float left = center_x - width / 2.0f;
        float top = center_y - height / 2.0f;

        float original_left = (left - pad_left) / scale;
        float original_top = (top - pad_top) / scale;

        float original_width = width / scale;
        float original_height = height / scale;

        cv::Rect box(
        static_cast<int>(original_left),
        static_cast<int>(original_top),
        static_cast<int>(original_width),
        static_cast<int>(original_height)
    );

    boxes.push_back(box);
    scores.push_back(static_cast<float>(max_score));
    class_ids.push_back(class_id);

    }

    const float nms_threshold = 0.4f;

    std::vector<int> indices;

    cv::dnn::NMSBoxesBatched(
        boxes,
        scores,
        class_ids,
        confidence_threshold,
        nms_threshold,
        indices
    );

    for (int index : indices) {
        cv::Rect box = boxes[index];

        cv::Rect frame_rect(
            0,
            0,
            frame.cols,
            frame.rows
        );

        box &= frame_rect;

        if (box.empty()) {
            continue;
        }

        Detection detection;

        detection.bbox = cv::Rect2f(
            static_cast<float>(box.x),
            static_cast<float>(box.y),
            static_cast<float>(box.width),
            static_cast<float>(box.height)
        );

        detection.confidence = scores[index];
        detection.class_id = class_ids[index];

        detections.push_back(detection);
    }


    return detections;
}