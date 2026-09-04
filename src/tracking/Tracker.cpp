#include "tracking/Tracker.hpp"

#include <algorithm>

float Tracker::calculateIoU(
    const cv::Rect2f& box_a,
    const cv::Rect2f& box_b
) const {
    cv::Rect2f intersection = box_a & box_b;

    float intersection_area = intersection.area();

    float union_area = box_a.area() + box_b.area() - intersection_area;

    if (union_area <= 0.0f) {
        return 0.0f;
    }

    return intersection_area / union_area;
}

void Tracker::initializeKalmanFilter(Track& track) {
    track.kalman_filter.init(
        6,
        4,
        0,
        CV_32F
    );

    track.kalman_filter.transitionMatrix = cv::Mat::eye(6, 6, CV_32F);
    track.kalman_filter.transitionMatrix.at<float>(0, 2) = 1.0f;
    track.kalman_filter.transitionMatrix.at<float>(1, 3) = 1.0f;

    track.kalman_filter.measurementMatrix =
        cv::Mat::zeros(
            4,
            6,
            CV_32F
        );

    track.kalman_filter.measurementMatrix.at<float>(0, 0) = 1.0f;
    track.kalman_filter.measurementMatrix.at<float>(1, 1) = 1.0f;
    track.kalman_filter.measurementMatrix.at<float>(2, 4) = 1.0f;
    track.kalman_filter.measurementMatrix.at<float>(3, 5) = 1.0f;

    cv::setIdentity(
        track.kalman_filter.processNoiseCov,
        cv::Scalar::all(1e-2)
    );

    cv::setIdentity(
        track.kalman_filter.measurementNoiseCov,
        cv::Scalar::all(1e-1)
    );

    cv::setIdentity(
        track.kalman_filter.errorCovPost,
        cv::Scalar::all(1.0)
    );

    float center_x = track.bbox.x + track.bbox.width * 0.5f;
    float center_y = track.bbox.y + track.bbox.height * 0.5f;

    track.kalman_filter.statePost = cv::Mat::zeros(6, 1, CV_32F);
    track.kalman_filter.statePost.at<float>(0) = center_x;
    track.kalman_filter.statePost.at<float>(1) = center_y;
    track.kalman_filter.statePost.at<float>(2) = 0.0f;
    track.kalman_filter.statePost.at<float>(3) = 0.0f;
    track.kalman_filter.statePost.at<float>(4) = track.bbox.width;
    track.kalman_filter.statePost.at<float>(5) = track.bbox.height;

    track.predicted_bbox = track.bbox;
}

void Tracker::createTrack(
    const Detection& detection
) {
    Track track;

    track.track_id = next_track_id_++;
    track.bbox = detection.bbox;

    track.predicted_bbox = detection.bbox;

    track.class_id = detection.class_id;

    track.age = 1;
    track.missed_frames = 0;

    float center_x = track.bbox.x + track.bbox.width * 0.5f;
    float center_y = track.bbox.y + track.bbox.height * 0.5f;

    track.trajectory.push_back(cv::Point2f(center_x, center_y));

    initializeKalmanFilter(track);

    tracks_.push_back(track);
}

void Tracker::predictTrack(
    Track& track
) {
    cv::Mat prediction = track.kalman_filter.predict();

    float center_x = prediction.at<float>(0);
    float center_y = prediction.at<float>(1);

    float width = std::max(prediction.at<float>(4), 1.0f);
    float height = std::max(prediction.at<float>(5), 1.0f);

    track.predicted_bbox = cv::Rect2f(
        center_x - width * 0.5f,
        center_y - height * 0.5f,
        width,
        height
    );
}

void Tracker::correctTrack(
    Track& track,
    const Detection& detection
) {
    float center_x = detection.bbox.x + detection.bbox.width * 0.5f;
    float center_y = detection.bbox.y + detection.bbox.height * 0.5f;

    cv::Mat measurement = cv::Mat::zeros(4, 1, CV_32F);

    measurement.at<float>(0) = center_x;
    measurement.at<float>(1) = center_y;
    measurement.at<float>(2) = detection.bbox.width;
    measurement.at<float>(3) = detection.bbox.height;

    track.kalman_filter.correct(measurement);

    track.bbox = detection.bbox;
}

void Tracker::update(const std::vector<Detection>& detections) {
    const float iou_threshold = 0.3f;
    const int max_missed_frames = 10;
    const int max_trajectory_length = 50;

    if (tracks_.empty()) {
        for (const Detection& detection : detections) {
           createTrack(detection);
        }
        return;
    }

    for (Track& track : tracks_) {
        predictTrack(track);
    }


    std::vector<bool> detection_matched(
        detections.size(),
        false
    );

    for (Track& track : tracks_) {
        float best_iou = 0.0f;
        int best_detection_index = -1;

        for (int i = 0; i < static_cast<int>(detections.size()); ++i) {
            if (detection_matched[i]) {
                continue;
            }

            if (track.class_id != detections[i].class_id) {
                continue;
            }

            float iou = calculateIoU(
                track.predicted_bbox,
                detections[i].bbox
            );
            
            if (iou > best_iou) {
                best_iou = iou;
                best_detection_index = i;
            }
        }

        if (best_detection_index >= 0 && best_iou >= iou_threshold) {
            const Detection& matched_detection = detections[best_detection_index];

            correctTrack(
                track,
                matched_detection
            );

            track.missed_frames = 0;
            track.age += 1;

            float center_x = track.bbox.x + track.bbox.width * 0.5f;
            float center_y = track.bbox.y + track.bbox.height * 0.5f;

            track.trajectory.push_back(
                cv::Point2f(
                    center_x,
                    center_y
                )
            );

            if (static_cast<int>(track.trajectory.size()) > max_trajectory_length) {
                track.trajectory.erase(track.trajectory.begin());
            }

            detection_matched[best_detection_index] = true;
        }
        else {
            track.bbox = track.predicted_bbox;

            track.missed_frames += 1;
            track.age += 1;
        }
    }

    for (int i = 0; i < static_cast<int>(detections.size()); ++i) {
        if (!detection_matched[i]) {
            createTrack(detections[i]);
        }
    }

    tracks_.erase(
        std::remove_if(
            tracks_.begin(),
            tracks_.end(),
            [max_missed_frames](const Track& track) {
                return track.missed_frames > max_missed_frames;
            }
        ),
        tracks_.end()
    );
}

const std::vector<Track>& Tracker::getTracks() const {
    return tracks_;
}