#include <iomanip>
#include <iostream>
#include <memory>

#include "../include/multi_dimensional_vector.h"
#include "../include/resource_visualization.h"
#include "../include/temporal_runtime.h"

using namespace chronovyan;

// Helper function to print a separator line
void printSeparator() { std::cout << "\n" << std::string(70, '-') << "\n" << std::endl; }

// Helper function to create and initialize a 2D matrix
std::shared_ptr<MultiDimensionalVector> createMatrix(int rows, int cols,
                                                     std::shared_ptr<TemporalRuntime> runtime) {
    std::vector<size_t> dimensions = {static_cast<size_t>(rows), static_cast<size_t>(cols)};
    auto matrix = std::make_shared<MultiDimensionalVector>(dimensions, runtime);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix->set({static_cast<size_t>(i), static_cast<size_t>(j)},
                        Value(static_cast<int64_t>(i * cols + j + 1)));
        }
    }

    return matrix;
}

// Helper function to print a matrix
void printMatrix(const std::shared_ptr<MultiDimensionalVector>& matrix, const std::string& name) {
    std::cout << name << " (" << matrix->getDimensions()[0] << "x" << matrix->getDimensions()[1]
              << "):" << std::endl;
    std::cout << matrix->toString() << std::endl;
}

int main() {
    // Initialize the temporal runtime
    auto runtime = std::make_shared<TemporalRuntime>();
    runtime->setChrononsLimit(10000);  // Set a high limit for the demo

    std::cout << "Chronovyan VECTOR Type Demonstration" << std::endl;
    printSeparator();

    // Basic 1D Vector Operations
    std::cout << "1. Basic 1D Vector Operations" << std::endl;
    std::vector<size_t> dimensions1D = {5};
    auto vector1D = std::make_shared<MultiDimensionalVector>(dimensions1D, runtime);

    std::cout << "Creating a 1D vector of size 5..." << std::endl;
    for (size_t i = 0; i < 5; ++i) {
        vector1D->set({i}, Value(static_cast<int64_t>(i * i)));
    }

    std::cout << "Vector values (squares of indices): " << vector1D->toString() << std::endl;

    // Mapping a function over a vector
    std::cout << "Applying a function (multiply by 2) to each element..." << std::endl;
    auto doubledVector = vector1D->map([](const Value& v) -> Value {
        if (v.isInteger()) {
            return Value(static_cast<int64_t>(v.asInteger() * 2));
        }
        return v;
    });

    std::cout << "Doubled vector: " << doubledVector->toString() << std::endl;
    printSeparator();

    // 2D Matrix Operations
    std::cout << "2. 2D Matrix Operations" << std::endl;

    // Create a 3x4 matrix
    auto matrixA = createMatrix(3, 4, runtime);
    printMatrix(matrixA, "Matrix A");

    // Reshape the matrix
    std::cout << "Reshaping Matrix A from 3x4 to 4x3..." << std::endl;
    std::vector<size_t> newDimensions = {4, 3};
    if (matrixA->reshape(newDimensions)) {
        printMatrix(matrixA, "Reshaped Matrix A");
    } else {
        std::cout << "Reshape failed due to incompatible dimensions." << std::endl;
    }

    // Create two matrices for multiplication
    auto matrixB = createMatrix(3, 2, runtime);
    auto matrixC = createMatrix(2, 4, runtime);

    printMatrix(matrixB, "Matrix B");
    printMatrix(matrixC, "Matrix C");

    // Matrix multiplication
    std::cout << "Performing matrix multiplication: B * C..." << std::endl;
    try {
        auto matrixProduct = matrixB->matrixMultiply(*matrixC);
        printMatrix(matrixProduct, "Matrix B * C");
    } catch (const std::exception& e) {
        std::cout << "Matrix multiplication error: " << e.what() << std::endl;
    }

    // Transposing a matrix
    std::cout << "Transposing Matrix B..." << std::endl;
    auto matrixBTransposed = matrixB->transpose();
    printMatrix(matrixBTransposed, "Matrix B Transposed");

    // Element-wise operations
    std::cout << "Creating two 2x2 matrices for element-wise operations..." << std::endl;
    auto matrixD = createMatrix(2, 2, runtime);
    auto matrixE = std::make_shared<MultiDimensionalVector>(std::vector<size_t>{2, 2}, runtime);

    // Fill matrixE with constant values
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            matrixE->set({i, j}, Value(static_cast<int64_t>(10)));
        }
    }

    printMatrix(matrixD, "Matrix D");
    printMatrix(matrixE, "Matrix E");

    // Element-wise addition
    std::cout << "Performing element-wise addition: D + E..." << std::endl;
    auto additionResult =
        matrixD->elementWise(*matrixE, [](const Value& a, const Value& b) -> Value {
            if (a.isInteger() && b.isInteger()) {
                return Value(static_cast<int64_t>(a.asInteger() + b.asInteger()));
            }
            return Value();
        });

    printMatrix(additionResult, "Matrix D + E");
    printSeparator();

    // 3D Tensor Operations
    std::cout << "3. 3D Tensor Operations" << std::endl;
    std::vector<size_t> dimensions3D = {2, 3, 4};
    auto tensor3D = std::make_shared<MultiDimensionalVector>(dimensions3D, runtime);

    // Fill the tensor with values
    int value = 1;
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            for (size_t k = 0; k < 4; ++k) {
                tensor3D->set({i, j, k}, Value(static_cast<int64_t>(value++)));
            }
        }
    }

    std::cout << "Created a 3D tensor with dimensions 2x3x4" << std::endl;
    std::cout << "Total elements: " << tensor3D->getTotalSize() << std::endl;

    // Slicing operations
    std::cout << "Extracting a 2D slice (first element along first dimension)..." << std::endl;
    auto slice = tensor3D->slice(0, 0);
    std::cout << "Slice dimensions: " << slice->getDimensions()[0] << "x"
              << slice->getDimensions()[1] << std::endl;
    std::cout << "Slice values:" << std::endl << slice->toString() << std::endl;
    printSeparator();

    // Temporal Features
    std::cout << "4. Temporal Features" << std::endl;

    // Create a 2x2 matrix for temporal operations
    auto temporalMatrix = createMatrix(2, 2, runtime);
    printMatrix(temporalMatrix, "Initial Matrix");

    // Save the current state at cycle 1
    std::cout << "Saving the current state at cycle 1..." << std::endl;
    temporalMatrix->saveHistoryCheckpoint(1);

    // Modify the matrix
    std::cout << "Modifying the matrix..." << std::endl;
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 2; ++j) {
            temporalMatrix->set(
                {i, j}, Value(static_cast<int64_t>(temporalMatrix->at({i, j}).asInteger() * 2)));
        }
    }

    printMatrix(temporalMatrix, "Modified Matrix");

    // Save the current state at cycle 2
    std::cout << "Saving the current state at cycle 2..." << std::endl;
    temporalMatrix->saveHistoryCheckpoint(2);

    // Display values at different cycles
    std::cout << "Value at position (0,0) at cycle 1: "
              << temporalMatrix->getAtCycle({0, 0}, 1).asInteger() << std::endl;
    std::cout << "Value at position (0,0) at cycle 2: "
              << temporalMatrix->getAtCycle({0, 0}, 2).asInteger() << std::endl;

    // Restore from cycle 1
    std::cout << "Restoring the matrix from cycle 1..." << std::endl;
    if (temporalMatrix->restoreFromCycle(1)) {
        printMatrix(temporalMatrix, "Restored Matrix");
    } else {
        std::cout << "Restore failed." << std::endl;
    }

    // Temporal gradient
    std::cout << "Calculating temporal gradient between cycles 1 and 2..." << std::endl;
    auto gradient = temporalMatrix->temporalGradient(1, 2);
    printMatrix(gradient, "Temporal Gradient");

    // Predict future state
    std::cout << "Predicting state at cycle 3 based on cycles 1 and 2..." << std::endl;
    auto predicted = temporalMatrix->predictFutureState(3, {1, 2});
    printMatrix(predicted, "Predicted State at Cycle 3");
    printSeparator();

    // Resource Usage Tracking
    std::cout << "5. Resource Usage Tracking" << std::endl;

    // Get chronons consumed
    int chrononsUsed = runtime->getChrononsUsed();
    std::cout << "Total Chronons used during this demonstration: " << chrononsUsed << std::endl;

    // Visualize resource usage if available
    try {
        ResourceVisualization resourceViz;
        std::cout << "Resource usage visualization:" << std::endl;
        resourceViz.generateResourceUsageReport(*runtime);
    } catch (const std::exception& e) {
        std::cout << "Resource visualization not available: " << e.what() << std::endl;
    }

    printSeparator();
    std::cout << "VECTOR Type Demonstration Completed" << std::endl;

    return 0;
}