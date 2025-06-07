#ifndef CHRONOVYAN_CHRON_TYPES_H
#define CHRONOVYAN_CHRON_TYPES_H

#include <cstdint>
#include <string>

namespace chronovyan {

/**
 * @enum ChronType
 * @brief Defines the fundamental temporal types in Chronovyan
 */
enum class ChronType {
    TIMEPOINT,     // A specific point in time
    TIMELINE,      // A sequence of events in chronological order
    TEMPORAL_SEQ,  // A temporal sequence of values
    TEMPORAL_MAP,  // A temporal map of key-value pairs
    PARADOX,       // A representation of temporal paradox
    GLITCH,        // A temporal glitch or anomaly
    QUANTUM_STATE  // A quantum state representation
};

/**
 * @enum ParadoxLevel
 * @brief Defines the severity levels of temporal paradoxes
 */
enum class ParadoxLevel {
    NONE,      // No paradox
    MINOR,     // Minor paradox with minimal impact
    MODERATE,  // Moderate paradox requiring attention
    MAJOR,     // Major paradox with significant impact
    CRITICAL   // Critical paradox that could destabilize the system
};

/**
 * @enum TemporalStability
 * @brief Defines the stability of temporal operations
 */
enum class TemporalStability {
    STABLE,       // Completely stable
    FLUCTUATING,  // Minor fluctuations
    UNSTABLE,     // Unstable but manageable
    CHAOTIC       // Highly unstable and unpredictable
};

/**
 * @struct TimePoint
 * @brief Represents a point in time with cycle information
 */
struct TimePoint {
    int64_t cycle;     // The temporal cycle
    double timestamp;  // Relative timestamp within cycle

    TimePoint() : cycle(0), timestamp(0.0) {}
    TimePoint(int64_t c, double ts = 0.0) : cycle(c), timestamp(ts) {}

    bool operator<(const TimePoint& other) const {
        if (cycle != other.cycle) {
            return cycle < other.cycle;
        }
        return timestamp < other.timestamp;
    }

    bool operator==(const TimePoint& other) const {
        return cycle == other.cycle && timestamp == other.timestamp;
    }

    std::string toString() const {
        return "TimePoint(" + std::to_string(cycle) + ", " + std::to_string(timestamp) + ")";
    }
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_CHRON_TYPES_H