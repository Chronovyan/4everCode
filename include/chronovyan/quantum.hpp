#pragma once

#include <chrono>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace chronovyan {
namespace sync {

// Forward declarations
class MLModel;
class RealTimeOptimizer;
class PatternRecognizer;
class ErrorPredictor;
class PerformanceOptimizer;

// Basic synchronization point structure
struct SyncPoint {
    std::chrono::system_clock::time_point timestamp;
    double accuracy{0.0};
    double precision{0.0};
    double recall{0.0};
    int sequence_id{0};
    std::unordered_map<std::string, std::string> metadata;

    SyncPoint() = default;

    // Constructor with timestamp and metrics
    SyncPoint(std::chrono::system_clock::time_point ts, double acc = 0.0, double prec = 0.0,
              double rec = 0.0, std::unordered_map<std::string, std::string> meta = {})
        : timestamp(ts), accuracy(acc), precision(prec), recall(rec), metadata(std::move(meta)) {}

    // Calculate overall sync score (weighted average of metrics)
    double overall_sync() const {
        // Simple average for now, could be weighted differently
        return (accuracy + precision + recall) / 3.0;
    }
};

// Pattern recognition for temporal sequences
struct SyncPattern {
    std::vector<SyncPoint> points;
    double confidence{0.0};
    std::string pattern_type;
    int priority{0};

    SyncPattern() = default;
};

// Metrics for synchronization performance
struct SyncMetrics {
    double latency{0.0};
    double jitter{0.0};
    double efficiency{0.0};
    double stability{0.0};
    double error_rate{0.0};

    SyncMetrics() = default;
};

// Recovery strategy type
using RecoveryStrategy = std::function<void()>;

}  // namespace sync
}  // namespace chronovyan