#include "chronovyan/stdlib/collections/Set.h"
#include "chronovyan/stdlib/core/Core.h"

namespace chronovyan::stdlib::collections {

ValuePtr Set::get(const std::string &key) const {
  // For Set, we can check if the key exists as an element
  auto strVal = String::create(key);
  if (contains(strVal)) {
    return strVal;
  }
  return NilValue::instance();
}

bool Set::has(const std::string &key) const {
  // Check if the key exists as an element in the set
  return contains(String::create(key));
}

ValuePtr Set::call(const std::vector<ValuePtr> &args) {
  // Handle common set operations based on the first argument
  if (args.empty()) {
    // Return the set itself if no arguments
    return shared_from_this();
  }

  if (args.size() == 1) {
    // Check if the argument is in the set
    return Boolean::create(contains(args[0]));
  }

  // For multiple arguments, treat them as elements to check
  auto result = Array::create();
  for (const auto &arg : args) {
    result->add(Boolean::create(contains(arg)));
  }
  return result;
}

} // namespace chronovyan::stdlib::collections
