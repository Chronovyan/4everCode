#pragma once

#include <algorithm>
#include <unordered_map>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

class Map : public chronovyan::stdlib::Object {
public:
    using KeyType = std::string;  // For now, we'll use string keys
    using ValueType = chronovyan::stdlib::ValuePtr;
    using MapType = std::unordered_map<KeyType, ValueType>;

    // Create a new empty map
    static std::shared_ptr<Map> create() { return std::make_shared<Map>(); }

    // Create a map with initial key-value pairs
    static std::shared_ptr<Map> create(
        std::initializer_list<std::pair<const KeyType, ValueType>> init) {
        auto map = create();
        for (const auto& [key, value] : init) {
            map->set(key, value);
        }
        return map;
    }

    // Set a key-value pair
    void set(const KeyType& key, const ValueType& value) override {
        if (key.empty())
            return;
        map_[key] = value ? value : NIL;
    }

    // Get a value by key, returns nil if key doesn't exist
    ValueType get(const KeyType& key) const override {
        auto it = map_.find(key);
        return it != map_.end() ? it->second : NIL;
    }

    // Check if the map contains a key
    bool has(const KeyType& key) const override { return map_.find(key) != map_.end(); }

    // Remove a key-value pair by key, returns the removed value or nil
    ValueType remove(const KeyType& key) {
        auto it = map_.find(key);
        if (it == map_.end())
            return NIL;

        auto value = it->second;
        map_.erase(it);
        return value;
    }

    // Get all keys as an array
    ArrayPtr keys() const {
        auto result = Array::create();
        for (const auto& [key, _] : map_) {
            result->add(String::create(key));
        }
        return result;
    }

    // Get all values as an array
    ArrayPtr values() const {
        auto result = Array::create();
        for (const auto& [_, value] : map_) {
            result->add(value);
        }
        return result;
    }

    // Get all entries as an array of [key, value] pairs
    ArrayPtr entries() const {
        auto result = Array::create();
        for (const auto& [key, value] : map_) {
            auto entry = Array::create();
            entry->add(String::create(key));
            entry->add(value);
            result->add(entry);
        }
        return result;
    }

    // Get the number of key-value pairs
    size_t size() const { return map_.size(); }

    // Check if the map is empty
    bool isEmpty() const { return map_.empty(); }

    // Clear all key-value pairs
    void clear() { map_.clear(); }

    // Execute a function for each key-value pair
    void forEach(const FunctionPtr& callback) const {
        if (!callback)
            return;

        for (const auto& [key, value] : map_) {
            callback->call({String::create(key), value});
        }
    }

    // Map over the values in the map
    std::shared_ptr<Map> map(const FunctionPtr& transform) const {
        auto result = create();
        if (!transform)
            return result;

        for (const auto& [key, value] : map_) {
            auto newValue = transform->call({value, String::create(key)});
            result->set(key, newValue);
        }
        return result;
    }

    // Filter the map based on a predicate function
    std::shared_ptr<Map> filter(const FunctionPtr& predicate) const {
        auto result = create();
        if (!predicate)
            return result;

        for (const auto& [key, value] : map_) {
            auto shouldKeep = predicate->call({value, String::create(key)});
            if (shouldKeep && shouldKeep->isTruthy()) {
                result->set(key, value);
            }
        }
        return result;
    }

    // Reduce the map to a single value
    ValueType reduce(const FunctionPtr& reducer, ValueType initial = nullptr) const {
        if (!reducer)
            return NIL;

        ValueType accumulator = initial ? initial : NIL;
        size_t index = 0;

        for (const auto& [key, value] : map_) {
            accumulator = reducer->call({accumulator, value, String::create(key),
                                         Number::create(static_cast<double>(index++))});
        }

        return accumulator;
    }

    // Convert the map to an array of objects with key and value properties
    ArrayPtr toArray() const {
        auto result = Array::create();
        for (const auto& [key, value] : map_) {
            auto entry = Object::create();
            entry->set("key", String::create(key));
            entry->set("value", value);
            result->add(entry);
        }
        return result;
    }

    // Merge this map with another map
    std::shared_ptr<Map> merge(const std::shared_ptr<Map>& other) const {
        auto result = create();

        // Add all entries from this map
        for (const auto& [key, value] : map_) {
            result->set(key, value);
        }

        // Add or update with entries from the other map
        if (other) {
            for (const auto& [key, value] : other->map_) {
                result->set(key, value);
            }
        }

        return result;
    }

    // Get the underlying map
    const MapType& getMap() const { return map_; }

    // Convert the map to a string representation
    std::string toString() const override {
        std::string result = "{";
        bool first = true;

        for (const auto& [key, value] : map_) {
            if (!first)
                result += ", ";
            result += key + ": " + (value ? value->toString() : "nil");
            first = false;
        }

        return result + "}";
    }

private:
    MapType map_;
};

}  // namespace chronovyan::stdlib::collections
