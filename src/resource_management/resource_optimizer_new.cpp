#include <algorithm>
#include <cmath>
#include <iomanip>
#include <numeric>
#include <sstream>

#include "../temporal/temporal_debt_tracker.h"
#include "../temporal/temporal_runtime.h"
#include "resource_optimizer.h"

namespace chronovyan {

ResourceOptimizer::ResourceOptimizer(
    std::shared_ptr<TemporalRuntime> runtime,
    std::shared_ptr<TemporalDebtTracker> debt_tracker)
    : m_runtime(runtime), m_debt_tracker(debt_tracker) {
  // Initialize optimization state
}

double ResourceOptimizer::optimizeChronons(const std::string &operationId) {
  // Implement chronon optimization for the given operation
  // For now, return a simple optimization factor based on debt and stability
  double debt_ratio = m_debt_tracker->getChrononsDebt() /
                      std::max(1.0, m_debt_tracker->getChrononsCapacity());
  double stability = m_debt_tracker->calculateTemporalStability();

  // Lower debt and higher stability means better optimization
  double optimization_factor = 1.0 - (debt_ratio * 0.5) + (stability * 0.2);

  // Ensure optimization factor is within reasonable bounds
  return std::max(0.5, std::min(1.5, optimization_factor));
}

double ResourceOptimizer::optimizeAethel(const std::string &timelineId) {
  // Implement aethel optimization for the given timeline
  // Similar to chronon optimization, but with different weights
  double debt_ratio = m_debt_tracker->getAethelDebt() /
                      std::max(1.0, m_debt_tracker->getAethelCapacity());
  double paradox_risk = m_debt_tracker->calculateParadoxRisk();

  // Lower debt and lower paradox risk means better optimization
  double optimization_factor = 1.0 - (debt_ratio * 0.4) - (paradox_risk * 0.3);

  // Ensure optimization factor is within reasonable bounds
  return std::max(0.6, std::min(1.4, optimization_factor));
}

std::map<std::string, double> ResourceOptimizer::analyzeResourceUsage() {
  // Perform analysis of resource usage patterns
  std::map<std::string, double> analysis;

  // Add key metrics to the analysis map
  analysis["chronon_efficiency"] = calculateChronoEfficiency();
  analysis["aethel_efficiency"] = calculateAethelEfficiency();
  analysis["paradox_risk"] = m_debt_tracker->calculateParadoxRisk();
  analysis["temporal_stability"] = m_debt_tracker->calculateTemporalStability();
  analysis["debt_ratio"] = calculateDebtRatio();
  analysis["optimization_potential"] = calculateOptimizationPotential();

  return analysis;
}

std::vector<std::pair<int, int>>
ResourceOptimizer::predictResourceNeeds(int steps) {
  // Predict resource needs for the next N steps
  std::vector<std::pair<int, int>> predictions;

  // Get current resource state
  double current_chronons = m_debt_tracker->getAvailableChronons();
  double current_aethel = m_debt_tracker->getAvailableAethel();

  // Simple linear projection based on recent trends
  double chronon_growth_rate = 0.05; // 5% growth per step
  double aethel_growth_rate = 0.03;  // 3% growth per step

  // Generate predictions for each step
  for (int i = 1; i <= steps; ++i) {
    int predicted_chronons = static_cast<int>(
        current_chronons * std::pow(1 + chronon_growth_rate, i));
    int predicted_aethel =
        static_cast<int>(current_aethel * std::pow(1 + aethel_growth_rate, i));

    predictions.push_back(std::make_pair(predicted_chronons, predicted_aethel));
  }

  return predictions;
}

// Private helper methods

double ResourceOptimizer::calculateChronoEfficiency() const {
  // Calculate efficiency of chronon usage
  double available = m_debt_tracker->getAvailableChronons();
  double capacity = m_debt_tracker->getChrononsCapacity();
  double debt = m_debt_tracker->getChrononsDebt();

  // Higher is better (range 0.0 to 1.0)
  return std::max(0.0, std::min(1.0, (available / (capacity + debt)) *
                                         (1.0 - debt / (capacity * 2))));
}

double ResourceOptimizer::calculateAethelEfficiency() const {
  // Calculate efficiency of aethel usage
  double available = m_debt_tracker->getAvailableAethel();
  double capacity = m_debt_tracker->getAethelCapacity();
  double debt = m_debt_tracker->getAethelDebt();

  // Higher is better (range 0.0 to 1.0)
  return std::max(0.0, std::min(1.0, (available / (capacity + debt)) *
                                         (1.0 - debt / (capacity * 2))));
}

double ResourceOptimizer::calculateDebtRatio() const {
  // Calculate the ratio of total debt to total capacity
  double chronon_debt = m_debt_tracker->getChrononsDebt();
  double aethel_debt = m_debt_tracker->getAethelDebt();
  double chronon_capacity = m_debt_tracker->getChrononsCapacity();
  double aethel_capacity = m_debt_tracker->getAethelCapacity();

  double total_debt = chronon_debt + aethel_debt;
  double total_capacity = chronon_capacity + aethel_capacity;

  // Lower is better (range 0.0 to potentially > 1.0 if debt exceeds capacity)
  return total_debt / std::max(1.0, total_capacity);
}

double ResourceOptimizer::calculateOptimizationPotential() const {
  // Calculate the potential for optimization (higher means more room for
  // improvement)
  double chronon_efficiency = calculateChronoEfficiency();
  double aethel_efficiency = calculateAethelEfficiency();
  double stability = m_debt_tracker->calculateTemporalStability();

  // Lower efficiency and higher stability means more optimization potential
  // Range: 0.0 (no potential) to 1.0 (high potential)
  return std::max(0.0, std::min(1.0, (1.0 - chronon_efficiency) * 0.4 +
                                         (1.0 - aethel_efficiency) * 0.4 +
                                         stability * 0.2));
}

// Additional public methods from the header file

std::vector<OptimizationRecommendation>
ResourceOptimizer::generateRecommendations() const {
  std::vector<OptimizationRecommendation> recommendations;

  // Check if we have enough data for meaningful recommendations
  if (!hasEnoughData()) {
    OptimizationRecommendation recommendation;
    recommendation.description =
        "Insufficient data for meaningful recommendations";
    recommendation.estimated_improvement = 0.0;
    recommendation.confidence = 0.5;
    recommendations.push_back(recommendation);
    return recommendations;
  }

  // Generate recommendations based on resource usage patterns
  auto pattern_recommendations = analyzeUsagePatterns();
  recommendations.insert(recommendations.end(), pattern_recommendations.begin(),
                         pattern_recommendations.end());

  // Check for resource spikes and suggest optimizations
  auto spike_recommendations = detectResourceSpikes();
  recommendations.insert(recommendations.end(), spike_recommendations.begin(),
                         spike_recommendations.end());

  // Suggest balanced resource usage if needed
  auto balance_recommendations = suggestBalancedUsage();
  recommendations.insert(recommendations.end(), balance_recommendations.begin(),
                         balance_recommendations.end());

  return recommendations;
}

std::map<std::string, double> ResourceOptimizer::getEfficiencyMetrics() const {
  std::map<std::string, double> metrics;

  metrics["chronon_efficiency"] = calculateChronoEfficiency();
  metrics["aethel_efficiency"] = calculateAethelEfficiency();
  metrics["temporal_stability"] = m_debt_tracker->calculateTemporalStability();
  metrics["paradox_risk"] = m_debt_tracker->calculateParadoxRisk();
  metrics["debt_ratio"] = calculateDebtRatio();
  metrics["resource_utilization"] = calculateResourceUtilizationVariance();

  return metrics;
}

std::string ResourceOptimizer::generateOptimizationReport() const {
  std::stringstream report;

  // Report header
  report << "=== RESOURCE OPTIMIZATION REPORT ===" << std::endl << std::endl;

  // Efficiency metrics
  auto metrics = getEfficiencyMetrics();
  report << "EFFICIENCY METRICS:" << std::endl;
  report << "  Chronon Efficiency: " << std::fixed << std::setprecision(2)
         << metrics["chronon_efficiency"] * 100 << "%" << std::endl;
  report << "  Aethel Efficiency: " << std::fixed << std::setprecision(2)
         << metrics["aethel_efficiency"] * 100 << "%" << std::endl;
  report << "  Temporal Stability: " << std::fixed << std::setprecision(2)
         << metrics["temporal_stability"] * 100 << "%" << std::endl;
  report << "  Paradox Risk: " << std::fixed << std::setprecision(2)
         << metrics["paradox_risk"] * 100 << "%" << std::endl;
  report << "  Debt Ratio: " << std::fixed << std::setprecision(2)
         << metrics["debt_ratio"] * 100 << "%" << std::endl;
  report << std::endl;

  // Recommendations
  auto recommendations = generateRecommendations();
  report << "OPTIMIZATION RECOMMENDATIONS:" << std::endl;
  if (recommendations.empty()) {
    report << "  No specific recommendations at this time." << std::endl;
  } else {
    for (size_t i = 0; i < recommendations.size(); ++i) {
      const auto &rec = recommendations[i];
      report << "  " << i + 1 << ". " << rec.description << std::endl;
      report << "     Est. Improvement: " << std::fixed << std::setprecision(1)
             << rec.estimated_improvement * 100 << "%" << std::endl;
      report << "     Confidence: " << std::fixed << std::setprecision(1)
             << rec.confidence * 100 << "%" << std::endl;
    }
  }

  return report.str();
}

void ResourceOptimizer::setMinimumDataPoints(size_t min_data_points) {
  min_data_points_ = min_data_points;
}

// Additional private methods from the header file

bool ResourceOptimizer::hasEnoughData() const {
  // In a real implementation, this would check historical data
  // For now, just return true to allow recommendations
  return true;
}

double ResourceOptimizer::calculateTemporalStability() const {
  return m_debt_tracker->calculateTemporalStability();
}

double ResourceOptimizer::calculateResourceUtilizationVariance() const {
  // In a real implementation, this would calculate the variance in resource
  // usage For now, return a placeholder value
  return 0.15; // 15% variance
}

std::vector<OptimizationRecommendation>
ResourceOptimizer::analyzeUsagePatterns() const {
  std::vector<OptimizationRecommendation> recommendations;

  // Add a sample recommendation
  OptimizationRecommendation rec1;
  rec1.description = "Optimize chronon allocation across parallel timelines";
  rec1.estimated_improvement = 0.12; // 12% improvement
  rec1.confidence = 0.85;            // 85% confidence
  recommendations.push_back(rec1);

  return recommendations;
}

std::vector<OptimizationRecommendation>
ResourceOptimizer::detectResourceSpikes() const {
  std::vector<OptimizationRecommendation> recommendations;

  // Add a sample recommendation
  OptimizationRecommendation rec1;
  rec1.description = "Implement aethel caching to smooth consumption spikes";
  rec1.estimated_improvement = 0.08; // 8% improvement
  rec1.confidence = 0.75;            // 75% confidence
  recommendations.push_back(rec1);

  return recommendations;
}

std::vector<OptimizationRecommendation>
ResourceOptimizer::suggestBalancedUsage() const {
  std::vector<OptimizationRecommendation> recommendations;

  // Add a sample recommendation
  OptimizationRecommendation rec1;
  rec1.description =
      "Balance chronon and aethel usage ratio to improve stability";
  rec1.estimated_improvement = 0.15; // 15% improvement
  rec1.confidence = 0.82;            // 82% confidence
  recommendations.push_back(rec1);

  return recommendations;
}

// New members for the constructor parameter
std::shared_ptr<TemporalRuntime> m_runtime;
std::shared_ptr<TemporalDebtTracker> m_debt_tracker;

} // namespace chronovyan