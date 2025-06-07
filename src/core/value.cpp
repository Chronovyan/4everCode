#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "chronovyan_array.h"
#include "chronovyan_map.h"
#include "environment.h"
#include "error.h"
#include "error_handler.h"
#include "instance_data.h"
#include "interpreter.h"
#include "native_function.h"
#include "object_value.h"
#include "value.h"

namespace chronovyan {

// ObjectValue implementation
void ObjectValue::setProperty(const std::string& name, Value value) {
    m_properties[name] = std::move(value);
}

Value ObjectValue::getProperty(const std::string& name) const {
    auto it = m_properties.find(name);
    if (it != m_properties.end()) {
        return it->second;
    }

    return Value();  // Return nil if property doesn't exist
}

bool ObjectValue::hasProperty(const std::string& name) const {
    return m_properties.find(name) != m_properties.end();
}

// ChronovyanArray implementation

ChronovyanArray::ChronovyanArray(std::vector<Value> elements) : m_elements(std::move(elements)) {}

size_t ChronovyanArray::size() const { return m_elements.size(); }

const Value& ChronovyanArray::at(size_t index) const {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Array index out of bounds");
    }
    return m_elements[index];
}

Value& ChronovyanArray::at(size_t index) {
    if (index >= m_elements.size()) {
        throw std::out_of_range("Array index out of bounds");
    }
    return m_elements[index];
}

void ChronovyanArray::push(Value value) { m_elements.push_back(std::move(value)); }

const std::vector<Value>& ChronovyanArray::getElements() const { return m_elements; }

// ChronovyanMap implementation

ChronovyanMap::ChronovyanMap(std::map<std::string, Value> elements)
    : m_elements(std::move(elements)) {}

size_t ChronovyanMap::size() const { return m_elements.size(); }

bool ChronovyanMap::contains(const std::string& key) const {
    return m_elements.find(key) != m_elements.end();
}

const Value& ChronovyanMap::at(const std::string& key) const {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found: " + key);
    }
    return it->second;
}

Value& ChronovyanMap::at(const std::string& key) {
    auto it = m_elements.find(key);
    if (it == m_elements.end()) {
        throw std::out_of_range("Map key not found: " + key);
    }
    return it->second;
}

void ChronovyanMap::set(const std::string& key, Value value) { m_elements[key] = std::move(value); }

// Value implementation

Value::Value() : m_value(nullptr) {}

Value::Value(bool value) : m_value(value) {}

Value::Value(int64_t value) : m_value(static_cast<double>(value)) {}

Value::Value(double value) : m_value(value) {}

Value::Value(const std::string& value) : m_value(value) {}

Value::Value(const std::vector<Value>& value) : m_value(value) {}

Value::Value(const std::map<std::string, Value>& value) : m_value(value) {}

Value::Value(std::shared_ptr<InstanceData> value) : m_value(value) {}

Value::Value(std::shared_ptr<ObjectValue> value) : m_value(std::move(value)) {}

Value::Value(std::shared_ptr<ChronovyanArray> value) : m_value(std::move(value)) {}

Value::Value(std::shared_ptr<ChronovyanMap> value) : m_value(std::move(value)) {}

Value::Value(std::shared_ptr<NativeFunction> value) : m_value(std::move(value)) {}

Value::Value(std::shared_ptr<ChronovyanFunction> value) : m_value(std::move(value)) {}

Value::Type Value::getType() const {
    if (std::holds_alternative<std::nullptr_t>(m_value)) {
        return Type::NIL;
    } else if (std::holds_alternative<bool>(m_value)) {
        return Type::BOOLEAN;
    } else if (std::holds_alternative<double>(m_value)) {
        return Type::NUMBER;
    } else if (std::holds_alternative<std::string>(m_value)) {
        return Type::STRING;
    } else if (std::holds_alternative<std::vector<ValuePtr>>(m_value)) {
        return Type::ARRAY;
    } else if (std::holds_alternative<std::map<std::string, ValuePtr>>(m_value)) {
        return Type::MAP;
    } else if (std::holds_alternative<std::shared_ptr<InstanceData>>(m_value)) {
        return Type::INSTANCE;
    } else if (std::holds_alternative<std::shared_ptr<ObjectValue>>(m_value)) {
        return Type::OBJECT;
    } else if (std::holds_alternative<std::shared_ptr<NativeFunction>>(m_value)) {
        return Type::NATIVE_FUNCTION;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanFunction>>(m_value)) {
        return Type::CHRONOVYAN_FUNCTION;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value)) {
        return Type::ARRAY;
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value)) {
        return Type::MAP;
    } else if (std::holds_alternative<std::shared_ptr<ExprNode>>(m_value)) {
        return Type::FUNCTION;  // Assuming ExprNode represents a function
    }

    return Type::NIL;  // Should never reach here
}

std::string Value::getTypeName() const {
    switch (getType()) {
        case Type::NIL:
            return "nil";
        case Type::BOOLEAN:
            return "boolean";
        case Type::NUMBER:
            return "number";
        case Type::STRING:
            return "string";
        case Type::ARRAY:
            return "array";
        case Type::MAP:
            return "map";
        case Type::INSTANCE:
            return "instance";
        case Type::FUNCTION:
            return "function";
        case Type::NATIVE_FUNCTION:
            return "native_function";
        case Type::CHRONOVYAN_FUNCTION:
            return "chronovyan_function";
        case Type::TIMEPOINT:
            return "timepoint";
        case Type::OBJECT:
            return "object";
        default:
            return "unknown";
    }
}

bool Value::isNil() const { return std::holds_alternative<std::nullptr_t>(m_value); }

bool Value::isBoolean() const { return std::holds_alternative<bool>(m_value); }

bool Value::isNumeric() const { return std::holds_alternative<double>(m_value); }

bool Value::isString() const { return std::holds_alternative<std::string>(m_value); }

bool Value::isArray() const {
    return std::holds_alternative<std::vector<Value>>(m_value) ||
           std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value);
}

bool Value::isMap() const {
    return std::holds_alternative<std::map<std::string, Value>>(m_value) ||
           std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value);
}

bool Value::isInstance() const {
    return std::holds_alternative<std::shared_ptr<InstanceData>>(m_value);
}

bool Value::isObject() const {
    return std::holds_alternative<std::shared_ptr<ObjectValue>>(m_value);
}

bool Value::isNativeFunction() const {
    return std::holds_alternative<std::shared_ptr<NativeFunction>>(m_value);
}

bool Value::isChronovyanFunction() const {
    return std::holds_alternative<std::shared_ptr<ChronovyanFunction>>(m_value);
}

bool Value::isInteger() const {
    if (!isNumeric()) {
        return false;
    }

    double value = std::get<double>(m_value);
    return value == std::floor(value);
}

bool Value::isFloat() const {
    if (!isNumeric()) {
        return false;
    }

    double value = std::get<double>(m_value);
    return value != std::floor(value);
}

bool Value::equals(const Value& other) const {
    // If types are different, values are not equal
    if (getType() != other.getType()) {
        return false;
    }

    switch (getType()) {
        case Type::NIL:
            return true;  // All nil values are equal

        case Type::BOOLEAN:
            return asBool() == other.asBool();

        case Type::NUMBER:
            return asNumber() == other.asNumber();

        case Type::STRING:
            return asString() == other.asString();

        case Type::ARRAY:
            if (std::holds_alternative<std::vector<ValuePtr>>(m_value) &&
                std::holds_alternative<std::vector<ValuePtr>>(other.m_value)) {
                const auto& a = std::get<std::vector<ValuePtr>>(m_value);
                const auto& b = std::get<std::vector<ValuePtr>>(other.m_value);

                if (a.size() != b.size()) {
                    return false;
                }

                for (size_t i = 0; i < a.size(); ++i) {
                    if (!a[i] || !b[i] || !a[i]->equals(*b[i])) {
                        return false;
                    }
                }

                return true;
            } else if (std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value) &&
                       std::holds_alternative<std::shared_ptr<ChronovyanArray>>(other.m_value)) {
                const auto& a = std::get<std::shared_ptr<ChronovyanArray>>(m_value);
                const auto& b = std::get<std::shared_ptr<ChronovyanArray>>(other.m_value);

                if (a->size() != b->size()) {
                    return false;
                }

                for (size_t i = 0; i < a->size(); ++i) {
                    if (!a->at(i).equals(b->at(i))) {
                        return false;
                    }
                }

                return true;
            }
            return false;

        case Type::MAP:
            if (std::holds_alternative<std::map<std::string, ValuePtr>>(m_value) &&
                std::holds_alternative<std::map<std::string, ValuePtr>>(other.m_value)) {
                const auto& a = std::get<std::map<std::string, ValuePtr>>(m_value);
                const auto& b = std::get<std::map<std::string, ValuePtr>>(other.m_value);

                if (a.size() != b.size()) {
                    return false;
                }

                for (const auto& [key, value] : a) {
                    auto it = b.find(key);
                    if (it == b.end() || !value || !it->second || !value->equals(*it->second)) {
                        return false;
                    }
                }

                return true;
            } else if (std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value) &&
                       std::holds_alternative<std::shared_ptr<ChronovyanMap>>(other.m_value)) {
                const auto& a = std::get<std::shared_ptr<ChronovyanMap>>(m_value);
                const auto& b = std::get<std::shared_ptr<ChronovyanMap>>(other.m_value);
                const auto& aElements = a->getElements();
                const auto& bElements = b->getElements();

                if (aElements.size() != bElements.size()) {
                    return false;
                }

                for (const auto& [key, value] : aElements) {
                    auto it = bElements.find(key);
                    if (it == bElements.end() || !value.equals(it->second)) {
                        return false;
                    }
                }

                return true;
            }
            return false;

        default:
            // For other types, just compare the pointers
            return &m_value == &other.m_value;
    }
}

bool Value::operator<(const Value& other) const {
    // If types are different, compare type IDs
    if (getType() != other.getType()) {
        return static_cast<int>(getType()) < static_cast<int>(other.getType());
    }

    switch (getType()) {
        case Type::NIL:
            return false;  // All nil values are equal

        case Type::BOOLEAN:
            return asBool() < other.asBool();

        case Type::NUMBER:
            return asNumber() < other.asNumber();

        case Type::STRING:
            return asString() < other.asString();

        case Type::ARRAY:
            if (std::holds_alternative<std::vector<ValuePtr>>(m_value) &&
                std::holds_alternative<std::vector<ValuePtr>>(other.m_value)) {
                const auto& a = std::get<std::vector<ValuePtr>>(m_value);
                const auto& b = std::get<std::vector<ValuePtr>>(other.m_value);

                if (a.size() != b.size()) {
                    return a.size() < b.size();
                }

                for (size_t i = 0; i < a.size(); ++i) {
                    if ((!a[i] && b[i]) || (a[i] && b[i] && !a[i]->equals(*b[i]))) {
                        if (!a[i])
                            return true;
                        if (!b[i])
                            return false;
                        return *a[i] < *b[i];
                    }
                }

                return false;  // Arrays are equal
            } else if (std::holds_alternative<std::shared_ptr<ChronovyanArray>>(m_value) &&
                       std::holds_alternative<std::shared_ptr<ChronovyanArray>>(other.m_value)) {
                const auto& a = std::get<std::shared_ptr<ChronovyanArray>>(m_value);
                const auto& b = std::get<std::shared_ptr<ChronovyanArray>>(other.m_value);

                if (a->size() != b->size()) {
                    return a->size() < b->size();
                }

                for (size_t i = 0; i < a->size(); ++i) {
                    if (!a->at(i).equals(b->at(i))) {
                        return a->at(i) < b->at(i);
                    }
                }

                return false;  // Arrays are equal
            }
            return false;

        case Type::MAP:
            // For maps, we'll first compare sizes, then compare key-value pairs
            if (std::holds_alternative<std::map<std::string, ValuePtr>>(m_value) &&
                std::holds_alternative<std::map<std::string, ValuePtr>>(other.m_value)) {
                const auto& a = std::get<std::map<std::string, ValuePtr>>(m_value);
                const auto& b = std::get<std::map<std::string, ValuePtr>>(other.m_value);

                if (a.size() != b.size()) {
                    return a.size() < b.size();
                }

                // Compare key-value pairs
                auto itA = a.begin();
                auto itB = b.begin();

                while (itA != a.end() && itB != b.end()) {
                    if (itA->first != itB->first) {
                        return itA->first < itB->first;
                    }

                    if ((!itA->second && itB->second) ||
                        (itA->second && itB->second && !itA->second->equals(*itB->second))) {
                        if (!itA->second)
                            return true;
                        if (!itB->second)
                            return false;
                        return *itA->second < *itB->second;
                    }

                    ++itA;
                    ++itB;
                }

                return false;  // Maps are equal
            } else if (std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value) &&
                       std::holds_alternative<std::shared_ptr<ChronovyanMap>>(other.m_value)) {
                const auto& a = std::get<std::shared_ptr<ChronovyanMap>>(m_value);
                const auto& b = std::get<std::shared_ptr<ChronovyanMap>>(other.m_value);
                const auto& aElements = a->getElements();
                const auto& bElements = b->getElements();

                if (aElements.size() != bElements.size()) {
                    return aElements.size() < bElements.size();
                }

                // Compare key-value pairs
                auto itA = aElements.begin();
                auto itB = bElements.begin();

                while (itA != aElements.end() && itB != bElements.end()) {
                    if (itA->first != itB->first) {
                        return itA->first < itB->first;
                    }

                    if (!itA->second.equals(itB->second)) {
                        return itA->second < itB->second;
                    }

                    ++itA;
                    ++itB;
                }

                return false;  // Maps are equal
            }
            return &m_value < &other.m_value;

        default:
            // For other types, just compare the pointers
            return &m_value < &other.m_value;
    }
}

bool Value::asBool() const {
    if (!isBoolean()) {
        throw std::runtime_error("Value is not a boolean");
    }

    return std::get<bool>(m_value);
}

bool Value::asBoolean() const { return asBool(); }

double Value::asNumber() const {
    if (!isNumeric()) {
        throw std::runtime_error("Value is not a number");
    }

    return std::get<double>(m_value);
}

double Value::asFloat() const { return asNumber(); }

int64_t Value::asInteger() const {
    if (!isNumeric()) {
        throw std::runtime_error("Value is not a number");
    }

    double value = std::get<double>(m_value);

    if (value != std::floor(value)) {
        throw std::runtime_error("Value is not an integer");
    }

    return static_cast<int64_t>(value);
}

const std::string& Value::asString() const {
    if (!isString()) {
        throw std::runtime_error("Value is not a string");
    }

    return std::get<std::string>(m_value);
}

std::vector<Value> Value::asArray() const {
    if (auto ptr = std::get_if<std::shared_ptr<ValueArray>>(&m_value)) {
        const auto& array = *ptr;
        if (!array) {
            return {};
        }
        std::vector<Value> result;
        result.reserve(array->size());
        for (const auto& item : *array) {
            result.push_back(item ? *item : Value());
        }
        return result;
    }
    if (auto ptr = std::get_if<std::shared_ptr<ChronovyanArray>>(&m_value)) {
        const auto& array = *ptr;
        return array ? array->getElements() : std::vector<Value>{};
    }
    throw std::runtime_error("Value is not an array or cannot be converted to one");
}

const std::vector<ValuePtr>& Value::asArrayPtr() const {
    if (auto ptr = std::get_if<std::shared_ptr<ValueArray>>(&m_value)) {
        if (!*ptr) {
            throw std::runtime_error("Value contains a null ValueArray");
        }
        return **ptr;
    }
    throw std::runtime_error("Value is not a ValueArray");
    return *std::get<std::shared_ptr<ValueArray>>(m_value);
}

std::shared_ptr<ChronovyanArray> Value::asChronovyanArray() const {
    if (auto ptr = std::get_if<std::shared_ptr<ChronovyanArray>>(&m_value)) {
        return *ptr;
    }
    if (isArray()) {
        const auto& array = std::get<std::shared_ptr<ValueArray>>(m_value);
        if (!array) {
            return std::make_shared<ChronovyanArray>();
        }
        // Use move semantics to avoid extra copies
        std::vector<Value> elements;
        elements.reserve(array->size());
        for (const auto& item : *array) {
            elements.push_back(item ? *item : Value());
        }
        return std::make_shared<ChronovyanArray>(std::move(elements));
    }
    throw std::runtime_error("Value is not a ChronovyanArray or convertible to one");
}

std::shared_ptr<ChronovyanMap> Value::asChronovyanMap() const {
    if (auto ptr = std::get_if<std::shared_ptr<ChronovyanMap>>(&m_value)) {
        return *ptr;
    }
    if (isMap()) {
        const auto& map = std::get<std::shared_ptr<ValueMap>>(m_value);
        if (!map) {
            return std::make_shared<ChronovyanMap>();
        }
        std::map<std::string, Value> elements;
        for (const auto& [key, value] : *map) {
            elements[key] = value ? *value : Value();
        }
        return std::make_shared<ChronovyanMap>(std::move(elements));
    }
    throw std::runtime_error("Value is not a ChronovyanMap or convertible to one");
}

std::shared_ptr<InstanceData> Value::asInstance() const {
    if (!isInstance()) {
        throw std::runtime_error("Value is not an instance");
    }

    return std::get<std::shared_ptr<InstanceData>>(m_value);
}

std::shared_ptr<ObjectValue> Value::asObject() const {
    if (!isObject()) {
        throw std::runtime_error("Value is not an object");
    }

    return std::get<std::shared_ptr<ObjectValue>>(m_value);
}

NativeFunction& Value::asNativeFunction() const {
    if (!isNativeFunction()) {
        throw std::runtime_error("Value is not a native function");
    }

    return *std::get<std::shared_ptr<NativeFunction>>(m_value);
}

std::shared_ptr<ChronovyanFunction> Value::asChronovyanFunction() const {
    if (!isChronovyanFunction()) {
        throw std::runtime_error("Value is not a Chronovyan function");
    }

    return std::get<std::shared_ptr<ChronovyanFunction>>(m_value);
}

void Value::setFlag(VariableFlag flag) {
    if (std::find(m_flags.begin(), m_flags.end(), flag) == m_flags.end()) {
        m_flags.push_back(flag);
    }
}

void Value::clearFlag(VariableFlag flag) {
    auto it = std::find(m_flags.begin(), m_flags.end(), flag);
    if (it != m_flags.end()) {
        m_flags.erase(it);
    }
}

bool Value::hasFlag(VariableFlag flag) const {
    return std::find(m_flags.begin(), m_flags.end(), flag) != m_flags.end();
}

std::string Value::toString() const {
    std::ostringstream ss;

    switch (getType()) {
        case Type::NIL:
            ss << "nil";
            break;

        case Type::BOOLEAN:
            ss << (asBool() ? "true" : "false");
            break;

        case Type::NUMBER:
            if (isInteger()) {
                ss << asInteger();
            } else {
                ss << std::fixed << std::setprecision(6) << asNumber();
                // Remove trailing zeros
                std::string str = ss.str();
                if (str.find('.') != std::string::npos) {
                    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
                    if (str.back() == '.') {
                        str.pop_back();
                    }
                }
                return str;
            }
            break;

        case Type::STRING:
            ss << "\"" << asString() << "\"";
            break;

        case Type::ARRAY:
            ss << "[";
            if (std::holds_alternative<std::vector<Value>>(m_value)) {
            }
            ss << array[i].toString();
    }
    ss << "]";
    break;

    // ...
    ss << "{";
    if (std::holds_alternative<std::map<std::string, Value>>(m_value)) {
        const auto& map = std::get<std::map<std::string, Value>>(m_value);
        bool first = true;
        for (const auto& [key, value] : map) {
            if (!first) {
                ss << ", ";
            }
            first = false;
            ss << "\"" << key << "\": " << value.toString();
        }
    } else if (std::holds_alternative<std::shared_ptr<ChronovyanMap>>(m_value)) {
        const auto& map = std::get<std::shared_ptr<ChronovyanMap>>(m_value);
        const auto& elements = map->getElements();
        bool first = true;
        for (const auto& [key, value] : elements) {
            if (!first) {
                ss << ", ";
            }
            first = false;
            ss << "\"" << key << "\": " << value.toString();
        }
    }
    ss << "}";
    break;

    case Type::INSTANCE:
        ss << "<instance>";
        break;

    case Type::FUNCTION:
        ss << "<function>";
        break;

    case Type::NATIVE_FUNCTION:
        ss << "<native_function>";
        break;

    case Type::CHRONOVYAN_FUNCTION:
        ss << "<chronovyan_function>";
        break;

    case Type::TIMEPOINT:
        ss << "<timepoint>";
        break;

    case Type::OBJECT:
        ss << "<object>";
        break;
}

return ss.str();
}

// Helper functions for arithmetic operations
Value add(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        return Value(a.asNumber() + b.asNumber());
    } else if (a.isString() && b.isString()) {
        return Value(a.asString() + b.asString());
    } else {
        throw std::runtime_error("Cannot add values of these types");
    }
}

Value subtract(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        return Value(a.asNumber() - b.asNumber());
    } else {
        throw std::runtime_error("Cannot subtract values of these types");
    }
}

Value multiply(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        return Value(a.asNumber() * b.asNumber());
    } else {
        throw std::runtime_error("Cannot multiply values of these types");
    }
}

Value divide(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        if (b.asNumber() == 0) {
            throw std::runtime_error("Division by zero");
        }
        return Value(a.asNumber() / b.asNumber());
    } else {
        throw std::runtime_error("Cannot divide values of these types");
    }
}

Value modulo(const Value& a, const Value& b) {
    if (a.isNumeric() && b.isNumeric()) {
        if (b.asNumber() == 0) {
            throw std::runtime_error("Modulo by zero");
        }
        return Value(std::fmod(a.asNumber(), b.asNumber()));
    } else {
        throw std::runtime_error("Cannot perform modulo on values of these types");
    }
}

Value negate(const Value& a) {
    if (a.isNumeric()) {
        return Value(-a.asNumber());
    } else {
        throw std::runtime_error("Cannot negate a non-numeric value");
    }
}

Value logicalNot(const Value& a) { return Value(!a.asBool()); }

}  // namespace chronovyan