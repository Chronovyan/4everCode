#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <vector>

#include "../include/multi_dimensional_vector.h"
#include "../include/temporal_runtime.h"

namespace chronovyan {
namespace {

class MultiDimensionalVectorTest : public ::testing::Test {
protected:
    void SetUp() override { runtime = std::make_shared<TemporalRuntime>(); }

    std::shared_ptr<TemporalRuntime> runtime;
};

// Tests basic construction and access
TEST_F(MultiDimensionalVectorTest, BasicConstruction) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Check dimensions
    EXPECT_EQ(mdv.getDimensionCount(), 2);
    EXPECT_EQ(mdv.getDimensions()[0], 2);
    EXPECT_EQ(mdv.getDimensions()[1], 3);
    EXPECT_EQ(mdv.getTotalSize(), 6);

    // Set some values
    mdv.set({0, 0}, Value(1));
    mdv.set({0, 1}, Value(2));
    mdv.set({0, 2}, Value(3));
    mdv.set({1, 0}, Value(4));
    mdv.set({1, 1}, Value(5));
    mdv.set({1, 2}, Value(6));

    // Check values
    EXPECT_EQ(mdv.at({0, 0}).asInteger(), 1);
    EXPECT_EQ(mdv.at({0, 1}).asInteger(), 2);
    EXPECT_EQ(mdv.at({0, 2}).asInteger(), 3);
    EXPECT_EQ(mdv.at({1, 0}).asInteger(), 4);
    EXPECT_EQ(mdv.at({1, 1}).asInteger(), 5);
    EXPECT_EQ(mdv.at({1, 2}).asInteger(), 6);
}

// Tests construction with a flat vector
TEST_F(MultiDimensionalVectorTest, ConstructionWithFlatVector) {
    // Create a flat vector
    auto flatVector = std::make_shared<ChronovyanVector>(runtime);
    for (int i = 1; i <= 6; ++i) {
        flatVector->push_back(Value(i));
    }

    // Create a 2x3 matrix from the flat vector
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(flatVector, dimensions, runtime);

    // Check values
    EXPECT_EQ(mdv.at({0, 0}).asInteger(), 1);
    EXPECT_EQ(mdv.at({0, 1}).asInteger(), 2);
    EXPECT_EQ(mdv.at({0, 2}).asInteger(), 3);
    EXPECT_EQ(mdv.at({1, 0}).asInteger(), 4);
    EXPECT_EQ(mdv.at({1, 1}).asInteger(), 5);
    EXPECT_EQ(mdv.at({1, 2}).asInteger(), 6);
}

// Tests index validation
TEST_F(MultiDimensionalVectorTest, IndexValidation) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Set a value
    mdv.set({0, 0}, Value(1));

    // Test invalid indices
    EXPECT_THROW(mdv.at({2, 0}), std::out_of_range);
    EXPECT_THROW(mdv.at({0, 3}), std::out_of_range);
    EXPECT_THROW(mdv.at({0}), std::out_of_range);
    EXPECT_THROW(mdv.at({0, 0, 0}), std::out_of_range);

    // Test setting with invalid indices
    EXPECT_THROW(mdv.set({2, 0}, Value(1)), std::out_of_range);
}

// Tests slicing
TEST_F(MultiDimensionalVectorTest, Slicing) {
    // Create a 2x3x4 tensor
    std::vector<size_t> dimensions = {2, 3, 4};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Fill with values
    int value = 1;
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            for (size_t k = 0; k < 4; ++k) {
                mdv.set({i, j, k}, Value(value++));
            }
        }
    }

    // Get a slice along the first dimension
    auto slice1 = mdv.slice(0, 0);

    // Check dimensions of the slice
    EXPECT_EQ(slice1->getDimensionCount(), 2);
    EXPECT_EQ(slice1->getDimensions()[0], 3);
    EXPECT_EQ(slice1->getDimensions()[1], 4);

    // Check values in the slice
    for (size_t j = 0; j < 3; ++j) {
        for (size_t k = 0; k < 4; ++k) {
            EXPECT_EQ(slice1->at({j, k}).asInteger(), mdv.at({0, j, k}).asInteger());
        }
    }

    // Get a slice along the second dimension
    auto slice2 = mdv.slice(1, 1);

    // Check dimensions of the slice
    EXPECT_EQ(slice2->getDimensionCount(), 2);
    EXPECT_EQ(slice2->getDimensions()[0], 2);
    EXPECT_EQ(slice2->getDimensions()[1], 4);

    // Check values in the slice
    for (size_t i = 0; i < 2; ++i) {
        for (size_t k = 0; k < 4; ++k) {
            EXPECT_EQ(slice2->at({i, k}).asInteger(), mdv.at({i, 1, k}).asInteger());
        }
    }
}

// Tests reshaping
TEST_F(MultiDimensionalVectorTest, Reshaping) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Fill with values
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            mdv.set({i, j}, Value(i * 3 + j + 1));
        }
    }

    // Reshape to 3x2
    std::vector<size_t> newDimensions = {3, 2};
    EXPECT_TRUE(mdv.reshape(newDimensions));

    // Check dimensions
    EXPECT_EQ(mdv.getDimensions()[0], 3);
    EXPECT_EQ(mdv.getDimensions()[1], 2);

    // Check values
    EXPECT_EQ(mdv.at({0, 0}).asInteger(), 1);
    EXPECT_EQ(mdv.at({0, 1}).asInteger(), 2);
    EXPECT_EQ(mdv.at({1, 0}).asInteger(), 3);
    EXPECT_EQ(mdv.at({1, 1}).asInteger(), 4);
    EXPECT_EQ(mdv.at({2, 0}).asInteger(), 5);
    EXPECT_EQ(mdv.at({2, 1}).asInteger(), 6);

    // Try to reshape to an incompatible size
    std::vector<size_t> incompatibleDimensions = {2, 2};
    EXPECT_FALSE(mdv.reshape(incompatibleDimensions));
}

// Tests transposing
TEST_F(MultiDimensionalVectorTest, Transposing) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Fill with values
    mdv.set({0, 0}, Value(1));
    mdv.set({0, 1}, Value(2));
    mdv.set({0, 2}, Value(3));
    mdv.set({1, 0}, Value(4));
    mdv.set({1, 1}, Value(5));
    mdv.set({1, 2}, Value(6));

    // Transpose the matrix
    auto transposed = mdv.transpose();

    // Check dimensions
    EXPECT_EQ(transposed->getDimensions()[0], 3);
    EXPECT_EQ(transposed->getDimensions()[1], 2);

    // Check values
    EXPECT_EQ(transposed->at({0, 0}).asInteger(), 1);
    EXPECT_EQ(transposed->at({0, 1}).asInteger(), 4);
    EXPECT_EQ(transposed->at({1, 0}).asInteger(), 2);
    EXPECT_EQ(transposed->at({1, 1}).asInteger(), 5);
    EXPECT_EQ(transposed->at({2, 0}).asInteger(), 3);
    EXPECT_EQ(transposed->at({2, 1}).asInteger(), 6);

    // Test transpose on non-2D matrix
    std::vector<size_t> dimensions3D = {2, 2, 2};
    MultiDimensionalVector mdv3D(dimensions3D, runtime);
    EXPECT_THROW(mdv3D.transpose(), std::runtime_error);
}

// Tests matrix multiplication
TEST_F(MultiDimensionalVectorTest, MatrixMultiplication) {
    // Create two matrices: 2x3 and 3x2
    std::vector<size_t> dimensions1 = {2, 3};
    std::vector<size_t> dimensions2 = {3, 2};

    MultiDimensionalVector mdv1(dimensions1, runtime);
    MultiDimensionalVector mdv2(dimensions2, runtime);

    // Fill first matrix
    mdv1.set({0, 0}, Value(1));
    mdv1.set({0, 1}, Value(2));
    mdv1.set({0, 2}, Value(3));
    mdv1.set({1, 0}, Value(4));
    mdv1.set({1, 1}, Value(5));
    mdv1.set({1, 2}, Value(6));

    // Fill second matrix
    mdv2.set({0, 0}, Value(7));
    mdv2.set({0, 1}, Value(8));
    mdv2.set({1, 0}, Value(9));
    mdv2.set({1, 1}, Value(10));
    mdv2.set({2, 0}, Value(11));
    mdv2.set({2, 1}, Value(12));

    // Multiply matrices
    auto result = mdv1.matrixMultiply(mdv2);

    // Check dimensions
    EXPECT_EQ(result->getDimensions()[0], 2);
    EXPECT_EQ(result->getDimensions()[1], 2);

    // Check values: result = [[58, 64], [139, 154]]
    EXPECT_EQ(result->at({0, 0}).asInteger(), 58);   // 1*7 + 2*9 + 3*11
    EXPECT_EQ(result->at({0, 1}).asInteger(), 64);   // 1*8 + 2*10 + 3*12
    EXPECT_EQ(result->at({1, 0}).asInteger(), 139);  // 4*7 + 5*9 + 6*11
    EXPECT_EQ(result->at({1, 1}).asInteger(), 154);  // 4*8 + 5*10 + 6*12

    // Test incompatible dimensions
    std::vector<size_t> dimensions3 = {2, 2};
    MultiDimensionalVector mdv3(dimensions3, runtime);
    EXPECT_THROW(mdv1.matrixMultiply(mdv3), std::runtime_error);

    // Test non-2D matrices
    std::vector<size_t> dimensions3D = {2, 2, 2};
    MultiDimensionalVector mdv3D(dimensions3D, runtime);
    EXPECT_THROW(mdv1.matrixMultiply(mdv3D), std::runtime_error);
}

// Tests element-wise operations
TEST_F(MultiDimensionalVectorTest, ElementWiseOperations) {
    // Create two 2x3 matrices
    std::vector<size_t> dimensions = {2, 3};

    MultiDimensionalVector mdv1(dimensions, runtime);
    MultiDimensionalVector mdv2(dimensions, runtime);

    // Fill matrices
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            mdv1.set({i, j}, Value(static_cast<int64_t>(i * 3 + j + 1)));
            mdv2.set({i, j}, Value(static_cast<int64_t>(10)));
        }
    }

    // Define addition operation
    auto addOp = [](const Value& a, const Value& b) -> Value {
        if (a.isInteger() && b.isInteger()) {
            return Value(static_cast<int64_t>(a.asInteger() + b.asInteger()));
        } else {
            return Value();
        }
    };

    // Apply element-wise addition
    auto result = mdv1.elementWise(mdv2, addOp);

    // Check values
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(result->at({i, j}).asInteger(), mdv1.at({i, j}).asInteger() + 10);
        }
    }

    // Test with incompatible dimensions
    std::vector<size_t> differentDimensions = {3, 2};
    MultiDimensionalVector mdv3(differentDimensions, runtime);
    EXPECT_THROW(mdv1.elementWise(mdv3, addOp), std::runtime_error);
}

// Tests mapping function over elements
TEST_F(MultiDimensionalVectorTest, Mapping) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Fill with values
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            mdv.set({i, j}, Value(static_cast<int64_t>(i * 3 + j + 1)));
        }
    }

    // Define a squaring function
    auto squareFunc = [](const Value& v) -> Value {
        if (v.isInteger()) {
            int64_t val = v.asInteger();
            return Value(static_cast<int64_t>(val * val));
        } else {
            return Value();
        }
    };

    // Apply the mapping
    auto result = mdv.map(squareFunc);

    // Check values
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            int64_t original = i * 3 + j + 1;
            EXPECT_EQ(result->at({i, j}).asInteger(), original * original);
        }
    }
}

// Tests reduction along a dimension
TEST_F(MultiDimensionalVectorTest, Reduction) {
    // Create a 2x3 matrix
    std::vector<size_t> dimensions = {2, 3};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Fill with values
    mdv.set({0, 0}, Value(1));
    mdv.set({0, 1}, Value(2));
    mdv.set({0, 2}, Value(3));
    mdv.set({1, 0}, Value(4));
    mdv.set({1, 1}, Value(5));
    mdv.set({1, 2}, Value(6));

    // Define a sum function
    auto sumFunc = [](const Value& a, const Value& b) -> Value {
        if (a.isInteger() && b.isInteger()) {
            return Value(static_cast<int64_t>(a.asInteger() + b.asInteger()));
        } else {
            return Value();
        }
    };

    // Reduce along the first dimension (sum of columns)
    auto result1 = mdv.reduce(0, sumFunc, Value(static_cast<int64_t>(0)));

    // Check dimensions
    EXPECT_EQ(result1->getDimensionCount(), 1);
    EXPECT_EQ(result1->getDimensions()[0], 3);

    // Check values: [1+4, 2+5, 3+6] = [5, 7, 9]
    EXPECT_EQ(result1->at({0}).asInteger(), 5);
    EXPECT_EQ(result1->at({1}).asInteger(), 7);
    EXPECT_EQ(result1->at({2}).asInteger(), 9);

    // Reduce along the second dimension (sum of rows)
    auto result2 = mdv.reduce(1, sumFunc, Value(static_cast<int64_t>(0)));

    // Check dimensions
    EXPECT_EQ(result2->getDimensionCount(), 1);
    EXPECT_EQ(result2->getDimensions()[0], 2);

    // Check values: [1+2+3, 4+5+6] = [6, 15]
    EXPECT_EQ(result2->at({0}).asInteger(), 6);
    EXPECT_EQ(result2->at({1}).asInteger(), 15);

    // Test invalid dimension
    EXPECT_THROW(mdv.reduce(2, sumFunc, Value(static_cast<int64_t>(0))), std::out_of_range);
}

// Tests string representation
TEST_F(MultiDimensionalVectorTest, StringRepresentation) {
    // Test 1D vector
    std::vector<size_t> dimensions1D = {3};
    MultiDimensionalVector mdv1D(dimensions1D, runtime);
    mdv1D.set({0}, Value(1));
    mdv1D.set({1}, Value(2));
    mdv1D.set({2}, Value(3));

    std::string str1D = mdv1D.toString();
    EXPECT_FALSE(str1D.empty());
    EXPECT_NE(str1D.find("1"), std::string::npos);
    EXPECT_NE(str1D.find("2"), std::string::npos);
    EXPECT_NE(str1D.find("3"), std::string::npos);

    // Test 2D matrix
    std::vector<size_t> dimensions2D = {2, 2};
    MultiDimensionalVector mdv2D(dimensions2D, runtime);
    mdv2D.set({0, 0}, Value(1));
    mdv2D.set({0, 1}, Value(2));
    mdv2D.set({1, 0}, Value(3));
    mdv2D.set({1, 1}, Value(4));

    std::string str2D = mdv2D.toString();
    EXPECT_FALSE(str2D.empty());
    EXPECT_NE(str2D.find("1"), std::string::npos);
    EXPECT_NE(str2D.find("2"), std::string::npos);
    EXPECT_NE(str2D.find("3"), std::string::npos);
    EXPECT_NE(str2D.find("4"), std::string::npos);

    // Test 3D tensor (should use the general representation)
    std::vector<size_t> dimensions3D = {2, 2, 2};
    MultiDimensionalVector mdv3D(dimensions3D, runtime);

    std::string str3D = mdv3D.toString();
    EXPECT_FALSE(str3D.empty());
    EXPECT_NE(str3D.find("MultiDimensionalVector"), std::string::npos);
    EXPECT_NE(str3D.find("dimensions"), std::string::npos);
    EXPECT_NE(str3D.find("size"), std::string::npos);
}

// Tests temporal features
TEST_F(MultiDimensionalVectorTest, TemporalFeatures) {
    // Create a 2x2 matrix
    std::vector<size_t> dimensions = {2, 2};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Set initial values at current cycle
    mdv.set({0, 0}, Value(1));
    mdv.set({0, 1}, Value(2));
    mdv.set({1, 0}, Value(3));
    mdv.set({1, 1}, Value(4));

    // Save history checkpoint at cycle 1
    mdv.saveHistoryCheckpoint(1);

    // Change values
    mdv.set({0, 0}, Value(5));
    mdv.set({0, 1}, Value(6));
    mdv.set({1, 0}, Value(7));
    mdv.set({1, 1}, Value(8));

    // Save history checkpoint at cycle 2
    mdv.saveHistoryCheckpoint(2);

    // Check values at cycle 1
    EXPECT_EQ(mdv.getAtCycle({0, 0}, 1).asInteger(), 1);
    EXPECT_EQ(mdv.getAtCycle({0, 1}, 1).asInteger(), 2);
    EXPECT_EQ(mdv.getAtCycle({1, 0}, 1).asInteger(), 3);
    EXPECT_EQ(mdv.getAtCycle({1, 1}, 1).asInteger(), 4);

    // Check values at cycle 2
    EXPECT_EQ(mdv.getAtCycle({0, 0}, 2).asInteger(), 5);
    EXPECT_EQ(mdv.getAtCycle({0, 1}, 2).asInteger(), 6);
    EXPECT_EQ(mdv.getAtCycle({1, 0}, 2).asInteger(), 7);
    EXPECT_EQ(mdv.getAtCycle({1, 1}, 2).asInteger(), 8);

    // Restore from cycle 1
    EXPECT_TRUE(mdv.restoreFromCycle(1));

    // Check current values (should be the values from cycle 1)
    EXPECT_EQ(mdv.at({0, 0}).asInteger(), 1);
    EXPECT_EQ(mdv.at({0, 1}).asInteger(), 2);
    EXPECT_EQ(mdv.at({1, 0}).asInteger(), 3);
    EXPECT_EQ(mdv.at({1, 1}).asInteger(), 4);

    // Try to restore from a non-existent cycle
    EXPECT_FALSE(mdv.restoreFromCycle(0));
}

// Tests temporal transform
TEST_F(MultiDimensionalVectorTest, TemporalTransform) {
    // Create a 2x2 matrix
    std::vector<size_t> dimensions = {2, 2};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Set initial values at current cycle
    mdv.set({0, 0}, Value(0));
    mdv.set({0, 1}, Value(0));
    mdv.set({1, 0}, Value(0));
    mdv.set({1, 1}, Value(0));

    // Save history checkpoint at cycle 1
    mdv.saveHistoryCheckpoint(1);

    // Change values
    mdv.set({0, 0}, Value(10));
    mdv.set({0, 1}, Value(20));
    mdv.set({1, 0}, Value(30));
    mdv.set({1, 1}, Value(40));

    // Save history checkpoint at cycle 11 (10 cycles later)
    mdv.saveHistoryCheckpoint(11);

    // Define a transformation function (average of start and end)
    auto avgFunc = [](const Value& start, const Value& end, double timeSpan) -> Value {
        if (start.isInteger() && end.isInteger()) {
            return Value(static_cast<double>((start.asInteger() + end.asInteger()) / 2.0));
        } else {
            return Value();
        }
    };

    // Apply temporal transform
    auto transformed = mdv.temporalTransform(1, 11, avgFunc);

    // Check transformed values
    EXPECT_DOUBLE_EQ(transformed->at({0, 0}).asFloat(), 5.0);
    EXPECT_DOUBLE_EQ(transformed->at({0, 1}).asFloat(), 10.0);
    EXPECT_DOUBLE_EQ(transformed->at({1, 0}).asFloat(), 15.0);
    EXPECT_DOUBLE_EQ(transformed->at({1, 1}).asFloat(), 20.0);
}

// Tests temporal gradient
TEST_F(MultiDimensionalVectorTest, TemporalGradient) {
    // Create a 2x2 matrix
    std::vector<size_t> dimensions = {2, 2};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Set initial values at current cycle
    mdv.set({0, 0}, Value(0));
    mdv.set({0, 1}, Value(0));
    mdv.set({1, 0}, Value(0));
    mdv.set({1, 1}, Value(0));

    // Save history checkpoint at cycle 1
    mdv.saveHistoryCheckpoint(1);

    // Change values
    mdv.set({0, 0}, Value(10));
    mdv.set({0, 1}, Value(20));
    mdv.set({1, 0}, Value(30));
    mdv.set({1, 1}, Value(40));

    // Save history checkpoint at cycle 11 (10 cycles later)
    mdv.saveHistoryCheckpoint(11);

    // Calculate temporal gradient
    auto gradient = mdv.temporalGradient(1, 11);

    // Check gradient values (should be change/timespan = value/10)
    EXPECT_DOUBLE_EQ(gradient->at({0, 0}).asFloat(), 1.0);
    EXPECT_DOUBLE_EQ(gradient->at({0, 1}).asFloat(), 2.0);
    EXPECT_DOUBLE_EQ(gradient->at({1, 0}).asFloat(), 3.0);
    EXPECT_DOUBLE_EQ(gradient->at({1, 1}).asFloat(), 4.0);

    // Test with equal cycles
    EXPECT_THROW(mdv.temporalGradient(1, 1), std::invalid_argument);
}

// Tests future state prediction
TEST_F(MultiDimensionalVectorTest, FutureStatePrediction) {
    // Create a 2x2 matrix
    std::vector<size_t> dimensions = {2, 2};
    MultiDimensionalVector mdv(dimensions, runtime);

    // Set values at cycle 1
    mdv.set({0, 0}, Value(10));
    mdv.set({0, 1}, Value(20));
    mdv.set({1, 0}, Value(30));
    mdv.set({1, 1}, Value(40));
    mdv.saveHistoryCheckpoint(1);

    // Set values at cycle 11
    mdv.set({0, 0}, Value(20));
    mdv.set({0, 1}, Value(40));
    mdv.set({1, 0}, Value(60));
    mdv.set({1, 1}, Value(80));
    mdv.saveHistoryCheckpoint(11);

    // Predict values at cycle 21 based on cycles 1 and 11
    auto predicted = mdv.predictFutureState(21, {1, 11});

    // Check predicted values (linear extrapolation)
    // Rate of change is 1 per cycle for {0,0}, 2 per cycle for {0,1}, etc.
    EXPECT_DOUBLE_EQ(predicted->at({0, 0}).asFloat(), 30.0);
    EXPECT_DOUBLE_EQ(predicted->at({0, 1}).asFloat(), 60.0);
    EXPECT_DOUBLE_EQ(predicted->at({1, 0}).asFloat(), 90.0);
    EXPECT_DOUBLE_EQ(predicted->at({1, 1}).asFloat(), 120.0);

    // Test with empty historical cycles
    EXPECT_THROW(mdv.predictFutureState(21, {}), std::invalid_argument);
}

}  // namespace
}  // namespace chronovyan