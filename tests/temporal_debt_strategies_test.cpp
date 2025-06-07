#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "../include/resource_manager.h"
#include "../include/temporal_debt_tracker.h"

using namespace testing;
using namespace chronovyan;

class TemporalDebtStrategiesTest : public Test {
protected:
    void SetUp() override {
        // Initialize resource manager with default settings
        resourceManager = std::make_shared<ResourceManager>();

        // Initialize debt tracker with the resource manager
        debtTracker = std::make_shared<TemporalDebtTracker>(resourceManager);

        // Add some sample debts for testing
        TemporalDebt chrononDebt1{.id = "chronon-debt-1",
                                  .debtType = DebtType::CHRONON,
                                  .amount = 100.0,
                                  .dueDate = Timestamp::now() + 10000,  // Future debt
                                  .interestRate = 0.05,
                                  .isCritical = false};

        TemporalDebt chrononDebt2{.id = "chronon-debt-2",
                                  .debtType = DebtType::CHRONON,
                                  .amount = 200.0,
                                  .dueDate = Timestamp::now() - 5000,  // Past due debt
                                  .interestRate = 0.08,
                                  .isCritical = true};

        TemporalDebt aethelDebt1{.id = "aethel-debt-1",
                                 .debtType = DebtType::AETHEL,
                                 .amount = 150.0,
                                 .dueDate = Timestamp::now() + 15000,  // Future debt
                                 .interestRate = 0.03,
                                 .isCritical = false};

        TemporalDebt aethelDebt2{.id = "aethel-debt-2",
                                 .debtType = DebtType::AETHEL,
                                 .amount = 50.0,
                                 .dueDate = Timestamp::now() - 2000,  // Past due debt
                                 .interestRate = 0.06,
                                 .isCritical = true};

        // Add debts to the tracker
        debtTracker->addDebt(chrononDebt1);
        debtTracker->addDebt(chrononDebt2);
        debtTracker->addDebt(aethelDebt1);
        debtTracker->addDebt(aethelDebt2);
    }

    std::shared_ptr<ResourceManager> resourceManager;
    std::shared_ptr<TemporalDebtTracker> debtTracker;
};

TEST_F(TemporalDebtStrategiesTest, OldestFirstStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::OLDEST_FIRST);

    // Verify that past due debts were paid first
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Check that the oldest debt (chronon-debt-2) was prioritized
    bool foundPastDueChronon = false;
    for (const auto& debt : remainingDebts) {
        if (debt.id == "chronon-debt-2") {
            foundPastDueChronon = true;
            ASSERT_LT(debt.amount, 200.0);  // Should be partially or fully repaid
        }
    }

    ASSERT_TRUE(foundPastDueChronon || remainingDebts.size() < 4);
}

TEST_F(TemporalDebtStrategiesTest, HighestInterestStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::HIGHEST_INTEREST);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Check that the highest interest debt (chronon-debt-2 with 0.08) was prioritized
    bool foundHighInterestDebt = false;
    for (const auto& debt : remainingDebts) {
        if (debt.id == "chronon-debt-2") {
            foundHighInterestDebt = true;
            ASSERT_LT(debt.amount, 200.0);  // Should be partially or fully repaid
        }
    }

    ASSERT_TRUE(foundHighInterestDebt || remainingDebts.size() < 4);
}

TEST_F(TemporalDebtStrategiesTest, CriticalFirstStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::CRITICAL_FIRST);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Both critical debts should be prioritized: chronon-debt-2 and aethel-debt-2
    for (const auto& debt : remainingDebts) {
        if (debt.id == "chronon-debt-2" || debt.id == "aethel-debt-2") {
            ASSERT_LT(debt.amount, debt.id == "chronon-debt-2" ? 200.0 : 50.0);
        }
    }
}

TEST_F(TemporalDebtStrategiesTest, BalancedStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement = debtTracker->applyRepaymentStrategy(RepaymentStrategy::BALANCED);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // All debts should be reduced proportionally
    for (const auto& debt : remainingDebts) {
        if (debt.debtType == DebtType::CHRONON) {
            ASSERT_LT(debt.amount, debt.id == "chronon-debt-1" ? 100.0 : 200.0);
        } else {  // AETHEL
            ASSERT_LT(debt.amount, debt.id == "aethel-debt-1" ? 150.0 : 50.0);
        }
    }
}

TEST_F(TemporalDebtStrategiesTest, MinimumPaymentsStrategy) {
    // Set available resources - limited to force minimum payments
    resourceManager->setChronons(50.0);
    resourceManager->setAethel(30.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::MINIMUM_PAYMENTS);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // With limited resources, all debts should receive minimum payments
    for (const auto& debt : remainingDebts) {
        if (debt.debtType == DebtType::CHRONON) {
            ASSERT_LT(debt.amount, debt.id == "chronon-debt-1" ? 100.0 : 200.0);
        } else {  // AETHEL
            ASSERT_LT(debt.amount, debt.id == "aethel-debt-1" ? 150.0 : 50.0);
        }
    }
}

TEST_F(TemporalDebtStrategiesTest, SnowballStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement = debtTracker->applyRepaymentStrategy(RepaymentStrategy::SNOWBALL);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Smallest debt (aethel-debt-2 with 50.0) should be prioritized
    bool foundSmallestDebt = false;
    for (const auto& debt : remainingDebts) {
        if (debt.id == "aethel-debt-2") {
            foundSmallestDebt = true;
            ASSERT_LT(debt.amount, 50.0);  // Should be partially or fully repaid
        }
    }

    ASSERT_TRUE(foundSmallestDebt || remainingDebts.size() < 4);
}

TEST_F(TemporalDebtStrategiesTest, AvalancheStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement = debtTracker->applyRepaymentStrategy(RepaymentStrategy::AVALANCHE);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Highest interest debt (chronon-debt-2 with 0.08) should be prioritized
    bool foundHighestInterestDebt = false;
    for (const auto& debt : remainingDebts) {
        if (debt.id == "chronon-debt-2") {
            foundHighestInterestDebt = true;
            ASSERT_LT(debt.amount, 200.0);  // Should be partially or fully repaid
        }
    }

    ASSERT_TRUE(foundHighestInterestDebt || remainingDebts.size() < 4);
}

TEST_F(TemporalDebtStrategiesTest, StabilityOptimizedStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::STABILITY_OPTIMIZED);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // This strategy prioritizes debts that provide the most stability improvement per resource
    // Get remaining debts to verify results are reasonable
    auto remainingDebts = debtTracker->getAllDebts();
    ASSERT_LE(remainingDebts.size(), 4);
}

TEST_F(TemporalDebtStrategiesTest, ChronosPriorityStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::CHRONOS_PRIORITY);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Chronon debts should be prioritized
    bool chronosRepaid = false;
    for (const auto& debt : remainingDebts) {
        if (debt.debtType == DebtType::CHRONON) {
            ASSERT_LT(debt.amount, debt.id == "chronon-debt-1" ? 100.0 : 200.0);
            chronosRepaid = true;
        }
    }

    ASSERT_TRUE(chronosRepaid);
}

TEST_F(TemporalDebtStrategiesTest, AethelPriorityStrategy) {
    // Set available resources
    resourceManager->setChronons(300.0);
    resourceManager->setAethel(200.0);

    // Apply the strategy
    double stabilityImprovement =
        debtTracker->applyRepaymentStrategy(RepaymentStrategy::AETHEL_PRIORITY);

    // Verify stability improved
    ASSERT_GT(stabilityImprovement, 0.0);

    // Get remaining debts to verify
    auto remainingDebts = debtTracker->getAllDebts();

    // Aethel debts should be prioritized
    bool aethelRepaid = false;
    for (const auto& debt : remainingDebts) {
        if (debt.debtType == DebtType::AETHEL) {
            ASSERT_LT(debt.amount, debt.id == "aethel-debt-1" ? 150.0 : 50.0);
            aethelRepaid = true;
        }
    }

    ASSERT_TRUE(aethelRepaid);
}

// Test for paradox risk projection
TEST_F(TemporalDebtStrategiesTest, ParadoxRiskProjection) {
    // Test different operation types and cycles
    double risk1 = debtTracker->projectParadoxRisk(RebelOperationType::TIMELOOP, 1, 2);
    double risk2 = debtTracker->projectParadoxRisk(RebelOperationType::PARADOX_INSERTION, 3, 1);

    // Higher risk operations should have higher risk values
    ASSERT_LT(risk1, risk2);

    // Risk should be between 0 and 1
    ASSERT_GE(risk1, 0.0);
    ASSERT_LE(risk1, 1.0);
    ASSERT_GE(risk2, 0.0);
    ASSERT_LE(risk2, 1.0);

    // More operations should increase risk
    double risk3 = debtTracker->projectParadoxRisk(RebelOperationType::TIMELOOP, 1, 5);
    ASSERT_GT(risk3, risk1);
}