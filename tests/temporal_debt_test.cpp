#include <memory>

#include "gtest/gtest.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

class TemporalDebtTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        // Note: Runtime starts with 100 chronons and 100 aethel by default,
        // and we add 100 more of each, so we have 200 total to start with
        runtime->replenishChronons(100.0);
        runtime->replenishAethel(100.0);
        debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
};

TEST_F(TemporalDebtTest, BorrowChronons) {
    ASSERT_TRUE(debt_tracker->borrowChronons(20.0, "op1", false));
    EXPECT_NEAR(runtime->getChrononsLevel(), 220.0, 0.001);
    EXPECT_NEAR(debt_tracker->getTotalChrononsDebt(), 20.0, 0.001);
}

TEST_F(TemporalDebtTest, BorrowAethel) {
    ASSERT_TRUE(debt_tracker->borrowAethel(30.0, "op1", false));
    EXPECT_NEAR(runtime->getAethelLevel(), 230.0, 0.001);
    EXPECT_NEAR(debt_tracker->getTotalAethelDebt(), 30.0, 0.001);
}

// New tests for Rebel operation debt accrual

TEST_F(TemporalDebtTest, RebelOperationAcrueDebt) {
    // Test basic rebel operation debt accrual
    ASSERT_TRUE(
        debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, "rewind_op", -1.0, false));

    // Verify that Aethel was consumed
    double default_cost = debt_tracker->getDefaultAethelCost(RebelOperationType::REWIND_FLOW);
    EXPECT_NEAR(runtime->getAethelLevel(), 200.0 - default_cost, 0.001);

    // Verify debt record
    auto debts = debt_tracker->getAllDebts();
    ASSERT_EQ(debts.size(), 1);
    EXPECT_EQ(debts[0].operation_id, "rewind_op");
    EXPECT_EQ(debts[0].rebel_type, RebelOperationType::REWIND_FLOW);
    EXPECT_NEAR(debts[0].aethel_debt, default_cost, 0.001);
    EXPECT_EQ(debts[0].operation_count, 1);
}

TEST_F(TemporalDebtTest, RebelOperationWithCustomAethel) {
    // Test with custom Aethel amount
    double custom_amount = 15.0;
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::TEMPORAL_ECHO_LOOP, "echo_op",
                                              custom_amount, false));

    // Verify that the custom amount was used
    EXPECT_NEAR(runtime->getAethelLevel(), 200.0 - custom_amount, 0.001);

    // Verify debt record
    auto debts = debt_tracker->getAllDebts();
    ASSERT_EQ(debts.size(), 1);
    EXPECT_NEAR(debts[0].aethel_debt, custom_amount, 0.001);
}

TEST_F(TemporalDebtTest, RebelOperationInsufficientAethel) {
    // Set low Aethel
    runtime->consumeAethel(195.0);
    EXPECT_NEAR(runtime->getAethelLevel(), 5.0, 0.001);

    // Try to perform an operation requiring more Aethel
    ASSERT_FALSE(
        debt_tracker->accrueRebelDebt(RebelOperationType::QUANTUM_MANIPULATION, "quantum_op"));

    // Verify no debt was accrued
    EXPECT_EQ(debt_tracker->getAllDebts().size(), 0);
}

TEST_F(TemporalDebtTest, RepeatRebelOperations) {
    // Perform the same operation multiple times
    std::string op_id = "repeat_rewind";

    // First operation
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, op_id));

    // Get the interest rate and due cycle from first operation
    auto debts_after_first = debt_tracker->getAllDebts();
    ASSERT_EQ(debts_after_first.size(), 1);
    double first_interest_rate = debts_after_first[0].interest_rate;

    // Perform the same operation again
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, op_id));

    // Verify that the operation count increased and interest rate is higher
    auto debts_after_second = debt_tracker->getAllDebts();
    ASSERT_EQ(debts_after_second.size(), 1);  // Still one record, but updated
    EXPECT_EQ(debts_after_second[0].operation_count, 2);
    EXPECT_GT(debts_after_second[0].interest_rate, first_interest_rate);

    // Third operation
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, op_id));

    // Verify operation count
    EXPECT_EQ(debt_tracker->getRebelOperationCount(RebelOperationType::REWIND_FLOW, op_id), 3);
}

TEST_F(TemporalDebtTest, DifferentRebelOperationTypes) {
    // Test recording different types of rebel operations
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, "rewind_op"));
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::SUSPEND_TIMELINE, "suspend_op"));
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::SHATTER_ITERATION, "shatter_op"));

    // Verify operation counts
    EXPECT_EQ(debt_tracker->getRebelOperationCount(RebelOperationType::REWIND_FLOW), 1);
    EXPECT_EQ(debt_tracker->getRebelOperationCount(RebelOperationType::SUSPEND_TIMELINE), 1);
    EXPECT_EQ(debt_tracker->getRebelOperationCount(RebelOperationType::SHATTER_ITERATION), 1);

    // Verify total rebel debt calculation
    auto total_rebel_debt = debt_tracker->getRebelDebtTotal(RebelOperationType::NONE);
    double expected_aethel_debt =
        debt_tracker->getDefaultAethelCost(RebelOperationType::REWIND_FLOW) +
        debt_tracker->getDefaultAethelCost(RebelOperationType::SUSPEND_TIMELINE) +
        debt_tracker->getDefaultAethelCost(RebelOperationType::SHATTER_ITERATION);

    // The second element of the pair is the aethel debt
    EXPECT_NEAR(total_rebel_debt.second, expected_aethel_debt, 0.001);
}

TEST_F(TemporalDebtTest, CriticalRebelOperations) {
    // Test critical rebel operations
    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::QUANTUM_MANIPULATION,
                                              "critical_quantum", -1.0, true));

    // Verify debt record
    auto debts = debt_tracker->getAllDebts();
    ASSERT_EQ(debts.size(), 1);
    EXPECT_TRUE(debts[0].critical);

    // Critical operations should have shorter due cycles
    int expected_max_due_cycle = debt_tracker->getCurrentCycle() + 3;  // Based on implementation
    EXPECT_LE(debts[0].due_cycle, expected_max_due_cycle);
}

TEST_F(TemporalDebtTest, DefaultAethelCosts) {
    // Test the default Aethel costs for different operations
    EXPECT_GT(debt_tracker->getDefaultAethelCost(RebelOperationType::QUANTUM_MANIPULATION),
              debt_tracker->getDefaultAethelCost(RebelOperationType::REWIND_FLOW));

    EXPECT_GT(debt_tracker->getDefaultAethelCost(RebelOperationType::TEMPORAL_ECHO_LOOP),
              debt_tracker->getDefaultAethelCost(RebelOperationType::SUSPEND_TIMELINE));
}

// Main function
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}