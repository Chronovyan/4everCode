#include <gtest/gtest.h>

#include "chronovyan/stdlib/collections/Array.h"
#include "chronovyan/stdlib/core/Core.h"

using namespace chronovyan::stdlib;
using namespace chronovyan::stdlib::collections;

class ArrayTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        emptyArray = Array::create();

        // Create an array with some initial values
        testArray = Array::create();
        testArray->push(Number::create(1));
        testArray->push(Number::create(2));
        testArray->push(Number::create(3));
    }

    ArrayPtr emptyArray;
    ArrayPtr testArray;
};

TEST_F(ArrayTest, TestEmptyArray) {
    EXPECT_TRUE(emptyArray->empty());
    EXPECT_EQ(emptyArray->size(), 0);
}

TEST_F(ArrayTest, TestPushAndGet) {
    emptyArray->push(Number::create(42));
    EXPECT_FALSE(emptyArray->empty());
    EXPECT_EQ(emptyArray->size(), 1);

    auto value = emptyArray->get(0);
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(value)->value(), 42);
}

TEST_F(ArrayTest, TestSet) {
    testArray->set(1, Number::create(99));
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(1))->value(), 99);

    // Test out of bounds set
    EXPECT_NO_THROW(testArray->set(10, Number::create(100)));  // Should resize
    EXPECT_EQ(testArray->size(), 11);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(10))->value(), 100);
}

TEST_F(ArrayTest, TestPop) {
    auto value = testArray->pop();
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(value->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(value)->value(), 3);
    EXPECT_EQ(testArray->size(), 2);

    // Pop from empty array
    emptyArray->pop();  // Should not crash
    EXPECT_TRUE(emptyArray->empty());
}

TEST_F(ArrayTest, TestShiftAndUnshift) {
    testArray->unshift(Number::create(0));
    EXPECT_EQ(testArray->size(), 4);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(0))->value(), 0);

    auto value = testArray->shift();
    ASSERT_NE(value, nullptr);
    EXPECT_EQ(std::static_pointer_cast<Number>(value)->value(), 0);
    EXPECT_EQ(testArray->size(), 3);

    // Shift from empty array
    emptyArray->shift();  // Should not crash
    EXPECT_TRUE(emptyArray->empty());
}

TEST_F(ArrayTest, TestSplice) {
    // Remove 1 element at index 1
    auto removed = testArray->splice(1, 1);
    EXPECT_EQ(removed->size(), 1);
    EXPECT_EQ(std::static_pointer_cast<Number>(removed->get(0))->value(), 2);
    EXPECT_EQ(testArray->size(), 2);

    // Insert elements at index 1
    removed = testArray->splice(1, 0, {Number::create(10), Number::create(20)});
    EXPECT_TRUE(removed->empty());
    EXPECT_EQ(testArray->size(), 4);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(1))->value(), 10);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(2))->value(), 20);

    // Test negative index (should count from end)
    removed = testArray->splice(-1, 1);
    EXPECT_EQ(removed->size(), 1);
    EXPECT_EQ(std::static_pointer_cast<Number>(removed->get(0))->value(), 3);
    EXPECT_EQ(testArray->size(), 3);
}

TEST_F(ArrayTest, TestSlice) {
    auto sliced = testArray->slice(1, 3);
    EXPECT_EQ(sliced->size(), 2);
    EXPECT_EQ(std::static_pointer_cast<Number>(sliced->get(0))->value(), 2);
    EXPECT_EQ(std::static_pointer_cast<Number>(sliced->get(1))->value(), 3);

    // Test negative indices
    sliced = testArray->slice(-2, -1);
    EXPECT_EQ(sliced->size(), 1);
    EXPECT_EQ(std::static_pointer_cast<Number>(sliced->get(0))->value(), 2);

    // Test out of bounds
    sliced = testArray->slice(10, 20);
    EXPECT_TRUE(sliced->empty());
}

TEST_F(ArrayTest, TestConcat) {
    auto otherArray = Array::create({Number::create(4), Number::create(5)});
    auto concatenated = testArray->concat(otherArray);

    EXPECT_EQ(concatenated->size(), 5);
    for (int i = 0; i < 5; ++i) {
        EXPECT_EQ(std::static_pointer_cast<Number>(concatenated->get(i))->value(), i + 1);
    }

    // Original arrays should remain unchanged
    EXPECT_EQ(testArray->size(), 3);
    EXPECT_EQ(otherArray->size(), 2);
}

TEST_F(ArrayTest, TestForEach) {
    int sum = 0;
    testArray->forEach([&sum](const ValuePtr& value, size_t index) {
        if (value->type() == Type::NUMBER) {
            sum += static_cast<int>(std::static_pointer_cast<Number>(value)->value());
        }
    });

    EXPECT_EQ(sum, 6);  // 1 + 2 + 3
}

TEST_F(ArrayTest, TestMap) {
    auto doubled = testArray->map([](const ValuePtr& value, size_t index) -> ValuePtr {
        if (value->type() == Type::NUMBER) {
            double num = std::static_pointer_cast<Number>(value)->value();
            return Number::create(num * 2);
        }
        return value;
    });

    EXPECT_EQ(doubled->size(), 3);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get(0))->value(), 2.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get(1))->value(), 4.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(doubled->get(2))->value(), 6.0);
}

TEST_F(ArrayTest, TestFilter) {
    // Filter even numbers
    auto evens = testArray->filter([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            int num = static_cast<int>(std::static_pointer_cast<Number>(value)->value());
            return num % 2 == 0;
        }
        return false;
    });

    EXPECT_EQ(evens->size(), 1);
    EXPECT_EQ(std::static_pointer_cast<Number>(evens->get(0))->value(), 2.0);
}

TEST_F(ArrayTest, TestReduce) {
    // Sum all elements
    auto sum = testArray->reduce(
        [](const ValuePtr& acc, const ValuePtr& value, size_t index) -> ValuePtr {
            double currentSum = acc ? std::static_pointer_cast<Number>(acc)->value() : 0.0;
            if (value->type() == Type::NUMBER) {
                currentSum += std::static_pointer_cast<Number>(value)->value();
            }
            return Number::create(currentSum);
        },
        Number::create(0.0));

    ASSERT_NE(sum, nullptr);
    EXPECT_EQ(sum->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(sum)->value(), 6.0);

    // Test without initial value
    sum =
        testArray->reduce([](const ValuePtr& acc, const ValuePtr& value, size_t index) -> ValuePtr {
            double currentSum = acc ? std::static_pointer_cast<Number>(acc)->value() : 0.0;
            if (value->type() == Type::NUMBER) {
                currentSum += std::static_pointer_cast<Number>(value)->value();
            }
            return Number::create(currentSum);
        });

    ASSERT_NE(sum, nullptr);
    EXPECT_EQ(sum->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(sum)->value(), 6.0);
}

TEST_F(ArrayTest, TestFind) {
    // Find first even number
    auto found = testArray->find([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            int num = static_cast<int>(std::static_pointer_cast<Number>(value)->value());
            return num % 2 == 0;
        }
        return false;
    });

    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->type(), Type::NUMBER);
    EXPECT_EQ(std::static_pointer_cast<Number>(found)->value(), 2.0);

    // Test not found
    found = testArray->find([](const ValuePtr& value, size_t index) -> bool {
        return false;  // No element matches
    });

    EXPECT_EQ(found, nullptr);
}

TEST_F(ArrayTest, TestFindIndex) {
    // Find index of first even number
    int index = testArray->findIndex([](const ValuePtr& value, size_t i) -> bool {
        if (value->type() == Type::NUMBER) {
            int num = static_cast<int>(std::static_pointer_cast<Number>(value)->value());
            return num % 2 == 0;
        }
        return false;
    });

    EXPECT_EQ(index, 1);  // Index of 2 is 1

    // Test not found
    index = testArray->findIndex([](const ValuePtr& value, size_t i) -> bool {
        return false;  // No element matches
    });

    EXPECT_EQ(index, -1);
}

TEST_F(ArrayTest, TestSome) {
    // Check if any element is even
    bool hasEven = testArray->some([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            int num = static_cast<int>(std::static_pointer_cast<Number>(value)->value());
            return num % 2 == 0;
        }
        return false;
    });

    EXPECT_TRUE(hasEven);

    // Check for non-existent condition
    bool hasNegative = testArray->some([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            return std::static_pointer_cast<Number>(value)->value() < 0;
        }
        return false;
    });

    EXPECT_FALSE(hasNegative);
}

TEST_F(ArrayTest, TestEvery) {
    // Check if all elements are positive
    bool allPositive = testArray->every([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            return std::static_pointer_cast<Number>(value)->value() > 0;
        }
        return false;
    });

    EXPECT_TRUE(allPositive);

    // Add a non-positive number
    testArray->push(Number::create(0));

    allPositive = testArray->every([](const ValuePtr& value, size_t index) -> bool {
        if (value->type() == Type::NUMBER) {
            return std::static_pointer_cast<Number>(value)->value() > 0;
        }
        return false;
    });

    EXPECT_FALSE(allPositive);
}

TEST_F(ArrayTest, TestSort) {
    // Add more elements for better testing
    testArray->push(Number::create(5));
    testArray->push(Number::create(4));

    // Default sort (ascending)
    testArray->sort();

    EXPECT_EQ(testArray->size(), 5);
    for (size_t i = 0; i < testArray->size(); ++i) {
        EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(i))->value(),
                  static_cast<double>(i + 1));
    }

    // Custom sort (descending)
    testArray->sort([](const ValuePtr& a, const ValuePtr& b) -> bool {
        return std::static_pointer_cast<Number>(a)->value() >
               std::static_pointer_cast<Number>(b)->value();
    });

    for (size_t i = 0; i < testArray->size(); ++i) {
        EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(i))->value(),
                  static_cast<double>(testArray->size() - i));
    }
}

TEST_F(ArrayTest, TestReverse) {
    testArray->reverse();

    EXPECT_EQ(testArray->size(), 3);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(0))->value(), 3.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(1))->value(), 2.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(2))->value(), 1.0);

    // Reverse back
    testArray->reverse();
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(0))->value(), 1.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(1))->value(), 2.0);
    EXPECT_EQ(std::static_pointer_cast<Number>(testArray->get(2))->value(), 3.0);
}

// Add more tests for edge cases and additional functionality
