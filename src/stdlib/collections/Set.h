#pragma once

#include <unordered_set>

#include "chronovyan/stdlib/collections/Array.h"

namespace chronovyan::stdlib::collections {

class Set : public chronovyan::stdlib::Object {
public:
    using ElementType = chronovyan::stdlib::ValuePtr;
    using SetType = std::unordered_set<ElementType, ValueHash, ValueEqual>;

    // Virtual methods from Object
    ValuePtr get(const std::string& key) const override;
    void set(const std::string& key, const ValuePtr& value) override;
    bool has(const std::string& key) const override;
    ValuePtr call(const std::vector<ValuePtr>& args) override;

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
            set_.insert(element);
        }
    }

    // Remove an element from the set, returns true if the element was present
    bool remove(const ElementType& element) {
        if (!element)
            return false;
        return set_.erase(element) > 0;
    }

    // Check if the set contains an element
    bool has(const ElementType& element) const {
        if (!element)
            return false;
        return set_.find(element) != set_.end();
    }

    // Get the number of elements in the set
    size_t size() const { return set_.size(); }

    // Check if the set is empty
    bool isEmpty() const { return set_.empty(); }

    // Clear all elements from the set
    void clear() { set_.clear(); }

    // Create a union with another set
    std::shared_ptr<Set> unionWith(const std::shared_ptr<Set>& other) const {
        auto result = create();

        // Add all elements from this set
        for (const auto& element : set_) {
            result->add(element);
        }

        // Add all elements from the other set
        if (other) {
            for (const auto& element : other->set_) {
                result->add(element);
            }
        }

        return result;
    }

    // Create an intersection with another set
    std::shared_ptr<Set> intersect(const std::shared_ptr<Set>& other) const {
        auto result = create();
        if (!other)
            return result;

        for (const auto& element : set_) {
            if (other->has(element)) {
                result->add(element);
            }
        }

        return result;
    }

    // Create a set with elements from this set that are not in the other set
    std::shared_ptr<Set> difference(const std::shared_ptr<Set>& other) const {
        auto result = create();

        for (const auto& element : set_) {
            if (!other || !other->has(element)) {
                result->add(element);
            }
        }

        return result;
    }

    // Check if this set is a subset of another set
    bool isSubsetOf(const std::shared_ptr<Set>& other) const {
        if (!other)
            return false;
        if (set_.size() > other->size())
            return false;

        for (const auto& element : set_) {
            if (!other->has(element)) {
                return false;
            }
        }

        return true;
    }

    // Convert the set to an array
    ArrayPtr toArray() const {
        auto result = Array::create();
        for (const auto& element : set_) {
            result->add(element);
        }
        return result;
    }

    // Execute a function for each element in the set
    void forEach(const FunctionPtr& callback) const {
        if (!callback)
            return;

        for (const auto& element : set_) {
            callback->call({element});
        }
    }

    // Create a new set with elements that pass the test implemented by the provided function
    std::shared_ptr<Set> filter(const FunctionPtr& predicate) const {
        auto result = create();
        if (!predicate)
            return result;

        for (const auto& element : set_) {
            auto shouldKeep = predicate->call({element});
            if (shouldKeep && shouldKeep->isTruthy()) {
                result->add(element);
            }
        }
        return result;
    }

    // Convert the set to a string representation
    std::string toString() const override {
        std::string result = "{";
        bool first = true;

        for (const auto& element : set_) {
            if (!first)
                result += ", ";
            result += element ? element->toString() : "nil";
            first = false;
        }

        return result + "}";
    }

private:
    // Custom hash function for ValuePtr
    struct ValueHash {
        size_t operator()(const ElementType& value) const {
            if (!value)
                return 0;
            return std::hash<std::string>{}(value->toString());
        }
    };

    // Custom equality function for ValuePtr
    struct ValueEqual {
        bool operator()(const ElementType& a, const ElementType& b) const {
            if (!a && !b)
                return true;
            if (!a || !b)
                return false;
            return valuesEqual(a, b);
        }
    };

    SetType set_;
};

}  // namespace chronovyan::stdlib::collections
