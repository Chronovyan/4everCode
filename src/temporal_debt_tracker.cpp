#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>

#include "../include/temporal_debt_tracker.h"

namespace chronovyan {

// Constructor
TemporalDebtTracker::TemporalDebtTracker(std::shared_ptr<TemporalRuntime> runtime)
    : m_runtime(runtime)
    , m_current_cycle(0)
    , m_debt_limit(1000.0)
    ,  // Default debt limit
    m_low_alert_threshold(0.3)
    , m_medium_alert_threshold(0.5)
    , m_high_alert_threshold(0.7)
    , m_critical_alert_threshold(0.9) {
    // Initialize strategies
    initializeStrategies();
}

// Initialize strategy implementations
void TemporalDebtTracker::initializeStrategies() {
    m_strategies[RepaymentStrategyType::OLDEST_FIRST] = std::make_shared<OldestFirstStrategy>();
    m_strategies[RepaymentStrategyType::HIGHEST_INTEREST] =
        std::make_shared<HighestInterestStrategy>();
    m_strategies[RepaymentStrategyType::CRITICAL_FIRST] = std::make_shared<CriticalFirstStrategy>();
    m_strategies[RepaymentStrategyType::BALANCED] = std::make_shared<BalancedStrategy>();
    m_strategies[RepaymentStrategyType::MINIMUM_PAYMENTS] =
        std::make_shared<MinimumPaymentsStrategy>();
    m_strategies[RepaymentStrategyType::SNOWBALL] = std::make_shared<SnowballStrategy>();
    m_strategies[RepaymentStrategyType::AVALANCHE] = std::make_shared<AvalancheStrategy>();
    m_strategies[RepaymentStrategyType::STABILITY_OPTIMIZED] =
        std::make_shared<StabilityOptimizedStrategy>();
    m_strategies[RepaymentStrategyType::CHRONON_PRIORITY] =
        std::make_shared<ChronosPriorityStrategy>();
    m_strategies[RepaymentStrategyType::AETHEL_PRIORITY] =
        std::make_shared<AethelPriorityStrategy>();
}

// Get a strategy implementation by type
std::shared_ptr<RepaymentStrategy> TemporalDebtTracker::getStrategy(
    RepaymentStrategyType strategy_type) const {
    auto it = m_strategies.find(strategy_type);
    if (it != m_strategies.end()) {
        return it->second;
    }

    // Default to balanced strategy if requested type isn't found
    return m_strategies.at(RepaymentStrategyType::BALANCED);
}

// Apply a repayment strategy to the debts
RepaymentResult TemporalDebtTracker::applyRepaymentStrategy(RepaymentStrategyType strategy_type,
                                                            double available_chronons,
                                                            double available_aethel) {
    // Get the strategy implementation
    auto strategy = getStrategy(strategy_type);

    // Apply the strategy to the debts
    return strategy->apply(m_debts, available_chronons, available_aethel);
}

// Borrow chronons
bool TemporalDebtTracker::borrowChronons(double amount, const std::string& operation_id,
                                         bool critical) {
    // Check if exceeding limit
    if (wouldExceedLimit(amount)) {
        // Create a debt alert for the failed borrowing attempt
        DebtAlert alert(DebtAlertLevel::CRITICAL, "Cannot borrow chronons: would exceed limit",
                        operation_id, calculateDebtRatio(), "chronon");

        triggerAlert(alert);
        return false;
    }

    // Calculate interest rate and due cycle
    double interest_rate = calculateInterestRate(amount, critical, RebelOperationType::NONE, 1);
    int due_cycle = calculateDueCycle(amount, critical, RebelOperationType::NONE);

    // Create debt record
    DebtRecord record(operation_id, amount, 0.0, interest_rate, due_cycle, critical);

    // Add to records
    m_debts.push_back(record);

    return true;
}

// Borrow aethel
bool TemporalDebtTracker::borrowAethel(double amount, const std::string& operation_id,
                                       bool critical) {
    // Check if exceeding limit
    if (wouldExceedLimit(amount)) {
        // Create a debt alert for the failed borrowing attempt
        DebtAlert alert(DebtAlertLevel::CRITICAL, "Cannot borrow aethel: would exceed limit",
                        operation_id, calculateDebtRatio(), "aethel");

        triggerAlert(alert);
        return false;
    }

    // Calculate interest rate and due cycle
    double interest_rate = calculateInterestRate(amount, critical, RebelOperationType::NONE, 1);
    int due_cycle = calculateDueCycle(amount, critical, RebelOperationType::NONE);

    // Create debt record
    DebtRecord record(operation_id, 0.0, amount, interest_rate, due_cycle, critical);

    // Add to records
    m_debts.push_back(record);

    return true;
}

// Accrue debt from Rebel operations
bool TemporalDebtTracker::accrueRebelDebt(RebelOperationType operation_type,
                                          const std::string& operation_id,
                                          double custom_aethel_amount, bool is_critical) {
    // Determine aethel amount
    double aethel_amount =
        custom_aethel_amount >= 0.0 ? custom_aethel_amount : getDefaultAethelCost(operation_type);

    // Check if exceeding limit
    if (wouldExceedLimit(aethel_amount)) {
        // Create a debt alert for the failed borrowing attempt
        DebtAlert alert(DebtAlertLevel::CRITICAL, "Cannot accrue rebel debt: would exceed limit",
                        operation_id, calculateDebtRatio(), "rebel_aethel");

        triggerAlert(alert);
        return false;
    }

    // Check for existing debt for this operation
    auto it = findExistingDebt(operation_id, operation_type);
    int operation_count = 1;

    if (it != m_debts.end()) {
        // Increment operation count
        operation_count = it->operation_count + 1;

        // Add to existing debt
        it->aethel_debt += aethel_amount;
        it->operation_count = operation_count;

        // Recalculate interest rate
        it->interest_rate =
            calculateInterestRate(it->aethel_debt, is_critical, operation_type, operation_count);

        return true;
    }

    // Calculate interest rate and due cycle
    double interest_rate = calculateInterestRate(aethel_amount, is_critical, operation_type, 1);
    int due_cycle = calculateDueCycle(aethel_amount, is_critical, operation_type);

    // Create new debt record
    DebtRecord record(operation_id, 0.0, aethel_amount, interest_rate, due_cycle, is_critical,
                      operation_type, 1);

    // Add to records
    m_debts.push_back(record);

    return true;
}

// Accrue debt from Rebel operations with both chronon and aethel costs
bool TemporalDebtTracker::accrueRebelDebtWithChronons(RebelOperationType operation_type,
                                                      const std::string& operation_id,
                                                      double custom_chronon_amount,
                                                      double custom_aethel_amount,
                                                      bool is_critical) {
    // Determine chronon and aethel amounts
    double chronon_amount =
        custom_chronon_amount >= 0.0 ? custom_chronon_amount : getDefaultChronCost(operation_type);
    double aethel_amount =
        custom_aethel_amount >= 0.0 ? custom_aethel_amount : getDefaultAethelCost(operation_type);

    // Check if exceeding limit
    if (wouldExceedLimit(chronon_amount + aethel_amount)) {
        // Create a debt alert for the failed borrowing attempt
        DebtAlert alert(DebtAlertLevel::CRITICAL, "Cannot accrue rebel debt: would exceed limit",
                        operation_id, calculateDebtRatio(), "rebel_combined");

        triggerAlert(alert);
        return false;
    }

    // Check for existing debt for this operation
    auto it = findExistingDebt(operation_id, operation_type);
    int operation_count = 1;

    if (it != m_debts.end()) {
        // Increment operation count
        operation_count = it->operation_count + 1;

        // Add to existing debt
        it->chronon_debt += chronon_amount;
        it->aethel_debt += aethel_amount;
        it->operation_count = operation_count;

        // Recalculate interest rate
        it->interest_rate = calculateInterestRate(it->chronon_debt + it->aethel_debt, is_critical,
                                                  operation_type, operation_count);

        return true;
    }

    // Calculate interest rate and due cycle
    double interest_rate =
        calculateInterestRate(chronon_amount + aethel_amount, is_critical, operation_type, 1);
    int due_cycle = calculateDueCycle(chronon_amount + aethel_amount, is_critical, operation_type);

    // Create new debt record
    DebtRecord record(operation_id, chronon_amount, aethel_amount, interest_rate, due_cycle,
                      is_critical, operation_type, 1);

    // Add to records
    m_debts.push_back(record);

    return true;
}

// Repay chronons
double TemporalDebtTracker::repayChronons(double amount, const std::string& operation_id) {
    double total_repaid = 0.0;

    if (!operation_id.empty()) {
        // Repay specific operation
        auto it = findExistingDebt(operation_id);
        if (it != m_debts.end() && it->chronon_debt > 0) {
            double to_repay = std::min(amount, it->chronon_debt);
            it->chronon_debt -= to_repay;
            total_repaid = to_repay;

            // If debt is fully repaid, remove it
            if (it->chronon_debt <= 0 && it->aethel_debt <= 0) {
                m_debts.erase(it);
            }
        }
    } else {
        // Repay any operation
        RepaymentResult result =
            getStrategy(RepaymentStrategyType::HIGHEST_INTEREST)->apply(m_debts, amount, 0.0);
        total_repaid = result.chronons_repaid;
    }

    return total_repaid;
}

// Repay aethel
double TemporalDebtTracker::repayAethel(double amount, const std::string& operation_id) {
    double total_repaid = 0.0;

    if (!operation_id.empty()) {
        // Repay specific operation
        auto it = findExistingDebt(operation_id);
        if (it != m_debts.end() && it->aethel_debt > 0) {
            double to_repay = std::min(amount, it->aethel_debt);
            it->aethel_debt -= to_repay;
            total_repaid = to_repay;

            // If debt is fully repaid, remove it
            if (it->chronon_debt <= 0 && it->aethel_debt <= 0) {
                m_debts.erase(it);
            }
        }
    } else {
        // Repay any operation
        RepaymentResult result =
            getStrategy(RepaymentStrategyType::HIGHEST_INTEREST)->apply(m_debts, 0.0, amount);
        total_repaid = result.aethel_repaid;
    }

    return total_repaid;
}

// Check if would exceed limit
bool TemporalDebtTracker::wouldExceedLimit(double amount) const {
    double total_debt = getTotalChrononsDebt() + getTotalAethelDebt() + amount;
    return total_debt > m_debt_limit;
}

// Find existing debt
std::vector<DebtRecord>::iterator TemporalDebtTracker::findExistingDebt(
    const std::string& operation_id, RebelOperationType rebel_type) {
    return std::find_if(m_debts.begin(), m_debts.end(), [&](const DebtRecord& debt) {
        return debt.operation_id == operation_id &&
               (rebel_type == RebelOperationType::NONE || debt.rebel_type == rebel_type);
    });
}

// Register alert callback
void TemporalDebtTracker::registerAlertCallback(DebtAlertCallback callback) {
    m_alert_callbacks.push_back(callback);
}

// Calculate interest rate
double TemporalDebtTracker::calculateInterestRate(double amount, bool critical,
                                                  RebelOperationType rebel_type,
                                                  int operation_count) const {
    // Base interest rate is 5%
    double base_rate = 0.05;

    // Adjust based on amount (larger amounts have higher rates)
    double amount_factor = std::min(0.05, amount / 10000.0);

    // Adjust based on criticality
    double critical_factor = critical ? 0.02 : 0.0;

    // Adjust based on rebel operation type
    double rebel_factor = getInterestRateModifier(rebel_type);

    // Adjust based on operation count (repeated operations have higher rates)
    double compounding_factor = calculateCompoundingFactor(rebel_type, operation_count);

    // Combine factors
    return base_rate + amount_factor + critical_factor + rebel_factor + compounding_factor;
}

// Calculate due cycle
int TemporalDebtTracker::calculateDueCycle(double amount, bool critical,
                                           RebelOperationType rebel_type) const {
    // Base due cycle is 10 cycles
    int base_due_cycle = 10;

    // Adjust based on amount (larger amounts have longer terms)
    int amount_offset = static_cast<int>(amount / 100.0);

    // Adjust based on criticality (critical debts are due sooner)
    int critical_offset = critical ? -3 : 0;

    // Adjust based on rebel operation type
    int rebel_offset = getDueCycleOffset(rebel_type);

    // Combine offsets
    return std::max(1, base_due_cycle + amount_offset + critical_offset + rebel_offset);
}

// Get interest rate modifier
double TemporalDebtTracker::getInterestRateModifier(RebelOperationType operation_type) const {
    switch (operation_type) {
        case RebelOperationType::NONE:
            // No specific factor for generic operations
            return 1.0;
        case RebelOperationType::SHATTER_ITERATION:
            // Shattering iterations is costly
            return 2.5;
        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Timeline splicing creates significant discontinuities
            return 3.0;
        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations cause complex ripple effects
            return 1.75;
        case RebelOperationType::CAUSALITY_VIOLATION:
            // Inverting causality is extremely costly
            return 4.0;
        case RebelOperationType::PARADOX_CREATION:
            // Creating paradoxes has exponential cost
            return 5.0;
        case RebelOperationType::UNKNOWN:
        default:
            // Unknown operations have a high default factor
            return 2.0;
    }
}

// Get due cycle offset
int TemporalDebtTracker::getDueCycleOffset(RebelOperationType operation_type) const {
    double base_cost = 0;
    double factor = 1.0;

    switch (operation_type) {
        case RebelOperationType::NONE:
            return 10;  // Base cost for any temporal operation

        case RebelOperationType::SHATTER_ITERATION:
            // Fracturing time requires significant Chronon energy
            base_cost = 50;
            factor = std::pow(1.05, 1);
            break;

        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Splicing timelines requires both Chronon and Aethel
            base_cost = 75;
            factor = std::pow(1.08, 1);
            break;

        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations primarily consume Aethel
            base_cost = 30;
            factor = std::pow(1.03, 1);
            break;

        case RebelOperationType::CAUSALITY_VIOLATION:
            // Causality violations are extremely costly
            base_cost = 100;
            factor = std::pow(1.12, 1);
            break;

        case RebelOperationType::PARADOX_CREATION:
            // Paradoxes have the highest cost
            base_cost = 150;
            factor = std::pow(1.15, 1);
            break;

        default:
            // Default costs for other operation types
            base_cost = 25;
            factor = std::pow(1.04, 1);
            break;
    }

    return static_cast<int>(base_cost * factor);
}

// Get default aethel cost
double TemporalDebtTracker::getDefaultAethelCost(RebelOperationType operation_type) const {
    switch (operation_type) {
        case RebelOperationType::NONE:
            // No specific factor for generic operations
            return 1.0;
        case RebelOperationType::SHATTER_ITERATION:
            // Shattering iterations is costly
            return 2.5;
        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Timeline splicing creates significant discontinuities
            return 3.0;
        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations cause complex ripple effects
            return 1.75;
        case RebelOperationType::CAUSALITY_VIOLATION:
            // Inverting causality is extremely costly
            return 4.0;
        case RebelOperationType::PARADOX_CREATION:
            // Creating paradoxes has exponential cost
            return 5.0;
        default:
            // Unknown operations have a high default factor
            return 2.0;
    }
}

// Get default chronon cost
double TemporalDebtTracker::getDefaultChronCost(RebelOperationType operation_type) const {
    switch (operation_type) {
        case RebelOperationType::NONE:
            return 10;  // Base cost for any temporal operation

        case RebelOperationType::SHATTER_ITERATION:
            // Fracturing time requires significant Chronon energy
            return 50;

        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Splicing timelines requires both Chronon and Aethel
            return 75;

        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations primarily consume Aethel
            return 30;

        case RebelOperationType::CAUSALITY_VIOLATION:
            // Causality violations are extremely costly
            return 100;

        case RebelOperationType::PARADOX_CREATION:
            // Paradoxes have the highest cost
            return 150;

        default:
            // Default costs for other operation types
            return 25;
    }
}

// Set alert thresholds
void TemporalDebtTracker::setAlertThresholds(double low_threshold, double medium_threshold,
                                             double high_threshold, double critical_threshold) {
    m_low_alert_threshold = low_threshold;
    m_medium_alert_threshold = medium_threshold;
    m_high_alert_threshold = high_threshold;
    m_critical_alert_threshold = critical_threshold;
}

// Apply maximum repayment
RepaymentResult TemporalDebtTracker::applyMaximumRepayment(double max_chronon_percent,
                                                           double max_aethel_percent) {
    if (m_runtime) {
        double available_chronons = m_runtime->getChronons() * max_chronon_percent;
        double available_aethel = m_runtime->getAethel() * max_aethel_percent;

        return applyRepaymentStrategy(getRecommendedStrategy(), available_chronons,
                                      available_aethel);
    }

    return RepaymentResult();
}

// Get recommended strategy
RepaymentStrategyType TemporalDebtTracker::getRecommendedStrategy() const {
    // Get debt totals
    double total_chronon_debt = getTotalChrononsDebt();
    double total_aethel_debt = getTotalAethelDebt();

    // Count critical debts
    int critical_debt_count = 0;
    for (const auto& debt : m_debts) {
        if (debt.critical) {  // Updated from is_critical to critical
            critical_debt_count++;
        }
    }

    // If we have critical debts, prioritize them
    if (critical_debt_count > 0) {
        return RepaymentStrategyType::CRITICAL_FIRST;
    }

    // If chronon debt is much higher than aethel debt, prioritize chronons
    if (total_chronon_debt > total_aethel_debt * 2.0) {
        return RepaymentStrategyType::CHRONON_PRIORITY;
    }

    // If aethel debt is much higher than chronon debt, prioritize aethel
    if (total_aethel_debt > total_chronon_debt * 2.0) {
        return RepaymentStrategyType::AETHEL_PRIORITY;
    }

    // If we have limited resources compared to debt, use minimum payments
    if (m_runtime) {
        double available_chronons = m_runtime->getChronons();
        double available_aethel = m_runtime->getAethel();

        if (available_chronons < total_chronon_debt * 0.2 &&
            available_aethel < total_aethel_debt * 0.2) {
            return RepaymentStrategyType::MINIMUM_PAYMENTS;
        }
    }

    // If we have a lot of small debts, use snowball
    int small_debt_count = 0;
    for (const auto& debt : m_debts) {
        if (debt.chronon_debt < 50.0 && debt.aethel_debt < 50.0) {
            small_debt_count++;
        }
    }

    if (static_cast<size_t>(small_debt_count) >= m_debts.size() / 2) {
        return RepaymentStrategyType::SNOWBALL;
    }

    // Default to balanced strategy
    return RepaymentStrategyType::BALANCED;
}

// Trigger alert
void TemporalDebtTracker::triggerAlert(const DebtAlert& alert) {
    // Notify all callbacks
    for (const auto& callback : m_alert_callbacks) {
        callback(alert);
    }
}

// Get strategy description
std::string TemporalDebtTracker::getStrategyDescription(RepaymentStrategyType strategy_type) const {
    std::shared_ptr<RepaymentStrategy> strategy = getStrategy(strategy_type);
    return strategy->getDescription();
}

// Get the total chronon debt
double TemporalDebtTracker::getTotalChrononsDebt() const {
    double total = 0.0;
    for (const auto& debt : m_debts) {
        total += debt.chronon_debt;
    }
    return total;
}

// Get the total aethel debt
double TemporalDebtTracker::getTotalAethelDebt() const {
    double total = 0.0;
    for (const auto& debt : m_debts) {
        total += debt.aethel_debt;
    }
    return total;
}

// Calculate debt ratio
double TemporalDebtTracker::calculateDebtRatio() const {
    double total_debt = getTotalChrononsDebt() + getTotalAethelDebt();
    return total_debt / m_debt_limit;
}

// Get system stability
double TemporalDebtTracker::getStability() const {
    // Calculate stability based on debt ratio
    double debt_ratio = calculateDebtRatio();

    // Stability decreases as debt ratio increases
    return std::max(0.0, 1.0 - (debt_ratio * 0.8));
}

// Advance cycle
int TemporalDebtTracker::advanceCycle() {
    m_current_cycle++;

    int defaulted_debts = 0;

    // Check for defaulted debts and apply interest
    for (auto it = m_debts.begin(); it != m_debts.end();) {
        // Check if debt has defaulted
        if (it->due_cycle <= m_current_cycle) {
            // Create a debt alert for the default
            DebtAlert alert(DebtAlertLevel::CRITICAL, "Debt has defaulted: " + it->operation_id,
                            it->operation_id, calculateDebtRatio(), "default");

            triggerAlert(alert);

            // Remove the debt
            it = m_debts.erase(it);
            defaulted_debts++;
        } else {
            // Apply interest
            double chronon_interest = it->chronon_debt * it->interest_rate;
            double aethel_interest = it->aethel_debt * it->interest_rate;

            it->chronon_debt += chronon_interest;
            it->aethel_debt += aethel_interest;

            // Check if debt ratio is above alert thresholds
            double debt_ratio = calculateDebtRatio();

            if (debt_ratio >= m_critical_alert_threshold) {
                DebtAlert alert(DebtAlertLevel::CRITICAL, "Critical debt level reached",
                                it->operation_id, debt_ratio, "ratio");

                triggerAlert(alert);
            } else if (debt_ratio >= m_high_alert_threshold) {
                DebtAlert alert(DebtAlertLevel::HIGH, "High debt level reached", it->operation_id,
                                debt_ratio, "ratio");

                triggerAlert(alert);
            } else if (debt_ratio >= m_medium_alert_threshold) {
                DebtAlert alert(DebtAlertLevel::MEDIUM, "Medium debt level reached",
                                it->operation_id, debt_ratio, "ratio");

                triggerAlert(alert);
            } else if (debt_ratio >= m_low_alert_threshold) {
                DebtAlert alert(DebtAlertLevel::LOW, "Low debt level reached", it->operation_id,
                                debt_ratio, "ratio");

                triggerAlert(alert);
            }

            ++it;
        }
    }

    return defaulted_debts;
}

// Project future debt
double TemporalDebtTracker::projectFutureDebt(int cycles) const {
    double projected_debt = 0.0;

    for (const auto& debt : m_debts) {
        // Calculate compound interest for the debt
        double chronon_future = debt.chronon_debt * std::pow(1.0 + debt.interest_rate, cycles);
        double aethel_future = debt.aethel_debt * std::pow(1.0 + debt.interest_rate, cycles);

        projected_debt += chronon_future + aethel_future;
    }

    return projected_debt;
}

// Get rebel operation risk level
double TemporalDebtTracker::getRebelOperationRiskLevel(RebelOperationType operation_type) const {
    switch (operation_type) {
        case RebelOperationType::NONE:
            // No specific factor for generic operations
            return 1.0;
        case RebelOperationType::SHATTER_ITERATION:
            // Shattering iterations is costly
            return 0.7;
        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Timeline splicing creates significant discontinuities
            return 0.9;
        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations cause complex ripple effects
            return 0.3;
        case RebelOperationType::CAUSALITY_VIOLATION:
            // Inverting causality is extremely costly
            return 0.8;
        case RebelOperationType::PARADOX_CREATION:
            // Creating paradoxes has exponential cost
            return 0.9;
        default:
            // Unknown operations have a high default factor
            return 0.1;
    }
}

// Get rebel operation stabilization factor
double TemporalDebtTracker::getRebelOperationStabilizationFactor(
    RebelOperationType operation_type) const {
    // Default stabilization factor
    double base_factor = 1.0;

    // Apply modifiers based on operation type
    switch (operation_type) {
        case RebelOperationType::SHATTER_ITERATION:
            base_factor *= 0.9;  // Slightly less stable
            break;
        case RebelOperationType::TIMELINE_CONVERGENCE:
            base_factor *= 0.7;  // Moderately less stable
            break;
        case RebelOperationType::TIMELINE_ALTERATION:
            base_factor *= 0.8;  // Less stable
            break;
        case RebelOperationType::CAUSALITY_VIOLATION:
            base_factor *= 0.5;  // Significantly less stable
            break;
        case RebelOperationType::PARADOX_CREATION:
            base_factor *= 0.3;  // Highly unstable
            break;
        case RebelOperationType::NONE:
        default:
            // No change for other types
            break;
    }

    return std::max(0.1, base_factor);  // Ensure a minimum stabilization factor
}

// Calculate compounding factor based on operation type and count
double TemporalDebtTracker::calculateCompoundingFactor(RebelOperationType rebel_type,
                                                       int operation_count) const {
    double type_multiplier = 1.0;
    switch (rebel_type) {
        case RebelOperationType::NONE:
            // No specific factor for generic operations
            return 1.0;
        case RebelOperationType::SHATTER_ITERATION:
            // Shattering iterations is costly
            type_multiplier = 2.5;
            break;
        case RebelOperationType::TIMELINE_CONVERGENCE:
            // Timeline splicing creates significant discontinuities
            type_multiplier = 3.0;
            break;
        case RebelOperationType::TIMELINE_ALTERATION:
            // Memory alterations cause complex ripple effects
            type_multiplier = 1.75;
            break;
        case RebelOperationType::CAUSALITY_VIOLATION:
            // Inverting causality is extremely costly
            type_multiplier = 4.0;
            break;
        case RebelOperationType::PARADOX_CREATION:
            // Creating paradoxes has exponential cost
            type_multiplier = 5.0;
            break;
        default:
            // Unknown operations have a high default factor
            type_multiplier = 2.0;
            break;
    }

    // Calculate compounding based on operation count with diminishing returns
    double count_factor = std::log10(std::max(1, operation_count) + 1) * 0.05;

    // Combine factors
    return type_multiplier + count_factor;
}

// Calculate chrono impact of a rebel operation
double TemporalDebtTracker::calculateChronoImpact(RebelOperationType operation_type,
                                                  int operation_count) const {
    double base_impact = getDefaultChronCost(operation_type);
    double risk_factor = getRebelOperationRiskLevel(operation_type);

    // Apply compounding effect for multiple operations
    double compounding = calculateCompoundingFactor(operation_type, operation_count);

    return base_impact * (1.0 + risk_factor) * (1.0 + compounding);
}

// Calculate aethel impact of a rebel operation
double TemporalDebtTracker::calculateAethelImpact(RebelOperationType operation_type,
                                                  int operation_count) const {
    double base_impact = getDefaultAethelCost(operation_type);
    double risk_factor = getRebelOperationRiskLevel(operation_type);

    // Apply compounding effect for multiple operations
    double compounding = calculateCompoundingFactor(operation_type, operation_count);

    return base_impact * (1.0 + risk_factor) * (1.0 + compounding);
}

// Get rebel operation name
std::string TemporalDebtTracker::getRebelOperationName(RebelOperationType operation_type) const {
    switch (operation_type) {
        case RebelOperationType::TIMELINE_ALTERATION:
            return "Timeline Alteration";

        case RebelOperationType::PARADOX_CREATION:
            return "Paradox Creation";

        case RebelOperationType::BRANCH_CREATION:
            return "Branch Creation";

        case RebelOperationType::OBSERVER_EFFECT:
            return "Observer Effect";

        case RebelOperationType::CAUSALITY_VIOLATION:
            return "Causality Violation";

        default:
            return "Unknown Rebel Operation";
    }
}

// Get rebel operation info
std::string TemporalDebtTracker::getRebelOperationInfo(RebelOperationType operation_type) const {
    std::stringstream ss;
    ss << getRebelOperationName(operation_type) << "\n";
    ss << "Risk Level: " << getRebelOperationRiskLevel(operation_type) << "\n";
    ss << "Default Chronon Cost: " << getDefaultChronCost(operation_type) << "\n";
    ss << "Default Aethel Cost: " << getDefaultAethelCost(operation_type) << "\n";
    ss << "Interest Rate Modifier: " << getInterestRateModifier(operation_type) << "\n";
    ss << "Due Cycle Offset: " << getDueCycleOffset(operation_type) << "\n";

    switch (operation_type) {
        case RebelOperationType::TIMELINE_ALTERATION:
            ss << "Alters existing timeline events, creating temporal distortions.\n";
            ss << "High risk of instability and resource drain over time.\n";
            break;

        case RebelOperationType::PARADOX_CREATION:
            ss << "Deliberately creates a temporal paradox for strategic advantage.\n";
            ss << "Very high risk of timeline collapse and severe resource penalties.\n";
            break;

        case RebelOperationType::BRANCH_CREATION:
            ss << "Creates a new timeline branch from an existing one.\n";
            ss << "Moderate risk of inconsistencies and resource leakage.\n";
            break;

        case RebelOperationType::OBSERVER_EFFECT:
            ss << "Modifies the timeline through the act of observation.\n";
            ss << "Lower risk but creates subtle inconsistencies that may compound.\n";
            break;

        case RebelOperationType::CAUSALITY_VIOLATION:
            ss << "Violates the normal cause-effect relationship of temporal events.\n";
            ss << "High risk of logical contradictions and resource spirals.\n";
            break;

        default:
            ss << "No additional information available for this operation type.\n";
            break;
    }

    return ss.str();
}

// Project paradox risk of rebel operations
double TemporalDebtTracker::projectParadoxRisk(RebelOperationType operation_type, int cycles,
                                               int operation_count) const {
    // Base risk depends on operation type
    double base_risk = getRebelOperationRiskLevel(operation_type);

    // Apply compounding factor
    double compounding = calculateCompoundingFactor(operation_type, operation_count);

    // Calculate risk growth over cycles (using a logarithmic scale)
    double cycle_factor = std::log10(cycles + 1) / std::log10(10);

    // Combine factors with diminishing returns
    double combined_risk = base_risk * compounding * cycle_factor;

    // Cap at 1.0 (100% risk)
    return std::min(1.0, combined_risk);
}

// Get the total debt for rebel operations
std::pair<double, double> TemporalDebtTracker::getRebelDebtTotal(
    RebelOperationType operation_type) const {
    double total_chronons = 0.0;
    double total_aethel = 0.0;

    for (const auto& debt : m_debts) {
        if (operation_type == RebelOperationType::NONE || debt.rebel_type == operation_type) {
            total_chronons += debt.chronon_debt;
            total_aethel += debt.aethel_debt;
        }
    }

    return std::make_pair(total_chronons, total_aethel);
}

// Get the rebel debt total (single value)
double TemporalDebtTracker::getOverallDebtLevel() const {
    double total_debt = 0.0;
    for (const auto& debt : m_debts) {
        total_debt += debt.chronon_debt + debt.aethel_debt;
    }
    return total_debt;
}

// Get past due debts
std::vector<DebtRecord> TemporalDebtTracker::getPastDueDebts() const {
    std::vector<DebtRecord> past_due_debts;
    for (const DebtRecord& debt : m_debts) {
        if (debt.is_past_due) {
            past_due_debts.push_back(debt);
        }
    }
    return past_due_debts;
}

// Get operation counts
std::map<RebelOperationType, int> TemporalDebtTracker::getOperationCounts() const {
    std::cerr << "[getOperationCounts] Entered." << std::endl;
    std::map<RebelOperationType, int> counts;
    std::cerr << "[getOperationCounts] std::map counts declared." << std::endl;
    try {
        std::cerr << "[getOperationCounts] Attempting counts[RebelOperationType::NONE] = 0;"
                  << std::endl;
        counts[RebelOperationType::NONE] = 0;  // Explicitly insert a benign key for diagnostics
        std::cerr << "[getOperationCounts] counts[RebelOperationType::NONE] = 0; successful."
                  << std::endl;

        std::cerr << "[getOperationCounts] Attempting to iterate m_debts. Size: " << m_debts.size()
                  << std::endl;
        for (const auto& debt_record : m_debts) {  // m_debts is std::vector<DebtRecord>
            std::cerr << "[getOperationCounts] Processing debt_record. Rebel Type (int): "
                      << static_cast<int>(debt_record.rebel_type)
                      << ", Operation ID: " << debt_record.operation_id << std::endl;
            counts[debt_record.rebel_type]++;
            std::cerr << "[getOperationCounts] Incremented count for Rebel Type (int): "
                      << static_cast<int>(debt_record.rebel_type) << std::endl;
        }
        std::cerr << "[getOperationCounts] Finished iterating m_debts." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[getOperationCounts] EXCEPTION (std::exception): " << e.what() << std::endl;
        throw;  // Rethrow
    } catch (...) {
        std::cerr << "[getOperationCounts] UNKNOWN EXCEPTION CAUGHT." << std::endl;
        throw;  // Rethrow
    }
    std::cerr << "[getOperationCounts] Returning counts. Map size: " << counts.size() << std::endl;
    return counts;
}

// Get the count of operations of a specific type
int TemporalDebtTracker::getRebelOperationCount(RebelOperationType operation_type,
                                                const std::string& operation_id) const {
    if (operation_id.empty()) {
        auto it = m_operationCounts.find(operation_type);
        if (it != m_operationCounts.end()) {
            return it->second;
        }
    } else {
        int count = 0;
        for (const auto& debt : m_debts) {
            if (debt.rebel_type == operation_type && debt.operation_id == operation_id) {
                count++;
            }
        }
        return count;
    }
    return 0;
}

// Check debt status and trigger alerts if necessary
bool TemporalDebtTracker::checkAlerts() { return false; }

// Repayment strategy and calculation methods - Stubs
std::string TemporalDebtTracker::suggestRepaymentStrategy() const {
    return "No specific strategy suggested (stub).";
}

std::pair<double, double> TemporalDebtTracker::calculateMinimumSafeRepayment() const {
    return {0.0, 0.0};
}

std::map<std::string, std::pair<double, double>> TemporalDebtTracker::calculateOptimalRepayment(
    double available_chronons, double available_aethel) const {
    (void)available_chronons;
    (void)available_aethel;
    return {};
}

// Add the new getParadoxRisk method, typically after other getter methods
double TemporalDebtTracker::getParadoxRisk() const {
    // Placeholder implementation
    // A more sophisticated implementation might consider debt levels, critical debts, stability,
    // etc. For now, returning a fixed value or a simple calculation based on existing members.
    // double risk = (getOverallDebtLevel() / m_debt_limit) * 0.5; // Example: half of debt ratio
    // return std::min(1.0, std::max(0.0, risk));
    return 0.0;
}

}  // namespace chronovyan