#ifndef CHRONOVYAN_NATIVE_FUNCTION_H
#define CHRONOVYAN_NATIVE_FUNCTION_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace chronovyan {

// Forward declarations
class Value;
class Interpreter;

/**
 * @class NativeFunction
 * @brief Represents a native C++ function that can be called from Chronovyan code
 */
class NativeFunction {
public:
    // Define the function type
    using FunctionType = std::function<Value(Interpreter&, const std::vector<Value>&)>;

    /**
     * @brief Constructor for a function with fixed arity
     * @param func The function implementation
     * @param arity The number of arguments
     */
    NativeFunction(FunctionType func, int arity);

    /**
     * @brief Constructor for a function with variable arity
     * @param func The function implementation
     * @param minArity The minimum number of arguments
     * @param maxArity The maximum number of arguments
     */
    NativeFunction(FunctionType func, int minArity, int maxArity);

    /**
     * @brief Call the function with the given arguments
     * @param interpreter The interpreter context
     * @param args The arguments to the function
     * @return The return value of the function
     */
    Value call(Interpreter& interpreter, const std::vector<Value>& args) const;

    /**
     * @brief Get the arity of the function
     * @return The number of arguments the function expects
     */
    int getArity() const;

    /**
     * @brief Get the minimum arity of the function
     * @return The minimum number of arguments the function expects
     */
    int getMinArity() const;

    /**
     * @brief Get the maximum arity of the function
     * @return The maximum number of arguments the function accepts
     */
    int getMaxArity() const;

    /**
     * @brief Check if the function has variable arity
     * @return Whether the function accepts a variable number of arguments
     */
    bool hasVariableArity() const;

private:
    FunctionType m_func;
    int m_arity;
    int m_minArity;
    int m_maxArity;
    bool m_variableArity;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_NATIVE_FUNCTION_H