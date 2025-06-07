#include <algorithm>
#include <chronovyan/timeline.hpp>
#include <shared_mutex>
#include <stdexcept>

namespace chronovyan {
namespace sync {

void Timeline::add_sync_point(const SyncPoint& point) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    sync_points_.push_back(point);

    // Sort by timestamp to maintain chronological order
    std::sort(sync_points_.begin(), sync_points_.end(),
              [](const SyncPoint& a, const SyncPoint& b) { return a.timestamp < b.timestamp; });
}

SyncPoint Timeline::get_current_sync_point() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (sync_points_.empty()) {
        throw std::runtime_error("No sync points available in timeline");
    }
    return sync_points_.back();
}

std::vector<SyncPoint> Timeline::get_recent_sync_points(size_t count) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (sync_points_.empty()) {
        return {};
    }

    size_t start = sync_points_.size() > count ? sync_points_.size() - count : 0;
    return std::vector<SyncPoint>(sync_points_.begin() + start, sync_points_.end());
}

void Timeline::clear() {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    sync_points_.clear();
}

size_t Timeline::size() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return sync_points_.size();
}

bool Timeline::empty() const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return sync_points_.empty();
}

// TimelineManager implementation

Timeline& TimelineManager::get_timeline(const std::string& name) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    return timelines_[name];
}

bool TimelineManager::has_timeline(const std::string& name) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return timelines_.find(name) != timelines_.end();
}

void TimelineManager::remove_timeline(const std::string& name) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    timelines_.erase(name);
}

void TimelineManager::add_event(const TimelineEvent& event) {
    std::unique_lock<std::shared_mutex> lock(mutex_);
    event_log_.push_back(event);

    // Keep only the most recent 1000 events
    const size_t max_events = 1000;
    if (event_log_.size() > max_events) {
        event_log_.erase(event_log_.begin(), event_log_.begin() + (event_log_.size() - max_events));
    }
}

std::vector<TimelineEvent> TimelineManager::get_recent_events(size_t count) const {
    std::shared_lock<std::shared_mutex> lock(mutex_);
    if (event_log_.empty()) {
        return {};
    }

    size_t start = event_log_.size() > count ? event_log_.size() - count : 0;
    return std::vector<TimelineEvent>(event_log_.begin() + start, event_log_.end());
}

}  // namespace sync
}  // namespace chronovyan
