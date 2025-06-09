#include <cmath>

#include "temporal_runtime.h"
#include "time_anchor.h"

namespace chronovyan {

TimeAnchor::TimeAnchor(const std::string &id, double stability,
                       const std::string &description)
    : m_id(id),
      m_stability(
          std::max(0.0, std::min(1.0, stability))), // Clamp between 0.0 and 1.0
      m_creation_time(getCurrentTimestamp()), m_description(description) {}

std::string TimeAnchor::getId() const { return m_id; }

double TimeAnchor::getStability() const { return m_stability; }

Timestamp TimeAnchor::getCreationTime() const { return m_creation_time; }

std::string TimeAnchor::getDescription() const { return m_description; }

void TimeAnchor::setDescription(const std::string &description) {
  m_description = description;
}

bool TimeAnchor::isMoreStableThan(const TimeAnchor &other) const {
  return m_stability > other.m_stability;
}

bool TimeAnchor::isOlderThan(const TimeAnchor &other) const {
  return m_creation_time < other.m_creation_time;
}

double TimeAnchor::calculateTemporalDistance(const TimeAnchor &other) const {
  // Calculate temporal distance based on creation timestamps
  // The distance is proportional to the difference in timestamps
  // and inversely proportional to the product of stabilities

  // Get absolute time difference
  double time_diff = std::abs(static_cast<double>(m_creation_time) -
                              static_cast<double>(other.m_creation_time));

  // Scale by stability (more stable anchors are "closer" in temporal space)
  double stability_factor =
      1.0 / (m_stability * other.m_stability + 0.1); // Avoid division by zero

  return time_diff * stability_factor;
}

} // namespace chronovyan