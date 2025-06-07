#ifndef CHRONOVYAN_RESOURCE_OPTIMIZER_H
#define CHRONOVYAN_RESOURCE_OPTIMIZER_H

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace chronovyan {

// Forward declarations
class ResourceTracker;
class TemporalRuntime;
class TemporalDebtTracker;

/**
 * @struct OptimizationRecommendation
 * @brief Represents a suggestion for resource optimization
 */
struct OptimizationRecommendation {
    enum class Type {
        RESOURCE_ALLOCATION,     // Adjust resource allocation
        EFFICIENCY_IMPROVEMENT,  // Improve efficiency
        DEBT_REDUCTION,          // Reduce temporal debt
        CACHING_OPPORTUNITY,     // Opportunity for caching
        SEQUENCE_REORDERING      // Reordering execution sequence
    };

    Type type;                     // Type of suggestion
    std::string description;       // Human-readable description
    double estimated_improvement;  // Estimated improvement percentage
    double confidence;             // Confidence level (0.0-1.0)

    OptimizationRecommendation(Type t, const std::string& desc, double improvement, double conf)
        : type(t), description(desc), estimated_improvement(improvement), confidence(conf) {}
};

/**
 * @class ResourceOptimizer
 * @brief Analyzes resource usage and suggests optimizations
 *
 * The ResourceOptimizer analyzes historical resource usage data and
 * suggests potential optimizations to improve efficiency and reduce
 * resource consumption.
 */
class ResourceOptimizer {
public:
    // Constants for default algorithm weights
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_CHRONONS_CACHING = 0.4;
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_CHRONONS_BATCHING = 0.3;
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_CHRONONS_PARADOX = 0.3;
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_AETHEL_RESONANCE = 0.4;
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_AETHEL_ALIGNMENT = 0.4;
    static constexpr double DEFAULT_ALGORITHM_WEIGHT_AETHEL_STABILIZATION = 0.2;

    // Constants for resource baseline calculations
    static constexpr double RESOURCE_BASELINE_SCALING_FACTOR = 1000.0;
    static constexpr double MIN_RESOURCE_BASELINE_THRESHOLD = 0.1;

    // Constants for overall efficiency calculation
    static constexpr double OVERALL_EFFICIENCY_CHRONONS_WEIGHT_FACTOR = 0.6;
    static constexpr double OVERALL_EFFICIENCY_AETHEL_WEIGHT_FACTOR = 0.4;
    static constexpr double DEFAULT_FALLBACK_OVERALL_EFFICIENCY = 0.75;  // Added for CD-2023-08-002

    // Constants for optimization logic thresholds
    static constexpr size_t ADAPTATION_TRIGGER_MIN_HISTORY_COUNT = 3;  // For optimizeChronons
    static constexpr int REPETITIVE_PATTERN_MILD_OCCURRENCE_THRESHOLD =
        5;  // For optimizeChrononsAdvanced
    static constexpr double SPIKE_DETECTION_AVERAGE_USAGE_MULTIPLIER =
        1.5;  // For optimizeChrononsAdvanced
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_REPETITIVE_PATTERN =
        0.85;  // For optimizeChrononsAdvanced
    static constexpr int REPETITIVE_PATTERN_AGGRESSIVE_OCCURRENCE_THRESHOLD =
        10;  // For optimizeChrononsAdvanced
    static constexpr double AGGRESSIVE_OPTIMIZATION_FACTOR_REPETITIVE_PATTERN =
        0.75;                                                        // For optimizeChrononsAdvanced
    static constexpr double PLACEHOLDER_FLUX_STABILITY_VALUE = 0.9;  // For optimizeChrononsAdvanced
    static constexpr double FLUX_STABILITY_OPTIMIZATION_ADJUSTMENT_FACTOR =
        0.2;  // For optimizeChrononsAdvanced

    // --- Additions for Phase 2 (Lines 250-499) ---

    // For optimizeChrononsAdvanced (Spike pattern)
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_CHRONONS_SPIKE = 0.80;
    static constexpr const char* SPIKE_OPERATION_HISTORY_PREFIX = "spike_";
    static constexpr double PLACEHOLDER_SPIKE_SIMILARITY_MEASURE = 0.8;  // Placeholder
    static constexpr double BLENDED_OPTIMIZATION_FACTOR_DIVISOR = 2.0;

    // For optimizeChrononsAdvanced (Steady pattern)
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_CHRONONS_STEADY = 0.90;
    static constexpr double HIGH_STABILITY_THRESHOLD_CHRONONS_STEADY = 0.8;
    static constexpr double AGGRESSIVE_OPTIMIZATION_FACTOR_CHRONONS_STEADY_HIGH_STABILITY = 0.85;

    // Common Debt Awareness Thresholds & Factors (potentially reused)
    static constexpr double LOW_SYSTEM_STABILITY_THRESHOLD = 0.7;
    static constexpr double HIGH_DEBT_RATIO_THRESHOLD = 0.3;
    static constexpr double DEBT_FACTOR_CALCULATION_DIVISOR = 0.7;  // For normalizing debt factor
    static constexpr double CRITICAL_PAST_DUE_DEBT_CONSERVATIVE_FACTOR_CHRONONS = 0.95;

    // For optimizeAethelAdvanced
    static constexpr double DEFAULT_OPTIMIZATION_FACTOR_AETHEL_MILD_ENHANCEMENT = 1.1;
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_AETHEL_REPETITIVE = 1.15;
    static constexpr double AGGRESSIVE_OPTIMIZATION_FACTOR_AETHEL_REPETITIVE =
        1.25;  // Corrected for Aethel
    static constexpr double AETHEL_EFFICIENCY_ADJUSTMENT_FACTOR_REPETITIVE = 0.2;
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_AETHEL_SPIKE = 1.35;
    static constexpr double MIN_OPTIMIZATION_FACTOR_AETHEL_SPIKE_BLEND = 1.31;
    static constexpr double INITIAL_OPTIMIZATION_FACTOR_AETHEL_STEADY =
        1.1;  // Same as default mild enhancement
    static constexpr double HIGH_EFFICIENCY_THRESHOLD_AETHEL_STEADY = 0.8;
    static constexpr double AGGRESSIVE_OPTIMIZATION_FACTOR_AETHEL_STEADY_HIGH_EFFICIENCY =
        1.15;  // Same as initial repetitive Aethel

    // --- Additions for Phase 3 (Lines 500-749) ---

    // Common numeric literals
    static constexpr double ZERO_POINT_ZERO = 0.0;
    static constexpr double ONE_POINT_ZERO = 1.0;
    static constexpr int ZERO_AS_INT = 0;
    static constexpr int ONE_AS_INT = 1;
    static constexpr size_t ZERO_AS_SIZE_T = 0;
    static constexpr size_t ONE_AS_SIZE_T = 1;

    // For optimizeAethelAdvanced (Debt Impact)
    static constexpr double HIGH_DEBT_RATIO_THRESHOLD_AETHEL_IMPACT = 0.5;
    static constexpr double DEBT_FACTOR_NORMALIZATION_AETHEL =
        0.5;  // Divisor for (debtRatio - THRESHOLD)
    static constexpr double DEBT_IMPACT_FACTOR_AETHEL =
        0.5;  // Multiplier for debtFactor's effect on optimization
    static constexpr double CRITICAL_PAST_DUE_DEBT_OPTIMIZATION_FACTOR_AETHEL =
        1.05;  // Nearly halts optimization

    // For predictResourceNeeds
    static constexpr size_t MIN_HISTORY_FOR_PREDICTION = 2;
    static constexpr size_t PREDICTION_HISTORY_OFFSET =
        1;  // For calculating rate of change m_opHistory.size() - 1
    static constexpr double AETHEL_EFFICIENCY_PREDICTION_MULTIPLIER = 2.0;
    static constexpr int MINIMUM_PREDICTED_RESOURCE_VALUE = 0;  // Used with std::max(0, ...)

    // For calculateChrononsEfficiency
    static constexpr double CHRONON_EFFICIENCY_CONSUMPTION_NORMALIZATION_FACTOR = 100.0;

    // For calculateAethelEfficiency
    // Note: AETHEL_EFFICIENCY_LEVEL_NORMALIZATION_FACTOR uses existing
    // RESOURCE_BASELINE_SCALING_FACTOR (1000.0)
    static constexpr size_t AETHEL_EFFICIENCY_RECENT_OPERATIONS_COUNT = 10;
    static constexpr double AETHEL_TO_CHRONON_RATIO_EFFICIENCY_DIVISOR = 5.0;
    static constexpr size_t MIN_OPERATIONS_FOR_VARIANCE_EXCLUSIVE =
        2;  // for condition: m_operationHistory.size() > 2
    static constexpr size_t VARIANCE_CALCULATION_DENOMINATOR_OFFSET =
        1;  // for: m_operationHistory.size() - startIdx - 1
    static constexpr double AETHEL_VARIANCE_EFFICIENCY_NORMALIZATION_FACTOR = 100.0;

    // Weights for combined Aethel efficiency factors in calculateAethelEfficiency
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_BASE = 0.25;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_RATIO = 0.20;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_STABILITY = 0.15;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_VARIANCE = 0.10;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_COHERENCE = 0.10;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_RESONANCE = 0.10;
    static constexpr double AETHEL_EFFICIENCY_WEIGHT_ALIGNMENT = 0.10;

    // For applyOptimizationAlgorithms (Random Distribution)
    // UNIFORM_DISTRIBUTION_MIN uses ZERO_POINT_ZERO
    // UNIFORM_DISTRIBUTION_MAX uses ONE_POINT_ZERO

    // For applyOptimizationAlgorithms (Chronons)
    static constexpr double CHRONON_CACHING_FACTOR_BASE = 0.95;
    static constexpr double CHRONON_ALGORITHM_RANDOM_FACTOR_RANGE =
        0.1;  // Shared by chronon algorithms' random part
    static constexpr double CHRONON_BATCHING_FACTOR_BASE = 0.90;
    static constexpr double CHRONON_PARADOX_FACTOR_BASE = 0.95;  // Same as caching base

    // For applyOptimizationAlgorithms (Aethel)
    static constexpr double AETHEL_RESONANCE_FACTOR_BASE = 1.1;
    static constexpr double AETHEL_ALGORITHM_RANDOM_FACTOR_RANGE =
        0.1;  // Shared by aethel algorithms' random part
    static constexpr double AETHEL_ALIGNMENT_FACTOR_BASE = 1.05;
    static constexpr double AETHEL_STABILIZATION_FACTOR_BASE = 1.1;  // Same as resonance base

    /**
     * @brief Construct a new Resource Optimizer with a reference to ResourceTracker
     * @param tracker The resource tracker to analyze
     */
    explicit ResourceOptimizer(const ResourceTracker& tracker);

    /**
     * @brief Construct a new Resource Optimizer
     * @param resourceTracker The resource tracker to analyze
     * @param runtime The temporal runtime to analyze
     */
    ResourceOptimizer(std::shared_ptr<ResourceTracker> resourceTracker,
                      std::shared_ptr<TemporalRuntime> runtime);

    // Additional constructor for use with TemporalDebtTracker
    ResourceOptimizer(std::shared_ptr<TemporalRuntime> runtime,
                      std::shared_ptr<TemporalDebtTracker> debt_tracker);

    // Simple constructor for tests and examples
    explicit ResourceOptimizer(std::shared_ptr<TemporalRuntime> runtime);

    /**
     * @brief Analyze resource usage and generate optimization suggestions
     * @return Vector of optimization suggestions
     */
    std::vector<OptimizationRecommendation> generateRecommendations() const;

    /**
     * @brief Calculate the current overall efficiency rating
     * @return A value representing the overall efficiency
     */
    double getOverallEfficiency() const;

    /**
     * @brief Get the last optimization suggestion provided
     * @return A string describing the last suggestion, or empty if none.
     */
    std::string getLastOptimizationSuggestion() const;

    /**
     * @brief Calculate the current efficiency rating
     * @return A value between 0.0 (inefficient) and 1.0 (optimal)
     */
    double calculateEfficiencyRating() const;

    /**
     * @brief Identify resource usage patterns
     * @return Map of pattern names to occurrence counts
     */
    std::map<std::string, int> identifyUsagePatterns() const;

    /**
     * @brief Get a report of potential resource savings
     * @return Human-readable report of potential savings
     */
    std::string generateOptimizationReport() const;

    // Additional methods from the implementation file
    double optimizeChronons(const std::string& operationId);
    double optimizeChrononsAdvanced(const std::string& operationId,
                                    const std::string& operationPattern);
    double optimizeChrononsWithPatternAnalysis(const std::string& operationId, size_t historyDepth);
    double optimizeAethel(const std::string& timelineId);
    double optimizeAethelAdvanced(const std::string& timelineId,
                                  const std::string& operationPattern);
    std::map<std::string, double> analyzeResourceUsage();
    std::vector<std::pair<int, int>> predictResourceNeeds(int steps);
    std::map<std::string, double> getOptimizationHistory(const std::string& resourceType) const;
    bool adaptOptimizationParameters();
    std::string getCurrentOptimizationStrategy() const;
    void setTemporalDebtTracker(std::shared_ptr<TemporalDebtTracker> debt_tracker);
    double getEfficiencyMetric(const std::string& metricName);
    std::map<std::string, double> getDetailedAethelEfficiencyMetrics() const;
    double calculateAethelGenerationRate(size_t timeWindow) const;
    double calculateAethelToChrononRatio(size_t timeWindow) const;
    double calculateAethelStability() const;

    /**
     * @brief Get various efficiency metrics.
     * @return A map of metric names to their values.
     */
    std::map<std::string, double> getEfficiencyMetrics() const;

    /**
     * @brief Sets the minimum number of data points required for analysis.
     * @param points The minimum number of data points.
     */
    void setMinimumDataPoints(unsigned __int64 points);

private:
    std::shared_ptr<ResourceTracker> m_resourceTracker;
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<TemporalDebtTracker> m_debt_tracker;
    unsigned __int64 m_minimumDataPoints;

    // Private data members from the implementation
    int m_adaptationCycles;
    std::map<std::string, double> m_algorithmWeights;
    std::map<std::string, std::map<std::string, double>> m_optimizationHistory;
    std::map<std::string, double> m_efficiencyMetrics;
    std::vector<std::pair<std::string, int>> m_operationHistory;
    std::vector<OptimizationRecommendation> m_last_recommendations;
    std::vector<std::string> m_optimization_log;
    std::map<std::string, double> m_operationEfficiency;

    // Adding missing member variable for resource usage history
    std::vector<std::map<std::string, std::string>> m_resourceUsageHistory;

    /**
     * @brief Detect resource usage patterns from historical data
     * @return Map of patterns to their confidence levels
     */
    std::map<std::string, double> detectPatterns() const;

    /**
     * @brief Evaluate the impact of potential optimizations
     * @param patterns The detected patterns
     * @return Vector of evaluated optimizations
     */
    std::vector<OptimizationRecommendation> evaluateOptimizations(
        const std::map<std::string, double>& patterns) const;

    // Private methods from the implementation
    double calculateChrononsEfficiency();
    double calculateAethelEfficiency();
    double applyOptimizationAlgorithms(const std::string& resourceType);
    double applyAdaptiveOptimization(const std::string& resourceType,
                                     const std::string& operationId);
    void trackOptimizationResult(const std::string& resourceType, const std::string& operationId,
                                 double factor);
    double applyMachineLearning(const std::string& resourceType);
    std::map<std::string, double> detectOperationPatterns(const std::string& operationId,
                                                          size_t historyDepth);
    double calculateAethelCoherence() const;
    double calculateAethelResonance() const;
    double calculateAethelFluxAlignment() const;

    // Method to track resource usage for pattern detection
    void trackResourceUsage(const std::string& operation, const std::string& context);

    // --- Additions for Phase 4 (Lines 750-999) ---

    // For applyAdaptiveOptimization
    static constexpr double ADAPTIVE_FACTOR_SENSITIVITY = 0.2;

    // For trackOptimizationResult
    static constexpr double EFFICIENCY_TRACKING_CHRONON_FACTOR_DIVISOR = 2.0;
    static constexpr double EFFICIENCY_TRACKING_AETHEL_FACTOR_MULTIPLIER = 2.0;
    static constexpr int ADAPT_PARAMS_TRIGGER_INTERVAL = 3;

    // For applyMachineLearning
    static constexpr double ML_CHRONON_LOWER_BOUND_FACTOR = 0.5;
    static constexpr double ML_CHRONON_ADJUSTMENT_FACTOR = 0.95;
    static constexpr double ML_AETHEL_UPPER_BOUND_FACTOR = 1.5;
    static constexpr double ML_AETHEL_ADJUSTMENT_FACTOR = 1.05;

    // For adaptOptimizationParameters (Thresholds & Limits)
    static constexpr double CHRONON_WEIGHT_ADAPT_AGGRESSIVE_AVG_FACTOR_THRESHOLD = 0.7;
    static constexpr double CHRONON_WEIGHT_ADAPT_CONSERVATIVE_AVG_FACTOR_THRESHOLD = 0.9;
    static constexpr double AETHEL_WEIGHT_ADAPT_CONSERVATIVE_AVG_FACTOR_THRESHOLD = 1.1;
    static constexpr double AETHEL_WEIGHT_ADAPT_AGGRESSIVE_AVG_FACTOR_THRESHOLD = 1.3;

    static constexpr double CHRONON_CACHING_WEIGHT_MAX = 0.6;
    static constexpr double AETHEL_RESONANCE_WEIGHT_MAX = 0.6;

    static constexpr double ALGORITHM_WEIGHT_MIN_GENERAL = 0.2;  // For most algorithm weights
    static constexpr double AETHEL_STABILIZATION_WEIGHT_MIN = 0.1;
    static constexpr double AETHEL_RESONANCE_WEIGHT_MIN = 0.3;

    // For adaptOptimizationParameters (Adjustment Steps)
    static constexpr double WEIGHT_ADJUSTMENT_STEP_PRIMARY = 0.05;  // For directly targeted weights
    static constexpr double WEIGHT_ADJUSTMENT_STEP_SECONDARY =
        0.025;  // For compensatory adjustments

    // For detectOperationPatterns (Steady Pattern)
    static constexpr double PATTERN_STEADY_DEFAULT_CONFIDENCE_LOW_DATA = 0.5;
    static constexpr double PATTERN_STEADY_VAR_COEFF_THRESHOLD_1 = 0.08;  // High confidence
    static constexpr double PATTERN_STEADY_CONFIDENCE_1 = 0.95;
    static constexpr double PATTERN_STEADY_VAR_COEFF_THRESHOLD_2 = 0.15;  // Good confidence
    static constexpr double PATTERN_STEADY_CONFIDENCE_2 = 0.8;
    static constexpr double PATTERN_STEADY_VAR_COEFF_THRESHOLD_3 = 0.25;  // Medium confidence
    static constexpr double PATTERN_STEADY_CONFIDENCE_3 = 0.6;
    static constexpr double PATTERN_STEADY_VAR_COEFF_THRESHOLD_4 = 0.4;  // Low confidence
    static constexpr double PATTERN_STEADY_CONFIDENCE_4 = 0.3;
    static constexpr double PATTERN_STEADY_CONFIDENCE_5 = 0.1;  // Very low confidence (else case)

    // --- Additions for Phase 5 (Lines 1000-1249) ---

    // For detectOperationPatterns (Spike Detection)
    static constexpr double Z_SCORE_OUTLIER_THRESHOLD = 2.0;
    static constexpr int MIN_OUTLIER_COUNT_FOR_SPIKE_BIAS_DIVISOR = 1;  // Prevents division by zero
    static constexpr double SPIKE_CONFIDENCE_OUTLIER_RATIO_MULTIPLIER = 3.0;

    // For detectOperationPatterns (Cyclic Detection)
    static constexpr double CYCLIC_EXPECTED_SIGN_CHANGES_DIVISOR = 2.0;
    static constexpr double CYCLIC_CONSECUTIVE_PENALTY_FACTOR = 0.5;

    // For detectOperationPatterns (Trend Detection)
    static constexpr double LINEAR_REGRESSION_DENOMINATOR_EPSILON = 1e-10;
    static constexpr double NORMALIZED_SLOPE_MAX_ABS_VALUE = 1.0;  // For clamping slope to [-1, 1]
    static constexpr double TREND_PATTERN_SLOPE_BOOST_FACTOR =
        1.0;  // Added to abs(normalizedSlope)

    // For optimizeChrononsWithPatternAnalysis (General Pattern Logic)
    static constexpr double PATTERN_SIGNIFICANCE_THRESHOLD = 0.6;
    static constexpr double DOMINANT_PATTERN_HIGH_CONFIDENCE_THRESHOLD = 0.75;

    // For optimizeChrononsWithPatternAnalysis (Pattern Weights/Coefficients)
    static constexpr double PATTERN_WEIGHT_REPETITIVE = 0.25;
    static constexpr double PATTERN_WEIGHT_SPIKE = 0.20;
    static constexpr double PATTERN_WEIGHT_CYCLIC = 0.18;
    static constexpr double PATTERN_WEIGHT_INCREASING = 0.15;
    static constexpr double PATTERN_WEIGHT_DECREASING = 0.22;
    static constexpr double PATTERN_WEIGHT_STEADY = 0.12;

    // For optimizeChrononsWithPatternAnalysis (Repetitive Pattern Specialization)
    static constexpr double REPETITIVE_CACHE_EFFICIENCY_MAX = 0.95;
    static constexpr double REPETITIVE_CACHE_EFFICIENCY_BASE = 0.8;
    static constexpr double REPETITIVE_CACHE_EFFICIENCY_CONFIDENCE_OFFSET =
        0.75;  // highestConfidence - this
    static constexpr double REPETITIVE_CACHE_EFFICIENCY_CONFIDENCE_FACTOR = 0.6;
    static constexpr double REPETITIVE_EXTRA_OPTIMIZATION_FACTOR = 0.5;  // of remaining margin

    // For optimizeChrononsWithPatternAnalysis (Spike Pattern Specialization)
    static constexpr double SPIKE_EFFECTIVENESS_BASE = 0.7;
    static constexpr double SPIKE_EFFECTIVENESS_CONFIDENCE_OFFSET =
        0.75;  // highestConfidence - this
    static constexpr double SPIKE_EFFECTIVENESS_CONFIDENCE_FACTOR = 0.6;
    static constexpr double SPIKE_DEBT_PENALTY_FACTOR = 0.8;  // Multiplied when debt > 0

    // For optimizeChrononsWithPatternAnalysis (Cyclic Pattern Specialization)
    static constexpr double CYCLIC_CLARITY_CONFIDENCE_WEIGHT = 0.8;
    static constexpr double CYCLIC_CLARITY_BASE_ADDITION = 0.2;
    static constexpr double CYCLIC_OPTIMIZATION_FACTOR_VS_CLARITY = 0.15;

    // --- Additions for Phase 6 (Lines 1250-End) ---

    // For optimizeChrononsWithPatternAnalysis (Increasing/Decreasing, Historical, Debt, Bounds,
    // Perturbation)
    static constexpr double OCPA_INCREASING_ADJUSTMENT_FACTOR = 0.05;
    static constexpr double OCPA_DECREASING_RECOVERY_BOOST_MULTIPLIER = 0.1;
    static constexpr double OCPA_LOWER_BOUND_DECREASING = 0.6;
    static constexpr double OCPA_HISTORICAL_BLEND_CURRENT_WEIGHT = 0.7;
    static constexpr double OCPA_HISTORICAL_BLEND_HISTORICAL_WEIGHT = 0.3;
    static constexpr double OCPA_DEBT_RATIO_ADJUST_THRESHOLD = 0.3;
    static constexpr double OCPA_DEBT_RATIO_NORMALIZATION_DIVISOR = 0.7;
    static constexpr double OCPA_CRITICAL_DEBT_ADJUST_FACTOR = 1.5;
    static constexpr double OCPA_FINAL_LOWER_BOUND = 0.6;
    // OCPA_FINAL_UPPER_BOUND uses ONE_POINT_ZERO
    static constexpr double OCPA_PERTURBATION_MIN = -0.02;
    static constexpr double OCPA_PERTURBATION_MAX = 0.02;
    static constexpr int OCPA_ADAPT_PARAMS_INTERVAL = 5;

    // For getDetailedAethelEfficiencyMetrics (Normalization)
    // GENERATION_RATE_NORMALIZATION_DIVISOR reuses RESOURCE_BASELINE_SCALING_FACTOR

    // Operation Prefixes for Aethel/Chronon tracking
    static constexpr const char* OP_PREFIX_OPTIMIZATION_ADVANCED = "optimization_advanced_";
    static constexpr const char* OP_PREFIX_AETHEL = "aethel_";
    static constexpr const char* OP_PREFIX_OPTIMIZATION_GENERAL = "optimization_";
    static constexpr const char* OP_PREFIX_CHRONON = "chronon_";

    // For calculateAethelStability
    // MIN_HISTORY_COUNT reuses ADAPTATION_TRIGGER_MIN_HISTORY_COUNT
    // DEFAULT_VALUE reuses ONE_POINT_ZERO
    static constexpr double AETHEL_STABILITY_VARIANCE_POWER = 2.0;
    // CV_TO_STABILITY_NUMERATOR & CV_TO_STABILITY_DENOMINATOR_ADD reuses ONE_POINT_ZERO

    // For calculateAethelCoherence
    static constexpr size_t AETHEL_COHERENCE_MIN_HISTORY = 4;
    // DEFAULT_VALUE reuses ONE_POINT_ZERO
    // AUTOCORR_TO_COHERENCE_ADD reuses ONE_POINT_ZERO
    static constexpr double AUTOCORR_TO_COHERENCE_DIVISOR = 2.0;

    // For calculateAethelResonance
    static constexpr double AETHEL_RESONANCE_DEFAULT_NO_DEBT_TRACKER = 0.7;
    static constexpr double AETHEL_RESONANCE_LOW_STABILITY_THRESHOLD = 0.5;
    static constexpr double AETHEL_RESONANCE_LOW_STABILITY_FACTOR = 0.8;
    static constexpr double AETHEL_RESONANCE_HIGH_STABILITY_BASE = 0.4;
    static constexpr double AETHEL_RESONANCE_HIGH_STABILITY_FACTOR = 0.6;

    // For calculateAethelFluxAlignment
    static constexpr size_t FLUX_ALIGNMENT_MIN_HISTORY = 5;
    static constexpr double FLUX_ALIGNMENT_DEFAULT_VALUE = 0.5;
    static constexpr int FLUX_ALIGNMENT_OP_TYPE_CHRONON = 1;
    static constexpr int FLUX_ALIGNMENT_OP_TYPE_AETHEL = -1;
    static constexpr int FLUX_ALIGNMENT_OP_TYPE_NEUTRAL = 0;
    // INITIAL_MAX_OP_VALUE reuses ONE_AS_INT
    // NEUTRAL_OP_NORMALIZED_MAGNITUDE reuses ZERO_POINT_ZERO
    static constexpr double FLUX_ALIGNMENT_BALANCE_TARGET_RATIO = 0.5;
    static constexpr double FLUX_ALIGNMENT_BALANCE_FACTOR_MULTIPLIER = 2.0;
    // SEQUENCE_MAX_ALT_OFFSET reuses ONE_POINT_ZERO
    static constexpr double FLUX_ALIGNMENT_MAGNITUDE_CORR_STDEV_MULTIPLIER = 2.0;
    static constexpr double FLUX_ALIGNMENT_DEFAULT_STABILITY_NO_DEBT_TRACKER = 0.7;
    static constexpr double FLUX_ALIGNMENT_RECENT_TREND_HISTORY_PERCENTAGE = 0.7;
    static constexpr double FLUX_ALIGNMENT_RECENT_TREND_DEFAULT_FACTOR = 0.5;
    // RECENT_TREND_MIN_OPS reuses FLUX_ALIGNMENT_MIN_HISTORY
    static constexpr double FLUX_ALIGNMENT_RECENT_TREND_OPTIMAL_AETHEL_RATIO = 0.6;
    static constexpr double FLUX_ALIGNMENT_RECENT_TREND_AETHEL_RATIO_ADJUST_MULTIPLIER = 1.25;
    static constexpr double FLUX_ALIGNMENT_WEIGHT_BALANCE = 0.25;
    static constexpr double FLUX_ALIGNMENT_WEIGHT_SEQUENCE = 0.25;
    static constexpr double FLUX_ALIGNMENT_WEIGHT_MAGNITUDE_CORRELATION = 0.20;
    static constexpr double FLUX_ALIGNMENT_WEIGHT_STABILITY = 0.15;
    static constexpr double FLUX_ALIGNMENT_WEIGHT_RECENT_TREND = 0.15;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_OPTIMIZER_H