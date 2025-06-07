#ifndef CHRONOVYAN_ARRAY_H
#define CHRONOVYAN_ARRAY_H

#include <memory>
#include <string>
#include <vector>

#include "value_fwd.h"

namespace chronovyan {

// Forward declarations
class Value;
using ValuePtr = std::shared_ptr<Value>;

/**
 * @class ChronovyanArray
 * @brief Represents an array value in the Chronovyan language
 *
 * The ChronovyanArray class is a collection of values that can be accessed
 * by index and provides various array manipulation operations.
 */
class ChronovyanArray : public std::enable_shared_from_this<ChronovyanArray> {
public:
    /**
     * @brief Construct a new Chronovyan Array
     * @param elements The initial elements of the array
     */
    explicit ChronovyanArray(std::vector<ValuePtr> elements = {});

    /**
     * @brief Get the size of the array
     * @return The number of elements in the array
     */
    size_t size() const;

    /**
     * @brief Get the element at the specified index
     * @param index The index of the element to get
     * @return The element at the specified index
     * @throws std::out_of_range if the index is out of bounds
     */
    const ValuePtr& at(size_t index) const;

    /**
     * @brief Get the element at the specified index for modification
     * @param index The index of the element to get
     * @return The element at the specified index
     * @throws std::out_of_range if the index is out of bounds
     */
    ValuePtr& at(size_t index);

    /**
     * @brief Get an element from the array
     * @param index The index of the element
     * @return The value at the specified index
     * @throws std::out_of_range if the index is out of range
     */
    const Value& get(size_t index) const;

    /**
     * @brief Set an element in the array
     * @param index The index of the element
     * @param value The value to set
     * @throws std::out_of_range if the index is out of range
     */
    void set(size_t index, Value value);

    /**
     * @brief Add an element to the end of the array
     * @param value The value to add
     */
    void push(Value value);

    /**
     * @brief Remove and return the last element of the array
     * @return The last element of the array
     * @throws std::runtime_error if the array is empty
     */
    Value pop();

    /**
     * @brief Insert an element at the specified index
     * @param index The index to insert at
     * @param value The value to insert
     * @throws std::out_of_range if the index is out of range
     */
    void insert(size_t index, Value value);

    /**
     * @brief Remove an element at the specified index
     * @param index The index to remove at
     * @throws std::out_of_range if the index is out of range
     */
    void remove(size_t index);

    /**
     * @brief Get the index of the first occurrence of a value
     * @param value The value to find
     * @return The index of the value, or -1 if not found
     */
    int indexOf(const Value& value) const;

    /**
     * @brief Check if the array contains a value
     * @param value The value to check for
     * @return Whether the array contains the value
     */
    bool contains(const Value& value) const;

    /**
     * @brief Join the elements of the array into a string
     * @param separator The separator between elements
     * @return The joined string
     */
    std::string join(const std::string& separator = ", ") const;

    /**
     * @brief Get all elements of the array
     * @return A reference to the internal vector of elements
     */
    const std::vector<Value>& getElements() const;

    /**
     * @brief Get a string representation of the array
     * @return A string representation of the array
     */
    std::string toString() const;

private:
    std::vector<Value> m_elements;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_ARRAY_H