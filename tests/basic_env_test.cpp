#include "gtest/gtest.h"

// A simple test case
TEST(BasicTest, SimpleAssert) {
    EXPECT_EQ(2 + 2, 4);
    EXPECT_NE(2 + 2, 5);
}

// Another test case
TEST(BasicTest, BooleanAssertions) {
    EXPECT_TRUE(true);
    EXPECT_FALSE(false);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}