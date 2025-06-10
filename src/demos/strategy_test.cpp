#include <iomanip>
#include <iostream>
#include <memory>
#include <vector>

#include "temporal_debt_tracker.h"

using namespace chronovyan;

// Simple mock class for TemporalRuntime
class MockTemporalRuntime : public TemporalRuntime {
public:
  MockTemporalRuntime() : m_chronons(100.0), m_aethel(100.0) {}

  double getChronons() const override { return m_chronons; }
  double getAethel() const override { return m_aethel; }

  void setChronons(double amount) { m_chronons = amount; }
  void setAethel(double amount) { m_aethel = amount; }

private:
  double m_chronons;
  double m_aethel;
};

// Helper function to print a vector of debts
void printDebts(const std::vector<DebtRecord> &debts) {
  std::cout << "Current Debts:" << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << std::left << std::setw(15) << "Operation ID" << std::setw(15)
            << "Chronons" << std::setw(15) << "Aethel" << std::setw(15)
            << "Interest"
            << "Critical" << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;

  for (const auto &debt : debts) {
    std::cout << std::left << std::setw(15) << debt.operation_id << std::fixed
              << std::setprecision(2) << std::setw(15) << debt.chronon_debt
              << std::setw(15) << debt.aethel_debt << std::setw(15)
              << debt.interest_rate * 100 << "% "
              << (debt.is_critical ? "Yes" : "No") << std::endl;
  }
  std::cout << "-------------------------------------------------" << std::endl;
}

// Helper function to print a repayment result
void printRepaymentResult(const RepaymentResult &result) {
  std::cout << "Repayment Result:" << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "Chronons repaid:   " << result.chronons_repaid << std::endl;
  std::cout << "Aethel repaid:     " << result.aethel_repaid << std::endl;
  std::cout << "Debts cleared:     " << result.debts_cleared << std::endl;
  std::cout << "Stability gain:    " << result.stability_gain << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
}

int main() {
  // Create test data
  auto runtime = std::make_shared<MockTemporalRuntime>();
  TemporalDebtTracker tracker(runtime);

  // Create a set of debt records
  std::vector<DebtRecord> debts = {
      {"op1", 100.0, 50.0, 0.05, 5, false, OperationType::TIMESTREAM_BRANCH},
      {"op2", 200.0, 150.0, 0.08, 3, true, OperationType::TIMESTREAM_MERGE},
      {"op3", 50.0, 200.0, 0.03, 7, false,
       OperationType::CAUSALITY_PRESERVATION},
      {"op4", 150.0, 100.0, 0.06, 2, true,
       OperationType::TIMELINE_MANIPULATION},
      {"op5", 80.0, 70.0, 0.04, 4, false,
       OperationType::TIMELINE_MANIPULATION}};

  // Add the debts to the tracker
  for (const auto &debt : debts) {
    tracker.addDebtForTesting(debt.operation_id, debt.chronon_debt,
                              debt.aethel_debt, debt.interest_rate,
                              debt.age_in_cycles, debt.is_critical, debt.type,
                              1 // operation count
    );
  }

  // Print initial state
  std::cout << "Initial Debt State:" << std::endl;
  printDebts(tracker.getAllDebts());

  // Test each repayment strategy
  std::vector<RepaymentStrategyType> strategies = {
      RepaymentStrategyType::OLDEST_FIRST,
      RepaymentStrategyType::HIGHEST_INTEREST,
      RepaymentStrategyType::CRITICAL_FIRST,
      RepaymentStrategyType::BALANCED,
      RepaymentStrategyType::MINIMUM_PAYMENTS,
      RepaymentStrategyType::SNOWBALL,
      RepaymentStrategyType::AVALANCHE,
      RepaymentStrategyType::STABILITY_OPTIMIZED,
      RepaymentStrategyType::CHRONON_PRIORITY,
      RepaymentStrategyType::AETHEL_PRIORITY};

  // Available resources for repayment
  double available_chronons = 300.0;
  double available_aethel = 250.0;

  // Test each strategy
  for (const auto &strategy_type : strategies) {
    std::cout << "\n\n====================================================="
              << std::endl;
    std::cout << "Testing Strategy: ";
    switch (strategy_type) {
    case RepaymentStrategyType::OLDEST_FIRST:
      std::cout << "OLDEST_FIRST";
      break;
    case RepaymentStrategyType::HIGHEST_INTEREST:
      std::cout << "HIGHEST_INTEREST";
      break;
    case RepaymentStrategyType::CRITICAL_FIRST:
      std::cout << "CRITICAL_FIRST";
      break;
    case RepaymentStrategyType::BALANCED:
      std::cout << "BALANCED";
      break;
    case RepaymentStrategyType::MINIMUM_PAYMENTS:
      std::cout << "MINIMUM_PAYMENTS";
      break;
    case RepaymentStrategyType::SNOWBALL:
      std::cout << "SNOWBALL";
      break;
    case RepaymentStrategyType::AVALANCHE:
      std::cout << "AVALANCHE";
      break;
    case RepaymentStrategyType::STABILITY_OPTIMIZED:
      std::cout << "STABILITY_OPTIMIZED";
      break;
    case RepaymentStrategyType::CHRONON_PRIORITY:
      std::cout << "CHRONON_PRIORITY";
      break;
    case RepaymentStrategyType::AETHEL_PRIORITY:
      std::cout << "AETHEL_PRIORITY";
      break;
    }
    std::cout << std::endl;
    std::cout << "====================================================="
              << std::endl;

    // Reset debts
    for (auto &debt : tracker.getAllDebts()) {
      tracker.repayChronons(debt.chronon_debt, debt.operation_id);
      tracker.repayAethel(debt.aethel_debt, debt.operation_id);
    }

    // Re-add the debts
    for (const auto &debt : debts) {
      tracker.addDebtForTesting(debt.operation_id, debt.chronon_debt,
                                debt.aethel_debt, debt.interest_rate,
                                debt.age_in_cycles, debt.is_critical, debt.type,
                                1 // operation count
      );
    }

    // Apply the strategy
    auto result = tracker.applyRepaymentStrategy(
        strategy_type, available_chronons, available_aethel);

    // Print results
    printRepaymentResult(result);

    // Print remaining debts
    std::cout << "\nRemaining Debts:" << std::endl;
    printDebts(tracker.getAllDebts());
  }

  return 0;
}