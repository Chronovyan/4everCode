#pragma once

#include <algorithm>
#include <vector>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

class Array : public chronovyan::stdlib::Array {
public:
    using ElementList = chronovyan::stdlib::Array::ElementList;

    // Create a new empty array
    static ArrayPtr create() { return std::make_shared<Array>(); }

    // Create an array with initial elements
    static ArrayPtr create(ElementList elements) {
        auto array = create();
        array->elements_ = std::move(elements);
        return array;
    }

    // Create an array with a specific size filled with nil
    static ArrayPtr create(size_t size) {
        auto array = create();
        array->elements_.resize(size, NIL);
        return array;
    }

    // Create an array with a range of numbers
    static ArrayPtr range(double start, double end, double step = 1.0) {
        auto array = create();
        if (step == 0)
            return array;

        if (step > 0) {
            for (double i = start; i < end; i += step) {
                array->add(Number::create(i));
            }
        } else {
            for (double i = start; i > end; i += step) {
                array->add(Number::create(i));
            }
        }

        return array;
    }

    // Add an element to the end of the array
    void add(ValuePtr value) override { elements_.push_back(value ? value : NIL); }

    // Insert an element at the specified index
    void insert(size_t index, ValuePtr value) {
        if (index > elements_.size()) {
            index = elements_.size();
        }
        elements_.insert(elements_.begin() + index, value ? value : NIL);
    }

    // Remove and return the element at the specified index
    ValuePtr removeAt(size_t index) {
        if (index >= elements_.size())
            return NIL;
        auto value = elements_[index];
        elements_.erase(elements_.begin() + index);
        return value;
    }

    // Remove the first occurrence of a value
    bool remove(const ValuePtr& value) {
        auto it = std::find_if(elements_.begin(), elements_.end(),
                               [&value](const ValuePtr& v) { return valuesEqual(v, value); });

        if (it != elements_.end()) {
            elements_.erase(it);
            return true;
        }
        return false;
    }

    // Check if the array contains a value
    bool contains(const ValuePtr& value) const {
        return std::any_of(elements_.begin(), elements_.end(),
                           [&value](const ValuePtr& v) { return valuesEqual(v, value); });
    }

    // Get the index of the first occurrence of a value, or -1 if not found
    int64_t indexOf(const ValuePtr& value) const {
        auto it = std::find_if(elements_.begin(), elements_.end(),
                               [&value](const ValuePtr& v) { return valuesEqual(v, value); });

        if (it != elements_.end()) {
            return static_cast<int64_t>(std::distance(elements_.begin(), it));
        }
        return -1;
    }

    // Get a subarray from start to end (exclusive)
    ArrayPtr slice(int64_t start, int64_t end = -1) const {
        size_t size = elements_.size();
        if (start < 0)
            start = static_cast<int64_t>(size) + start;
        if (end < 0)
            end = static_cast<int64_t>(size) + end;

        start = std::max<int64_t>(0, std::min<int64_t>(start, size));
        end = std::max<int64_t>(start, std::min<int64_t>(end, size));

        auto result = create();
        result->elements_ = ElementList(elements_.begin() + start, elements_.begin() + end);
        return result;
    }

    // Concatenate this array with another array
    ArrayPtr concat(const ArrayPtr& other) const {
        auto result = create();
        result->elements_ = elements_;
        if (other) {
            result->elements_.insert(result->elements_.end(), other->elements_.begin(),
                                     other->elements_.end());
        }
        return result;
    }

    // Apply a function to each element
    ArrayPtr map(const FunctionPtr& fn) const {
        auto result = create();
        if (!fn)
            return result;

        for (const auto& element : elements_) {
            result->add(fn->call({element}));
        }
        return result;
    }

    // Filter elements based on a predicate function
    ArrayPtr filter(const FunctionPtr& predicate) const {
        auto result = create();
        if (!predicate)
            return result;

        for (const auto& element : elements_) {
            auto shouldKeep = predicate->call({element});
            if (shouldKeep && shouldKeep->isTruthy()) {
                result->add(element);
            }
        }
        return result;
    }

    // Reduce the array to a single value
    ValuePtr reduce(const FunctionPtr& reducer, ValuePtr initial = nullptr) const {
        if (!reducer)
            return NIL;

        ValuePtr accumulator = initial ? initial : (elements_.empty() ? NIL : elements_[0]);
        size_t start = initial ? 0 : 1;

        for (size_t i = start; i < elements_.size(); ++i) {
            accumulator =
                reducer->call({accumulator, elements_[i], Number::create(static_cast<double>(i))});
        }

        return accumulator;
    }

    // Sort the array in place using a comparison function
    void sort(const FunctionPtr& compare = nullptr) {
        if (compare) {
            std::sort(elements_.begin(), elements_.end(),
                      [&compare](const ValuePtr& a, const ValuePtr& b) {
                          auto result = compare->call({a, b});
                          return result && result->isTruthy();
                      });
        } else {
            // Default lexicographical comparison
            std::sort(elements_.begin(), elements_.end(), [](const ValuePtr& a, const ValuePtr& b) {
                if (!a)
                    return true;
                if (!b)
                    return false;
                return a->toString() < b->toString();
            });
        }
    }

    // Reverse the array in place
    void reverse() { std::reverse(elements_.begin(), elements_.end()); }

    // Get the first element, or nil if empty
    ValuePtr first() const { return elements_.empty() ? NIL : elements_.front(); }

    // Get the last element, or nil if empty
    ValuePtr last() const { return elements_.empty() ? NIL : elements_.back(); }

    // Remove and return the last element
    ValuePtr pop() {
        if (elements_.empty())
            return NIL;
        auto value = elements_.back();
        elements_.pop_back();
        return value;
    }

    // Add elements to the end of the array and return the new length
    size_t push(const ValuePtr& value) {
        add(value);
        return elements_.size();
    }

    // Remove and return the first element
    ValuePtr shift() {
        if (elements_.empty())
            return NIL;
        auto value = elements_.front();
        elements_.erase(elements_.begin());
        return value;
    }

    // Add elements to the beginning of the array and return the new length
    size_t unshift(const ValuePtr& value) {
        elements_.insert(elements_.begin(), value ? value : NIL);
        return elements_.size();
    }

    // Join all elements into a string with the given separator
    StringPtr join(const std::string& separator = ",") const {
        std::string result;
        bool first = true;

        for (const auto& element : elements_) {
            if (!first) {
                result += separator;
            }
            result += element ? element->toString() : "nil";
            first = false;
        }

        return String::create(result);
    }

    // Get the array as a vector of strings
    std::vector<std::string> toStringVector() const {
        std::vector<std::string> result;
        for (const auto& element : elements_) {
            result.push_back(element ? element->toString() : "nil");
        }
        return result;
    }

    // Get the array as a vector of numbers
    std::vector<double> toNumberVector() const {
        std::vector<double> result;
        for (const auto& element : elements_) {
            result.push_back(toNumber(element)->value());
        }
        return result;
    }

    // Clear all elements from the array
    void clear() { elements_.clear(); }

    // Check if the array is empty
    bool isEmpty() const { return elements_.empty(); }

    // Get the number of elements in the array
    size_t length() const { return elements_.size(); }

    // Get the underlying element list
    const ElementList& elements() const override { return elements_; }

    // Convert the array to a string representation
    std::string toString() const override { return chronovyan::stdlib::Array::toString(); }

private:
    ElementList elements_;
};

}  // namespace chronovyan::stdlib::collections
