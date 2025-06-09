#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>
#include <stdexcept>

#include "timestream_manager.h"

namespace chronovyan {

TimestreamManager::TimestreamManager(TemporalDebtTracker &debt_tracker)
    : m_debt_tracker(debt_tracker) {
  // Initialize the main timestream
  initializeMainTimestream();
}

std::shared_ptr<Timestream> TimestreamManager::getMainTimestream() const {
  return m_main_timestream;
}

std::shared_ptr<Timestream> TimestreamManager::getActiveTimestream() const {
  return m_active_timestream;
}

bool TimestreamManager::setActiveTimestream(const std::string &timestream_id) {
  auto it = m_timestreams.find(timestream_id);
  if (it == m_timestreams.end()) {
    return false;
  }

  m_active_timestream = it->second;
  return true;
}

std::vector<std::shared_ptr<Timestream>>
TimestreamManager::getAllTimestreams() const {
  std::vector<std::shared_ptr<Timestream>> result;
  result.reserve(m_timestreams.size());

  for (const auto &pair : m_timestreams) {
    result.push_back(pair.second);
  }

  return result;
}

std::shared_ptr<Timestream>
TimestreamManager::findTimestream(const std::string &id) const {
  auto it = m_timestreams.find(id);
  if (it != m_timestreams.end()) {
    return it->second;
  }

  return nullptr;
}

std::shared_ptr<Timestream>
TimestreamManager::createEcho(const std::string &name,
                              const std::string &source_timestream_id,
                              const std::string &anchor_id) {
  // Find the source timestream
  auto source_timestream = findTimestream(source_timestream_id);
  if (!source_timestream) {
    throw std::runtime_error("Source timestream not found: " +
                             source_timestream_id);
  }

  // Find the anchor
  auto anchor = source_timestream->findAnchor(anchor_id);
  if (!anchor) {
    throw std::runtime_error("Anchor not found: " + anchor_id);
  }

  // Generate a unique ID for the new timestream
  std::string new_id = generateTimestreamId();

  // Create the new timestream
  auto new_timestream =
      std::make_shared<Timestream>(new_id, name, source_timestream, anchor);

  // Record debt for this operation
  recordEchoDebt(source_timestream, anchor);

  // Add to the collection
  m_timestreams[new_id] = new_timestream;

  return new_timestream;
}

std::shared_ptr<TimeAnchor>
TimestreamManager::createAnchor(double stability,
                                const std::string &description) {
  if (!m_active_timestream) {
    throw std::runtime_error("No active timestream");
  }

  return m_active_timestream->createAnchor(stability, description);
}

bool TimestreamManager::harmonizeTimestreams(const std::string &source_id,
                                             const std::string &target_id,
                                             RepaymentStrategyType strategy) {
  // Find the source and target timestreams
  auto source_timestream = findTimestream(source_id);
  if (!source_timestream) {
    throw std::runtime_error("Source timestream not found: " + source_id);
  }

  auto target_timestream = findTimestream(target_id);
  if (!target_timestream) {
    throw std::runtime_error("Target timestream not found: " + target_id);
  }

  // Check if the timestreams can be harmonized
  // For example, we might want to prevent harmonizing if the divergence is too
  // high
  double divergence = source_timestream->calculateDivergence(target_timestream);
  if (divergence > 0.8) {
    // Too divergent, would cause instability
    return false;
  }

  // Record debt for this operation
  [[maybe_unused]] double debt_amount =
      recordHarmonizationDebt(source_timestream, target_timestream);

  // Create a new anchor in the target timestream to represent the harmonization
  double new_stability = std::min(source_timestream->calculateStability(),
                                  target_timestream->calculateStability()) *
                         (1.0 - divergence);

  std::string description = "Harmonized from " + source_timestream->getName();
  target_timestream->createAnchor(new_stability, description);

  // Apply the specified repayment strategy to handle the debt
  // Provide default values for chronons and aethel
  double available_chronons = 100.0; // Default value
  double available_aethel = 100.0;   // Default value
  m_debt_tracker.applyRepaymentStrategy(strategy, available_chronons,
                                        available_aethel);

  return true;
}

double TimestreamManager::calculateHarmonizationImpact(
    const std::string &source_id, const std::string &target_id) const {
  // Find the source and target timestreams
  auto source_timestream = findTimestream(source_id);
  if (!source_timestream) {
    throw std::runtime_error("Source timestream not found: " + source_id);
  }

  auto target_timestream = findTimestream(target_id);
  if (!target_timestream) {
    throw std::runtime_error("Target timestream not found: " + target_id);
  }

  // Calculate divergence between the timestreams
  double divergence = source_timestream->calculateDivergence(target_timestream);

  // Calculate stability impact based on divergence and current stabilities
  double source_stability = source_timestream->calculateStability();
  double target_stability = target_timestream->calculateStability();

  // Weighted combination of stabilities, affected by divergence
  double combined_stability = (source_stability + target_stability) / 2.0;

  // Impact is negative if the result would be less stable than the target
  double impact = combined_stability * (1.0 - divergence) - target_stability;

  return impact;
}

double TimestreamManager::calculateEchoParadoxRisk(
    const std::string &timestream_id, const std::string &anchor_id) const {
  // Find the timestream
  auto timestream = findTimestream(timestream_id);
  if (!timestream) {
    throw std::runtime_error("Timestream not found: " + timestream_id);
  }

  // Find the anchor
  auto anchor = timestream->findAnchor(anchor_id);
  if (!anchor) {
    throw std::runtime_error("Anchor not found: " + anchor_id);
  }

  // Calculate the paradox risk based on:
  // 1. The stability of the anchor
  // 2. The age of the anchor (older = higher risk)
  // 3. The number of existing echoes from this timestream

  // Count existing echoes from this timestream
  int echo_count = 0;
  for (const auto &ts : getAllTimestreams()) {
    if (ts->getParent() && ts->getParent()->getId() == timestream_id) {
      echo_count++;
    }
  }

  // Get the latest anchor for age comparison
  auto latest_anchor = timestream->getLatestAnchor();
  if (!latest_anchor) {
    return 0.5; // Moderate risk if no latest anchor to compare
  }

  // Calculate age factor (0-1, older = higher risk)
  double time_diff = static_cast<double>(latest_anchor->getCreationTime() -
                                         anchor->getCreationTime());
  double age_factor = std::min(1.0, time_diff / 10000.0); // Normalize to [0,1]

  // Calculate stability factor (lower stability = higher risk)
  double stability_factor = 1.0 - anchor->getStability();

  // Calculate echo count factor (more echoes = higher risk)
  double echo_factor = std::min(1.0, echo_count / 5.0); // Normalize to [0,1]

  // Combine factors with weights
  double risk = 0.4 * age_factor + 0.4 * stability_factor + 0.2 * echo_factor;

  return risk;
}

bool TimestreamManager::deleteTimestream(const std::string &timestream_id) {
  // Cannot delete the main timestream
  if (m_main_timestream && timestream_id == m_main_timestream->getId()) {
    return false;
  }

  // Check if the timestream exists
  auto timestream = findTimestream(timestream_id);
  if (!timestream) {
    return false;
  }

  // Check if any timestreams are derived from this one
  for (const auto &pair : m_timestreams) {
    auto ts = pair.second;
    if (ts->getParent() && ts->getParent()->getId() == timestream_id) {
      // There are derived timestreams, cannot delete
      return false;
    }
  }

  // If this is the active timestream, set active to main
  if (m_active_timestream && m_active_timestream->getId() == timestream_id) {
    m_active_timestream = m_main_timestream;
  }

  // Remove from the collection
  m_timestreams.erase(timestream_id);

  return true;
}

void TimestreamManager::initializeMainTimestream() {
  std::string main_id = "main";
  std::string main_name = "Main Timestream";

  // Create the main timestream
  m_main_timestream = std::make_shared<Timestream>(main_id, main_name);

  // Add to collection
  m_timestreams[main_id] = m_main_timestream;

  // Set as active
  m_active_timestream = m_main_timestream;

  // Create an initial anchor with perfect stability
  m_main_timestream->createAnchor(1.0, "Initial anchor");
}

std::string TimestreamManager::generateTimestreamId() const {
  // Generate a unique ID for a timestream
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);

  std::stringstream ss;
  ss << "ts_";

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

double TimestreamManager::recordEchoDebt(
    const std::shared_ptr<Timestream> source_timestream,
    const std::shared_ptr<TimeAnchor> anchor) {
  if (!source_timestream || !anchor) {
    return 0.0;
  }

  // Calculate base debt amount for creating an Echo
  // Factors:
  // 1. Age of the anchor (older = more debt)
  // 2. Stability of the anchor (less stable = more debt)
  // 3. Number of existing echoes (more echoes = more debt)

  // Get the latest anchor for age comparison
  auto latest_anchor = source_timestream->getLatestAnchor();
  if (!latest_anchor) {
    return 0.0; // No debt if no latest anchor (shouldn't happen)
  }

  // Calculate age factor
  double time_diff = static_cast<double>(latest_anchor->getCreationTime() -
                                         anchor->getCreationTime());
  double age_factor =
      std::min(2.0, time_diff / 5000.0 + 1.0); // 1.0 - 2.0 range

  // Calculate stability factor
  double stability_factor = 2.0 - anchor->getStability(); // 1.0 - 2.0 range

  // Count existing echoes
  int echo_count = 0;
  for (const auto &ts : getAllTimestreams()) {
    if (ts->getParent() &&
        ts->getParent()->getId() == source_timestream->getId()) {
      echo_count++;
    }
  }

  // Calculate echo count factor
  double echo_factor =
      1.0 + (echo_count * 0.1); // Increases by 0.1 for each echo

  // Calculate debt amount
  double debt_amount = 10.0 * age_factor * stability_factor * echo_factor;

  // Record the debt - using chronon debt for Echo operations
  std::string operation_id =
      "Echo_" + source_timestream->getName() + "_" + anchor->getId();
  std::string description = "Created Echo from " +
                            source_timestream->getName() + " at anchor " +
                            anchor->getId();

  // Due date is proportional to the stability of the anchor
  // More stable anchors have longer due dates
  int due_days = static_cast<int>(30 * anchor->getStability());
  due_days = std::max(7, due_days); // At least 7 days

  // Calculate interest rate based on anchor stability
  [[maybe_unused]] double interest_rate =
      0.05 * (2.0 - anchor->getStability()); // 0.05 - 0.10 range

  // Add the debt to the tracker
  m_debt_tracker.borrowChronons(debt_amount, operation_id, false);

  return debt_amount;
}

double TimestreamManager::recordHarmonizationDebt(
    const std::shared_ptr<Timestream> source_timestream,
    const std::shared_ptr<Timestream> target_timestream) {
  if (!source_timestream || !target_timestream) {
    return 0.0;
  }

  // Calculate divergence between the timestreams
  double divergence = source_timestream->calculateDivergence(target_timestream);

  // Calculate base debt amount for harmonization
  // Factors:
  // 1. Divergence between timestreams (higher = more debt)
  // 2. Stability of both timestreams (less stable = more debt)

  // Calculate stability factors
  double source_stability = source_timestream->calculateStability();
  double target_stability = target_timestream->calculateStability();
  double stability_factor =
      2.0 - ((source_stability + target_stability) / 2.0); // 1.0 - 2.0 range

  // Calculate divergence factor
  double divergence_factor = 1.0 + (divergence * 3.0); // 1.0 - 4.0 range

  // Calculate debt amount
  double debt_amount = 20.0 * stability_factor * divergence_factor;

  // Record the debt - using aethel debt for harmonization
  std::string operation_id = "Harmonize_" + source_timestream->getName() + "_" +
                             target_timestream->getName();
  std::string description = "Harmonized " + source_timestream->getName() +
                            " into " + target_timestream->getName();

  // Due date is inversely proportional to divergence
  // More divergent timestreams have shorter due dates
  int due_days = static_cast<int>(30 * (1.0 - divergence));
  due_days = std::max(3, due_days); // At least 3 days

  // Calculate interest rate based on divergence
  [[maybe_unused]] double interest_rate =
      0.08 + (divergence * 0.12); // 0.08 - 0.20 range

  // High divergence harmonizations are critical
  bool is_critical = (divergence > 0.5);

  // Add the debt to the tracker using aethel debt
  m_debt_tracker.borrowAethel(debt_amount, operation_id, is_critical);

  return debt_amount;
}

} // namespace chronovyan