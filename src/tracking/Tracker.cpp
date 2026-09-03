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
const int max_trajectory_length = 50;

void Tracker::update(const std::vector<Detection>& detections) {
    if (tracks_.empty()) {
        for (const Detection& detection : detections) {
            Track track;

            track.track_id = next_track_id_;
            next_track_id_ += 1;

            track.bbox = detection.bbox;
            track.class_id = detection.class_id;

            float center_x = track.bbox.x + track.bbox.width / 2.0f;
            float center_y = track.bbox.y + track.bbox.height / 2.0f;

            track.trajectory.push_back(cv::Point2f(center_x, center_y)
            );

            track.age = 1;
            track.missed_frames = 0;

            tracks_.push_back(track);
        }

        return;
    }

    const float iou_threshold = 0.3f;

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
                track.bbox,
                detections[i].bbox
            );
            
            if (iou > best_iou) {
                best_iou = iou;
                best_detection_index = i;
            }
        }

        if (best_detection_index != -1 && best_iou >= iou_threshold) {
                const Detection& matched_detection = detections[best_detection_index];

                track.bbox = matched_detection.bbox;
                track.missed_frames = 0;
                track.age += 1;

                float center_x = track.bbox.x + track.bbox.width / 2.0f;
                float center_y = track.bbox.y + track.bbox.height / 2.0f;

                track.trajectory.push_back(cv::Point2f(center_x, center_y));

                if (track.trajectory.size() > max_trajectory_length) {
                    track.trajectory.erase(track.trajectory.begin());
                }

                detection_matched[best_detection_index] = true;

        }
        else {
            track.missed_frames += 1;
            track.age += 1;
        }
    }

    for (int i = 0; i < static_cast<int>(detections.size()); ++i) {
        if (detection_matched[i]) {
            continue;
        }

        Track track;

        track.track_id = next_track_id_;
        next_track_id_ += 1;

        track.bbox = detections[i].bbox;
        track.class_id = detections[i].class_id;

        track.age = 1;
        track.missed_frames = 0;

        float center_x = track.bbox.x + track.bbox.width / 2.0f;
        float center_y = track.bbox.y + track.bbox.height / 2.0f;

        track.trajectory.push_back(cv::Point2f(center_x, center_y));

        tracks_.push_back(track);
    }

    const int max_missed_frames = 10;

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