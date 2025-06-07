#pragma once

#include <algorithm>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

class Map : public Object, public std::enable_shared_from_this<Map> {
public:
    using KeyType = std::string;
    using ValueType = ValuePtr;
    using MapType = std::unordered_map<KeyType, ValueType>;

    // Inherit constructors from Object
    using Object::Object;

    // Create a new empty map
    static std::shared_ptr<Map> create() { return std::make_shared<Map>(); }

    // Create a map with initial key-value pairs
    static std::shared_ptr<Map> create(const std::vector<std::pair<KeyType, ValueType>>& entries) {
        auto map = create();
        for (const auto& [key, value] : entries) {
            map->set(key, value);
        }
        return map;
    }

    // Set a key-value pair in the map
    void set(const KeyType& key, const ValueType& value) {
        if (value) {
            entries_[key] = value;
        } else {
            entries_[key] = NilValue::instance();
        }
    }

    // Get a value by key
    ValueType get(const KeyType& key) const {
        auto it = entries_.find(key);
        if (it == entries_.end()) {
            return NilValue::instance();
        }
        return it->second;
    }

    // Check if the map contains a key
    bool has(const KeyType& key) const { return entries_.find(key) != entries_.end(); }

    // Remove a key-value pair by key
    void remove(const KeyType& key) { entries_.erase(key); }

    // Get the number of key-value pairs in the map
    size_t size() const { return entries_.size(); }

    // Check if the map is empty
    bool empty() const { return entries_.empty(); }

    // Clear all key-value pairs from the map
    void clear() { entries_.clear(); }

    // Get all keys in the map
    std::vector<KeyType> keys() const {
        std::vector<KeyType> result;
        result.reserve(entries_.size());
        for (const auto& [key, _] : entries_) {
            result.push_back(key);
        }
        return result;
    }

    // Get all values in the map
    std::vector<ValueType> values() const {
        std::vector<ValueType> result;
        result.reserve(entries_.size());
        for (const auto& [_, value] : entries_) {
            result.push_back(value);
        }
        return result;
    }

    // Get all entries (key-value pairs) in the map
    std::vector<std::pair<KeyType, ValueType>> entries() const {
        return {entries_.begin(), entries_.end()};
    }

    // Merge this map with another map (this takes precedence for duplicate keys)
    std::shared_ptr<Map> merge(const std::shared_ptr<Map>& other) const {
        auto result = create();
        // First add all entries from the other map
        result->entries_ = other->entries_;
        // Then add/overwrite with entries from this map
        result->entries_.insert(entries_.begin(), entries_.end());
        return result;
    }

    // Filter the map based on a predicate
    std::shared_ptr<Map> filter(
        const std::function<bool(const KeyType&, const ValueType&)>& predicate) const {
        auto result = create();
        for (const auto& [key, value] : entries_) {
            if (predicate(key, value)) {
                result->set(key, value);
            }
        }
        return result;
    }

    // Map the values of the map to new values
    std::shared_ptr<Map> map(
        const std::function<ValueType(const KeyType&, const ValueType&)>& mapper) const {
        auto result = create();
        for (const auto& [key, value] : entries_) {
            result->set(key, mapper(key, value));
        }
        return result;
    }

    // Reduce the map to a single value
    ValueType reduce(
        const std::function<ValueType(const ValueType&, const KeyType&, const ValueType&)>& reducer,
        const ValueType& initial = nullptr) const {
        if (entries_.empty()) {
            return initial ? initial : NilValue::instance();
        }

        auto it = entries_.begin();
        ValueType accumulator = initial ? initial : it->second;

        if (initial) {
            // If we have an initial value, process all entries
            for (; it != entries_.end(); ++it) {
                accumulator = reducer(accumulator, it->first, it->second);
            }
        } else {
            // If no initial value, start from the first entry and skip it in the loop
            for (++it; it != entries_.end(); ++it) {
                accumulator = reducer(accumulator, it->first, it->second);
            }
        }

        return accumulator;
    }

    // Iterate over key-value pairs with a function
    void forEach(const std::function<void(const KeyType&, const ValueType&)>& callback) const {
        for (const auto& [key, value] : entries_) {
            callback(key, value);
        }
    }

    // Virtual methods from Object
    // Override Object methods with string keys
    ValuePtr get(const std::string& key) const override {
        auto it = entries_.find(key);
        return it != entries_.end() ? it->second : NilValue::instance();
    }

    void set(const std::string& key, const ValuePtr& value) override {
        if (value) {
            entries_[key] = value;
        } else {
            entries_[key] = NilValue::instance();
        }
    }

    bool has(const std::string& key) const override { return entries_.find(key) != entries_.end(); }

    ValuePtr call(const std::vector<ValuePtr>& args) override {
        if (args.empty()) {
            return shared_from_this();
        }

        // Handle map methods
        if (args[0]->type() == Type::STRING) {
            std::string method = std::static_pointer_cast<String>(args[0])->value();

            if (method == "set" && args.size() > 2) {
                set(args[1]->toString(), args[2]);
                return shared_from_this();
            } else if (method == "get" && args.size() > 1) {
                return get(args[1]->toString());
            } else if (method == "has" && args.size() > 1) {
                return Boolean::create(has(args[1]->toString()));
            } else if ((method == "delete" || method == "remove") && args.size() > 1) {
                bool existed = has(args[1]->toString());
                entries_.erase(args[1]->toString());
                return Boolean::create(existed);
            } else if (method == "clear") {
                entries_.clear();
                return shared_from_this();
            } else if (method == "size") {
                return Number::create(static_cast<double>(entries_.size()));
            } else if (method == "keys") {
                auto keysArray = Array::create();
                for (const auto& [key, _] : entries_) {
                    keysArray->add(String::create(key));
                }
                return keysArray;
            } else if (method == "values") {
                auto valuesArray = Array::create();
                for (const auto& [_, value] : entries_) {
                    valuesArray->add(value);
                }
                return valuesArray;
            } else if (method == "entries") {
                auto entriesArray = Array::create();
                for (const auto& [key, value] : entries_) {
                    auto entry = Array::create();
                    entry->add(String::create(key));
                    entry->add(value);
                    entriesArray->add(entry);
                }
                return entriesArray;
            } else if (method == "forEach" && args.size() > 1 &&
                       args[1]->type() == Type::FUNCTION) {
                // Handle forEach with a callback function
                auto func = std::static_pointer_cast<Function>(args[1]);
                for (const auto& [key, value] : entries_) {
                    std::vector<ValuePtr> funcArgs = {value, String::create(key),
                                                      shared_from_this()};
                    func->call(funcArgs);
                }
                return shared_from_this();
            } else if (method == "map" && args.size() > 1 && args[1]->type() == Type::FUNCTION) {
                // Handle map with a callback function that returns new values
                auto func = std::static_pointer_cast<Function>(args[1]);
                auto result = create();
                for (const auto& [key, value] : entries_) {
                    std::vector<ValuePtr> funcArgs = {value, String::create(key),
                                                      shared_from_this()};
                    auto newValue = func->call(funcArgs);
                    result->set(key, newValue);
                }
                return result;
            } else if (method == "filter" && args.size() > 1 && args[1]->type() == Type::FUNCTION) {
                // Handle filter with a predicate function
                auto func = std::static_pointer_cast<Function>(args[1]);
                auto result = create();
                for (const auto& [key, value] : entries_) {
                    std::vector<ValuePtr> funcArgs = {value, String::create(key),
                                                      shared_from_this()};
                    auto filterResult = func->call(funcArgs);
                    if (filterResult && filterResult->isTruthy()) {
                        result->set(key, value);
                    }
                }
                return result;
            } else if (method == "reduce" && args.size() > 1 && args[1]->type() == Type::FUNCTION) {
                // Handle reduce with a reducer function
                auto func = std::static_pointer_cast<Function>(args[1]);
                ValuePtr accumulator = args.size() > 2 ? args[2] : NilValue::instance();

                for (const auto& [key, value] : entries_) {
                    std::vector<ValuePtr> funcArgs = {accumulator, value, String::create(key),
                                                      shared_from_this()};
                    accumulator = func->call(funcArgs);
                }
                return accumulator;
            }
        } else if (args[0]->type() == Type::ARRAY) {
            // Handle array of [key, value] pairs
            auto array = std::static_pointer_cast<Array>(args[0]);
            for (size_t i = 0; i < array->size(); i++) {
                auto entry = array->get(i);
                if (entry->type() == Type::ARRAY) {
                    auto entryArray = std::static_pointer_cast<Array>(entry);
                    if (entryArray->size() >= 2) {
                        set(entryArray->get(0)->toString(), entryArray->get(1));
                    }
                }
            }
            return shared_from_this();
        } else if (args[0]->type() == Type::OBJECT) {
            // Handle object literal
            auto obj = std::static_pointer_cast<Object>(args[0]);
            auto props = obj->properties();
            for (const auto& key : props) {
                set(key, obj->get(key));
            }
            return shared_from_this();
        }

        return NilValue::instance();
    }

    // Get all property names (keys)
    std::vector<std::string> properties() const {
        std::vector<std::string> keys;
        keys.reserve(entries_.size());
        for (const auto& [key, _] : entries_) {
            keys.push_back(key);
        }
        return keys;
    }

    // String representation of the map
    std::string toString() const override {
        std::string result = "{";
        bool first = true;
        for (const auto& [key, value] : entries_) {
            if (!first)
                result += ", ";
            result += "\"" + key + "\": ";
            result += value ? value->toString() : "nil";
            first = false;
        }
        return result + "}";
    }

private:
    MapType entries_;
};

}  // namespace chronovyan::stdlib::collections
