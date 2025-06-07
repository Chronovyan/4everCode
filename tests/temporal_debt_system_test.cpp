#include <memory>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

class TemporalDebtSystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

        // Initialize with some resources
        runtime->replenishChronons(100.0);
        runtime->replenishAethel(100.0);
    }

    // Store alerts for testing
    void registerAlertHandler() {
        debt_tracker->registerAlertCallback(
            [this](const DebtAlert& alert) { received_alerts.push_back(alert); });
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
    std::vector<DebtAlert> received_alerts;
};

TEST_F(TemporalDebtSystemTest, AlertSystemRaisesAppropriateAlerts) {
    registerAlertHandler();

    // Borrow enough to trigger a low alert
    double debt_limit = debt_tracker->getDebtLimit();
    double low_alert_amount = debt_limit * 0.35;  // Should trigger low alert (> 0.3)

    ASSERT_TRUE(debt_tracker->borrowChronons(low_alert_amount, "low_alert_test"));
    ASSERT_TRUE(debt_tracker->checkAlerts());

    ASSERT_FALSE(received_alerts.empty());
    ASSERT_EQ(received_alerts[0].level, DebtAlertLevel::LOW);

    // Clear alerts and borrow more to trigger medium alert
    received_alerts.clear();
    double medium_alert_amount = debt_limit * 0.2;  // Total will be 0.35 + 0.2 = 0.55 > 0.5

    ASSERT_TRUE(debt_tracker->borrowChronons(medium_alert_amount, "medium_alert_test"));
    ASSERT_TRUE(debt_tracker->checkAlerts());

    ASSERT_FALSE(received_alerts.empty());
    ASSERT_EQ(received_alerts[0].level, DebtAlertLevel::MEDIUM);

    // Clear all alerts before testing due date alerts
    received_alerts.clear();

    // We need to manually create a debt that will be due soon (in 2-3 cycles)
    // First get the current cycle
    [[maybe_unused]] int current_cycle = 0;
    for (int i = 0; i < 5; i++) {
        debt_tracker->advanceCycle();
        current_cycle++;
    }

    // Now add a debt with a due date exactly 3 cycles from now
    int due_cycle = debt_tracker->getCurrentCycle() + 3;
    debt_tracker->addDebtForTesting("due_soon_test", 50.0, 0.0, 0.05, due_cycle, true);

    // Check alerts - should trigger an approaching due date alert
    ASSERT_TRUE(debt_tracker->checkAlerts());

    // Find the approaching due date alert
    bool found_approaching_due_alert = false;
    for (const auto& alert : received_alerts) {
        if (alert.message.find("due in 3 cycles") != std::string::npos) {
            found_approaching_due_alert = true;
            break;
        }
    }

    ASSERT_TRUE(found_approaching_due_alert);
}

TEST_F(TemporalDebtSystemTest, RepaymentStrategyGeneratesRecommendations) {
    // Create some debt
    ASSERT_TRUE(debt_tracker->borrowChronons(50.0, "operation_1", true));
    ASSERT_TRUE(debt_tracker->borrowAethel(30.0, "operation_2", false));

    // Get repayment strategy
    std::string strategy = debt_tracker->suggestRepaymentStrategy();

    // Strategy should contain basic elements
    ASSERT_NE(strategy.find("TEMPORAL DEBT REPAYMENT STRATEGY"), std::string::npos);
    ASSERT_NE(strategy.find("Total Chronon Debt: 50"), std::string::npos);
    ASSERT_NE(strategy.find("Total Aethel Debt: 30"), std::string::npos);
    ASSERT_NE(strategy.find("Recommended Repayment Schedule"), std::string::npos);

    // Make some debt past due
    for (int i = 0; i < 10; i++) {
        debt_tracker->advanceCycle();
    }

    // Strategy should now contain critical info
    strategy = debt_tracker->suggestRepaymentStrategy();
    ASSERT_NE(strategy.find("CRITICAL ACTION REQUIRED"), std::string::npos);
}

TEST_F(TemporalDebtSystemTest, DebtProjectionCalculatesCorrectly) {
    // Create debt with known interest rate
    ASSERT_TRUE(debt_tracker->borrowChronons(100.0, "projection_test"));

    // Get base interest rate by examining a debt record
    auto debts = debt_tracker->getAllDebts();
    ASSERT_FALSE(debts.empty());
    double interest_rate = debts[0].interest_rate;

    // Project one cycle into the future
    double projected_debt = debt_tracker->projectFutureDebt(1);
    double expected_debt = 100.0 * (1.0 + interest_rate);

    // Use a larger epsilon for floating-point comparison
    ASSERT_NEAR(projected_debt, expected_debt, 5.0);

    // Project multiple cycles
    int projection_cycles = 5;
    projected_debt = debt_tracker->projectFutureDebt(projection_cycles);

    // Calculate compound interest manually
    expected_debt = 100.0;
    for (int i = 0; i < projection_cycles; i++) {
        expected_debt *= (1.0 + interest_rate);
    }

    // Use a larger epsilon for the compound interest calculation
    ASSERT_NEAR(projected_debt, expected_debt, 10.0);
}

TEST_F(TemporalDebtSystemTest, CalculatesMinimumSafeRepayment) {
    // Add normal debt
    ASSERT_TRUE(debt_tracker->borrowChronons(50.0, "normal_debt", false));

    // Add critical debt
    ASSERT_TRUE(debt_tracker->borrowChronons(75.0, "critical_debt", true));

    // Initially all debt is current, so minimum safe repayment should be 0
    auto min_repayment = debt_tracker->calculateMinimumSafeRepayment();
    ASSERT_EQ(min_repayment.first, 0.0);

    // Get the initial debt amount for comparison
    auto initial_debts = debt_tracker->getAllDebts();
    double initial_critical_debt = 0.0;
    for (const auto& debt : initial_debts) {
        if (debt.operation_id == "critical_debt") {
            initial_critical_debt = debt.chronon_debt;
            break;
        }
    }

    // Advance cycles until critical debt is past due
    while (debt_tracker->getPastDueDebts().empty()) {
        debt_tracker->advanceCycle();
    }

    // Now minimum safe repayment should include the critical debt (with interest)
    min_repayment = debt_tracker->calculateMinimumSafeRepayment();

    // Get the current critical debt for comparison
    auto past_due_debts = debt_tracker->getPastDueDebts();
    double current_critical_debt = 0.0;
    for (const auto& debt : past_due_debts) {
        if (debt.operation_id == "critical_debt") {
            current_critical_debt = debt.chronon_debt;
            break;
        }
    }

    // The minimum repayment should match the current debt amount
    ASSERT_NEAR(min_repayment.first, current_critical_debt, 0.001);
    ASSERT_GT(min_repayment.first, initial_critical_debt);  // Should be greater due to interest
}

TEST_F(TemporalDebtSystemTest, OptimalRepaymentProducesValidPlan) {
    // Create several debts with different properties
    ASSERT_TRUE(debt_tracker->borrowChronons(20.0, "op1", true));   // Critical
    ASSERT_TRUE(debt_tracker->borrowAethel(30.0, "op2", false));    // Normal
    ASSERT_TRUE(debt_tracker->borrowChronons(40.0, "op3", false));  // Normal

    // Make the first debt past due
    while (debt_tracker->getPastDueDebts().empty()) {
        debt_tracker->advanceCycle();
    }

    // Get the current debt amounts
    auto debts = debt_tracker->getAllDebts();
    double op1_debt = 0.0;
    [[maybe_unused]] double op2_debt = 0.0;
    double op3_debt = 0.0;

    for (const auto& debt : debts) {
        if (debt.operation_id == "op1") {
            op1_debt = debt.chronon_debt;
        } else if (debt.operation_id == "op2") {
            op2_debt = debt.aethel_debt;
        } else if (debt.operation_id == "op3") {
            op3_debt = debt.chronon_debt;
        }
    }

    // Attempt to repay with limited resources
    double available_chronons = 50.0;
    double available_aethel = 20.0;

    auto repayment_plan =
        debt_tracker->calculateOptimalRepayment(available_chronons, available_aethel);

    // Plan should include all operations
    ASSERT_EQ(repayment_plan.size(), 3);

    // Critical past due debt should be fully repaid
    ASSERT_TRUE(repayment_plan.count("op1") > 0);
    ASSERT_NEAR(repayment_plan["op1"].first, op1_debt, 0.001);

    // Aethel debt should be partially repaid
    ASSERT_TRUE(repayment_plan.count("op2") > 0);
    ASSERT_NEAR(repayment_plan["op2"].second, 20.0, 0.001);

    // Remaining chronons should go to op3
    ASSERT_TRUE(repayment_plan.count("op3") > 0);
    double expected_op3_repayment = std::min(available_chronons - op1_debt, op3_debt);
    ASSERT_NEAR(repayment_plan["op3"].first, expected_op3_repayment, 0.001);

    // Total allocated resources should match available or be less
    double allocated_chronons = 0.0;
    double allocated_aethel = 0.0;

    for (const auto& [op, amounts] : repayment_plan) {
        allocated_chronons += amounts.first;
        allocated_aethel += amounts.second;
    }

    ASSERT_LE(allocated_chronons, available_chronons);
    ASSERT_LE(allocated_aethel, available_aethel);
}

TEST_F(TemporalDebtSystemTest, CustomAlertThresholdsWork) {
    registerAlertHandler();

    // Set custom thresholds
    debt_tracker->setAlertThresholds(0.1, 0.2, 0.3, 0.4);

    // Borrow to trigger low alert with custom threshold
    double debt_limit = debt_tracker->getDebtLimit();
    double low_alert_amount = debt_limit * 0.15;  // > 0.1 but < 0.2

    ASSERT_TRUE(debt_tracker->borrowChronons(low_alert_amount, "custom_threshold_test"));
    ASSERT_TRUE(debt_tracker->checkAlerts());

    ASSERT_FALSE(received_alerts.empty());
    ASSERT_EQ(received_alerts[0].level, DebtAlertLevel::LOW);

    // Clear alerts and borrow more to trigger medium alert
    received_alerts.clear();
    double medium_alert_amount = debt_limit * 0.1;  // Total will be 0.15 + 0.1 = 0.25 > 0.2

    ASSERT_TRUE(debt_tracker->borrowChronons(medium_alert_amount, "custom_threshold_test2"));
    ASSERT_TRUE(debt_tracker->checkAlerts());

    ASSERT_FALSE(received_alerts.empty());
    ASSERT_EQ(received_alerts[0].level, DebtAlertLevel::MEDIUM);
}