#include "chronovyan/stdlib/core/Core.h"
#include "chronovyan/stdlib/core/Clock.h"

// Forward declaration of valuesEqual function
namespace chronovyan::stdlib {
bool valuesEqual(const ValuePtr &a, const ValuePtr &b);
}

namespace chronovyan::stdlib {

// Core implementation

// Global nil value
const ValuePtr NIL = NilValue::instance();

// Global boolean values
const BooleanPtr TRUE = Boolean::create(true);
const BooleanPtr FALSE = Boolean::create(false);

// Global clock instance
const std::shared_ptr<Clock> CLOCK = Clock::instance();

// Helper function to convert a value to a string
std::string valueToString(const ValuePtr &value) {
  if (!value)
    return "nil";
  return value->toString();
}

// Helper function to check if a value is truthy
bool isTruthy(const ValuePtr &value) {
  if (!value)
    return false;
  return value->isTruthy();
}

// Helper function to convert a value to a boolean
BooleanPtr toBoolean(const ValuePtr &value) {
  return Boolean::create(isTruthy(value));
}

// Helper function to convert a value to a number
NumberPtr toNumber(const ValuePtr &value) {
  if (!value)
    return Number::create(0);

  switch (value->type()) {
  case Type::NUMBER:
    return std::static_pointer_cast<Number>(value);

  case Type::BOOLEAN: {
    auto b = std::static_pointer_cast<Boolean>(value);
    return Number::create(b->value() ? 1.0 : 0.0);
  }

  case Type::STRING: {
    // Simple string to number conversion
    try {
      auto str = std::static_pointer_cast<String>(value)->value();
      return Number::create(std::stod(str));
    } catch (...) {
      return Number::create(0);
    }
  }

  default:
    return Number::create(0);
  }
}

// Helper function to convert a value to a string
StringPtr toString(const ValuePtr &value) {
  if (!value)
    return String::create("nil");
  return String::create(value->toString());
}

// Helper function to check if two values are equal
BooleanPtr equal(const ValuePtr &a, const ValuePtr &b) {
  return Boolean::create(valuesEqual(a, b));
}

// Helper function to check if a value is of a specific type
BooleanPtr isType(const ValuePtr &value, Type type) {
  if (!value)
    return Boolean::create(type == Type::NIL);
  return Boolean::create(value->type() == type);
}

// Type checking functions
BooleanPtr isNil(const ValuePtr &value) { return isType(value, Type::NIL); }
BooleanPtr isNumber(const ValuePtr &value) {
  return isType(value, Type::NUMBER);
}
BooleanPtr isBoolean(const ValuePtr &value) {
  return isType(value, Type::BOOLEAN);
}
BooleanPtr isString(const ValuePtr &value) {
  return isType(value, Type::STRING);
}
BooleanPtr isArray(const ValuePtr &value) { return isType(value, Type::ARRAY); }
BooleanPtr isObject(const ValuePtr &value) {
  return isType(value, Type::OBJECT);
}
BooleanPtr isFunction(const ValuePtr &value) {
  return isType(value, Type::FUNCTION);
}
BooleanPtr isClock(const ValuePtr &value) { return isType(value, Type::CLOCK); }

} // namespace chronovyan::stdlib
