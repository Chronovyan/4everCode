#include <stdexcept>

#include "value.h"
#include "value_containers.h"

namespace chronovyan {

// ValueArray implementation
Value::Value(const std::vector<Value> &value)
    : m_value(std::make_shared<ValueArray>(value)) {}

Value::Value(std::shared_ptr<ValueArray> value)
    : m_value(value ? value : std::make_shared<ValueArray>()) {}

// ValueMap implementation
Value::Value(const std::map<std::string, Value> &value)
    : m_value(std::make_shared<ValueMap>(value)) {}

Value::Value(std::shared_ptr<ValueMap> value)
    : m_value(value ? value : std::make_shared<ValueMap>()) {}

// Array methods
bool Value::isArray() const {
  return std::holds_alternative<std::shared_ptr<ValueArray>>(m_value);
}

std::shared_ptr<ValueArray> Value::asArray() const {
  if (!isArray()) {
    throw std::runtime_error("Value is not an array");
  }
  return std::get<std::shared_ptr<ValueArray>>(m_value);
}

std::shared_ptr<const ValueArray> Value::asArray() const { return asArray(); }

std::vector<Value> Value::asVector() const {
  if (!isArray()) {
    throw std::runtime_error("Value is not an array");
  }
  auto array = asArray();
  return std::vector<Value>(array->begin(), array->end());
}

// Map methods
bool Value::isMap() const {
  return std::holds_alternative<std::shared_ptr<ValueMap>>(m_value);
}

std::shared_ptr<ValueMap> Value::asMap() const {
  if (auto ptr = std::get_if<std::shared_ptr<ValueMap>>(&m_value)) {
    return *ptr;
  }
  if (auto ptr = std::get_if<std::shared_ptr<ChronovyanMap>>(&m_value)) {
    const auto &chronovyanMap = *ptr;
    if (!chronovyanMap) {
      return std::make_shared<ValueMap>();
    }
    auto map = std::make_shared<ValueMap>();
    const auto &elements = chronovyanMap->getElements();
    for (const auto &[key, value] : elements) {
      map->emplace(key, std::make_shared<Value>(value));
    }
    return map;
  }
  throw std::runtime_error("Value is not a map or cannot be converted to one");
}

std::shared_ptr<const ValueMap> Value::asConstMap() const {
  if (auto ptr = std::get_if<std::shared_ptr<ValueMap>>(&m_value)) {
    return std::const_pointer_cast<const ValueMap>(*ptr);
  }
  if (auto ptr = std::get_if<std::shared_ptr<ChronovyanMap>>(&m_value)) {
    const auto &chronovyanMap = *ptr;
    if (!chronovyanMap) {
      return std::make_shared<ValueMap>();
    }
    auto map = std::make_shared<ValueMap>();
    const auto &elements = chronovyanMap->getElements();
    for (const auto &[key, value] : elements) {
      map->emplace(key, std::make_shared<Value>(value));
    }
    return std::const_pointer_cast<const ValueMap>(map);
  }
  throw std::runtime_error("Value is not a map or cannot be converted to one");
}

std::map<std::string, Value> Value::asStringMap() const {
  if (auto ptr = std::get_if<std::shared_ptr<ValueMap>>(&m_value)) {
    const auto &map = *ptr;
    if (!map) {
      return {};
    }
    std::map<std::string, Value> result;
    for (const auto &[key, value] : *map) {
      result.emplace(key, value ? *value : Value());
    }
    return result;
  }
  if (auto ptr = std::get_if<std::shared_ptr<ChronovyanMap>>(&m_value)) {
    const auto &map = *ptr;
    return map ? map->getElements() : std::map<std::string, Value>{};
  }
  throw std::runtime_error("Value is not a map or cannot be converted to one");
}

} // namespace chronovyan
