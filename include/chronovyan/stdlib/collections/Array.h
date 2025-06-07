#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

class Array : public Object, public std::enable_shared_from_this<Array> {
public:
    using ElementType = ValuePtr;
    using ElementList = std::vector<ElementType>;

    // Create a new empty array
    static std::shared_ptr<Array> create() { return std::make_shared<Array>(); }

    // Create an array with initial elements
    static std::shared_ptr<Array> create(const ElementList& elements) {
        auto array = create();
        array->elements_ = elements;
        return array;
    }

    // Add an element to the end of the array
    void push(const ElementType& element) {
        if (element) {
            elements_.push_back(element);
        } else {
            elements_.push_back(NilValue::instance());
        }
    }

    // Remove and return the last element
    ElementType pop() {
        if (elements_.empty()) {
            return NilValue::instance();
        }
        auto last = elements_.back();
        elements_.pop_back();
        return last;
    }

    // Get an element at the specified index
    ElementType get(size_t index) const {
        if (index >= elements_.size()) {
            return NilValue::instance();
        }
        return elements_[index];
    }

    // Set an element at the specified index
    void set(size_t index, const ElementType& element) {
        if (index >= elements_.size()) {
            elements_.resize(index + 1);
        }
        elements_[index] = element ? element : NilValue::instance();
    }

    // Get the size of the array
    size_t size() const { return elements_.size(); }

    // Check if the array is empty
    bool empty() const { return elements_.empty(); }

    // Clear all elements from the array
    void clear() { elements_.clear(); }

    // Find the index of an element, or -1 if not found
    int64_t indexOf(const ElementType& element) const {
        auto it = std::find(elements_.begin(), elements_.end(), element);
        if (it == elements_.end()) {
            return -1;
        }
        return std::distance(elements_.begin(), it);
    }

    // Check if the array contains an element
    bool contains(const ElementType& element) const { return indexOf(element) != -1; }

    // Remove an element at the specified index
    void removeAt(size_t index) {
        if (index < elements_.size()) {
            elements_.erase(elements_.begin() + index);
        }
    }

    // Insert an element at the specified index
    void insert(size_t index, const ElementType& element) {
        if (index > elements_.size()) {
            index = elements_.size();
        }
        elements_.insert(elements_.begin() + index, element ? element : NilValue::instance());
    }

    // Get a sub-array from start to end (exclusive)
    std::shared_ptr<Array> slice(int64_t start = 0, int64_t end = -1) const {
        auto result = create();
        if (elements_.empty()) {
            return result;
        }

        // Handle negative indices
        size_t startIdx = (start < 0) ? std::max<int64_t>(0, elements_.size() + start) : start;
        size_t endIdx = (end < 0) ? elements_.size() + end + 1 : end;

        // Clamp to valid range
        startIdx = std::min(startIdx, elements_.size());
        endIdx = std::min(endIdx, elements_.size());

        if (startIdx >= endIdx) {
            return result;
        }

        result->elements_.assign(elements_.begin() + startIdx, elements_.begin() + endIdx);
        return result;
    }

    // Concatenate with another array
    std::shared_ptr<Array> concat(const std::shared_ptr<Array>& other) const {
        auto result = create();
        result->elements_ = elements_;
        result->elements_.insert(result->elements_.end(), other->elements_.begin(),
                                 other->elements_.end());
        return result;
    }

    // Iterate over elements with a function
    void forEach(const std::function<void(const ElementType&, size_t)>& callback) const {
        for (size_t i = 0; i < elements_.size(); ++i) {
            callback(elements_[i], i);
        }
    }

    // Map elements using a function
    std::shared_ptr<Array> map(
        const std::function<ElementType(const ElementType&, size_t)>& mapper) const {
        auto result = create();
        for (size_t i = 0; i < elements_.size(); ++i) {
            result->push(mapper(elements_[i], i));
        }
        return result;
    }

    // Filter elements based on a predicate
    std::shared_ptr<Array> filter(
        const std::function<bool(const ElementType&, size_t)>& predicate) const {
        auto result = create();
        for (size_t i = 0; i < elements_.size(); ++i) {
            if (predicate(elements_[i], i)) {
                result->push(elements_[i]);
            }
        }
        return result;
    }

    // Reduce the array to a single value
    ElementType reduce(
        const std::function<ElementType(const ElementType&, const ElementType&, size_t)>& reducer,
        const ElementType& initial = nullptr) const {
        if (elements_.empty()) {
            return initial ? initial : NilValue::instance();
        }

        size_t startIdx = 0;
        ElementType accumulator = initial ? initial : elements_[startIdx++];

        for (; startIdx < elements_.size(); ++startIdx) {
            accumulator = reducer(accumulator, elements_[startIdx], startIdx);
        }

        return accumulator;
    }

    // Sort the array using a comparator function
    void sort(std::function<bool(const ElementType&, const ElementType&)> comparator = {}) {
        if (comparator) {
            std::sort(elements_.begin(), elements_.end(), comparator);
        } else {
            // Default lexicographical comparison
            std::sort(elements_.begin(), elements_.end(),
                      [](const ElementType& a, const ElementType& b) {
                          return a->toString() < b->toString();
                      });
        }
    }

    // Reverse the array in place
    void reverse() { std::reverse(elements_.begin(), elements_.end()); }

    // Virtual methods from Object
    ValuePtr get(const std::string& key) const override;
    void set(const std::string& key, const ValuePtr& value) override;
    bool has(const std::string& key) const override;
    ValuePtr call(const std::vector<ValuePtr>& args) override;

    // String representation of the array
    std::string toString() const override {
        std::string result = "[";
        bool first = true;
        for (const auto& element : elements_) {
            if (!first)
                result += ", ";
            result += element ? element->toString() : "nil";
            first = false;
        }
        return result + "]";
    }

    // Get the underlying elements
    const ElementList& elements() const { return elements_; }

private:
    ElementList elements_;
};

}  // namespace chronovyan::stdlib::collections
