#pragma once

#include <ctime>
#include <string>

#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @typedef Timestamp
 * @brief Represents a point in time
 */
using Timestamp = std::time_t;

/**
 * @brief Get the current timestamp
 * @return Current time as a Timestamp
 */
inline Timestamp getCurrentTimestamp() { return std::time(nullptr); }

/**
 * @class TimeAnchor
 * @brief Represents a specific point in time within a codebase (similar to a commit in version
 * control)
 *
 * TimeAnchors are stable reference points within a timestream that can be used for
 * navigation, comparison, and as the basis for creating new timestreams (Echoes).
 */
class TimeAnchor {
public:
    /**
     * @brief Constructor
     * @param id Unique identifier for this anchor
     * @param stability Stability value of this anchor (0.0-1.0)
     * @param description Text description of what this anchor represents
     */
    TimeAnchor(const std::string& id, double stability, const std::string& description = "");

    /**
     * @brief Get the unique identifier for this anchor
     * @return The anchor ID
     */
    std::string getId() const;

    /**
     * @brief Get the stability value of this anchor
     * @return Stability value (0.0-1.0)
     */
    double getStability() const;

    /**
     * @brief Get the creation timestamp of this anchor
     * @return The timestamp when this anchor was created
     */
    Timestamp getCreationTime() const;

    /**
     * @brief Get the description of this anchor
     * @return The text description
     */
    std::string getDescription() const;

    /**
     * @brief Set the description of this anchor
     * @param description The new description
     */
    void setDescription(const std::string& description);

    /**
     * @brief Compare stability with another anchor
     * @param other The anchor to compare with
     * @return True if this anchor is more stable
     */
    bool isMoreStableThan(const TimeAnchor& other) const;

    /**
     * @brief Check if this anchor was created before another
     * @param other The anchor to compare with
     * @return True if this anchor is older
     */
    bool isOlderThan(const TimeAnchor& other) const;

    /**
     * @brief Calculate the temporal distance between two anchors
     * @param other The anchor to measure distance to
     * @return Temporal distance in standard units
     */
    double calculateTemporalDistance(const TimeAnchor& other) const;

private:
    std::string m_id;           ///< Unique identifier
    double m_stability;         ///< Stability value (0.0-1.0)
    Timestamp m_creation_time;  ///< When this anchor was created
    std::string m_description;  ///< Text description
};

}  // namespace chronovyan