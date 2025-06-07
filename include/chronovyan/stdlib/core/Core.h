#pragma once

#include <chrono>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace chronovyan::stdlib {

// Forward declarations
class Value;
class Object;
class Array;
class String;
class Number;
class Boolean;
class Function;
class Clock;
class NilValue;

// Type aliases
using ValuePtr = std::shared_ptr<Value>;
using ObjectPtr = std::shared_ptr<Object>;
using ArrayPtr = std::shared_ptr<Array>;
using StringPtr = std::shared_ptr<String>;
using NumberPtr = std::shared_ptr<Number>;
using BooleanPtr = std::shared_ptr<Boolean>;
using FunctionPtr = std::shared_ptr<Function>;

// Type enumeration
enum class Type {
    NIL,
    NUMBER,
    BOOLEAN,
    STRING,
    ARRAY,
    OBJECT,
    FUNCTION,
    CLOCK,
    // Add more types as needed
};

// Forward declarations for friend classes
class Array;
class Object;
class Clock;

// Base class for all value types
class Value {
public:
    virtual ~Value() = default;

    // Type information
    virtual Type type() const = 0;
    virtual std::string toString() const = 0;

    // Type checking
    virtual bool isNil() const { return type() == Type::NIL; }
    virtual bool isBoolean() const { return type() == Type::BOOLEAN; }
    virtual bool isNumeric() const { return type() == Type::NUMBER; }
    virtual bool isString() const { return type() == Type::STRING; }
    virtual bool isArray() const { return type() == Type::ARRAY; }
    virtual bool isObject() const { return type() == Type::OBJECT; }
    virtual bool isFunction() const { return type() == Type::FUNCTION; }
    virtual bool isClock() const { return type() == Type::CLOCK; }
    virtual bool isInteger() const;
    virtual bool isFloat() const { return isNumeric() && !isInteger(); }

    // Type conversion
    virtual bool asBoolean() const;
    virtual double asNumber() const;
    virtual double asFloat() const { return asNumber(); }
    virtual int64_t asInteger() const;
    virtual std::string asString() const;
    virtual std::vector<ValuePtr> asArray();

    // Comparison
    virtual bool equals(const Value& other) const;
    virtual bool operator==(const Value& other) const { return equals(other); }
    virtual bool operator!=(const Value& other) const { return !equals(other); }
    virtual bool operator<(const Value& other) const;

    // Check if the value is truthy
    virtual bool isTruthy() const { return true; }

    // Support copy and move semantics
    Value(const Value&) = default;
    Value(Value&&) = default;
    Value& operator=(const Value&) = default;
    Value& operator=(Value&&) = default;

    // Virtual clone method for polymorphic copying
    virtual ValuePtr clone() const = 0;

    // String key access (for objects)
    virtual ValuePtr get(const std::string& key) const {
        (void)key;  // Unused parameter
        return nullptr;
    }

    virtual void set(const std::string& key, const ValuePtr& value) {
        (void)key;
        (void)value;  // Unused parameters
    }

    virtual bool has(const std::string& key) const {
        (void)key;  // Unused parameter
        return false;
    }

    // Numeric index access (for arrays)
    virtual ValuePtr get(size_t index) const {
        (void)index;  // Unused parameter
        return nullptr;
    }

    virtual void set(size_t index, const ValuePtr& value) {
        (void)index;
        (void)value;  // Unused parameters
    }

    // Function call
    virtual ValuePtr call(const std::vector<ValuePtr>& args) {
        (void)args;  // Unused parameter
        return nullptr;
    }

protected:
    // Protected default constructor to make this class abstract
    Value() = default;

    // Variant to hold the actual value
    std::variant<std::shared_ptr<NilValue>, std::shared_ptr<Number>, std::shared_ptr<Boolean>,
                 std::shared_ptr<String>, std::shared_ptr<Array>, std::shared_ptr<Object>,
                 std::shared_ptr<Function>, std::shared_ptr<Clock>>
        value_;
};

// Nil value
class NilValue : public Value {
public:
    Type type() const override { return Type::NIL; }
    std::string toString() const override { return "nil"; }
    bool isTruthy() const override { return false; }

    ValuePtr clone() const override { return instance(); }

    // Singleton instance
    static ValuePtr instance() {
        static auto instance = std::make_shared<NilValue>();
        return instance;
    }

    // Prevent instantiation except through instance()
    NilValue() = default;
    ~NilValue() override = default;

    // Allow copying and moving (returns the singleton instance)
    NilValue(const NilValue&) = default;
    NilValue(NilValue&&) = default;
    NilValue& operator=(const NilValue&) = default;
    NilValue& operator=(NilValue&&) = default;
};

// Number type
class Number : public Value {
public:
    explicit Number(double value) : value_(value) {}

    Type type() const override { return Type::NUMBER; }
    std::string toString() const override {
        // Check if the value is an integer
        if (value_ == static_cast<int64_t>(value_)) {
            return std::to_string(static_cast<int64_t>(value_));
        }
        return std::to_string(value_);
    }

    double value() const { return value_; }

    ValuePtr clone() const override { return create(value_); }

    static NumberPtr create(double value) { return std::make_shared<Number>(value); }

    ~Number() override = default;

    // Support copy and move
    Number(const Number&) = default;
    Number(Number&&) = default;
    Number& operator=(const Number&) = default;
    Number& operator=(Number&&) = default;

private:
    double value_;
};

// Boolean type
class Boolean : public Value {
public:
    explicit Boolean(bool value) : value_(value) {}

    Type type() const override { return Type::BOOLEAN; }
    std::string toString() const override { return value_ ? "true" : "false"; }
    bool isTruthy() const override { return value_; }

    bool value() const { return value_; }

    ValuePtr clone() const override { return create(value_); }

    static BooleanPtr create(bool value) { return value ? trueValue() : falseValue(); }

    static BooleanPtr trueValue() {
        static auto trueVal = std::make_shared<Boolean>(true);
        return trueVal;
    }

    static BooleanPtr falseValue() {
        static auto falseVal = std::make_shared<Boolean>(false);
        return falseVal;
    }

    ~Boolean() override = default;

    // Support copy and move
    Boolean(const Boolean&) = default;
    Boolean(Boolean&&) = default;
    Boolean& operator=(const Boolean&) = default;
    Boolean& operator=(Boolean&&) = default;

private:
    bool value_;
};

// String type
class String : public Value {
public:
    explicit String(std::string value) : value_(std::move(value)) {}

    Type type() const override { return Type::STRING; }
    std::string toString() const override { return value_; }
    const std::string& value() const { return value_; }

    ValuePtr clone() const override { return create(value_); }

    static StringPtr create(std::string value) {
        return std::make_shared<String>(std::move(value));
    }

    static StringPtr create(const char* value) {
        return std::make_shared<String>(std::string(value));
    }

    ~String() override = default;

    // Support copy and move
    String(const String&) = default;
    String(String&&) = default;
    String& operator=(const String&) = default;
    String& operator=(String&&) = default;

private:
    std::string value_;
};

// Array type
class Array : public Value {
public:
    using ElementList = std::vector<ValuePtr>;

    Type type() const override { return Type::ARRAY; }
    std::string toString() const override;

    // Element access
    size_t size() const { return elements_.size(); }
    bool empty() const { return elements_.empty(); }

    // Add an element to the end
    void add(ValuePtr value) {
        if (!value) {
            elements_.push_back(NilValue::instance());
        } else {
            elements_.push_back(std::move(value));
        }
    }

    // Override base class methods
    using Value::get;
    using Value::has;
    using Value::set;

    // Get element by index
    ValuePtr get(size_t index) const override {
        if (index >= elements_.size()) {
            return NilValue::instance();
        }
        return elements_[index];
    }

    // Set element at index
    void set(size_t index, const ValuePtr& value) override {
        if (index >= elements_.size()) {
            elements_.resize(index + 1);
        }
        elements_[index] = value;
    }

    const ElementList& elements() const { return elements_; }

    ValuePtr clone() const override {
        auto newArray = std::make_shared<Array>();
        for (const auto& element : elements_) {
            newArray->add(element ? element->clone() : nullptr);
        }
        return newArray;
    }

    // Factory methods
    static ArrayPtr create() { return std::make_shared<Array>(); }

    static ArrayPtr create(ElementList elements) {
        auto array = std::make_shared<Array>();
        array->elements_ = std::move(elements);
        return array;
    }

    ~Array() override = default;

    // Support copy and move
    Array() = default;
    Array(const Array&) = default;
    Array(Array&&) = default;
    Array& operator=(const Array&) = default;
    Array& operator=(Array&&) = default;

private:
    ElementList elements_;
};

// Object type
class Object : public Value {
public:
    using PropertyMap = std::map<std::string, ValuePtr>;

    Type type() const override { return Type::OBJECT; }
    std::string toString() const override {
        std::string result = "{";
        bool first = true;
        for (const auto& [key, value] : properties_) {
            if (!first)
                result += ", ";
            result += "\"" + key + "\": ";
            result += value ? value->toString() : "nil";
            first = false;
        }
        return result + "}";
    }

    // Override base class methods
    using Value::get;
    using Value::has;
    using Value::set;

    // Property access
    bool has(const std::string& key) const override {
        return properties_.find(key) != properties_.end();
    }

    ValuePtr get(const std::string& key) const override {
        auto it = properties_.find(key);
        if (it == properties_.end()) {
            return NilValue::instance();
        }
        return it->second;
    }

    void set(const std::string& key, const ValuePtr& value) override {
        if (!value) {
            properties_[key] = NilValue::instance();
        } else {
            properties_[key] = value;
        }
    }

    void remove(const std::string& key) { properties_.erase(key); }

    const PropertyMap& properties() const { return properties_; }

    ValuePtr clone() const override {
        auto newObject = std::make_shared<Object>();
        for (const auto& [key, value] : properties_) {
            newObject->set(key, value ? value->clone() : nullptr);
        }
        return newObject;
    }

    // Factory methods
    static ObjectPtr create() { return std::make_shared<Object>(); }

    ~Object() override = default;

    // Support copy and move
    Object() = default;
    Object(const Object&) = default;
    Object(Object&&) = default;
    Object& operator=(const Object&) = default;
    Object& operator=(Object&&) = default;

private:
    PropertyMap properties_;
};

// Clock function (native function)
// Moved to Clock.h

// Function type (base class for all callable values)
class Function : public Value {
public:
    virtual ~Function() = default;

    Type type() const override { return Type::FUNCTION; }
    std::string toString() const override { return "<function>"; }

    // Override base class method
    ValuePtr call(const std::vector<ValuePtr>& args) override = 0;

    // Clone the function - must be implemented by derived classes
    ValuePtr clone() const override = 0;

    // Prevent hiding of base class methods
    using Value::get;
    using Value::has;
    using Value::set;

    // Number of parameters this function expects
    virtual size_t arity() const = 0;
};

}  // namespace chronovyan::stdlib
