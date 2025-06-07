#include <thread>

#include "../src/resource_management/resource_tracker.h"
#include "gtest/gtest.h"

using namespace chronovyan;

TEST(ResourceTrackerTest, RecordsDataPoints) {
    ResourceTracker tracker;

    // Record some sample data
    tracker.recordCurrentUsage(10.0, 5.0);
    tracker.recordCurrentUsage(15.0, 7.5);
    tracker.recordCurrentUsage(12.0, 6.0);

    // Verify data was recorded
    auto history = tracker.getHistoricalData();
    EXPECT_EQ(history.size(), 3);
    EXPECT_DOUBLE_EQ(history[0].chronon_usage, 10.0);
    EXPECT_DOUBLE_EQ(history[1].chronon_usage, 15.0);
    EXPECT_DOUBLE_EQ(history[2].chronon_usage, 12.0);
}

TEST(ResourceTrackerTest, CalculatesStatistics) {
    ResourceTracker tracker;

    // Record sample data
    tracker.recordCurrentUsage(10.0, 5.0);
    tracker.recordCurrentUsage(20.0, 10.0);

    // Verify statistics
    auto stats = tracker.getResourceStatistics();
    EXPECT_DOUBLE_EQ(stats["avg_chronon_usage"], 15.0);
    EXPECT_DOUBLE_EQ(stats["max_chronon_usage"], 20.0);
    EXPECT_DOUBLE_EQ(stats["current_chronon_usage"], 20.0);
}

TEST(ResourceTrackerTest, LimitsHistorySize) {
    // Create tracker with small history limit
    ResourceTracker tracker(2);

    // Record more data points than the limit
    tracker.recordCurrentUsage(10.0, 5.0);
    tracker.recordCurrentUsage(20.0, 10.0);
    tracker.recordCurrentUsage(30.0, 15.0);

    // Verify only the most recent points are kept
    auto history = tracker.getHistoricalData();
    EXPECT_EQ(history.size(), 2);
    EXPECT_DOUBLE_EQ(history[0].chronon_usage, 20.0);
    EXPECT_DOUBLE_EQ(history[1].chronon_usage, 30.0);
}

TEST(ResourceTrackerTest, HandlesReset) {
    ResourceTracker tracker;

    // Add some data
    tracker.recordCurrentUsage(10.0, 5.0);
    tracker.recordCurrentUsage(20.0, 10.0);

    // Verify data exists
    EXPECT_EQ(tracker.getHistoricalData().size(), 2);

    // Reset and verify empty
    tracker.reset();
    EXPECT_EQ(tracker.getHistoricalData().size(), 0);
    EXPECT_TRUE(tracker.getResourceStatistics().empty());
}

TEST(ResourceTrackerTest, TracksTemporalDebtAndParadoxRisk) {
    ResourceTracker tracker;

    // Record with debt and risk values
    tracker.recordCurrentUsage(10.0, 5.0, 2.5, 0.3);
    tracker.recordCurrentUsage(15.0, 7.5, 3.0, 0.4);

    // Verify tracking
    auto stats = tracker.getResourceStatistics();
    EXPECT_DOUBLE_EQ(stats["max_temporal_debt"], 3.0);
    EXPECT_DOUBLE_EQ(stats["max_paradox_risk"], 0.4);
    EXPECT_DOUBLE_EQ(stats["current_temporal_debt"], 3.0);
    EXPECT_DOUBLE_EQ(stats["current_paradox_risk"], 0.4);
}