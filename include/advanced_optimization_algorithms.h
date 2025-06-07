#ifndef CHRONOVYAN_ADVANCED_OPTIMIZATION_ALGORITHMS_H
#define CHRONOVYAN_ADVANCED_OPTIMIZATION_ALGORITHMS_H

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "automatic_resource_optimizer.h"

namespace chronovyan {

/**
 * @enum OptimizationStrategy
 * @brief Different strategies for optimization
 */
enum class OptimizationStrategy {
    CONSERVATIVE,  // Prioritizes stability over efficiency
    BALANCED,      // Balances efficiency and stability
    AGGRESSIVE,    // Prioritizes efficiency over stability
    ADAPTIVE,      // Dynamically adjusts based on system state
    PREDICTIVE,    // Uses predictive modeling
    EXPERIMENTAL   // Uses experimental techniques (may be unstable)
};

/**
 * @enum PatternMatchingAlgorithm
 * @brief Algorithms used for detecting patterns in resource usage
 */
enum class PatternMatchingAlgorithm {
    BASIC_STATISTICAL,    // Simple statistical analysis
    SLIDING_WINDOW,       // Analysis using sliding windows
    FOURIER_TRANSFORM,    // Frequency domain analysis
    REGRESSION_ANALYSIS,  // Linear/nonlinear regression
    CLUSTERING,           // Cluster-based pattern detection
    NEURAL_NETWORK        // Neural network-based pattern recognition
};

/**
 * @struct OptimizationProfile
 * @brief Represents a pre-defined optimization profile
 */
struct OptimizationProfile {
    std::string name;
    OptimizationStrategy strategy;
    std::map<std::string, double> algorithm_weights;
    double risk_tolerance;
    double efficiency_target;

    OptimizationProfile(const std::string& profile_name, OptimizationStrategy strategy_type)
        : name(profile_name)
        , strategy(strategy_type)
        , risk_tolerance(0.5)
        , efficiency_target(0.8) {}
};

/**
 * @struct PatternRecognitionResult
 * @brief Result of pattern recognition analysis
 */
struct PatternRecognitionResult {
    std::string primary_pattern;
    double confidence;
    std::map<std::string, double> pattern_probabilities;
    std::vector<double> trend_coefficients;
    bool is_seasonal;
    int seasonality_period;

    PatternRecognitionResult()
        : primary_pattern("unknown"), confidence(0.0), is_seasonal(false), seasonality_period(0) {}
};

/**
 * @class AdvancedOptimizationAlgorithms
 * @brief Provides advanced optimization algorithms and strategies
 *
 * This class extends the AutomaticResourceOptimizer with more sophisticated
 * optimization strategies including machine learning approaches, advanced
 * pattern recognition, and predictive modeling.
 */
class AdvancedOptimizationAlgorithms : public AutomaticResourceOptimizer {
public:
    /**
     * @brief Constructor
     * @param runtime The temporal runtime
     * @param debt_tracker Optional debt tracker
     * @param config Configuration for automatic optimization
     * @param strategy The optimization strategy to use
     */
    AdvancedOptimizationAlgorithms(std::shared_ptr<TemporalRuntime> runtime,
                                   std::shared_ptr<TemporalDebtTracker> debt_tracker = nullptr,
                                   const OptimizationConfig& config = OptimizationConfig(),
                                   OptimizationStrategy strategy = OptimizationStrategy::BALANCED);

    /**
     * @brief Destructor
     */
    virtual ~AdvancedOptimizationAlgorithms();

    /**
     * @brief Set the optimization strategy
     * @param strategy The strategy to use
     */
    void setOptimizationStrategy(OptimizationStrategy strategy);

    /**
     * @brief Get the current optimization strategy
     * @return The current strategy
     */
    OptimizationStrategy getOptimizationStrategy() const;

    /**
     * @brief Set the pattern matching algorithm
     * @param algorithm The algorithm to use
     */
    void setPatternMatchingAlgorithm(PatternMatchingAlgorithm algorithm);

    /**
     * @brief Get the current pattern matching algorithm
     * @return The current algorithm
     */
    PatternMatchingAlgorithm getPatternMatchingAlgorithm() const;

    /**
     * @brief Load a predefined optimization profile
     * @param profile_name The name of the profile to load
     * @return True if the profile was loaded successfully
     */
    bool loadOptimizationProfile(const std::string& profile_name);

    /**
     * @brief Create a custom optimization profile
     * @param profile The profile to create
     * @return True if the profile was created successfully
     */
    bool createOptimizationProfile(const OptimizationProfile& profile);

    /**
     * @brief Get the available optimization profiles
     * @return Map of profile names to profiles
     */
    std::map<std::string, OptimizationProfile> getAvailableProfiles() const;

    /**
     * @brief Apply advanced pattern recognition to detect resource usage patterns
     * @param operation_id The operation to analyze
     * @param algorithm The pattern matching algorithm to use
     * @return Result of the pattern recognition
     */
    PatternRecognitionResult detectPatternAdvanced(
        const std::string& operation_id,
        PatternMatchingAlgorithm algorithm = PatternMatchingAlgorithm::BASIC_STATISTICAL);

    /**
     * @brief Perform predictive optimization based on historical data
     * @param operation_id The operation to optimize
     * @param prediction_horizon How far ahead to predict (in cycles)
     * @return The optimization factor achieved
     */
    double optimizePredictive(const std::string& operation_id, int prediction_horizon = 5);

    /**
     * @brief Optimize based on neural network predictions
     * @param operation_id The operation to optimize
     * @return The optimization factor achieved
     */
    double optimizeNeuralNetwork(const std::string& operation_id);

    /**
     * @brief Optimize resources using genetic algorithms
     * @param population_size Size of the genetic algorithm population
     * @param generations Number of generations to evolve
     * @return The optimization factor achieved
     */
    double optimizeGenetic(int population_size = 50, int generations = 10);

    /**
     * @brief Perform a comprehensive optimization using all available algorithms
     * @param operation_id The operation to optimize
     * @return The best optimization factor achieved
     */
    double optimizeComprehensive(const std::string& operation_id);

    /**
     * @brief Get detailed insights about the optimization process
     * @param operation_id The operation to analyze
     * @return Map of insight names to their values
     */
    std::map<std::string, double> getOptimizationInsights(const std::string& operation_id) const;

    /**
     * @brief Generate a visualization of optimization opportunities
     * @param format The visualization format to use
     * @return String containing the visualization
     */
    std::string visualizeOptimizationOpportunities(
        VisualizationFormat format = VisualizationFormat::TEXT) const;

    /**
     * @brief Generate a report of optimization efficiency
     * @param detailed Whether to include detailed information
     * @return String containing the report
     */
    std::string generateEfficiencyReport(bool detailed = false) const;

    /**
     * @brief Compare different optimization strategies
     * @param operation_id The operation to analyze
     * @param strategies List of strategies to compare
     * @return Map of strategies to their optimization factors
     */
    std::map<OptimizationStrategy, double> compareOptimizationStrategies(
        const std::string& operation_id, const std::vector<OptimizationStrategy>& strategies);

protected:
    /**
     * @brief Override from AutomaticResourceOptimizer to use advanced pattern detection
     * @param operation_id The operation ID to detect patterns for
     * @return The detected pattern
     */
    virtual std::string detectOperationPattern(const std::string& operation_id) const override;

    /**
     * @brief Override from AutomaticResourceOptimizer to use advanced learning
     * @param result The optimization result to learn from
     */
    virtual void learnFromOptimizationResult(const OptimizationResult& result) override;

    /**
     * @brief Apply genetic algorithm optimization for resource allocation
     * @param population_size Size of the genetic algorithm population
     * @param generations Number of generations to evolve
     * @param operation_id The operation to optimize
     * @return The best allocation strategy found
     */
    std::vector<double> geneticOptimization(int population_size, int generations,
                                            const std::string& operation_id);

    /**
     * @brief Apply neural network optimization
     * @param operation_id The operation to optimize
     * @return The optimized allocation strategy
     */
    std::vector<double> neuralNetworkOptimization(const std::string& operation_id);

    /**
     * @brief Apply Fourier transform pattern detection
     * @param data The data to analyze
     * @return Map of frequencies to amplitudes
     */
    std::map<double, double> fourierTransformAnalysis(const std::vector<double>& data);

    /**
     * @brief Apply clustering-based pattern detection
     * @param data The data to analyze
     * @param clusters Number of clusters to use
     * @return Vector of cluster centers
     */
    std::vector<double> clusteringAnalysis(const std::vector<double>& data, int clusters);

    /**
     * @brief Apply regression analysis for pattern detection
     * @param data The data to analyze
     * @param degree Polynomial degree
     * @return Vector of regression coefficients
     */
    std::vector<double> regressionAnalysis(const std::vector<double>& data, int degree);

    /**
     * @brief Apply window-based pattern detection
     * @param data The data to analyze
     * @param window_size Size of the sliding window
     * @return Map of pattern types to probabilities
     */
    std::map<std::string, double> slidingWindowAnalysis(const std::vector<double>& data,
                                                        int window_size);

private:
    OptimizationStrategy m_strategy;
    PatternMatchingAlgorithm m_patternAlgorithm;
    std::map<std::string, OptimizationProfile> m_profiles;
    std::mt19937 m_rng;

    // Genetic algorithm helper functions
    double evaluateFitness(const std::vector<double>& chromosome);
    std::vector<double> crossover(const std::vector<double>& parent1,
                                  const std::vector<double>& parent2);
    void mutate(std::vector<double>& chromosome, double mutation_rate);

    // Neural network helper functions
    class SimpleNeuralNetwork {
    public:
        SimpleNeuralNetwork(const std::vector<int>& layers);
        std::vector<double> predict(const std::vector<double>& input);
        void train(const std::vector<std::vector<double>>& inputs,
                   const std::vector<std::vector<double>>& targets, int epochs,
                   double learning_rate);

    private:
        std::vector<int> m_layers;
        std::vector<std::vector<std::vector<double>>> m_weights;
        std::vector<std::vector<double>> m_biases;
    };

    std::map<std::string, std::shared_ptr<SimpleNeuralNetwork>> m_networks;

    // Initialize standard optimization profiles
    void initializeDefaultProfiles();

    // Helper functions for pattern detection
    bool isPeriodicPattern(const std::vector<double>& data, int& period);
    bool isTrendingPattern(const std::vector<double>& data, double& slope);
    double calculatePatternConfidence(const std::map<std::string, double>& pattern_probabilities);
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_ADVANCED_OPTIMIZATION_ALGORITHMS_H