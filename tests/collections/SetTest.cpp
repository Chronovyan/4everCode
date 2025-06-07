#include <gtest/gtest.h>

#include "chronovyan/stdlib/collections/Array.h"
#include "chronovyan/stdlib/collections/Set.h"
#include "chronovyan/stdlib/core/Core.h"

using namespace chronovyan::stdlib;
using namespace chronovyan::stdlib::collections;

class SetTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Set up test data
        emptySet = Set::create();

        // Create a set with numbers 1, 2, 3
        auto numbers = Array::create();
        numbers->add(Number::create(1));
        numbers->add(Number::create(2));
        numbers->add(Number::create(3));
        numberSet = Set::create(numbers->elements());

        // Create another set with overlapping and new elements
        auto moreNumbers = Array::create();
        moreNumbers->add(Number::create(3));
        moreNumbers->add(Number::create(4));
        moreNumbers->add(Number::create(5));
        otherSet = Set::create(moreNumbers->elements());
    }

    SetPtr emptySet;
    SetPtr numberSet;
    SetPtr otherSet;
};

TEST_F(SetTest, TestEmptySet) {
    EXPECT_TRUE(emptySet->isEmpty());
    EXPECT_TRUE(emptySet->empty());
    EXPECT_EQ(emptySet->size(), 0);
    EXPECT_FALSE(emptySet->has(Number::create(1)));
}

TEST_F(SetTest, TestAddAndContains) {
    // Test adding elements
    EXPECT_TRUE(emptySet->isEmpty());
    emptySet->add(Number::create(42));
    EXPECT_FALSE(emptySet->isEmpty());
    EXPECT_EQ(emptySet->size(), 1);
    EXPECT_TRUE(emptySet->contains(Number::create(42)));
    EXPECT_FALSE(emptySet->contains(Number::create(99)));

    // Test has() method
    EXPECT_TRUE(emptySet->has(Number::create(42)));
    EXPECT_FALSE(emptySet->has(Number::create(99)));

    // Test adding the same element again (should be idempotent)
    emptySet->add(Number::create(42));
    EXPECT_EQ(emptySet->size(), 1);
}

TEST_F(SetTest, TestRemove) {
    // Test removing existing element
    auto value = Number::create(1);
    EXPECT_TRUE(numberSet->contains(value));
    EXPECT_EQ(numberSet->size(), 3);

    bool removed = numberSet->remove(value);
    EXPECT_TRUE(removed);
    EXPECT_FALSE(numberSet->contains(value));
    EXPECT_EQ(numberSet->size(), 2);

    // Test removing non-existent element
    removed = numberSet->remove(Number::create(99));
    EXPECT_FALSE(removed);
    EXPECT_EQ(numberSet->size(), 2);
}

TEST_F(SetTest, TestClear) {
    EXPECT_FALSE(numberSet->isEmpty());
    numberSet->clear();
    EXPECT_TRUE(numberSet->isEmpty());
    EXPECT_EQ(numberSet->size(), 0);

    // Clearing an empty set should be safe
    emptySet->clear();
    EXPECT_TRUE(emptySet->isEmpty());
}

TEST_F(SetTest, TestUnion) {
    auto unionSet = numberSet->unionWith(otherSet);

    // Should contain elements from both sets (1,2,3,4,5)
    EXPECT_EQ(unionSet->size(), 5);
    for (int i = 1; i <= 5; ++i) {
        EXPECT_TRUE(unionSet->contains(Number::create(i)));
    }

    // Original sets should remain unchanged
    EXPECT_EQ(numberSet->size(), 3);
    EXPECT_EQ(otherSet->size(), 3);
}

TEST_F(SetTest, TestIntersect) {
    auto intersection = numberSet->intersect(otherSet);

    // Should only contain the common element (3)
    EXPECT_EQ(intersection->size(), 1);
    EXPECT_TRUE(intersection->contains(Number::create(3)));

    // Test with no intersection
    auto emptyIntersection = numberSet->intersect(Set::create());
    EXPECT_TRUE(emptyIntersection->isEmpty());
}

TEST_F(SetTest, TestDifference) {
    auto difference = numberSet->difference(otherSet);

    // Should contain elements in numberSet but not in otherSet (1,2)
    EXPECT_EQ(difference->size(), 2);
    EXPECT_TRUE(difference->contains(Number::create(1)));
    EXPECT_TRUE(difference->contains(Number::create(2)));
    EXPECT_FALSE(difference->contains(Number::create(3)));

    // Test with empty set
    auto allElements = numberSet->difference(Set::create());
    EXPECT_EQ(allElements->size(), numberSet->size());
}

TEST_F(SetTest, TestIsSubset) {
    // numberSet: {1,2,3}
    // otherSet: {3,4,5}

    // Create a subset {1,2}
    auto subset = Set::create({Number::create(1), Number::create(2)});

    EXPECT_FALSE(numberSet->isSubsetOf(otherSet));
    EXPECT_FALSE(otherSet->isSubsetOf(numberSet));
    EXPECT_TRUE(subset->isSubsetOf(numberSet));

    // Every set is a subset of itself
    EXPECT_TRUE(numberSet->isSubsetOf(numberSet));

    // Empty set is a subset of every set
    EXPECT_TRUE(emptySet->isSubsetOf(numberSet));
}

TEST_F(SetTest, TestToArray) {
    auto array = numberSet->toArray();
    EXPECT_EQ(array->size(), 3);

    // Convert back to set to check contents (order doesn't matter)
    auto newSet = Set::create(array->elements());
    EXPECT_EQ(newSet->size(), 3);
    for (int i = 1; i <= 3; ++i) {
        EXPECT_TRUE(newSet->contains(Number::create(i)));
    }
}

TEST_F(SetTest, TestForEach) {
    std::vector<int> found;
    numberSet->forEach([&](const ValuePtr& value) {
        found.push_back(std::static_pointer_cast<Number>(value)->value());
    });

    EXPECT_EQ(found.size(), 3);
    std::sort(found.begin(), found.end());
    EXPECT_EQ(found, std::vector<int>({1, 2, 3}));
}

TEST_F(SetTest, TestFilter) {
    // Filter for even numbers
    auto evenSet = numberSet->filter([](const ValuePtr& value) {
        return std::static_pointer_cast<Number>(value)->value() % 2 == 0;
    });

    EXPECT_EQ(evenSet->size(), 1);
    EXPECT_TRUE(evenSet->contains(Number::create(2)));
}

TEST_F(SetTest, TestToString) {
    // The exact string format might vary, but it should contain all elements
    std::string str = numberSet->toString();
    EXPECT_NE(str.find('1'), std::string::npos);
    EXPECT_NE(str.find('2'), std::string::npos);
    EXPECT_NE(str.find('3'), std::string::npos);
}

TEST_F(SetTest, TestObjectInterface) {
    // Test get() method
    auto one = Number::create(1);
    EXPECT_EQ(numberSet->get("1")->toString(), "1");
    EXPECT_EQ(numberSet->get("nonexistent")->type(), Type::NIL);

    // Test set() method (should add the value)
    auto four = Number::create(4);
    numberSet->set("4", four);
    EXPECT_TRUE(numberSet->contains(four));

    // Test has() with string key
    EXPECT_TRUE(numberSet->has("1"));
    EXPECT_FALSE(numberSet->has("99"));

    // Test call() method
    auto result = numberSet->call({one});
    EXPECT_EQ(result->type(), Type::BOOLEAN);
    EXPECT_TRUE(std::static_pointer_cast<Boolean>(result)->value());

    // Test call() with multiple arguments
    result = numberSet->call({one, Number::create(2), Number::create(99)});
    EXPECT_EQ(result->type(), Type::ARRAY);
    auto resultArray = std::static_pointer_cast<Array>(result);
    EXPECT_EQ(resultArray->size(), 3);
    EXPECT_TRUE(std::static_pointer_cast<Boolean>(resultArray->get(0))->value());
    EXPECT_TRUE(std::static_pointer_cast<Boolean>(resultArray->get(1))->value());
    EXPECT_FALSE(std::static_pointer_cast<Boolean>(resultArray->get(2))->value());
}
