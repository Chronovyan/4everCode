#include <algorithm>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>

#include "timestream.h"

namespace chronovyan {

Timestream::Timestream(const std::string &id, const std::string &name,
                       std::shared_ptr<Timestream> parent,
                       std::shared_ptr<TimeAnchor> source_anchor)
    : m_id(id), m_name(name), m_parent(parent), m_source_anchor(source_anchor) {
}

std::string Timestream::getId() const { return m_id; }

std::string Timestream::getName() const { return m_name; }

void Timestream::setName(const std::string &name) { m_name = name; }

std::shared_ptr<Timestream> Timestream::getParent() const { return m_parent; }

std::shared_ptr<TimeAnchor> Timestream::getSourceAnchor() const {
  return m_source_anchor;
}

std::shared_ptr<TimeAnchor>
Timestream::createAnchor(double stability, const std::string &description) {
  // Generate a unique ID for the anchor
  std::string anchor_id = generateAnchorId();

  // Create the new anchor
  std::shared_ptr<TimeAnchor> new_anchor =
      std::make_shared<TimeAnchor>(anchor_id, stability, description);

  // Add it to the collection
  m_anchors.push_back(new_anchor);

  return new_anchor;
}

std::vector<std::shared_ptr<TimeAnchor>> Timestream::getAnchors() const {
  return m_anchors;
}

std::shared_ptr<TimeAnchor> Timestream::getLatestAnchor() const {
  if (m_anchors.empty()) {
    return nullptr;
  }

  // Find the anchor with the latest creation time
  return *std::max_element(m_anchors.begin(), m_anchors.end(),
                           [](const std::shared_ptr<TimeAnchor> &a,
                              const std::shared_ptr<TimeAnchor> &b) {
                             return a->getCreationTime() < b->getCreationTime();
                           });
}

std::shared_ptr<TimeAnchor>
Timestream::findAnchor(const std::string &id) const {
  auto it = std::find_if(m_anchors.begin(), m_anchors.end(),
                         [&id](const std::shared_ptr<TimeAnchor> &anchor) {
                           return anchor->getId() == id;
                         });

  if (it != m_anchors.end()) {
    return *it;
  }

  return nullptr;
}

double Timestream::calculateStability() const {
  if (m_anchors.empty()) {
    // If no anchors, use parent's stability or default to 0.5
    return m_parent ? m_parent->calculateStability() : 0.5;
  }

  // Calculate weighted average stability
  // More recent anchors have higher weight
  double total_stability = 0.0;
  double total_weight = 0.0;

  // Get the most recent timestamp for weight calculation
  Timestamp latest_time = getLatestAnchor()->getCreationTime();

  for (const auto &anchor : m_anchors) {
    // Calculate weight based on recency (more recent = higher weight)
    double time_diff =
        static_cast<double>(latest_time - anchor->getCreationTime()) + 1.0;
    double weight = 1.0 / time_diff;

    total_stability += anchor->getStability() * weight;
    total_weight += weight;
  }

  return total_stability / total_weight;
}

bool Timestream::isDerivedFrom(const std::shared_ptr<Timestream> other) const {
  if (!other) {
    return false;
  }

  // Check if this timestream is directly derived from the other
  if (m_parent && m_parent->getId() == other->getId()) {
    return true;
  }

  // Check ancestry recursively
  std::shared_ptr<Timestream> current_parent = m_parent;
  while (current_parent) {
    if (current_parent->getId() == other->getId()) {
      return true;
    }
    current_parent = current_parent->getParent();
  }

  return false;
}

double
Timestream::calculateDivergence(const std::shared_ptr<Timestream> other) const {
  if (!other) {
    return 1.0; // Maximum divergence if comparing to null
  }

  // If this is the same timestream, divergence is 0
  if (m_id == other->getId()) {
    return 0.0;
  }

  // Find common ancestor
  std::shared_ptr<Timestream> common_ancestor = nullptr;

  // Check if one is derived from the other
  if (isDerivedFrom(other)) {
    common_ancestor = other;
  } else if (other->isDerivedFrom(std::make_shared<Timestream>(*this))) {
    common_ancestor = std::make_shared<Timestream>(*this);
  } else {
    // Find the most recent common ancestor by traversing up the parent chain
    std::vector<std::string> this_ancestry;
    std::shared_ptr<Timestream> current = std::make_shared<Timestream>(*this);

    while (current) {
      this_ancestry.push_back(current->getId());
      current = current->getParent();
    }

    current = other;
    while (current && !common_ancestor) {
      auto it = std::find(this_ancestry.begin(), this_ancestry.end(),
                          current->getId());
      if (it != this_ancestry.end()) {
        common_ancestor = current;
        break;
      }
      current = current->getParent();
    }
  }

  // If no common ancestor, maximum divergence
  if (!common_ancestor) {
    return 1.0;
  }

  // Calculate divergence based on:
  // 1. Number of commits/anchors since common ancestor
  // 2. Temporal distance between the latest anchors

  // Count anchors since common ancestor in both timestreams
  int this_anchor_count = 0;
  int other_anchor_count = 0;

  // For simplicity, we'll just use the total number of anchors as an
  // approximation
  this_anchor_count = m_anchors.size();
  other_anchor_count = other->getAnchors().size();

  // Get the latest anchors from each timestream
  auto this_latest = getLatestAnchor();
  auto other_latest = other->getLatestAnchor();

  if (!this_latest || !other_latest) {
    return 0.8; // High divergence if one has no anchors
  }

  // Calculate temporal distance between latest anchors
  double temporal_distance =
      this_latest->calculateTemporalDistance(*other_latest);

  // Normalize temporal distance to [0,1] range (using arbitrary scale factor)
  double normalized_distance = std::min(1.0, temporal_distance / 1000.0);

  // Calculate anchor count difference factor
  double count_diff =
      std::abs(this_anchor_count - other_anchor_count) /
      static_cast<double>(std::max(this_anchor_count, other_anchor_count) + 1);

  // Combine factors with appropriate weights
  double divergence = 0.6 * normalized_distance + 0.4 * count_diff;

  return divergence;
}

std::string Timestream::generateAnchorId() const {
  // Generate a unique ID for an anchor
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);

  std::stringstream ss;
  ss << "anchor_" << m_id << "_";

  // Add timestamp
  auto now = std::chrono::system_clock::now().time_since_epoch();
  auto millis =
      std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  ss << millis << "_";

  // Add some random hex digits
  for (int i = 0; i < 8; i++) {
    ss << std::hex << dis(gen);
  }

  return ss.str();
}

} // namespace chronovyan