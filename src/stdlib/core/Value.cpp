#include <cmath>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib {

// Helper function to get type name as string
std::string typeToString(Type type) {
    switch (type) {
        case Type::NIL:
            return "nil";
        case Type::NUMBER:
            return "number";
        case Type::BOOLEAN:
            return "boolean";
        case Type::STRING:
            return "string";
        case Type::ARRAY:
            return "array";
        case Type::OBJECT:
            return "object";
        case Type::FUNCTION:
            return "function";
        case Type::CLOCK:
            return "clock";
        default:
            return "unknown";
    }
}

// Helper function to check if two values are equal
bool valuesEqual(const ValuePtr& a, const ValuePtr& b) {
    // If both are null, they're equal
    if (!a && !b)
        return true;
    // If only one is null, they're not equal
    if (!a || !b)
        return false;

    // Different types are not equal
    if (a->type() != b->type())
        return false;

    // Compare based on type
    switch (a->type()) {
        case Type::NIL:
            return true;  // All nils are equal

        case Type::BOOLEAN:
            return a->asBoolean() == b->asBoolean();

        case Type::NUMBER:
            return a->asNumber() == b->asNumber();

        case Type::STRING:
            return a->toString() == b->toString();

        case Type::ARRAY: {
            auto aArr = a->asArray();
            auto bArr = b->asArray();
            if (aArr.size() != bArr.size())
                return false;
            for (size_t i = 0; i < aArr.size(); ++i) {
                if (!(aArr[i] && bArr[i] && *aArr[i] == *bArr[i])) {
                    return false;
                }
            }
            return true;
        }

        // For objects, we consider them equal only if they are the same object
        case Type::OBJECT:
        case Type::FUNCTION:
        case Type::CLOCK:
            return a.get() == b.get();

        default:
            return false;
    }
}

// Implementation of Value methods
bool Value::equals(const Value& other) const {
    return valuesEqual(std::make_shared<Value>(*this), std::make_shared<Value>(other));
}

bool Value::operator<(const Value& other) const {
    if (type() != other.type()) {
        return static_cast<int>(type()) < static_cast<int>(other.type());
    }

    switch (type()) {
        case Type::NUMBER:
            return asNumber() < other.asNumber();
        case Type::STRING:
            return toString() < other.toString();
        case Type::BOOLEAN:
            return asBoolean() < other.asBoolean();
        default:
            return this < &other;  // Just some arbitrary ordering
    }
}

bool Value::isInteger() const {
    if (!isNumeric())
        return false;
    double val = asNumber();
    return val == std::floor(val) && val <= std::numeric_limits<int64_t>::max() &&
           val >= std::numeric_limits<int64_t>::min();
}

bool Value::asBoolean() const {
    return std::visit(
        [](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return false;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return arg->value() != 0.0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return !arg->value().empty();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                return !arg->elements().empty();
            } else {
                return true;  // Objects, functions, clocks are truthy
            }
        },
        value_);
}

double Value::asNumber() const {
    return std::visit(
        [](auto&& arg) -> double {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value() ? 1.0 : 0.0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                try {
                    return std::stod(arg->value());
                } catch (...) {
                    return 0.0;
                }
            } else {
                return 0.0;  // For other types, return 0
            }
        },
        value_);
}

int64_t Value::asInteger() const { return static_cast<int64_t>(asNumber()); }

std::string Value::asString() const { return toString(); }

std::vector<ValuePtr> Value::asArray() {
    if (auto arr = std::get_if<std::shared_ptr<Array>>(&value_)) {
        return (*arr)->elements();
    }
    return {};
}

// Implementation of Value methods
std::string Value::toString() const {
    return std::visit(
        [](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return "nil";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                double val = arg->value();
                if (val == std::floor(val)) {
                    return std::to_string(static_cast<int64_t>(val));
                } else {
                    return std::to_string(val);
                }
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value() ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                std::string result = "[";
                bool first = true;
                for (const auto& item : arg->elements()) {
                    if (!first)
                        result += ", ";
                    result += item ? item->toString() : "nil";
                    first = false;
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                return arg->toString();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
                return "<function>";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Clock>>) {
                return "<clock>";
            } else {
                return "<unknown>";
            }
        },
        value_);
}

bool Value::isTruthy() const {
    return std::visit(
        [](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return false;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return arg->value() != 0.0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return !arg->value().empty();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>> ||
                                 std::is_same_v<T, std::shared_ptr<Object>> ||
                                 std::is_same_v<T, std::shared_ptr<Function>> ||
                                 std::is_same_v<T, std::shared_ptr<Clock>>) {
                return true;
            } else {
                return false;
            }
        },
        value_);
}

Type Value::type() const {
    return std::visit(
        [](auto&& arg) -> Type {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return Type::NIL;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return Type::NUMBER;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return Type::BOOLEAN;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return Type::STRING;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                return Type::ARRAY;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                return Type::OBJECT;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
                return Type::FUNCTION;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Clock>>) {
                return Type::CLOCK;
            } else {
                throw std::runtime_error("Unknown value type");
            }
        },
        value_);
}

ValuePtr Value::clone() const {
    return std::visit(
        [](auto&& arg) -> ValuePtr {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>> ||
                                 std::is_same_v<T, std::shared_ptr<Boolean>> ||
                                 std::is_same_v<T, std::shared_ptr<String>>) {
                return std::make_shared<Value>(*arg);
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>> ||
                                 std::is_same_v<T, std::shared_ptr<Object>> ||
                                 std::is_same_v<T, std::shared_ptr<Function>> ||
                                 std::is_same_v<T, std::shared_ptr<Clock>>) {
                return arg->clone();
            } else {
                throw std::runtime_error("Cannot clone unknown value type");
            }
        },
        value_);
}

std::string Value::getTypeName() const { return typeToString(type()); }

bool Value::isNil() const { return type() == Type::NIL; }
bool Value::isBoolean() const { return type() == Type::BOOLEAN; }
bool Value::isNumeric() const { return type() == Type::NUMBER; }
bool Value::isString() const { return type() == Type::STRING; }
bool Value::isArray() const { return type() == Type::ARRAY; }
bool Value::isObject() const { return type() == Type::OBJECT; }
bool Value::isFunction() const { return type() == Type::FUNCTION; }
bool Value::isClock() const { return type() == Type::CLOCK; }

bool Value::isInteger() const {
    if (!isNumeric())
        return false;
    double val = asNumber();
    return std::floor(val) == val;
}

bool Value::isFloat() const { return isNumeric() && !isInteger(); }

bool Value::equals(const Value& other) const {
    return valuesEqual(std::make_shared<Value>(*this), std::make_shared<Value>(other));
}

bool Value::operator<(const Value& other) const {
    if (type() != other.type()) {
        return static_cast<int>(type()) < static_cast<int>(other.type());
    }

    switch (type()) {
        case Type::NIL:
            return false;
        case Type::BOOLEAN:
            return asBoolean() < other.asBoolean();
        case Type::NUMBER:
            return asNumber() < other.asNumber();
        case Type::STRING:
            return toString() < other.toString();
        // For complex types, we just compare pointers as a fallback
        default:
            return this < &other;
    }
}

bool Value::asBoolean() const {
    return std::visit(
        [this](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return false;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return arg->value() != 0.0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return !arg->value().empty();
            } else {
                return true;  // All other types are truthy
            }
        },
        value_);
}

double Value::asNumber() const {
    return std::visit(
        [this](auto&& arg) -> double {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value() ? 1.0 : 0.0;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                try {
                    return std::stod(arg->value());
                } catch (...) {
                    return 0.0;
                }
            } else {
                return 0.0;
            }
        },
        value_);
}

double Value::asFloat() const { return asNumber(); }

int64_t Value::asInteger() const { return static_cast<int64_t>(asNumber()); }

std::string Value::asString() const { return toString(); }

std::vector<ValuePtr> Value::asArray() const {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }

    return std::visit(
        [](auto&& arg) -> std::vector<ValuePtr> {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                return arg->elements();
            } else {
                throw std::runtime_error("Value is not an array");
            }
        },
        value_);
}

ValuePtr Value::get(size_t index) const {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }

    return std::visit(
        [index](auto&& arg) -> ValuePtr {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                if (index >= arg->size()) {
                    throw std::out_of_range("Array index out of range");
                }
                return arg->get(index);
            } else {
                throw std::runtime_error("Value is not an array");
            }
        },
        value_);
}

void Value::set(size_t index, const ValuePtr& value) {
    if (!isArray()) {
        throw std::runtime_error("Value is not an array");
    }

    std::visit(
        [index, &value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                arg->set(index, value);
            } else {
                throw std::runtime_error("Value is not an array");
            }
        },
        value_);
}

ValuePtr Value::get(const std::string& key) const {
    if (!isObject()) {
        throw std::runtime_error("Value is not an object");
    }

    return std::visit(
        [&key](auto&& arg) -> ValuePtr {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                return arg->get(key);
            } else {
                throw std::runtime_error("Value is not an object");
            }
        },
        value_);
}

void Value::set(const std::string& key, const ValuePtr& value) {
    if (!isObject()) {
        throw std::runtime_error("Value is not an object");
    }

    std::visit(
        [&key, &value](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                arg->set(key, value);
            } else {
                throw std::runtime_error("Value is not an object");
            }
        },
        value_);
}

bool Value::has(const std::string& key) const {
    if (!isObject()) {
        return false;
    }

    return std::visit(
        [&key](auto&& arg) -> bool {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                return arg->has(key);
            } else {
                return false;
            }
        },
        value_);
}

std::string Value::toString() const {
    return std::visit(
        [](auto&& arg) -> std::string {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::shared_ptr<NilValue>>) {
                return "nil";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Number>>) {
                double val = arg->value();
                if (val == std::floor(val)) {
                    return std::to_string(static_cast<int64_t>(val));
                } else {
                    return std::to_string(val);
                }
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Boolean>>) {
                return arg->value() ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<String>>) {
                return arg->value();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Array>>) {
                std::string result = "[";
                bool first = true;
                for (const auto& item : arg->elements()) {
                    if (!first)
                        result += ", ";
                    result += item ? item->toString() : "nil";
                    first = false;
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Object>>) {
                return arg->toString();
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Function>>) {
                return "<function>";
            } else if constexpr (std::is_same_v<T, std::shared_ptr<Clock>>) {
                return "<clock>";
            } else {
                return "<unknown>";
            }
        },
        value_);
}

}  // namespace chronovyan::stdlib
