#ifndef CHRONOVYAN_TEMPORAL_DEBT_TRACKER_H
#define CHRONOVYAN_TEMPORAL_DEBT_TRACKER_H

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "rebel_operation.h"
#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @enum DebtAlertLevel
 * @brief Defines the severity level of debt alerts
 */
enum class DebtAlertLevel {
    NONE,     // No alert
    LOW,      // Low alert - informational
    MEDIUM,   // Medium alert - warning
    HIGH,     // High alert - critical
    CRITICAL  // Critical alert - potential paradox
};

/**
 * @enum RepaymentStrategyType
 * @brief Defines strategies for repaying temporal debt
 */
enum class RepaymentStrategyType {
    NONE,                 // No specific strategy
    OLDEST_FIRST,         // Repay oldest debts first
    HIGHEST_INTEREST,     // Repay highest interest debts first
    CRITICAL_FIRST,       // Repay critical debts first
    BALANCED,             // Balance repayment across all debts
    MINIMUM_PAYMENTS,     // Make minimum payments on all debts
    SNOWBALL,             // Focus on smallest debts first
    AVALANCHE,            // Focus on highest interest debts first
    STABILITY_OPTIMIZED,  // Optimize for system stability
    CHRONON_PRIORITY,     // Prioritize chronon debt repayment
    AETHEL_PRIORITY       // Prioritize aethel debt repayment
};

// Forward declarations
class RepaymentStrategy;
class TemporalRuntime;

/**
 * @struct RepaymentResult
 * @brief Result of applying a repayment strategy
 */
struct RepaymentResult {
    double chronons_repaid;        // Amount of chronons repaid
    double aethel_repaid;          // Amount of aethel repaid
    int debts_cleared;             // Number of debts completely cleared
    double stability_improvement;  // Improvement in system stability

    RepaymentResult()
        : chronons_repaid(0.0), aethel_repaid(0.0), debts_cleared(0), stability_improvement(0.0) {}
};

/**
 * @struct DebtAlert
 * @brief Represents an alert about the debt situation
 */
struct DebtAlert {
    DebtAlertLevel level;      // Alert severity level
    std::string message;       // Alert message
    std::string operation_id;  // Related operation ID (if applicable)
    double debt_ratio;         // Current debt to limit ratio
    std::string debt_type;     // Type of debt (chronon, aethel, etc.)

    DebtAlert(DebtAlertLevel lvl, std::string msg, std::string op_id = "", double ratio = 0.0,
              std::string type = "")
        : level(lvl)
        , message(std::move(msg))
        , operation_id(std::move(op_id))
        , debt_ratio(ratio)
        , debt_type(std::move(type)) {}
};

// Define callback type for debt alerts
using DebtAlertCallback = std::function<void(const DebtAlert&)>;

/**
 * @struct TemporalDebt
 * @brief Represents a temporal debt entity for benchmarking and tracking
 */
struct TemporalDebt {
    std::string id;            // Unique identifier for the debt
    double amount;             // Total debt amount
    double chronon_component;  // Chronon component of the debt
    double aethel_component;   // Aethel component of the debt
    double interest_rate;      // Interest rate for this debt
    int age_in_cycles;         // Age of the debt in cycles
    bool is_critical;          // Whether this debt is critical
    RebelOperationType type;   // Type of rebel operation that caused the debt

    TemporalDebt(std::string debt_id, double debt_amount, double chronon_part, double aethel_part,
                 double rate, int age, bool critical,
                 RebelOperationType op_type = RebelOperationType::NONE)
        : id(std::move(debt_id))
        , amount(debt_amount)
        , chronon_component(chronon_part)
        , aethel_component(aethel_part)
        , interest_rate(rate)
        , age_in_cycles(age)
        , is_critical(critical)
        , type(op_type) {}
};

/**
 * @struct DebtRecord
 * @brief Records a single instance of temporal debt
 */
struct DebtRecord {
    std::string operation_id;       // The operation that caused the debt
    double chronon_debt;            // Amount of chronon debt
    double aethel_debt;             // Amount of aethel debt
    double interest_rate;           // Interest rate per cycle
    int due_cycle;                  // When the debt is due to be repaid
    bool critical;                  // Whether the debt is critical (causes paradoxes if not repaid)
    RebelOperationType rebel_type;  // Type of rebel operation that caused the debt
    int operation_count;            // Count of operations of this type (for compounding effects)
    std::chrono::system_clock::time_point timestamp;  // When the debt was incurred
    std::chrono::system_clock::time_point due_date;   // When the debt should be repaid
    bool is_past_due;                                 // Whether the debt is past due

    // Default constructor
    DebtRecord()
        : operation_id("")
        , chronon_debt(0.0)
        , aethel_debt(0.0)
        , interest_rate(0.0)
        , due_cycle(0)
        , critical(false)
        , rebel_type(RebelOperationType::NONE)
        , operation_count(0)
        , timestamp(std::chrono::system_clock::now())
        , due_date(std::chrono::system_clock::now() + std::chrono::hours(24))
        , is_past_due(false) {}

    DebtRecord(std::string op_id, double chronons, double aethel, double rate, int due,
               bool is_critical, RebelOperationType type = RebelOperationType::NONE, int count = 1)
        : operation_id(std::move(op_id))
        , chronon_debt(chronons)
        , aethel_debt(aethel)
        , interest_rate(rate)
        , due_cycle(due)
        , critical(is_critical)
        , rebel_type(type)
        , operation_count(count)
        , timestamp(std::chrono::system_clock::now())
        , due_date(std::chrono::system_clock::now() + std::chrono::hours(24))
        , is_past_due(false) {}

    // Conversion operator to TemporalDebt for compatibility
    operator TemporalDebt() const {
        return TemporalDebt(operation_id, chronon_debt + aethel_debt, chronon_debt, aethel_debt,
                            interest_rate, due_cycle, critical, rebel_type);
    }
};

// Type aliases for clarity
using DebtCallbackId = size_t;

/**
 * @class RepaymentStrategy
 * @brief Base class for debt repayment strategies
 */
class RepaymentStrategy {
public:
    virtual ~RepaymentStrategy() = default;

    /**
     * @brief Apply the repayment strategy to the debts
     * @param debts The collection of debt records to repay
     * @param available_chronons Available chronons for repayment
     * @param available_aethel Available aethel for repayment
     * @return The result of applying the repayment strategy
     */
    virtual RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                                  double available_aethel) const = 0;

    /**
     * @brief Get the name of this repayment strategy
     * @return The strategy name
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get a description of this repayment strategy
     * @return The strategy description
     */
    virtual std::string getDescription() const = 0;

protected:
    /**
     * @brief Calculate the interest that would be saved by repaying a debt
     * @param debt The debt record
     * @param chronon_amount Amount of chronons to repay
     * @param aethel_amount Amount of aethel to repay
     * @return The amount of interest saved
     */
    double calculateInterestSaved(const DebtRecord& debt, double chronon_amount,
                                  double aethel_amount) const {
        // Calculate interest saved based on the amount repaid and the interest rate
        double total_repaid = chronon_amount + aethel_amount;
        double total_debt = debt.chronon_debt + debt.aethel_debt;

        if (total_debt <= 0.0) {
            return 0.0;
        }

        // Interest saved is proportional to the amount repaid
        double proportion_repaid = total_repaid / total_debt;

        // Simple interest calculation
        double interest_per_cycle = total_debt * debt.interest_rate;
        int cycles_remaining = std::max(1, debt.due_cycle);

        return proportion_repaid * interest_per_cycle * cycles_remaining;
    }
};

/**
 * @class OldestFirstStrategy
 * @brief Repays the oldest debts first
 */
class OldestFirstStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "OldestFirst"; }
    std::string getDescription() const override {
        return "Repays the oldest debts first, prioritizing by creation time.";
    }
};

/**
 * @class HighestInterestStrategy
 * @brief Repays debts with the highest interest rates first
 */
class HighestInterestStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "HighestInterest"; }
    std::string getDescription() const override {
        return "Repays debts with the highest interest rates first to minimize total interest "
               "paid.";
    }
};

/**
 * @class CriticalFirstStrategy
 * @brief Repays critical debts first
 */
class CriticalFirstStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "CriticalFirst"; }
    std::string getDescription() const override {
        return "Prioritizes repayment of critical debts to maintain timeline stability.";
    }
};

/**
 * @class BalancedStrategy
 * @brief Balances repayment across all debts
 */
class BalancedStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "Balanced"; }
    std::string getDescription() const override {
        return "Distributes repayment across all debts based on their relative size and priority.";
    }
};

/**
 * @class MinimumPaymentsStrategy
 * @brief Makes minimum payments on all debts
 */
class MinimumPaymentsStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "MinimumPayments"; }
    std::string getDescription() const override {
        return "Makes minimum payments on all debts to prevent default, preserving resources for "
               "other operations.";
    }
};

/**
 * @class SnowballStrategy
 * @brief Repays smallest debts first (Debt Snowball method)
 */
class SnowballStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "Snowball"; }
    std::string getDescription() const override {
        return "Focuses on repaying the smallest debts first to achieve quick wins and build "
               "momentum.";
    }
};

/**
 * @class AvalancheStrategy
 * @brief Repays highest interest debts first (Debt Avalanche method)
 */
class AvalancheStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "Avalanche"; }
    std::string getDescription() const override {
        return "Focuses on repaying debts with the highest interest rates first to minimize total "
               "interest paid over time.";
    }
};

/**
 * @class StabilityOptimizedStrategy
 * @brief Optimizes repayment for timeline stability
 */
class StabilityOptimizedStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "StabilityOptimized"; }
    std::string getDescription() const override {
        return "Optimizes debt repayment to maximize timeline stability, considering temporal "
               "paradox risks.";
    }
};

/**
 * @class ChronosPriorityStrategy
 * @brief Prioritizes chronon debt repayment
 */
class ChronosPriorityStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "ChronosPriority"; }
    std::string getDescription() const override {
        return "Prioritizes repaying chronon debt to optimize temporal processing capacity.";
    }
};

/**
 * @class AethelPriorityStrategy
 * @brief Prioritizes aethel debt repayment
 */
class AethelPriorityStrategy : public RepaymentStrategy {
public:
    RepaymentResult apply(std::vector<DebtRecord>& debts, double available_chronons,
                          double available_aethel) const override;
    std::string getName() const override { return "AethelPriority"; }
    std::string getDescription() const override {
        return "Prioritizes repaying aethel debt to optimize parallel timeline management.";
    }
};

/**
 * @class TemporalDebtTracker
 * @brief Tracks and manages temporal debt from operations
 */
class TemporalDebtTracker {
public:
    /**
     * @brief Constructor
     * @param runtime The temporal runtime
     */
    TemporalDebtTracker(std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Record a rebel operation and its associated debt
     * @param operation The type of rebel operation
     * @param debtAmount The amount of temporal debt incurred
     * @param targetTime The time affected by the operation
     */
    void recordRebelOperation(RebelOperationType operation, double debtAmount, int targetTime);

    /**
     * @brief Get the total accumulated rebel debt (now overall debt level)
     * @return The total debt value
     */
    double getOverallDebtLevel() const;

    /**
     * @brief Get the current temporal stability metric
     * @return A value between 0.0 (unstable) and 1.0 (stable)
     */
    double getStability() const;

    /**
     * @brief Get the current paradox risk level
     * @return A value representing the risk of a paradox
     */
    double getParadoxRisk() const;

    /**
     * @brief Get the count of rebel operations by type
     * @return A map of operation types to counts
     */
    std::map<RebelOperationType, int> getOperationCounts() const;

    /**
     * @brief Get historical debt data for visualization
     * @return Vector of debt values over time
     */
    std::vector<double> getHistoricalDebtData() const;

    /**
     * @brief Reset the debt tracker to initial state
     */
    void reset();

    /**
     * @brief Calculate interest for a debt item
     * @param amount The debt amount
     * @param critical Whether this is critical debt
     * @param op_type The operation type that created the debt
     * @return The interest rate
     */
    double calculateInterestRate(double amount, bool critical, RebelOperationType op_type,
                                 int age) const;

    /**
     * @brief Calculate when a debt is due
     * @param amount The debt amount
     * @param critical Whether this is critical debt
     * @param rebel_type The operation type that created the debt
     * @return The cycle when the debt is due
     */
    int calculateDueCycle(double amount, bool critical, RebelOperationType rebel_type) const;

    /**
     * @brief Check if the debt would exceed the limit
     * @param amount The amount to check
     * @return Whether the amount would exceed the debt limit
     */
    bool wouldExceedLimit(double amount) const;

    /**
     * @brief Borrow chronons when resources are insufficient
     * @param amount The amount of chronons to borrow
     * @param operation_id The operation requiring the chronons
     * @param critical Whether failure to repay is critical
     * @return True if borrowing was successful
     */
    bool borrowChronons(double amount, const std::string& operation_id, bool critical = false);

    /**
     * @brief Borrow aethel when resources are insufficient
     * @param amount The amount of aethel to borrow
     * @param operation_id The operation requiring the aethel
     * @param critical Whether failure to repay is critical
     * @return True if borrowing was successful
     */
    bool borrowAethel(double amount, const std::string& operation_id, bool critical = false);

    /**
     * @brief Accrue debt from Rebel operations
     * @param operation_type The type of Rebel operation
     * @param operation_id The unique operation identifier
     * @param custom_aethel_amount Custom aethel amount (use -1.0 for default)
     * @param is_critical Whether the debt is critical
     * @return True if debt was successfully accrued
     */
    bool accrueRebelDebt(RebelOperationType operation_type, const std::string& operation_id,
                         double custom_aethel_amount = -1.0, bool is_critical = false);

    /**
     * @brief Accrue debt from Rebel operations with both chronon and aethel costs
     * @param operation_type The type of Rebel operation
     * @param operation_id The unique operation identifier
     * @param custom_chronon_amount Custom chronon amount (use -1.0 for default)
     * @param custom_aethel_amount Custom aethel amount (use -1.0 for default)
     * @param is_critical Whether the debt is critical
     * @return True if debt was successfully accrued
     */
    bool accrueRebelDebtWithChronons(RebelOperationType operation_type,
                                     const std::string& operation_id,
                                     double custom_chronon_amount = -1.0,
                                     double custom_aethel_amount = -1.0, bool is_critical = false);

    /**
     * @brief Repay chronon debt
     * @param amount The amount of chronons to repay
     * @param operation_id The operation to repay debt for (if empty, repay oldest debt)
     * @return The amount of debt that was repaid
     */
    double repayChronons(double amount, const std::string& operation_id = "");

    /**
     * @brief Repay aethel debt
     * @param amount The amount of aethel to repay
     * @param operation_id The operation to repay debt for (if empty, repay oldest debt)
     * @return The amount of debt that was repaid
     */
    double repayAethel(double amount, const std::string& operation_id = "");

    /**
     * @brief Apply a repayment strategy with available resources
     * @param strategy_type The repayment strategy to apply
     * @param available_chronons Available chronons for repayment
     * @param available_aethel Available aethel for repayment
     * @return Result of the repayment operation
     */
    RepaymentResult applyRepaymentStrategy(RepaymentStrategyType strategy_type,
                                           double available_chronons, double available_aethel);

    /**
     * @brief Apply the optimal repayment strategy for the current situation
     * @param max_chronon_percent Maximum percentage of available chronons to use (0.0-1.0)
     * @param max_aethel_percent Maximum percentage of available aethel to use (0.0-1.0)
     * @return Result of the repayment operation
     */
    RepaymentResult applyMaximumRepayment(double max_chronon_percent = 0.3,
                                          double max_aethel_percent = 0.3);

    /**
     * @brief Get the recommended repayment strategy for the current situation
     * @return The recommended strategy
     */
    RepaymentStrategyType getRecommendedStrategy() const;

    /**
     * @brief Analyze the effectiveness of different repayment strategies
     * @param available_chronons Available chronons for repayment
     * @param available_aethel Available aethel for repayment
     * @return Map of strategies to their projected effectiveness score (0.0-1.0)
     */
    std::map<RepaymentStrategyType, double> analyzeRepaymentStrategies(
        double available_chronons, double available_aethel) const;

    /**
     * @brief Advance the debt cycle, applying interest and checking for due debts
     * @return The number of debts that are now due
     */
    int advanceCycle();

    /**
     * @brief Get the current total chronon debt
     * @return The total chronon debt
     */
    double getTotalChrononsDebt() const;

    /**
     * @brief Get the current total aethel debt
     * @return The total aethel debt
     */
    double getTotalAethelDebt() const;

    /**
     * @brief Get all debt records
     * @return Vector of all debt records
     */
    std::vector<DebtRecord> getAllDebts() const { return m_debts; }

    /**
     * @brief Get debts that are past due
     * @return Vector of past due debt records
     */
    std::vector<DebtRecord> getPastDueDebts() const;

    /**
     * @brief Get the current debt limit
     * @return The maximum allowable debt
     */
    double getDebtLimit() const { return m_debt_limit; }

    /**
     * @brief Register a callback for debt alerts
     * @param callback The function to call when an alert is triggered
     */
    void registerAlertCallback(DebtAlertCallback callback);

    /**
     * @brief Check debt status and trigger alerts if necessary
     * @return True if any alerts were triggered
     */
    bool checkAlerts();

    /**
     * @brief Set the alert thresholds for different alert levels
     * @param low_threshold Ratio of debt to limit for LOW alerts
     * @param medium_threshold Ratio of debt to limit for MEDIUM alerts
     * @param high_threshold Ratio of debt to limit for HIGH alerts
     * @param critical_threshold Ratio of debt to limit for CRITICAL alerts
     */
    void setAlertThresholds(double low_threshold, double medium_threshold, double high_threshold,
                            double critical_threshold);

    /**
     * @brief Suggest debt repayment strategy
     * @return Text description of recommended repayment actions
     */
    std::string suggestRepaymentStrategy() const;

    /**
     * @brief Project future debt status if no repayment occurs
     * @param cycles Number of cycles to project into the future
     * @return Projected total debt after specified cycles
     */
    double projectFutureDebt(int cycles) const;

    /**
     * @brief Calculate minimum repayment needed to prevent paradoxes
     * @return Minimum amount of chronons and aethel to repay
     */
    std::pair<double, double> calculateMinimumSafeRepayment() const;

    /**
     * @brief Calculate optimal repayment distribution across operations
     * @param available_chronons Available chronons for repayment
     * @param available_aethel Available aethel for repayment
     * @return Map of operation IDs to recommended repayment amounts
     */
    std::map<std::string, std::pair<double, double>> calculateOptimalRepayment(
        double available_chronons, double available_aethel) const;

    /**
     * @brief Get the current cycle
     * @return The current cycle number
     */
    int getCurrentCycle() const { return m_current_cycle; }

    /**
     * @brief Get a description of a repayment strategy
     * @param strategy_type The strategy to describe
     * @return Text description of the strategy
     */
    std::string getStrategyDescription(RepaymentStrategyType strategy_type) const;

    /**
     * @brief Get the strategy for the specified type
     * @param strategy_type The type of strategy to get
     * @return A pointer to the strategy implementation
     */
    std::shared_ptr<RepaymentStrategy> getStrategy(RepaymentStrategyType strategy_type) const;

    /**
     * @brief Add a debt record directly (for testing)
     * @param operation_id The operation ID
     * @param chronon_debt The chronon debt amount
     * @param aethel_debt The aethel debt amount
     * @param interest_rate The interest rate
     * @param due_cycle The due cycle
     * @param critical Whether the debt is critical
     * @param rebel_type The type of rebel operation
     * @param operation_count Count of operations of this type
     */
    void addDebtForTesting(const std::string& operation_id, double chronon_debt, double aethel_debt,
                           double interest_rate, int due_cycle, bool critical,
                           RebelOperationType rebel_type = RebelOperationType::NONE,
                           int operation_count = 1) {
        m_debts.emplace_back(operation_id, chronon_debt, aethel_debt, interest_rate, due_cycle,
                             critical, rebel_type, operation_count);
    }

    /**
     * @brief Project the paradox risk of a rebel operation
     * @param operation_type The type of operation
     * @param cycles The number of cycles to project
     * @param operation_count The count of operations
     * @return The projected paradox risk (0.0-1.0)
     */
    double projectParadoxRisk(RebelOperationType operation_type, int cycles,
                              int operation_count = 1) const;

    /**
     * @brief Project the stability impact of a rebel operation over multiple cycles
     * @param operation_type The type of operation
     * @param cycles The number of cycles to project
     * @param operation_count The count of operations per cycle
     * @return Vector of stability values for each cycle
     */
    std::vector<double> projectRebelOperationStabilityImpact(RebelOperationType operation_type,
                                                             int cycles,
                                                             int operation_count = 1) const;

    /**
     * @brief Get information about a rebel operation
     * @param operation_type The type of operation
     * @return Text description of the operation
     */
    std::string getRebelOperationInfo(RebelOperationType operation_type) const;

    /**
     * @brief Get the count of operations of a specific type
     * @param operation_type The type of operation to count
     * @param operation_id Optional operation ID to filter by
     * @return The count of operations
     */
    int getRebelOperationCount(RebelOperationType operation_type,
                               const std::string& operation_id = "") const;

    /**
     * @brief Get the default aethel cost for a rebel operation
     * @param operation_type The type of operation
     * @return The default aethel cost
     */
    double getDefaultAethelCost(RebelOperationType operation_type) const;

    /**
     * @brief Get the default chronon cost for a rebel operation
     * @param operation_type The type of operation
     * @return The default chronon cost
     */
    double getDefaultChronCost(RebelOperationType operation_type) const;

    /**
     * @brief Find an existing debt record for an operation
     * @param operation_id The operation ID
     * @param rebel_type The type of rebel operation
     * @return Iterator to the debt record, or m_debts.end() if not found
     */
    std::vector<DebtRecord>::iterator findExistingDebt(
        const std::string& operation_id, RebelOperationType rebel_type = RebelOperationType::NONE);

    /**
     * @brief Get the total debt for rebel operations
     * @param operation_type The type of operation, or NONE for all types
     * @return Pair of (chronon_debt, aethel_debt)
     */
    std::pair<double, double> getRebelDebtTotal(
        RebelOperationType operation_type = RebelOperationType::NONE) const;

    /**
     * @brief Get the stability impact of a rebel operation
     * @param operation_type The type of operation
     * @param operation_count The count of operations
     * @return The stability impact (0.0-1.0)
     */
    double getRebelOperationStabilityImpact(RebelOperationType operation_type,
                                            int operation_count = 1) const;

    /**
     * @brief Apply interest to all debts
     */
    void applyInterest();

    /**
     * @brief Check for paradoxes caused by unpaid critical debts
     * @return The number of paradoxes detected
     */
    int checkForParadoxes();

    /**
     * @brief Trigger a debt alert
     * @param alert The alert to trigger
     */
    void triggerAlert(const DebtAlert& alert);

    /**
     * @brief Calculate the ratio of total debt to debt limit
     * @return The debt ratio (0.0-1.0)
     */
    double calculateDebtRatio() const;

    /**
     * @brief Get the name of a rebel operation
     * @param operation_type The type of operation
     * @return The operation name
     */
    std::string getRebelOperationName(RebelOperationType operation_type) const;

    /**
     * @brief Get the interest rate modifier for a rebel operation
     * @param operation_type The type of operation
     * @return The interest rate modifier
     */
    double getInterestRateModifier(RebelOperationType operation_type) const;

    /**
     * @brief Get the due cycle offset for a rebel operation
     * @param operation_type The type of operation
     * @return The due cycle offset
     */
    int getDueCycleOffset(RebelOperationType operation_type) const;

    /**
     * @brief Get the rebel operation risk level
     * @param operation_type The type of operation
     * @return The risk level (0.0-1.0)
     */
    double getRebelOperationRiskLevel(RebelOperationType operation_type) const;

    /**
     * @brief Get the rebel operation stabilization factor
     * @param operation_type The type of operation
     * @return The stabilization factor (0.0-1.0)
     */
    double getRebelOperationStabilizationFactor(RebelOperationType operation_type) const;

    /**
     * @brief Calculate compounding factor based on operation type and count
     * @param rebel_type The type of rebel operation
     * @param operation_count The count of operations
     * @return The compounding factor
     */
    double calculateCompoundingFactor(RebelOperationType rebel_type, int operation_count) const;

    /**
     * @brief Calculate compounding factor for a specific debt
     * @param debt The temporal debt
     * @return The compounding factor
     */
    double calculateCompoundingFactor(const TemporalDebt& debt) const;

    /**
     * @brief Calculate the chrono impact of a rebel operation
     * @param operation_type The type of operation
     * @param operation_count The count of operations
     * @return The chrono impact
     */
    double calculateChronoImpact(RebelOperationType operation_type, int operation_count) const;

    /**
     * @brief Calculate the aethel impact of a rebel operation
     * @param operation_type The type of operation
     * @param operation_count The count of operations
     * @return The aethel impact
     */
    double calculateAethelImpact(RebelOperationType operation_type, int operation_count) const;

private:
    struct RebelOperationRecord {
        RebelOperationType type;
        double debtAmount;
        int targetTime;
        int recordedAt;
    };

    std::shared_ptr<TemporalRuntime> m_runtime;  // The temporal runtime being tracked
    std::vector<DebtRecord> m_debts;             // List of all debts
    int m_current_cycle;                         // Current cycle number
    double m_base_interest_rate = 0.05;          // 5% default interest rate
    double m_debt_limit;                         // Maximum allowed debt

    // Strategy implementations
    std::map<RepaymentStrategyType, std::shared_ptr<RepaymentStrategy>> m_strategies;

    /**
     * @brief Initialize strategies
     * Creates all strategy implementations
     */
    void initializeStrategies();

    // Alert thresholds as ratios of debt to debt limit
    double m_low_alert_threshold;
    double m_medium_alert_threshold;
    double m_high_alert_threshold;
    double m_critical_alert_threshold;

    std::vector<DebtAlertCallback> m_alert_callbacks;  // Callbacks for debt alerts
    std::vector<RebelOperationRecord> m_operationHistory;
    double m_totalDebt;
    double m_stability;
    std::map<RebelOperationType, int> m_operationCounts;
    std::vector<double> m_historicalDebt;

    /**
     * @brief Update stability metrics based on current debt
     */
    void updateStabilityMetrics();
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_TEMPORAL_DEBT_TRACKER_H