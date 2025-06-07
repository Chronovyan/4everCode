#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <stdexcept>

#include "../include/error_handler.h"
#include "../include/multi_dimensional_vector.h"

namespace chronovyan {

// Constructor with dimensions
MultiDimensionalVector::MultiDimensionalVector(const std::vector<size_t>& dimensions,
                                               std::shared_ptr<TemporalRuntime> runtime)
    : m_dimensions(dimensions), m_runtime(runtime) {
    // Calculate total size
    size_t totalSize = 1;
    for (size_t dim : dimensions) {
        totalSize *= dim;
    }

    // Create a flat vector of the appropriate size
    m_flatVector = std::make_shared<ChronovyanVector>(runtime);

    // Initialize with nil values
    for (size_t i = 0; i < totalSize; ++i) {
        m_flatVector->push_back(Value());  // Nil value
    }

    // Calculate strides for index conversion
    calculateStrides();

    // Track resource usage
    trackResourceUsage("init");
}

// Constructor with flat vector
MultiDimensionalVector::MultiDimensionalVector(std::shared_ptr<ChronovyanVector> flatVector,
                                               const std::vector<size_t>& dimensions,
                                               std::shared_ptr<TemporalRuntime> runtime)
    : m_flatVector(flatVector), m_dimensions(dimensions), m_runtime(runtime) {
    // Validate dimensions
    size_t expectedSize = 1;
    for (size_t dim : dimensions) {
        expectedSize *= dim;
    }

    if (flatVector->size() != expectedSize) {
        throw std::invalid_argument("Flat vector size doesn't match the product of dimensions");
    }

    // Calculate strides for index conversion
    calculateStrides();

    // Track resource usage
    trackResourceUsage("init_with_elements");
}

// Get number of dimensions
size_t MultiDimensionalVector::getDimensionCount() const { return m_dimensions.size(); }

// Get dimension sizes
const std::vector<size_t>& MultiDimensionalVector::getDimensions() const { return m_dimensions; }

// Get total number of elements
size_t MultiDimensionalVector::getTotalSize() const { return m_flatVector->size(); }

// Get element at multi-dimensional indices (const)
const Value& MultiDimensionalVector::at(const std::vector<size_t>& indices) const {
    if (!validateIndices(indices)) {
        throw std::out_of_range("Vector indices out of range");
    }

    size_t flatIndex = indicesToFlatIndex(indices);
    return m_flatVector->at(flatIndex);
}

// Get element at multi-dimensional indices (mutable)
Value& MultiDimensionalVector::at(const std::vector<size_t>& indices) {
    if (!validateIndices(indices)) {
        throw std::out_of_range("Vector indices out of range");
    }

    size_t flatIndex = indicesToFlatIndex(indices);
    return m_flatVector->at(flatIndex);
}

// Set element at multi-dimensional indices
void MultiDimensionalVector::set(const std::vector<size_t>& indices, const Value& value) {
    if (!validateIndices(indices)) {
        throw std::out_of_range("Vector indices out of range");
    }

    size_t flatIndex = indicesToFlatIndex(indices);
    m_flatVector->at(flatIndex) = value;

    // Track resource usage
    trackResourceUsage("set");
}

// Get a slice along a specific dimension
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::slice(size_t dimension,
                                                                      size_t index) const {
    if (dimension >= m_dimensions.size()) {
        throw std::out_of_range("Dimension index out of range");
    }

    if (index >= m_dimensions[dimension]) {
        throw std::out_of_range("Index out of range for the specified dimension");
    }

    // Create new dimensions with the specified dimension removed
    std::vector<size_t> newDimensions;
    for (size_t i = 0; i < m_dimensions.size(); ++i) {
        if (i != dimension) {
            newDimensions.push_back(m_dimensions[i]);
        }
    }

    // Calculate total size of the slice
    size_t totalSize = 1;
    for (size_t dim : newDimensions) {
        totalSize *= dim;
    }

    // Create a new flat vector for the slice
    auto sliceVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Calculate the flat indices for the slice
    for (size_t i = 0; i < totalSize; ++i) {
        // Convert i to multi-dimensional indices for the slice
        std::vector<size_t> sliceIndices;
        size_t remainingIndex = i;

        for (size_t j = 0; j < newDimensions.size(); ++j) {
            size_t dimSize = newDimensions[j];
            sliceIndices.push_back(remainingIndex % dimSize);
            remainingIndex /= dimSize;
        }

        // Convert slice indices to original indices
        std::vector<size_t> originalIndices;
        size_t slicePos = 0;

        for (size_t j = 0; j < m_dimensions.size(); ++j) {
            if (j == dimension) {
                originalIndices.push_back(index);
            } else {
                originalIndices.push_back(sliceIndices[slicePos++]);
            }
        }

        // Get the value from the original vector
        size_t flatIndex = indicesToFlatIndex(originalIndices);
        sliceVector->push_back(m_flatVector->at(flatIndex));
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("slice");

    // Create and return the new multi-dimensional vector
    return std::make_shared<MultiDimensionalVector>(sliceVector, newDimensions, m_runtime);
}

// Reshape the vector to new dimensions
bool MultiDimensionalVector::reshape(const std::vector<size_t>& newDimensions) {
    // Calculate new total size
    size_t newTotalSize = 1;
    for (size_t dim : newDimensions) {
        newTotalSize *= dim;
    }

    // Check if total size matches
    if (newTotalSize != m_flatVector->size()) {
        return false;
    }

    // Update dimensions and recalculate strides
    m_dimensions = newDimensions;
    calculateStrides();

    // Track resource usage
    trackResourceUsage("reshape");

    return true;
}

// Transpose a 2D matrix
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::transpose() const {
    // Check if it's a 2D matrix
    if (m_dimensions.size() != 2) {
        throw std::runtime_error("Transpose operation requires a 2D matrix");
    }

    // Get original dimensions
    size_t rows = m_dimensions[0];
    size_t cols = m_dimensions[1];

    // Create new dimensions for transposed matrix
    std::vector<size_t> newDimensions = {cols, rows};

    // Create a new flat vector for the transposed matrix
    auto transposedVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Fill the transposed vector
    for (size_t j = 0; j < cols; ++j) {
        for (size_t i = 0; i < rows; ++i) {
            size_t originalIndex = indicesToFlatIndex({i, j});
            transposedVector->push_back(m_flatVector->at(originalIndex));
        }
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("transpose");

    // Create and return the transposed matrix
    return std::make_shared<MultiDimensionalVector>(transposedVector, newDimensions, m_runtime);
}

// Matrix multiplication
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::matrixMultiply(
    const MultiDimensionalVector& other) const {
    // Check if both are 2D matrices
    if (m_dimensions.size() != 2 || other.getDimensions().size() != 2) {
        throw std::runtime_error("Matrix multiplication requires 2D matrices");
    }

    // Get dimensions
    size_t rows1 = m_dimensions[0];
    size_t cols1 = m_dimensions[1];
    size_t rows2 = other.getDimensions()[0];
    size_t cols2 = other.getDimensions()[1];

    // Check if matrices can be multiplied
    if (cols1 != rows2) {
        throw std::runtime_error(
            "Matrix dimensions don't match for multiplication: " + std::to_string(rows1) + "x" +
            std::to_string(cols1) + " and " + std::to_string(rows2) + "x" + std::to_string(cols2));
    }

    // Create new dimensions for result
    std::vector<size_t> resultDimensions = {rows1, cols2};

    // Create a new flat vector for the result
    auto resultVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Initialize with zeros
    for (size_t i = 0; i < rows1 * cols2; ++i) {
        resultVector->push_back(Value(static_cast<int64_t>(0)));
    }

    // Create the result multi-dimensional vector
    auto resultMatrix =
        std::make_shared<MultiDimensionalVector>(resultVector, resultDimensions, m_runtime);

    // Perform matrix multiplication
    for (size_t i = 0; i < rows1; ++i) {
        for (size_t j = 0; j < cols2; ++j) {
            Value sum(static_cast<int64_t>(0));

            for (size_t k = 0; k < cols1; ++k) {
                Value a = at({i, k});
                Value b = other.at({k, j});

                // Multiply elements
                Value product;

                if (a.isInteger() && b.isInteger()) {
                    product = Value(static_cast<int64_t>(a.asInteger() * b.asInteger()));
                } else if (a.isNumeric() && b.isNumeric()) {
                    double aVal = a.isInteger() ? a.asInteger() : a.asFloat();
                    double bVal = b.isInteger() ? b.asInteger() : b.asFloat();
                    product = Value(aVal * bVal);
                } else {
                    throw std::runtime_error("Cannot multiply non-numeric values");
                }

                // Add to sum
                if (sum.isInteger() && product.isInteger()) {
                    sum = Value(static_cast<int64_t>(sum.asInteger() + product.asInteger()));
                } else {
                    double sumVal = sum.isInteger() ? sum.asInteger() : sum.asFloat();
                    double productVal =
                        product.isInteger() ? product.asInteger() : product.asFloat();
                    sum = Value(sumVal + productVal);
                }
            }

            resultMatrix->set({i, j}, sum);
        }
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("matrixMultiply");

    return resultMatrix;
}

// Element-wise operation
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::elementWise(
    const MultiDimensionalVector& other,
    std::function<Value(const Value&, const Value&)> op) const {
    // Check if dimensions match
    if (m_dimensions.size() != other.getDimensions().size()) {
        throw std::runtime_error(
            "Vectors must have same number of dimensions for element-wise operations");
    }

    for (size_t i = 0; i < m_dimensions.size(); ++i) {
        if (m_dimensions[i] != other.getDimensions()[i]) {
            throw std::runtime_error(
                "Vectors must have same dimensions for element-wise operations");
        }
    }

    // Create a new flat vector for the result
    auto resultVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Apply the operation element-wise
    for (size_t i = 0; i < m_flatVector->size(); ++i) {
        resultVector->push_back(op(m_flatVector->at(i), other.getFlatVector()->at(i)));
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("elementWise");

    // Create and return the result
    return std::make_shared<MultiDimensionalVector>(resultVector, m_dimensions, m_runtime);
}

// Map function over elements
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::map(
    std::function<Value(const Value&)> func) const {
    // Create a new flat vector
    auto resultVector = m_flatVector->map(func);

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("map");

    // Create and return the result
    return std::make_shared<MultiDimensionalVector>(resultVector, m_dimensions, m_runtime);
}

// Reduce along a dimension
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::reduce(
    size_t dimension, std::function<Value(const Value&, const Value&)> func,
    const Value& initial) const {
    if (dimension >= m_dimensions.size()) {
        throw std::out_of_range("Dimension index out of range");
    }

    // Create new dimensions with the specified dimension removed
    std::vector<size_t> newDimensions;
    for (size_t i = 0; i < m_dimensions.size(); ++i) {
        if (i != dimension) {
            newDimensions.push_back(m_dimensions[i]);
        }
    }

    // Calculate total size of the result
    size_t totalSize = 1;
    for (size_t dim : newDimensions) {
        totalSize *= dim;
    }

    // Create a new flat vector for the result
    auto resultVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Initialize with initial values
    for (size_t i = 0; i < totalSize; ++i) {
        resultVector->push_back(initial);
    }

    // Create the result multi-dimensional vector
    auto resultMDVector =
        std::make_shared<MultiDimensionalVector>(resultVector, newDimensions, m_runtime);

    // Perform reduction
    // For each position in the result
    for (size_t i = 0; i < totalSize; ++i) {
        // Convert to multi-dimensional indices for the result
        std::vector<size_t> resultIndices = resultMDVector->flatIndexToIndices(i);

        // For each element along the reduction dimension
        for (size_t j = 0; j < m_dimensions[dimension]; ++j) {
            // Create original indices
            std::vector<size_t> originalIndices;
            size_t resultPos = 0;

            for (size_t k = 0; k < m_dimensions.size(); ++k) {
                if (k == dimension) {
                    originalIndices.push_back(j);
                } else {
                    originalIndices.push_back(resultIndices[resultPos++]);
                }
            }

            // Get value from original vector
            const Value& value = at(originalIndices);

            // Update result using the reduction function
            if (j == 0) {
                resultMDVector->set(resultIndices, value);
            } else {
                Value currentResult = resultMDVector->at(resultIndices);
                resultMDVector->set(resultIndices, func(currentResult, value));
            }
        }
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("reduce");

    return resultMDVector;
}

// Get string representation
std::string MultiDimensionalVector::toString() const {
    std::stringstream ss;

    // Special handling for common cases
    if (m_dimensions.size() == 1) {
        // 1D vector
        ss << "[";
        for (size_t i = 0; i < m_dimensions[0]; ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << at({i}).toString();
        }
        ss << "]";
    } else if (m_dimensions.size() == 2) {
        // 2D matrix
        ss << "[";
        for (size_t i = 0; i < m_dimensions[0]; ++i) {
            if (i > 0) {
                ss << ",\n ";
            }
            ss << "[";
            for (size_t j = 0; j < m_dimensions[1]; ++j) {
                if (j > 0) {
                    ss << ", ";
                }
                ss << at({i, j}).toString();
            }
            ss << "]";
        }
        ss << "]";
    } else {
        // Higher-dimensional tensors
        ss << "MultiDimensionalVector(dimensions=[";
        for (size_t i = 0; i < m_dimensions.size(); ++i) {
            if (i > 0) {
                ss << ", ";
            }
            ss << m_dimensions[i];
        }
        ss << "], size=" << getTotalSize() << ")";
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("toString");

    return ss.str();
}

// Get flat vector
std::shared_ptr<ChronovyanVector> MultiDimensionalVector::getFlatVector() const {
    return m_flatVector;
}

// Temporal Features

// Get element at cycle
Value MultiDimensionalVector::getAtCycle(const std::vector<size_t>& indices, int cycle) const {
    if (!validateIndices(indices)) {
        throw std::out_of_range("Vector indices out of range");
    }

    size_t flatIndex = indicesToFlatIndex(indices);
    return m_flatVector->getAtCycle(flatIndex, cycle);
}

// Save history checkpoint
void MultiDimensionalVector::saveHistoryCheckpoint(int cycle) {
    m_flatVector->saveHistoryCheckpoint(cycle);

    // Track resource usage
    trackResourceUsage("saveHistoryCheckpoint");
}

// Restore from cycle
bool MultiDimensionalVector::restoreFromCycle(int cycle) {
    bool success = m_flatVector->restoreFromCycle(cycle);

    if (success) {
        // Track resource usage
        trackResourceUsage("restoreFromCycle");
    }

    return success;
}

// Temporal transform
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::temporalTransform(
    int startCycle, int endCycle,
    std::function<Value(const Value&, const Value&, double)> transformFunc) const {
    // Create a new flat vector for the result
    auto resultVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Create the result multi-dimensional vector
    auto resultMDVector =
        std::make_shared<MultiDimensionalVector>(resultVector, m_dimensions, m_runtime);

    // Get the total element count
    size_t totalSize = getTotalSize();

    // For each element
    for (size_t i = 0; i < totalSize; ++i) {
        // Get the value at start and end cycles
        Value startValue = m_flatVector->getAtCycle(i, startCycle);
        Value endValue = m_flatVector->getAtCycle(i, endCycle);

        // Apply transformation function
        Value transformedValue =
            transformFunc(startValue, endValue, static_cast<double>(endCycle - startCycle));

        // Add to result
        resultVector->push_back(transformedValue);
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("temporalTransform");

    return resultMDVector;
}

// Temporal gradient
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::temporalGradient(
    int startCycle, int endCycle) const {
    if (startCycle == endCycle) {
        throw std::invalid_argument(
            "Start and end cycles must be different for gradient calculation");
    }

    // Define a gradient transformation function
    auto gradientFunc = [](const Value& start, const Value& end, double timeSpan) -> Value {
        if (start.isNumeric() && end.isNumeric()) {
            double startVal = start.isInteger() ? start.asInteger() : start.asFloat();
            double endVal = end.isInteger() ? end.asInteger() : end.asFloat();
            return Value((endVal - startVal) / timeSpan);
        } else {
            return Value();  // Nil for non-numeric values
        }
    };

    // Use temporal transform with the gradient function
    return temporalTransform(startCycle, endCycle, gradientFunc);
}

// Predict future state
std::shared_ptr<MultiDimensionalVector> MultiDimensionalVector::predictFutureState(
    int futureCycle, const std::vector<int>& historicalCycles) const {
    if (historicalCycles.empty()) {
        throw std::invalid_argument("Historical cycles cannot be empty");
    }

    // Create a new flat vector for the result
    auto resultVector = std::make_shared<ChronovyanVector>(m_runtime);

    // Get the total element count
    size_t totalSize = getTotalSize();

    // For each element
    for (size_t i = 0; i < totalSize; ++i) {
        // Collect historical values
        std::vector<std::pair<int, Value>> history;
        for (int cycle : historicalCycles) {
            history.push_back({cycle, m_flatVector->getAtCycle(i, cycle)});
        }

        // Sort by cycle
        std::sort(history.begin(), history.end(),
                  [](const auto& a, const auto& b) { return a.first < b.first; });

        // Simple linear prediction for numeric values
        if (history.size() >= 2 && history.back().second.isNumeric() &&
            history.front().second.isNumeric()) {
            double startVal = history.front().second.isInteger()
                                  ? history.front().second.asInteger()
                                  : history.front().second.asFloat();
            double endVal = history.back().second.isInteger() ? history.back().second.asInteger()
                                                              : history.back().second.asFloat();
            int startCycle = history.front().first;
            int endCycle = history.back().first;

            // Calculate slope
            double slope = (endVal - startVal) / (endCycle - startCycle);

            // Predict future value
            double predictedVal = endVal + slope * (futureCycle - endCycle);

            // Add to result
            resultVector->push_back(Value(predictedVal));
        } else {
            // For non-numeric values or insufficient history, use the most recent value
            resultVector->push_back(history.back().second);
        }
    }

    // Track resource usage
    const_cast<MultiDimensionalVector*>(this)->trackResourceUsage("predictFutureState");

    // Create the result multi-dimensional vector
    return std::make_shared<MultiDimensionalVector>(resultVector, m_dimensions, m_runtime);
}

// Private helper methods

// Convert multi-dimensional indices to flat index
size_t MultiDimensionalVector::indicesToFlatIndex(const std::vector<size_t>& indices) const {
    size_t flatIndex = 0;

    for (size_t i = 0; i < indices.size(); ++i) {
        flatIndex += indices[i] * m_strides[i];
    }

    return flatIndex;
}

// Convert flat index to multi-dimensional indices
std::vector<size_t> MultiDimensionalVector::flatIndexToIndices(size_t flatIndex) const {
    std::vector<size_t> indices(m_dimensions.size());

    for (size_t i = 0; i < m_dimensions.size(); ++i) {
        indices[i] = (flatIndex / m_strides[i]) % m_dimensions[i];
    }

    return indices;
}

// Validate indices are within bounds
bool MultiDimensionalVector::validateIndices(const std::vector<size_t>& indices) const {
    if (indices.size() != m_dimensions.size()) {
        return false;
    }

    for (size_t i = 0; i < indices.size(); ++i) {
        if (indices[i] >= m_dimensions[i]) {
            return false;
        }
    }

    return true;
}

// Calculate strides for index conversion
void MultiDimensionalVector::calculateStrides() {
    m_strides.resize(m_dimensions.size());

    size_t stride = 1;
    for (int i = static_cast<int>(m_dimensions.size()) - 1; i >= 0; --i) {
        m_strides[i] = stride;
        stride *= m_dimensions[i];
    }
}

// Track resource usage
void MultiDimensionalVector::trackResourceUsage(const std::string& operation) {
    if (m_runtime) {
        int64_t cost = getChrononsForOperation(operation);
        m_runtime->consumeChronons(static_cast<int>(cost));
    }
}

// Calculate chronon cost
int64_t MultiDimensionalVector::getChrononsForOperation(const std::string& operation) const {
    // Base cost for any operation
    int64_t baseCost = 1;

    // Scaling factor based on total size (logarithmic to be efficient for large tensors)
    double sizeFactor = getTotalSize() > 0 ? std::log2(getTotalSize() + 1) : 0;
    double dimensionFactor = m_dimensions.size();

    // Operation-specific costs
    if (operation == "init" || operation == "init_with_elements") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "set") {
        return baseCost;
    } else if (operation == "slice") {
        return baseCost + static_cast<int64_t>(sizeFactor * 1.5);
    } else if (operation == "reshape") {
        return baseCost + static_cast<int64_t>(sizeFactor);
    } else if (operation == "transpose") {
        return baseCost + static_cast<int64_t>(getTotalSize());
    } else if (operation == "matrixMultiply") {
        // Matrix multiplication is O(n^3) for n x n matrices
        if (m_dimensions.size() == 2) {
            double n = std::max(m_dimensions[0], m_dimensions[1]);
            return baseCost +
                   static_cast<int64_t>(n * n * n / 10);  // Divide by 10 to make it reasonable
        }
        return baseCost + static_cast<int64_t>(getTotalSize() * 2);
    } else if (operation == "elementWise") {
        return baseCost + static_cast<int64_t>(getTotalSize());
    } else if (operation == "map") {
        return baseCost + static_cast<int64_t>(getTotalSize());
    } else if (operation == "reduce") {
        return baseCost + static_cast<int64_t>(getTotalSize());
    } else if (operation == "toString") {
        return baseCost + static_cast<int64_t>(getTotalSize());
    } else if (operation == "saveHistoryCheckpoint" || operation == "restoreFromCycle") {
        return baseCost + static_cast<int64_t>(getTotalSize() * 2);
    } else if (operation == "temporalTransform" || operation == "temporalGradient" ||
               operation == "predictFutureState") {
        return baseCost + static_cast<int64_t>(getTotalSize() * 3);
    }

    // Default cost for unknown operations
    return baseCost + static_cast<int64_t>(sizeFactor * dimensionFactor);
}

// Register the MULTI_DIMENSIONAL_VECTOR type with the custom type system
void registerMultiDimensionalVectorType(CustomTypeSystem& typeSystem) {
    // Define properties for the MULTI_DIMENSIONAL_VECTOR type
    auto vectorProps = std::map<std::string, Value>{
        {"primitive", Value(false)},
        {"container", Value(true)},
        {"temporal", Value(true)},
        {"multi_dimensional", Value(true)},
        {"description",
         Value("Multi-dimensional vector type with temporal features and matrix operations")}};

    // Type parameters for generic vector (T and dimensions)
    auto vectorParams = std::vector<std::string>{"T", "D"};

    // Create the type definition
    auto vectorType = std::make_shared<CustomTypeDefinition>(
        "MULTI_DIMENSIONAL_VECTOR", CustomTypeKind::STRUCT, vectorProps, vectorParams);

    // Register the type
    typeSystem.registerType(vectorType);
}

}  // namespace chronovyan