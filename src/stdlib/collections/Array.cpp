#include <stdexcept>

#include "chronovyan/stdlib/collections/Array.h"
#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

// Default implementations for virtual methods from Value
ValuePtr Array::get(const std::string &key) const {
  try {
    size_t index = std::stoul(key);
    if (index < elements_.size()) {
      return elements_[index];
    }
  } catch (const std::exception &) {
    // Not a valid index
  }
  return NilValue::instance();
}

void Array::set(const std::string &key, const ValuePtr &value) {
  try {
    size_t index = std::stoul(key);
    if (index < elements_.size()) {
      elements_[index] = value ? value : NilValue::instance();
    }
  } catch (const std::exception &) {
    // Not a valid index
  }
}

bool Array::has(const std::string &key) const {
  try {
    size_t index = std::stoul(key);
    return index < elements_.size();
  } catch (const std::exception &) {
    return false;
  }
}

ValuePtr Array::call(const std::vector<ValuePtr> &args) {
  if (args.empty()) {
    return shared_from_this();
  }

  // Handle array methods
  if (args[0]->type() == Type::STRING) {
    std::string method = std::static_pointer_cast<String>(args[0])->value();

    if (method == "push" && args.size() > 1) {
      for (size_t i = 1; i < args.size(); ++i) {
        push(args[i]);
      }
      return shared_from_this();
    } else if (method == "pop") {
      return pop();
    } else if (method == "length" || method == "size") {
      return Number::create(static_cast<double>(size()));
    } else if (method == "join" && args.size() > 1) {
      std::string delimiter = args[1]->toString();
      std::string result;
      bool first = true;
      for (const auto &element : elements_) {
        if (!first)
          result += delimiter;
        result += element ? element->toString() : "";
        first = false;
      }
      return String::create(result);
    } else if (method == "map" && args.size() > 1) {
      // Handle map operation if a function is provided
      if (args[1]->type() == Type::FUNCTION) {
        auto func = std::static_pointer_cast<Function>(args[1]);
        auto result = create();
        for (size_t i = 0; i < elements_.size(); ++i) {
          std::vector<ValuePtr> funcArgs = {
              elements_[i], Number::create(static_cast<double>(i))};
          result->push(func->call(funcArgs));
        }
        return result;
      }
    } else if (method == "filter" && args.size() > 1) {
      // Handle filter operation if a function is provided
      if (args[1]->type() == Type::FUNCTION) {
        auto func = std::static_pointer_cast<Function>(args[1]);
        auto result = create();
        for (size_t i = 0; i < elements_.size(); ++i) {
          std::vector<ValuePtr> funcArgs = {
              elements_[i], Number::create(static_cast<double>(i))};
          auto filterResult = func->call(funcArgs);
          if (filterResult && filterResult->isTruthy()) {
            result->push(elements_[i]);
          }
        }
        return result;
      }
    } else if (method == "reduce" && args.size() > 1) {
      // Handle reduce operation if a function is provided
      if (args[1]->type() == Type::FUNCTION) {
        auto func = std::static_pointer_cast<Function>(args[1]);
        if (elements_.empty()) {
          return args.size() > 2 ? args[2] : NilValue::instance();
        }

        ValuePtr accumulator;
        size_t startIdx = 0;

        if (args.size() > 2) {
          accumulator = args[2];
        } else if (!elements_.empty()) {
          accumulator = elements_[0];
          startIdx = 1;
        } else {
          return NilValue::instance();
        }

        for (size_t i = startIdx; i < elements_.size(); ++i) {
          std::vector<ValuePtr> funcArgs = {
              accumulator, elements_[i],
              Number::create(static_cast<double>(i))};
          accumulator = func->call(funcArgs);
        }

        return accumulator ? accumulator : NilValue::instance();
      }
    } else if (method == "find" && args.size() > 1) {
      // Handle find operation if a function is provided
      if (args[1]->type() == Type::FUNCTION) {
        auto func = std::static_pointer_cast<Function>(args[1]);
        for (size_t i = 0; i < elements_.size(); ++i) {
          std::vector<ValuePtr> funcArgs = {
              elements_[i], Number::create(static_cast<double>(i))};
          auto findResult = func->call(funcArgs);
          if (findResult && findResult->isTruthy()) {
            return elements_[i];
          }
        }
        return NilValue::instance();
      }
    } else if (method == "findIndex" && args.size() > 1) {
      // Handle findIndex operation if a function is provided
      if (args[1]->type() == Type::FUNCTION) {
        auto func = std::static_pointer_cast<Function>(args[1]);
        for (size_t i = 0; i < elements_.size(); ++i) {
          std::vector<ValuePtr> funcArgs = {
              elements_[i], Number::create(static_cast<double>(i))};
          auto findResult = func->call(funcArgs);
          if (findResult && findResult->isTruthy()) {
            return Number::create(static_cast<double>(i));
          }
        }
        return Number::create(-1.0);
      }
    }
  }

  // Default: return the array itself
  return shared_from_this();
}

} // namespace chronovyan::stdlib::collections
