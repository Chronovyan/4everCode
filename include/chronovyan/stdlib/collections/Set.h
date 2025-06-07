#pragma once

#include <algorithm>
#include <functional>
#include <unordered_set>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

class Set : public Object, public std::enable_shared_from_this<Set> {
public:
    using ElementType = ValuePtr;
    using SetType = std::unordered_set<ElementType, std::hash<ElementType>, std::equal_to<>>;

    // Create a new empty set
    static std::shared_ptr<Set> create() { return std::make_shared<Set>(); }

    // Create a set with initial elements
    static std::shared_ptr<Set> create(const std::vector<ElementType>& elements) {
        auto set = create();
        for (const auto& element : elements) {
            set->add(element);
        }
        return set;
    }

    // Add an element to the set
    void add(const ElementType& element) {
        if (element) {
            elements_.insert(element);
        } else {
            elements_.insert(NilValue::instance());
        }
    }

    // Remove an element from the set
    // Returns true if the element was removed, false if it wasn't found
    bool remove(const ElementType& element) {
        if (element) {
            return elements_.erase(element) > 0;
        } else {
            return elements_.erase(NilValue::instance()) > 0;
        }
    }

    // Check if the set contains an element
    bool contains(const ElementType& element) const {
        if (element) {
            return elements_.find(element) != elements_.end();
        }
        return elements_.find(NilValue::instance()) != elements_.end();
    }

    // Get the size of the set
    size_t size() const { return elements_.size(); }

    // Check if the set is empty
    bool empty() const { return elements_.empty(); }

    // Alias for empty() for API consistency
    bool isEmpty() const { return empty(); }

    // Clear all elements from the set
    void clear() { elements_.clear(); }

    // Create a union with another set
    std::shared_ptr<Set> unionWith(const std::shared_ptr<Set>& other) const {
        auto result = create();
        result->elements_ = elements_;
        result->elements_.insert(other->elements_.begin(), other->elements_.end());
        return result;
    }

    // Create an intersection with another set
    std::shared_ptr<Set> intersect(const std::shared_ptr<Set>& other) const {
        auto result = create();
        for (const auto& element : elements_) {
            if (other->contains(element)) {
                result->add(element);
            }
        }
        return result;
    }

    // Create a difference with another set (elements in this set but not in the other)
    std::shared_ptr<Set> difference(const std::shared_ptr<Set>& other) const {
        auto result = create();
        for (const auto& element : elements_) {
            if (!other->contains(element)) {
                result->add(element);
            }
        }
        return result;
    }

    // Check if this set is a subset of another set
    bool isSubsetOf(const std::shared_ptr<Set>& other) const {
        for (const auto& element : elements_) {
            if (!other->contains(element)) {
                return false;
            }
        }
        return true;
    }

    // Convert the set to a vector
    std::vector<ElementType> toVector() const { return {elements_.begin(), elements_.end()}; }

    // Convert the set to an array
    ArrayPtr toArray() const {
        auto array = Array::create();
        for (const auto& element : elements_) {
            array->add(element);
        }
        return array;
    }

    // Iterate over elements with a function
    void forEach(const std::function<void(const ElementType&)>& callback) const {
        for (const auto& element : elements_) {
            callback(element);
        }
    }

    // Filter elements based on a predicate
    std::shared_ptr<Set> filter(const std::function<bool(const ElementType&)>& predicate) const {
        auto result = create();
        for (const auto& element : elements_) {
            if (predicate(element)) {
                result->add(element);
            }
        }
        return result;
    }

    // Virtual methods from Object
    ValuePtr get(const std::string& key) const override;
    void set(const std::string& /*key*/, const ValuePtr& value) override {
        // For Set, setting a key is equivalent to adding the value
        add(value);
    }
    bool has(const std::string& key) const override;
    ValuePtr call(const std::vector<ValuePtr>& args) override;

    // String representation of the set
    std::string toString() const override {
        std::string result = "{";
        bool first = true;
        for (const auto& element : elements_) {
            if (!first)
                result += ", ";
            result += element ? element->toString() : "nil";
            first = false;
        }
        return result + "}";
    }

    // Implementation of Value interface
    Type type() const override {
        return Type::OBJECT;  // Or a specific type if you have one for Set
    }

    // Clone the set (deep copy)
    ValuePtr clone() const override {
        auto newSet = create();
        for (const auto& element : elements_) {
            newSet->add(element ? element->clone() : nullptr);
        }
        return newSet;
    }

    // Support copy and move
    Set() = default;
    Set(const Set&) = default;
    Set(Set&&) = default;
    Set& operator=(const Set&) = default;
    Set& operator=(Set&&) = default;

private:
    SetType elements_;
};

}  // namespace chronovyan::stdlib::collections
