#ifndef CHRONOVYAN_MULTI_DIMENSIONAL_VECTOR_H
#define CHRONOVYAN_MULTI_DIMENSIONAL_VECTOR_H

#include <memory>
#include <vector>

#include "temporal_runtime.h"
#include "value.h"
#include "vector_type.h"

namespace chronovyan {

/**
 * @class MultiDimensionalVector
 * @brief Extended vector implementation for multi-dimensional data
 *
 * This class extends ChronovyanVector to support multi-dimensional operations,
 * matrix mathematics, and advanced temporal transformations specific to
 * multi-dimensional data.
 */
class MultiDimensionalVector {
public:
    /**
     * @brief Create a new empty multi-dimensional vector with specified dimensions
     * @param dimensions Vector of dimension sizes (e.g., [3, 4] for 3x4 matrix)
     * @param runtime Optional temporal runtime for resource tracking
     */
    MultiDimensionalVector(const std::vector<size_t>& dimensions,
                           std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Create a new multi-dimensional vector from a flat vector
     * @param flatVector The flat vector data
     * @param dimensions Vector of dimension sizes
     * @param runtime Optional temporal runtime for resource tracking
     */
    MultiDimensionalVector(std::shared_ptr<ChronovyanVector> flatVector,
                           const std::vector<size_t>& dimensions,
                           std::shared_ptr<TemporalRuntime> runtime = nullptr);

    /**
     * @brief Get the number of dimensions
     * @return The dimensionality
     */
    size_t getDimensionCount() const;

    /**
     * @brief Get the size of each dimension
     * @return Vector of dimension sizes
     */
    const std::vector<size_t>& getDimensions() const;

    /**
     * @brief Get the total number of elements
     * @return The total size
     */
    size_t getTotalSize() const;

    /**
     * @brief Get an element at a specific multi-dimensional index
     * @param indices The indices in each dimension
     * @return The value at the specified indices
     * @throws std::out_of_range if indices are out of bounds
     */
    const Value& at(const std::vector<size_t>& indices) const;

    /**
     * @brief Get a mutable reference to an element
     * @param indices The indices in each dimension
     * @return Mutable reference to the element
     * @throws std::out_of_range if indices are out of bounds
     */
    Value& at(const std::vector<size_t>& indices);

    /**
     * @brief Set an element at a specific multi-dimensional index
     * @param indices The indices in each dimension
     * @param value The value to set
     * @throws std::out_of_range if indices are out of bounds
     */
    void set(const std::vector<size_t>& indices, const Value& value);

    /**
     * @brief Get a slice along a specific dimension
     * @param dimension The dimension to slice along
     * @param index The index in that dimension
     * @return A new multi-dimensional vector with one less dimension
     * @throws std::out_of_range if dimension or index is out of bounds
     */
    std::shared_ptr<MultiDimensionalVector> slice(size_t dimension, size_t index) const;

    /**
     * @brief Reshape the vector to new dimensions
     * @param newDimensions The new dimensions
     * @return True if successful, false if dimensions don't match total size
     */
    bool reshape(const std::vector<size_t>& newDimensions);

    /**
     * @brief Transpose the vector (swap dimensions)
     * @return A new transposed multi-dimensional vector
     * @throws std::runtime_error if not a 2D matrix
     */
    std::shared_ptr<MultiDimensionalVector> transpose() const;

    /**
     * @brief Matrix multiplication (for 2D matrices)
     * @param other The other matrix to multiply with
     * @return The resulting matrix
     * @throws std::runtime_error if dimensions don't match for multiplication
     */
    std::shared_ptr<MultiDimensionalVector> matrixMultiply(
        const MultiDimensionalVector& other) const;

    /**
     * @brief Element-wise operation on two vectors
     * @param other The other vector
     * @param op The operation function
     * @return The resulting vector
     * @throws std::runtime_error if dimensions don't match
     */
    std::shared_ptr<MultiDimensionalVector> elementWise(
        const MultiDimensionalVector& other,
        std::function<Value(const Value&, const Value&)> op) const;

    /**
     * @brief Apply a function to each element
     * @param func The function to apply
     * @return A new vector with the mapped values
     */
    std::shared_ptr<MultiDimensionalVector> map(std::function<Value(const Value&)> func) const;

    /**
     * @brief Reduce along a dimension
     * @param dimension The dimension to reduce along
     * @param func The reduction function
     * @param initial The initial value
     * @return A new vector with one less dimension
     * @throws std::out_of_range if dimension is out of bounds
     */
    std::shared_ptr<MultiDimensionalVector> reduce(
        size_t dimension, std::function<Value(const Value&, const Value&)> func,
        const Value& initial) const;

    /**
     * @brief Get a string representation
     * @return String representation
     */
    std::string toString() const;

    /**
     * @brief Get the underlying flat vector
     * @return The flat vector
     */
    std::shared_ptr<ChronovyanVector> getFlatVector() const;

    // Temporal features

    /**
     * @brief Get the element value at specific indices and temporal cycle
     * @param indices The indices in each dimension
     * @param cycle The temporal cycle
     * @return The value at the specified indices and cycle
     * @throws std::out_of_range if indices are out of bounds
     */
    Value getAtCycle(const std::vector<size_t>& indices, int cycle) const;

    /**
     * @brief Save the current state as a history checkpoint
     * @param cycle The cycle to save for
     */
    void saveHistoryCheckpoint(int cycle);

    /**
     * @brief Restore the vector state from a checkpoint
     * @param cycle The cycle to restore from
     * @return True if successful, false if the cycle doesn't exist
     */
    bool restoreFromCycle(int cycle);

    /**
     * @brief Time-variant transformation
     * @param startCycle The starting cycle
     * @param endCycle The ending cycle
     * @param transformFunc Function to transform between cycles
     * @return A new vector representing the transformation
     */
    std::shared_ptr<MultiDimensionalVector> temporalTransform(
        int startCycle, int endCycle,
        std::function<Value(const Value&, const Value&, double)> transformFunc) const;

    /**
     * @brief Compute temporal gradient
     * @param startCycle The starting cycle
     * @param endCycle The ending cycle
     * @return A new vector representing the gradient over time
     */
    std::shared_ptr<MultiDimensionalVector> temporalGradient(int startCycle, int endCycle) const;

    /**
     * @brief Predict future state based on temporal patterns
     * @param futureCycle The cycle to predict
     * @param historicalCycles List of historical cycles to use for prediction
     * @return A new vector representing the predicted state
     */
    std::shared_ptr<MultiDimensionalVector> predictFutureState(
        int futureCycle, const std::vector<int>& historicalCycles) const;

private:
    // Convert multi-dimensional indices to flat index
    size_t indicesToFlatIndex(const std::vector<size_t>& indices) const;

    // Convert flat index to multi-dimensional indices
    std::vector<size_t> flatIndexToIndices(size_t flatIndex) const;

    // Validate that indices are within bounds
    bool validateIndices(const std::vector<size_t>& indices) const;

    // Calculate strides for each dimension
    void calculateStrides();

    // Track resource usage for operations
    void trackResourceUsage(const std::string& operation);

    // Calculate chronon cost for an operation
    int64_t getChrononsForOperation(const std::string& operation) const;

    std::shared_ptr<ChronovyanVector> m_flatVector;
    std::vector<size_t> m_dimensions;
    std::vector<size_t> m_strides;
    std::shared_ptr<TemporalRuntime> m_runtime;
};

// Register the MULTI_DIMENSIONAL_VECTOR type with the custom type system
void registerMultiDimensionalVectorType(CustomTypeSystem& typeSystem);

}  // namespace chronovyan

#endif  // CHRONOVYAN_MULTI_DIMENSIONAL_VECTOR_H