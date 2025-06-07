#ifndef CHRONOVYAN_VALUE_FWD_H
#define CHRONOVYAN_VALUE_FWD_H

#include <memory>

namespace chronovyan {

// Forward declarations
class Value;
using ValuePtr = std::shared_ptr<Value>;

// Forward declarations for other types that might be needed
class ExprNode;
class InstanceData;
class NativeFunction;
class ChronovyanArray;
class ChronovyanMap;
class ObjectValue;
class ChronovyanFunction;

}  // namespace chronovyan

#endif  // CHRONOVYAN_VALUE_FWD_H
