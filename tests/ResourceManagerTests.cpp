#include <memory>
#include <string>

#include "catch2/catch.hpp"
#include "gtest/gtest.h"
#include "interpreter/ResourceManager.h"
#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

// Mock classes for testing
class MockTemporalRuntime : public TemporalRuntime {
public:
    MockTemporalRuntime() : TemporalRuntime() {
        m_paradoxLevel = 0;
        m_stabilizationFactor = 1.0f;
    }

    int getParadoxLevel() const override { return m_paradoxLevel; }

    void setParadoxLevel(int level) { m_paradoxLevel = level; }

    float getStabilizationFactor() const override { return m_stabilizationFactor; }

    void setStabilizationFactor(float factor) { m_stabilizationFactor = factor; }

private:
    int m_paradoxLevel;
    float m_stabilizationFactor;
};

class MockResourceOptimizer : public ResourceOptimizer {
public:
    MockResourceOptimizer() : ResourceOptimizer() { m_optimizationFactor = 1.0; }

    double getOptimizationFactor() const override { return m_optimizationFactor; }

    void setOptimizationFactor(double factor) { m_optimizationFactor = factor; }

    double optimizeResourceCost(double cost, const std::string& operation) const override {
        // Apply simple optimization based on factor
        return cost * m_optimizationFactor;
    }

private:
    double m_optimizationFactor;
};

class MockTemporalDebtTracker : public TemporalDebtTracker {
public:
    MockTemporalDebtTracker() : TemporalDebtTracker() {}

    void trackTemporalDebt(const std::string& operation, double amount) override {
        m_debtTracking[operation] += amount;
    }

    double getDebtFor(const std::string& operation) const {
        auto it = m_debtTracking.find(operation);
        if (it != m_debtTracking.end()) {
            return it->second;
        }
        return 0.0;
    }

private:
    std::unordered_map<std::string, double> m_debtTracking;
};

class ResourceManagerTest : public ::testing::Test {
protected:
    std::shared_ptr<MockTemporalRuntime> runtime;
    std::shared_ptr<MockResourceOptimizer> optimizer;
    std::shared_ptr<MockTemporalDebtTracker> debtTracker;

    ResourceManagerTest() {
        runtime = std::make_shared<MockTemporalRuntime>();
        optimizer = std::make_shared<MockResourceOptimizer>();
        debtTracker = std::make_shared<MockTemporalDebtTracker>();
    }
};

TEST_F(ResourceManagerTest, InitializesWithDefaultResourceLevels) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // Check initial aethel and chronon levels
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS);

    // Check max values
    EXPECT_EQ(manager.getMaxAethel(), ResourceManager::DEFAULT_MAX_AETHEL);
    EXPECT_EQ(manager.getMaxChronons(), ResourceManager::DEFAULT_MAX_CHRONONS);
}

TEST_F(ResourceManagerTest, ConsumesResourcesCorrectly) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // Consume some resources
    double aethelAmount = 10.0;
    double chrononsAmount = 5.0;

    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption was successful
    EXPECT_TRUE(success);

    // Check new levels
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL - aethelAmount);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS - chrononsAmount);

    // Check resource usage tracking
    auto usageHistory = manager.getResourceUsageHistory();
    EXPECT_NE(usageHistory.find("test_operation"), usageHistory.end());
    EXPECT_EQ(usageHistory.at("test_operation").aethel, aethelAmount);
    EXPECT_EQ(usageHistory.at("test_operation").chronons, chrononsAmount);
    EXPECT_EQ(usageHistory.at("test_operation").count, 1);
}

TEST_F(ResourceManagerTest, ReplenishesResourcesCorrectly) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // First consume some resources
    manager.consumeResources(20.0, 15.0, "test_operation");

    // Then replenish
    manager.replenishAethel(10.0);
    manager.replenishChronons(5.0);

    // Check new levels
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL - 10.0);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS - 10.0);
}

TEST_F(ResourceManagerTest, FailsWhenInsufficientAethel) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // Try to consume more aethel than available
    double aethelAmount = ResourceManager::DEFAULT_MAX_AETHEL + 1;
    double chrononsAmount = 5.0;

    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption failed
    EXPECT_FALSE(success);

    // Check levels remain unchanged
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS);

    // Check no usage was tracked
    auto usageHistory = manager.getResourceUsageHistory();
    EXPECT_EQ(usageHistory.find("test_operation"), usageHistory.end());
}

TEST_F(ResourceManagerTest, FailsWhenInsufficientChronons) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // Try to consume more chronons than available
    double aethelAmount = 5.0;
    double chrononsAmount = ResourceManager::DEFAULT_MAX_CHRONONS + 1;

    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption failed
    EXPECT_FALSE(success);

    // Check levels remain unchanged
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS);
}

TEST_F(ResourceManagerTest, AppliesOptimizationsCorrectly) {
    // Configure optimizer to reduce costs by 50%
    optimizer->setOptimizationFactor(0.5);

    ResourceManager manager(runtime, optimizer, debtTracker);

    double aethelAmount = 10.0;
    double chrononsAmount = 10.0;

    // With 50% optimization, should only consume 5.0 of each
    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption succeeded
    EXPECT_TRUE(success);

    // Check optimized amounts were consumed
    EXPECT_EQ(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL - 5.0);
    EXPECT_EQ(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS - 5.0);
}

TEST_F(ResourceManagerTest, AdjustsResourceCostsBasedOnParadoxLevel) {
    // Set paradox level to 2
    runtime->setParadoxLevel(2);

    ResourceManager manager(runtime, optimizer, debtTracker);

    double aethelAmount = 10.0;
    double chrononsAmount = 10.0;

    // Higher paradox level should increase resource consumption
    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption succeeded
    EXPECT_TRUE(success);

    // Check amounts were increased due to paradox level
    // The exact formula may vary, but consumption should be higher
    EXPECT_LT(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL - aethelAmount);
    EXPECT_LT(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS - chrononsAmount);
}

TEST_F(ResourceManagerTest, AdjustsResourceCostsBasedOnStabilizationFactor) {
    // Set stabilization factor to 0.5 (less stable = more expensive)
    runtime->setStabilizationFactor(0.5f);

    ResourceManager manager(runtime, optimizer, debtTracker);

    double aethelAmount = 10.0;
    double chrononsAmount = 10.0;

    // Lower stabilization factor should increase resource consumption
    bool success = manager.consumeResources(aethelAmount, chrononsAmount, "test_operation");

    // Check consumption succeeded
    EXPECT_TRUE(success);

    // Check amounts were increased due to stabilization factor
    EXPECT_LT(manager.getAethelLevel(), ResourceManager::DEFAULT_MAX_AETHEL - aethelAmount);
    EXPECT_LT(manager.getChrononsLevel(), ResourceManager::DEFAULT_MAX_CHRONONS - chrononsAmount);
}

TEST_F(ResourceManagerTest, TracksTemporalDebtCorrectly) {
    ResourceManager manager(runtime, optimizer, debtTracker);

    // Consume resources for an operation
    manager.consumeResources(10.0, 5.0, "test_operation");

    // Check that debt was tracked
    double debt = debtTracker->getDebtFor("test_operation");

    // Debt should be positive and related to resource consumption
    EXPECT_GT(debt, 0.0);
}

TEST_CASE("ResourceManager calculates temporal operation costs", "[ResourceManager]") {
    auto runtime = std::make_shared<MockTemporalRuntime>();
    auto optimizer = std::make_shared<MockResourceOptimizer>();
    auto debtTracker = std::make_shared<MockTemporalDebtTracker>();

    ResourceManager manager(runtime, optimizer, debtTracker);

    SECTION("Base costs for different operations") {
        // Check relative costs of different operations
        double rewindCost = manager.calculateTemporalOperationCost("rewind", 1.0);
        double fastforwardCost = manager.calculateTemporalOperationCost("fastforward", 1.0);
        double snapshotCost = manager.calculateTemporalOperationCost("snapshot", 1.0);
        double restoreCost = manager.calculateTemporalOperationCost("restore", 1.0);
        double stabilizeCost = manager.calculateTemporalOperationCost("stabilize", 1.0);

        // Each operation should have a non-zero cost
        REQUIRE(rewindCost > 0);
        REQUIRE(fastforwardCost > 0);
        REQUIRE(snapshotCost > 0);
        REQUIRE(restoreCost > 0);
        REQUIRE(stabilizeCost > 0);

        // Operations have different base costs according to implementation
        // (The exact relationships would depend on your implementation)
    }

    SECTION("Paradox level affects operation costs") {
        // Get cost at paradox level 0
        runtime->setParadoxLevel(0);
        double baseCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        // Increase paradox level and check if cost increases
        runtime->setParadoxLevel(2);
        double higherCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        REQUIRE(higherCost > baseCost);

        // Further increase and check again
        runtime->setParadoxLevel(4);
        double evenHigherCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        REQUIRE(evenHigherCost > higherCost);
    }

    SECTION("Stabilization factor affects operation costs") {
        // Get cost at default stabilization factor (1.0)
        runtime->setStabilizationFactor(1.0f);
        double baseCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        // Decrease stabilization factor and check if cost increases
        runtime->setStabilizationFactor(0.5f);
        double higherCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        REQUIRE(higherCost > baseCost);

        // Further decrease and check again
        runtime->setStabilizationFactor(0.2f);
        double evenHigherCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        REQUIRE(evenHigherCost > higherCost);
    }

    SECTION("Duration affects operation costs") {
        // Get cost for duration of 1.0
        double baseCost = manager.calculateTemporalOperationCost("rewind", 1.0);

        // Increase duration and check if cost increases
        double higherCost = manager.calculateTemporalOperationCost("rewind", 2.0);

        REQUIRE(higherCost > baseCost);

        // Further increase and check again
        double evenHigherCost = manager.calculateTemporalOperationCost("rewind", 5.0);

        REQUIRE(evenHigherCost > higherCost);
    }
}

TEST_CASE("ResourceManager tracks temporal debt", "[ResourceManager]") {
    auto runtime = std::make_shared<MockTemporalRuntime>();
    auto optimizer = std::make_shared<MockResourceOptimizer>();
    auto debtTracker = std::make_shared<MockTemporalDebtTracker>();

    ResourceManager manager(runtime, optimizer, debtTracker);

    SECTION("Debt is tracked for temporal operations") {
        // Consume resources for a temporal operation
        manager.consumeResources(10.0, 10.0, "rewind");

        // Check if debt was tracked
        double debt = debtTracker->getDebtFor("rewind");
        REQUIRE(debt > 0.0);
    }

    SECTION("Multiple operations accumulate debt") {
        // Consume resources for multiple operations
        manager.consumeResources(10.0, 10.0, "rewind");
        manager.consumeResources(15.0, 15.0, "rewind");
        manager.consumeResources(20.0, 20.0, "fastforward");

        // Check if debt was accumulated correctly
        double rewindDebt = debtTracker->getDebtFor("rewind");
        double fastforwardDebt = debtTracker->getDebtFor("fastforward");

        REQUIRE(rewindDebt > 0.0);
        REQUIRE(fastforwardDebt > 0.0);
        REQUIRE(rewindDebt >
                fastforwardDebt);  // Based on our implementation, rewind was called twice
    }
}