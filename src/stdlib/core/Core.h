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

// Base value class
class Value {
public:
  virtual ~Value() = default;

  virtual Type type() const = 0;
  virtual std::string toString() const = 0;
  virtual bool isTruthy() const { return true; }

  // Type checking
  bool isNumber() const { return type() == Type::NUMBER; }
  bool isBoolean() const { return type() == Type::BOOLEAN; }
  bool isString() const { return type() == Type::STRING; }
  bool isArray() const { return type() == Type::ARRAY; }
  bool isObject() const { return type() == Type::OBJECT; }
  bool isFunction() const { return type() == Type::FUNCTION; }
  bool isClock() const { return type() == Type::CLOCK; }
  bool isNil() const { return type() == Type::NIL; }
};

// Nil value
class NilValue : public Value {
public:
  Type type() const override { return Type::NIL; }
  std::string toString() const override { return "nil"; }
  bool isTruthy() const override { return false; }

  static std::shared_ptr<NilValue> instance() {
    static auto instance = std::make_shared<NilValue>();
    return instance;
  }
};

// Number type
class Number : public Value {
public:
  explicit Number(double value) : value_(value) {}

  Type type() const override { return Type::NUMBER; }
  std::string toString() const override {
    std::string str = std::to_string(value_);
    // Remove trailing zeros and decimal point if not needed
    str.erase(str.find_last_not_of('0') + 1, std::string::npos);
    if (str.back() == '.') {
      str.pop_back();
    }
    return str;
  }
  double value() const { return value_; }

  static NumberPtr create(double value) {
    return std::make_shared<Number>(value);
  }

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

  static BooleanPtr create(bool value) {
    static auto trueValue = std::make_shared<Boolean>(true);
    static auto falseValue = std::make_shared<Boolean>(false);
    return value ? trueValue : falseValue;
  }

private:
  bool value_;
};

// String type
class String : public Value {
public:
  explicit String(std::string value) : value_(std::move(value)) {}

  Type type() const override { return Type::STRING; }
  std::string toString() const override { return value_; }
  const std::string &value() const { return value_; }

  static StringPtr create(std::string value) {
    return std::make_shared<String>(std::move(value));
  }

  static StringPtr create(const char *value) {
    return std::make_shared<String>(std::string(value));
  }

private:
  std::string value_;
};

// Array type
class Array : public Value, public std::enable_shared_from_this<Array> {
public:
  using ElementList = std::vector<ValuePtr>;

  Type type() const override { return Type::ARRAY; }
  std::string toString() const override {
    std::string result = "[";
    bool first = true;
    for (const auto &element : elements_) {
      if (!first)
        result += ", ";
      result += element ? element->toString() : "nil";
      first = false;
    }
    return result + "]";
  }

  size_t size() const { return elements_.size(); }
  bool empty() const { return elements_.empty(); }

  void add(ValuePtr value) {
    if (!value) {
      elements_.push_back(NilValue::instance());
    } else {
      elements_.push_back(std::move(value));
    }
  }

  ValuePtr get(size_t index) const {
    if (index >= elements_.size()) {
      return NilValue::instance();
    }
    return elements_[index];
  }

  void set(size_t index, ValuePtr value) {
    if (index >= elements_.size()) {
      elements_.resize(index + 1);
    }
    elements_[index] = std::move(value);
  }

  const ElementList &elements() const { return elements_; }

  static ArrayPtr create() { return std::make_shared<Array>(); }

  static ArrayPtr create(ElementList elements) {
    auto array = std::make_shared<Array>();
    array->elements_ = std::move(elements);
    return array;
  }

private:
  ElementList elements_;
};

// Object type
class Object : public Value, public std::enable_shared_from_this<Object> {
public:
  using PropertyMap = std::map<std::string, ValuePtr>;

  Type type() const override { return Type::OBJECT; }
  std::string toString() const override {
    std::string result = "{";
    bool first = true;
    for (const auto &[key, value] : properties_) {
      if (!first)
        result += ", ";
      result += key + ": " + (value ? value->toString() : "nil");
      first = false;
    }
    return result + "}";
  }

  bool has(const std::string &key) const {
    return properties_.find(key) != properties_.end();
  }

  ValuePtr get(const std::string &key) const {
    auto it = properties_.find(key);
    if (it == properties_.end()) {
      return NilValue::instance();
    }
    return it->second;
  }

  void set(const std::string &key, ValuePtr value) {
    if (!value) {
      properties_[key] = NilValue::instance();
    } else {
      properties_[key] = std::move(value);
    }
  }

  void remove(const std::string &key) { properties_.erase(key); }

  const PropertyMap &properties() const { return properties_; }

  static ObjectPtr create() { return std::make_shared<Object>(); }

private:
  PropertyMap properties_;
};

// Clock function (native function)
class Clock : public Value {
public:
  Type type() const override { return Type::CLOCK; }
  std::string toString() const override { return "<native fn>"; }

  // Call the clock function to get current time in seconds since epoch
  double call() const {
    using namespace std::chrono;
    return duration_cast<duration<double>>(
               system_clock::now().time_since_epoch())
        .count();
  }

  static std::shared_ptr<Clock> instance() {
    static auto instance = std::make_shared<Clock>();
    return instance;
  }
};

// Function type (base class for all functions)
class Function : public Value {
public:
  Type type() const override { return Type::FUNCTION; }
  virtual ~Function() = default;

  // Call the function with the given arguments
  virtual ValuePtr call(const std::vector<ValuePtr> &args) = 0;

  // Number of parameters this function expects
  virtual size_t arity() const = 0;

  std::string toString() const override { return "<fn>"; }
};

} // namespace chronovyan::stdlib
