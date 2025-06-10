#include <sstream>

#include "error_handler.h"
#include "interpreter.h"
#include "native_function.h"
#include "value.h"

namespace chronovyan {

NativeFunction::NativeFunction(FunctionType func, int arity)
    : m_func(func), m_arity(arity), m_minArity(arity), m_maxArity(arity),
      m_variableArity(false) {}

NativeFunction::NativeFunction(FunctionType func, int minArity, int maxArity)
    : m_func(func), m_arity(-1), m_minArity(minArity), m_maxArity(maxArity),
      m_variableArity(true) {}

Value NativeFunction::call(Interpreter &interpreter,
                           const std::vector<Value> &args) const {
  // Check if the number of arguments matches the expected arity
  if (!m_variableArity && static_cast<int>(args.size()) != m_arity) {
    std::stringstream error;
    error << "Expected " << m_arity << " arguments but got " << args.size();
    ErrorHandler::getInstance().reportError(SourceLocation(), error.str());
    return Value(); // Return nil on error
  }

  if (m_variableArity) {
    if (m_minArity >= 0 && static_cast<int>(args.size()) < m_minArity) {
      // Too few arguments
      std::stringstream error;
      error << "Expected at least " << m_minArity << " arguments but got "
            << args.size();
      ErrorHandler::getInstance().reportError(SourceLocation(), error.str());
      return Value(); // Return nil on error
    }

    if (m_maxArity >= 0 && static_cast<int>(args.size()) > m_maxArity) {
      // Too many arguments
      std::stringstream error;
      error << "Expected at most " << m_maxArity << " arguments but got "
            << args.size();
      ErrorHandler::getInstance().reportError(SourceLocation(), error.str());
      return Value(); // Return nil on error
    }
  }

  try {
    // Call the actual function implementation
    return m_func(interpreter, args);
  } catch (const std::exception &e) {
    // Handle any exceptions from the native function
    ErrorHandler::getInstance().reportError(
        SourceLocation(), std::string("Error in native function: ") + e.what());
    return Value(); // Return nil on error
  }
}

int NativeFunction::getArity() const { return m_arity; }

int NativeFunction::getMinArity() const { return m_minArity; }

int NativeFunction::getMaxArity() const { return m_maxArity; }

bool NativeFunction::hasVariableArity() const { return m_variableArity; }

} // namespace chronovyan