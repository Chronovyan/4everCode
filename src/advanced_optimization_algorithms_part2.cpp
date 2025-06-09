#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

#include "advanced_optimization_algorithms.h"

namespace chronovyan {

// Predictive optimization
double AdvancedOptimizationAlgorithms::optimizePredictive(
    const std::string &operation_id, int prediction_horizon) {
  // Get historical resource usage for the operation
  auto usage_history = getOperationUsageHistory(operation_id);
  if (usage_history.empty()) {
    return 0.0;
  }

  // Analyze the pattern
  auto pattern_result = detectPatternAdvanced(operation_id);

  // Extract chronon and aethel usage values for analysis
  std::vector<double> chronon_data;
  std::vector<double> aethel_data;
  for (const auto &entry : usage_history) {
    chronon_data.push_back(entry.chronons_used);
    aethel_data.push_back(entry.aethel_used);
  }

  // Predict future usage based on the pattern
  std::vector<double> predicted_chronons;
  std::vector<double> predicted_aethel;

  if (pattern_result.primary_pattern == "repetitive") {
    // For repetitive patterns, just use the average
    double avg_chronons =
        std::accumulate(chronon_data.begin(), chronon_data.end(), 0.0) /
        chronon_data.size();
    double avg_aethel =
        std::accumulate(aethel_data.begin(), aethel_data.end(), 0.0) /
        aethel_data.size();

    for (int i = 0; i < prediction_horizon; ++i) {
      predicted_chronons.push_back(avg_chronons);
      predicted_aethel.push_back(avg_aethel);
    }
  } else if (pattern_result.primary_pattern == "cyclic" &&
             pattern_result.is_seasonal) {
    // For cyclic patterns, use the values from the previous cycle
    int period = pattern_result.seasonality_period;
    if (period <= 0 || period >= static_cast<int>(chronon_data.size())) {
      period = 1; // Fallback
    }

    for (int i = 0; i < prediction_horizon; ++i) {
      int index =
          (chronon_data.size() - period + (i % period)) % chronon_data.size();
      predicted_chronons.push_back(chronon_data[index]);
      predicted_aethel.push_back(aethel_data[index]);
    }
  } else if (pattern_result.primary_pattern == "increasing" ||
             pattern_result.primary_pattern == "decreasing") {
    // For trending patterns, use linear regression
    auto chronon_coeffs = regressionAnalysis(chronon_data, 1);
    auto aethel_coeffs = regressionAnalysis(aethel_data, 1);

    double chronon_slope =
        (chronon_coeffs.size() > 1) ? chronon_coeffs[1] : 0.0;
    double chronon_intercept =
        (chronon_coeffs.size() > 0) ? chronon_coeffs[0] : 0.0;
    double aethel_slope = (aethel_coeffs.size() > 1) ? aethel_coeffs[1] : 0.0;
    double aethel_intercept =
        (aethel_coeffs.size() > 0) ? aethel_coeffs[0] : 0.0;

    for (int i = 0; i < prediction_horizon; ++i) {
      int time_point = static_cast<int>(chronon_data.size()) + i;
      predicted_chronons.push_back(chronon_intercept +
                                   chronon_slope * time_point);
      predicted_aethel.push_back(aethel_intercept + aethel_slope * time_point);
    }
  } else if (pattern_result.primary_pattern == "spike") {
    // For spike patterns, use a combination of average and max
    double avg_chronons =
        std::accumulate(chronon_data.begin(), chronon_data.end(), 0.0) /
        chronon_data.size();
    double avg_aethel =
        std::accumulate(aethel_data.begin(), aethel_data.end(), 0.0) /
        aethel_data.size();
    double max_chronons =
        *std::max_element(chronon_data.begin(), chronon_data.end());
    double max_aethel =
        *std::max_element(aethel_data.begin(), aethel_data.end());

    for (int i = 0; i < prediction_horizon; ++i) {
      // Assume a spike might occur in the middle of the prediction horizon
      if (i == prediction_horizon / 2) {
        predicted_chronons.push_back(max_chronons);
        predicted_aethel.push_back(max_aethel);
      } else {
        predicted_chronons.push_back(avg_chronons);
        predicted_aethel.push_back(avg_aethel);
      }
    }
  } else {
    // For random or unknown patterns, use the average of recent values
    int window_size = std::min(5, static_cast<int>(chronon_data.size()));
    double recent_avg_chronons =
        std::accumulate(chronon_data.end() - window_size, chronon_data.end(),
                        0.0) /
        window_size;
    double recent_avg_aethel = std::accumulate(aethel_data.end() - window_size,
                                               aethel_data.end(), 0.0) /
                               window_size;

    for (int i = 0; i < prediction_horizon; ++i) {
      predicted_chronons.push_back(recent_avg_chronons);
      predicted_aethel.push_back(recent_avg_aethel);
    }
  }

  // Now optimize based on the predictions
  double total_optimization = 0.0;

  // Pre-allocate resources based on predictions
  for (int i = 0; i < prediction_horizon; ++i) {
    // In a real implementation, we'd need to interact with the resource
    // allocator For now, just calculate the theoretical optimization
    double chronon_opt = optimizeResourceAllocation(
        ResourceType::CHRONON, predicted_chronons[i], operation_id);
    double aethel_opt = optimizeResourceAllocation(
        ResourceType::AETHEL, predicted_aethel[i], operation_id);

    total_optimization += (chronon_opt + aethel_opt) / 2.0;
  }

  // Return the average optimization factor
  return (prediction_horizon > 0) ? total_optimization / prediction_horizon
                                  : 0.0;
}

// Neural network optimization
double AdvancedOptimizationAlgorithms::optimizeNeuralNetwork(
    const std::string &operation_id) {
  // Get historical resource usage for the operation
  auto usage_history = getOperationUsageHistory(operation_id);
  if (usage_history.size() < 10) { // Need enough data for training
    return 0.0;
  }

  // Prepare training data
  std::vector<std::vector<double>> inputs;
  std::vector<std::vector<double>> targets;

  const int input_window = 5;
  const int output_window = 2;

  for (size_t i = input_window; i + output_window <= usage_history.size();
       ++i) {
    std::vector<double> input;
    for (int j = 0; j < input_window; ++j) {
      input.push_back(usage_history[i - input_window + j].chronons_used);
      input.push_back(usage_history[i - input_window + j].aethel_used);
    }

    std::vector<double> target;
    for (int j = 0; j < output_window; ++j) {
      target.push_back(usage_history[i + j].chronons_used);
      target.push_back(usage_history[i + j].aethel_used);
    }

    inputs.push_back(input);
    targets.push_back(target);
  }

  // Create or retrieve the neural network for this operation
  if (m_networks.find(operation_id) == m_networks.end()) {
    std::vector<int> layers = {input_window * 2, 10, output_window * 2};
    m_networks[operation_id] = std::make_shared<SimpleNeuralNetwork>(layers);
  }

  // Train the network
  m_networks[operation_id]->train(inputs, targets, 100, 0.01);

  // Make predictions for future usage
  std::vector<double> last_input;
  for (int i = 0; i < input_window; ++i) {
    size_t index = usage_history.size() - input_window + i;
    last_input.push_back(usage_history[index].chronons_used);
    last_input.push_back(usage_history[index].aethel_used);
  }

  std::vector<double> prediction =
      m_networks[operation_id]->predict(last_input);

  // Optimize based on the predictions
  double total_optimization = 0.0;
  int prediction_count = 0;

  for (size_t i = 0; i < prediction.size(); i += 2) {
    if (i + 1 < prediction.size()) {
      double predicted_chronons = prediction[i];
      double predicted_aethel = prediction[i + 1];

      // Ensure predictions are positive
      predicted_chronons = std::max(0.0, predicted_chronons);
      predicted_aethel = std::max(0.0, predicted_aethel);

      double chronon_opt = optimizeResourceAllocation(
          ResourceType::CHRONON, predicted_chronons, operation_id);
      double aethel_opt = optimizeResourceAllocation(
          ResourceType::AETHEL, predicted_aethel, operation_id);

      total_optimization += (chronon_opt + aethel_opt) / 2.0;
      prediction_count++;
    }
  }

  // Return the average optimization factor
  return (prediction_count > 0) ? total_optimization / prediction_count : 0.0;
}

// Genetic algorithm optimization
double AdvancedOptimizationAlgorithms::optimizeGenetic(int population_size,
                                                       int generations) {
  // Get all active operations
  auto operations = getActiveOperations();
  if (operations.empty()) {
    return 0.0;
  }

  // Optimize resource allocation across all operations using genetic algorithm
  double total_improvement = 0.0;

  for (const auto &operation_id : operations) {
    auto allocation_strategy =
        geneticOptimization(population_size, generations, operation_id);

    // Apply the optimized allocation if valid
    if (!allocation_strategy.empty() && allocation_strategy.size() >= 2) {
      double chronon_allocation = allocation_strategy[0];
      double aethel_allocation = allocation_strategy[1];

      // In a real implementation, we'd need to interact with the resource
      // allocator For now, just calculate the theoretical optimization
      double chronon_opt = optimizeResourceAllocation(
          ResourceType::CHRONON, chronon_allocation, operation_id);
      double aethel_opt = optimizeResourceAllocation(
          ResourceType::AETHEL, aethel_allocation, operation_id);

      double operation_improvement = (chronon_opt + aethel_opt) / 2.0;
      total_improvement += operation_improvement;
    }
  }

  // Return the average improvement across all operations
  return (operations.size() > 0) ? total_improvement / operations.size() : 0.0;
}

// Comprehensive optimization
double AdvancedOptimizationAlgorithms::optimizeComprehensive(
    const std::string &operation_id) {
  // Try all available optimization strategies and use the best one
  std::vector<double> optimization_results;

  // Basic optimization (from base class)
  double basic_result = optimizeOperation(operation_id);
  optimization_results.push_back(basic_result);

  // Predictive optimization
  double predictive_result = optimizePredictive(operation_id);
  optimization_results.push_back(predictive_result);

  // Neural network optimization
  double neural_result = optimizeNeuralNetwork(operation_id);
  optimization_results.push_back(neural_result);

  // Genetic algorithm optimization (for this operation only)
  std::vector<double> genetic_allocation =
      geneticOptimization(30, 5, operation_id);
  double genetic_result = 0.0;

  if (!genetic_allocation.empty() && genetic_allocation.size() >= 2) {
    double chronon_allocation = genetic_allocation[0];
    double aethel_allocation = genetic_allocation[1];

    double chronon_opt = optimizeResourceAllocation(
        ResourceType::CHRONON, chronon_allocation, operation_id);
    double aethel_opt = optimizeResourceAllocation(
        ResourceType::AETHEL, aethel_allocation, operation_id);

    genetic_result = (chronon_opt + aethel_opt) / 2.0;
  }
  optimization_results.push_back(genetic_result);

  // Return the best result
  return *std::max_element(optimization_results.begin(),
                           optimization_results.end());
}

// Get optimization insights
std::map<std::string, double>
AdvancedOptimizationAlgorithms::getOptimizationInsights(
    const std::string &operation_id) const {
  std::map<std::string, double> insights;

  // Get operation usage history
  auto usage_history = getOperationUsageHistory(operation_id);
  if (usage_history.empty()) {
    return insights;
  }

  // Calculate various metrics
  double total_chronons = 0.0;
  double total_aethel = 0.0;
  double max_chronons = 0.0;
  double max_aethel = 0.0;
  double min_chronons = std::numeric_limits<double>::max();
  double min_aethel = std::numeric_limits<double>::max();

  for (const auto &entry : usage_history) {
    total_chronons += entry.chronons_used;
    total_aethel += entry.aethel_used;

    max_chronons = std::max(max_chronons, entry.chronons_used);
    max_aethel = std::max(max_aethel, entry.aethel_used);

    min_chronons = std::min(min_chronons, entry.chronons_used);
    min_aethel = std::min(min_aethel, entry.aethel_used);
  }

  double avg_chronons = total_chronons / usage_history.size();
  double avg_aethel = total_aethel / usage_history.size();

  // Calculate variance
  double chronon_variance = 0.0;
  double aethel_variance = 0.0;

  for (const auto &entry : usage_history) {
    chronon_variance += (entry.chronons_used - avg_chronons) *
                        (entry.chronons_used - avg_chronons);
    aethel_variance +=
        (entry.aethel_used - avg_aethel) * (entry.aethel_used - avg_aethel);
  }

  chronon_variance /= usage_history.size();
  aethel_variance /= usage_history.size();

  // Store insights
  insights["avg_chronons"] = avg_chronons;
  insights["avg_aethel"] = avg_aethel;
  insights["max_chronons"] = max_chronons;
  insights["max_aethel"] = max_aethel;
  insights["min_chronons"] = min_chronons;
  insights["min_aethel"] = min_aethel;
  insights["chronon_variance"] = chronon_variance;
  insights["aethel_variance"] = aethel_variance;
  insights["chronon_stability"] =
      (max_chronons > 0) ? min_chronons / max_chronons : 0.0;
  insights["aethel_stability"] =
      (max_aethel > 0) ? min_aethel / max_aethel : 0.0;

  // Calculate optimization potential
  double chronon_potential =
      (avg_chronons > 0) ? ((max_chronons - avg_chronons) / avg_chronons) : 0.0;
  double aethel_potential =
      (avg_aethel > 0) ? ((max_aethel - avg_aethel) / avg_aethel) : 0.0;

  insights["chronon_optimization_potential"] = chronon_potential;
  insights["aethel_optimization_potential"] = aethel_potential;
  insights["overall_optimization_potential"] =
      (chronon_potential + aethel_potential) / 2.0;

  return insights;
}

// Visualization of optimization opportunities
std::string AdvancedOptimizationAlgorithms::visualizeOptimizationOpportunities(
    VisualizationFormat format) const {
  std::ostringstream output;

  // Get all active operations
  auto operations = getActiveOperations();
  if (operations.empty()) {
    return "No active operations to visualize.";
  }

  // Calculate optimization potential for each operation
  std::vector<std::pair<std::string, double>> optimization_potentials;

  for (const auto &operation_id : operations) {
    auto insights = getOptimizationInsights(operation_id);
    double potential = insights["overall_optimization_potential"];
    optimization_potentials.push_back({operation_id, potential});
  }

  // Sort by potential (highest first)
  std::sort(optimization_potentials.begin(), optimization_potentials.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });

  // Generate visualization based on format
  switch (format) {
  case VisualizationFormat::TEXT: {
    output << "Optimization Opportunities:\n";
    output << "==========================\n\n";

    for (const auto &[operation_id, potential] : optimization_potentials) {
      output << "Operation: " << operation_id << "\n";
      output << "Optimization Potential: " << std::fixed << std::setprecision(2)
             << potential * 100.0 << "%\n";
      output << "Recommended Algorithm: ";

      // Recommend algorithm based on potential
      if (potential > 0.5) {
        output << "Comprehensive Optimization";
      } else if (potential > 0.3) {
        output << "Predictive Optimization";
      } else if (potential > 0.1) {
        output << "Neural Network Optimization";
      } else {
        output << "Basic Optimization";
      }

      output << "\n\n";
    }
    break;
  }
  case VisualizationFormat::HTML: {
    output << "<html><head><title>Optimization "
              "Opportunities</title></head><body>\n";
    output << "<h1>Optimization Opportunities</h1>\n";
    output << "<table border='1'>\n";
    output << "<tr><th>Operation</th><th>Optimization "
              "Potential</th><th>Recommended "
              "Algorithm</th></tr>\n";

    for (const auto &[operation_id, potential] : optimization_potentials) {
      output << "<tr>";
      output << "<td>" << operation_id << "</td>";
      output << "<td>" << std::fixed << std::setprecision(2)
             << potential * 100.0 << "%</td>";
      output << "<td>";

      // Recommend algorithm based on potential
      if (potential > 0.5) {
        output << "Comprehensive Optimization";
      } else if (potential > 0.3) {
        output << "Predictive Optimization";
      } else if (potential > 0.1) {
        output << "Neural Network Optimization";
      } else {
        output << "Basic Optimization";
      }

      output << "</td>";
      output << "</tr>\n";
    }

    output << "</table></body></html>\n";
    break;
  }
  case VisualizationFormat::JSON: {
    output << "{\n";
    output << "  \"optimization_opportunities\": [\n";

    for (size_t i = 0; i < optimization_potentials.size(); ++i) {
      const auto &[operation_id, potential] = optimization_potentials[i];

      output << "    {\n";
      output << "      \"operation\": \"" << operation_id << "\",\n";
      output << "      \"potential\": " << std::fixed << std::setprecision(4)
             << potential << ",\n";
      output << "      \"recommended_algorithm\": \"";

      // Recommend algorithm based on potential
      if (potential > 0.5) {
        output << "comprehensive";
      } else if (potential > 0.3) {
        output << "predictive";
      } else if (potential > 0.1) {
        output << "neural_network";
      } else {
        output << "basic";
      }

      output << "\"\n";
      output << "    }";

      if (i < optimization_potentials.size() - 1) {
        output << ",";
      }

      output << "\n";
    }

    output << "  ]\n";
    output << "}\n";
    break;
  }
  default:
    output << "Unsupported visualization format.";
  }

  return output.str();
}

// Efficiency report generation
std::string
AdvancedOptimizationAlgorithms::generateEfficiencyReport(bool detailed) const {
  std::ostringstream output;

  // Get all active operations
  auto operations = getActiveOperations();
  if (operations.empty()) {
    return "No active operations to report on.";
  }

  // Calculate overall statistics
  double total_chronons_saved = 0.0;
  double total_aethel_saved = 0.0;
  double total_efficiency_improvement = 0.0;
  int total_optimizations = 0;

  // Get optimization history
  auto history = getOptimizationHistory();

  for (const auto &entry : history) {
    total_chronons_saved += entry.chronons_saved;
    total_aethel_saved += entry.aethel_saved;
    total_efficiency_improvement += entry.improvement_factor;
    total_optimizations++;
  }

  double avg_improvement =
      (total_optimizations > 0)
          ? total_efficiency_improvement / total_optimizations
          : 0.0;

  // Generate report
  output << "Efficiency Report\n";
  output << "================\n\n";

  output << "Overall Statistics:\n";
  output << "-----------------\n";
  output << "Total Operations: " << operations.size() << "\n";
  output << "Total Optimizations Performed: " << total_optimizations << "\n";
  output << "Total Chronons Saved: " << std::fixed << std::setprecision(2)
         << total_chronons_saved << "\n";
  output << "Total Aethel Saved: " << std::fixed << std::setprecision(2)
         << total_aethel_saved << "\n";
  output << "Average Efficiency Improvement: " << std::fixed
         << std::setprecision(2) << avg_improvement * 100.0 << "%\n\n";

  if (detailed) {
    output << "Operation Details:\n";
    output << "-----------------\n";

    for (const auto &operation_id : operations) {
      output << "Operation: " << operation_id << "\n";

      // Calculate operation-specific statistics
      double op_chronons_saved = 0.0;
      double op_aethel_saved = 0.0;
      double op_efficiency_improvement = 0.0;
      int op_optimizations = 0;

      for (const auto &entry : history) {
        if (entry.operation_id == operation_id) {
          op_chronons_saved += entry.chronons_saved;
          op_aethel_saved += entry.aethel_saved;
          op_efficiency_improvement += entry.improvement_factor;
          op_optimizations++;
        }
      }

      double op_avg_improvement =
          (op_optimizations > 0) ? op_efficiency_improvement / op_optimizations
                                 : 0.0;

      output << "  Optimizations Performed: " << op_optimizations << "\n";
      output << "  Chronons Saved: " << std::fixed << std::setprecision(2)
             << op_chronons_saved << "\n";
      output << "  Aethel Saved: " << std::fixed << std::setprecision(2)
             << op_aethel_saved << "\n";
      output << "  Average Efficiency Improvement: " << std::fixed
             << std::setprecision(2) << op_avg_improvement * 100.0 << "%\n";

      // Get insights for additional information
      auto insights = getOptimizationInsights(operation_id);
      output << "  Optimization Potential: " << std::fixed
             << std::setprecision(2)
             << insights["overall_optimization_potential"] * 100.0 << "%\n";

      // Add recommendations
      output << "  Recommendation: ";
      if (insights["overall_optimization_potential"] > 0.3) {
        output
            << "High optimization potential. Consider applying comprehensive "
               "optimization.\n";
      } else if (insights["overall_optimization_potential"] > 0.1) {
        output << "Moderate optimization potential. Regular predictive "
                  "optimization "
                  "recommended.\n";
      } else {
        output << "Low optimization potential. Current optimization level is "
                  "sufficient.\n";
      }

      output << "\n";
    }
  }

  return output.str();
}

// Strategy comparison
std::map<OptimizationStrategy, double>
AdvancedOptimizationAlgorithms::compareOptimizationStrategies(
    const std::string &operation_id,
    const std::vector<OptimizationStrategy> &strategies) {
  std::map<OptimizationStrategy, double> results;

  // Store current strategy
  OptimizationStrategy current_strategy = m_strategy;

  // Test each strategy
  for (const auto &strategy : strategies) {
    setOptimizationStrategy(strategy);

    double improvement = 0.0;

    switch (strategy) {
    case OptimizationStrategy::CONSERVATIVE:
      // Basic optimization
      improvement = optimizeOperation(operation_id);
      break;

    case OptimizationStrategy::BALANCED:
      // Use predictive with moderate horizon
      improvement = optimizePredictive(operation_id, 3);
      break;

    case OptimizationStrategy::AGGRESSIVE:
      // Use comprehensive optimization
      improvement = optimizeComprehensive(operation_id);
      break;

    case OptimizationStrategy::ADAPTIVE:
      // Adaptive based on pattern
      {
        auto pattern_result = detectPatternAdvanced(operation_id);
        if (pattern_result.primary_pattern == "cyclic" ||
            pattern_result.primary_pattern == "repetitive") {
          improvement = optimizePredictive(operation_id, 5);
        } else if (pattern_result.primary_pattern == "increasing" ||
                   pattern_result.primary_pattern == "decreasing") {
          improvement = optimizeNeuralNetwork(operation_id);
        } else {
          improvement = optimizeGenetic(30, 5);
        }
      }
      break;

    case OptimizationStrategy::PREDICTIVE:
      // Use predictive with longer horizon
      improvement = optimizePredictive(operation_id, 10);
      break;

    case OptimizationStrategy::EXPERIMENTAL:
      // Use neural network and genetic
      {
        double neural_improvement = optimizeNeuralNetwork(operation_id);
        double genetic_improvement = 0.0;

        std::vector<double> genetic_allocation =
            geneticOptimization(50, 10, operation_id);
        if (!genetic_allocation.empty() && genetic_allocation.size() >= 2) {
          double chronon_allocation = genetic_allocation[0];
          double aethel_allocation = genetic_allocation[1];

          double chronon_opt = optimizeResourceAllocation(
              ResourceType::CHRONON, chronon_allocation, operation_id);
          double aethel_opt = optimizeResourceAllocation(
              ResourceType::AETHEL, aethel_allocation, operation_id);

          genetic_improvement = (chronon_opt + aethel_opt) / 2.0;
        }

        improvement = std::max(neural_improvement, genetic_improvement);
      }
      break;
    }

    results[strategy] = improvement;
  }

  // Restore original strategy
  setOptimizationStrategy(current_strategy);

  return results;
}

// Genetic algorithm implementation
std::vector<double> AdvancedOptimizationAlgorithms::geneticOptimization(
    int population_size, int generations, const std::string &operation_id) {
  // Get usage history for the operation
  auto usage_history = getOperationUsageHistory(operation_id);
  if (usage_history.empty()) {
    return {};
  }

  // Initialize population (each chromosome is [chronon_allocation,
  // aethel_allocation])
  std::vector<std::vector<double>> population;

  // Calculate ranges for allocations based on historical data
  double max_chronons = 0.0;
  double max_aethel = 0.0;

  for (const auto &entry : usage_history) {
    max_chronons = std::max(max_chronons, entry.chronons_used);
    max_aethel = std::max(max_aethel, entry.aethel_used);
  }

  // Add some margin
  max_chronons *= 1.5;
  max_aethel *= 1.5;

  // Create initial population
  std::uniform_real_distribution<double> chronon_dist(0.0, max_chronons);
  std::uniform_real_distribution<double> aethel_dist(0.0, max_aethel);

  for (int i = 0; i < population_size; ++i) {
    std::vector<double> chromosome = {chronon_dist(m_rng), aethel_dist(m_rng)};
    population.push_back(chromosome);
  }

  // Evolutionary process
  for (int gen = 0; gen < generations; ++gen) {
    // Evaluate fitness
    std::vector<double> fitness_scores;
    for (const auto &chromosome : population) {
      fitness_scores.push_back(evaluateFitness(chromosome));
    }

    // Create new population
    std::vector<std::vector<double>> new_population;

    // Elitism: keep the best individual
    auto best_it =
        std::max_element(fitness_scores.begin(), fitness_scores.end());
    int best_index = std::distance(fitness_scores.begin(), best_it);
    new_population.push_back(population[best_index]);

    // Create the rest of the new population
    while (new_population.size() < static_cast<size_t>(population_size)) {
      // Selection (tournament selection)
      int idx1 =
          std::uniform_int_distribution<int>(0, population_size - 1)(m_rng);
      int idx2 =
          std::uniform_int_distribution<int>(0, population_size - 1)(m_rng);

      std::vector<double> parent1 =
          (fitness_scores[idx1] > fitness_scores[idx2]) ? population[idx1]
                                                        : population[idx2];

      idx1 = std::uniform_int_distribution<int>(0, population_size - 1)(m_rng);
      idx2 = std::uniform_int_distribution<int>(0, population_size - 1)(m_rng);

      std::vector<double> parent2 =
          (fitness_scores[idx1] > fitness_scores[idx2]) ? population[idx1]
                                                        : population[idx2];

      // Crossover
      std::vector<double> offspring = crossover(parent1, parent2);

      // Mutation
      mutate(offspring, 0.1); // 10% mutation rate

      new_population.push_back(offspring);
    }

    // Replace old population
    population = new_population;
  }

  // Return best solution
  std::vector<double> best_solution;
  double best_fitness = -1.0;

  for (const auto &chromosome : population) {
    double fitness = evaluateFitness(chromosome);
    if (fitness > best_fitness) {
      best_fitness = fitness;
      best_solution = chromosome;
    }
  }

  return best_solution;
}

// Genetic algorithm helper functions
double AdvancedOptimizationAlgorithms::evaluateFitness(
    const std::vector<double> &chromosome) {
  // In a real implementation, this would evaluate how well the allocation
  // performs based on historical data or simulations

  // For this simplified version, we'll use a heuristic
  if (chromosome.size() < 2) {
    return 0.0;
  }

  double chronon_allocation = chromosome[0];
  double aethel_allocation = chromosome[1];

  // Higher allocations are better, but with diminishing returns
  double chronon_fitness = std::sqrt(chronon_allocation);
  double aethel_fitness = std::sqrt(aethel_allocation);

  // But there's a penalty for over-allocation
  double chronon_penalty = chronon_allocation * 0.01;
  double aethel_penalty = aethel_allocation * 0.01;

  return (chronon_fitness + aethel_fitness) -
         (chronon_penalty + aethel_penalty);
}

std::vector<double>
AdvancedOptimizationAlgorithms::crossover(const std::vector<double> &parent1,
                                          const std::vector<double> &parent2) {
  // Simple arithmetic crossover
  std::vector<double> offspring;

  for (size_t i = 0; i < parent1.size() && i < parent2.size(); ++i) {
    double alpha = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
    offspring.push_back(alpha * parent1[i] + (1.0 - alpha) * parent2[i]);
  }

  return offspring;
}

void AdvancedOptimizationAlgorithms::mutate(std::vector<double> &chromosome,
                                            double mutation_rate) {
  for (auto &gene : chromosome) {
    if (std::uniform_real_distribution<double>(0.0, 1.0)(m_rng) <
        mutation_rate) {
      // Apply mutation: add or subtract a small random value
      double mutation_factor =
          std::uniform_real_distribution<double>(-0.2, 0.2)(m_rng);
      gene *= (1.0 + mutation_factor);

      // Ensure gene remains positive
      gene = std::max(0.0, gene);
    }
  }
}

} // namespace chronovyan