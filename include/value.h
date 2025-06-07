#ifndef CHRONOVYAN_VALUE_H
#define CHRONOVYAN_VALUE_H

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "chron_types.h"
#include "chronovyan_array.h"   // Include for ChronovyanArray class
#include "chronovyan_map.h"     // Include for ChronovyanMap class
#include "native_function.h"    // Include for NativeFunction class
#include "object_value.h"       // Include for ObjectValue class
#include "value_containers.h"   // Include for ValueArray and ValueMap
#include "value_fwd.h"          // Forward declarations
#include "variable_flag.h"      // Include for VariableFlag enum
#include "variable_modifier.h"  // Include for VariableModifier enum

namespace chronovyan {

/**
 * @class Value
 * @brief Represents a runtime value in the Chronovyan language
 *
 * The Value class is a variant type that can hold different types of values,
 * including primitive types, collections, and user-defined types.
 */
class Value {
public:
    /**
     * @enum Type
     * @brief Types of values that can be represented
     */
    enum class Type {
        NIL,                 // Nil/null value
        BOOLEAN,             // Boolean value
        NUMBER,              // Numeric value
        STRING,              // String value
        ARRAY,               // Array value
        MAP,                 // Map value
        INSTANCE,            // User-defined type instance
        FUNCTION,            // Function value
        TIMEPOINT,           // Temporal timepoint
        OBJECT,              // Object value
        NATIVE_FUNCTION,     // Native function
        CHRONOVYAN_FUNCTION  // Chronovyan function
    };

    /**
     * @brief Default constructor creates a nil value
     */
    Value();

    /**
     * @brief Construct a boolean value
     * @param value The boolean value
     */
    explicit Value(bool value);

    /**
     * @brief Construct a numeric value from int64_t
     * @param value The integer value
     */
    explicit Value(int64_t value);

    /**
     * @brief Construct a numeric value
     * @param value The numeric value
     */
    explicit Value(double value);

    /**
     * @brief Construct a string value
     * @param value The string value
     */
    explicit Value(const std::string& value);

    /**
     * @brief Construct an array value from a vector of values
     * @param value The array value
     */
    explicit Value(const std::vector<Value>& value);

    /**
     * @brief Construct an array value from a vector of shared pointers to values
     * @param value The array value as shared pointers
     */
    explicit Value(const std::vector<ValuePtr>& value);

    /**
     * @brief Construct a map value from a map of strings to values
     * @param value The map value
     */
    explicit Value(const std::map<std::string, Value>& value);

    /**
     * @brief Construct a map value from a map of strings to shared pointers to values
     * @param value The map value as shared pointers
     */
    explicit Value(const std::map<std::string, ValuePtr>& value);

    /**
     * @brief Construct an instance value
     * @param value The instance value
     */
    explicit Value(std::shared_ptr<InstanceData> value);

    /**
     * @brief Construct an object value
     * @param value The object value
     */
    explicit Value(std::shared_ptr<ObjectValue> value);

    /**
     * @brief Construct a ChronovyanArray value
     * @param value The ChronovyanArray value
     */
    explicit Value(std::shared_ptr<ChronovyanArray> value);

    /**
     * @brief Construct a ChronovyanMap value
     * @param value The ChronovyanMap value
     */
    explicit Value(std::shared_ptr<ChronovyanMap> value);

    /**
     * @brief Construct a NativeFunction value
     * @param value The NativeFunction value
     */
    explicit Value(std::shared_ptr<NativeFunction> value);

    /**
     * @brief Construct a ChronovyanFunction value
     * @param value The ChronovyanFunction value
     */
    explicit Value(std::shared_ptr<ChronovyanFunction> value);

    /**
     * @brief Get the type of the value
     * @return The value type
     */
    Type getType() const;

    /**
     * @brief Get the type name of the value as a string
     * @return The name of the value type
     */
    std::string getTypeName() const;

    /**
     * @brief Check if the value is nil
     * @return Whether the value is nil
     */
    bool isNil() const;

    /**
     * @brief Check if the value is a boolean
     * @return Whether the value is a boolean
     */
    bool isBoolean() const;

    /**
     * @brief Check if the value is a number
     * @return Whether the value is a number
     */
    bool isNumeric() const;

    /**
     * @brief Check if the value is a string
     * @return Whether the value is a string
     */
    bool isString() const;

    /**
     * @brief Check if the value is an array
     * @return Whether the value is an array
     */
    bool isArray() const;

    /**
     * @brief Check if the value is a map
     * @return Whether the value is a map
     */
    bool isMap() const;

    /**
     * @brief Check if the value is an instance
     * @return Whether the value is an instance
     */
    bool isInstance() const;

    /**
     * @brief Check if the value is an object
     * @return Whether the value is an object
     */
    bool isObject() const;

    /**
     * @brief Check if the value is a function
     * @return Whether the value is a function
     */
    bool isNativeFunction() const;

    /**
     * @brief Check if the value is a chronovyan function
     * @return Whether the value is a chronovyan function
     */
    bool isChronovyanFunction() const;

    /**
     * @brief Check if the value is an integer
     * @return Whether the value is an integer
     */
    bool isInteger() const;

    /**
     * @brief Check if the value is an integer
     * @return Whether the value is an integer
     */
    bool isInt() const { return isInteger(); }

    /**
     * @brief Check if the value is a floating-point number
     * @return true if the value is a floating-point number, false otherwise
     */
    bool isFloat() const;

    /**
     * @brief Check if value is equal to another value
     * @param other The other value to compare with
     * @return Whether the values are equal
     */
    bool equals(const Value& other) const;

    /**
     * @brief Less than comparison
     * @param other The other value to compare with
     * @return Whether this value is less than the other
     */
    bool operator<(const Value& other) const;

    /**
     * @brief Get the value as a boolean
     * @return The boolean value
     * @throws std::runtime_error if the value is not a boolean
     */
    bool asBool() const;

    /**
     * @brief Get the value as a boolean
     * @return The boolean value
     * @throws std::runtime_error if the value is not a boolean
     */
    bool asBoolean() const;

    /**
     * @brief Get the value as a number
     * @return The numeric value
     * @throws std::runtime_error if the value is not a number
     */
    double asNumber() const;

    /**
     * @brief Get the value as a float
     * @return The float value
     * @throws std::runtime_error if the value is not a number
     */
    double asFloat() const;

    /**
     * @brief Get the value as an integer
     * @return The integer value
     * @throws std::runtime_error if the value is not a number or is not an integer
     */
    int64_t asInteger() const;

    /**
     * @brief Get the value as an integer
     * @return The integer value
     * @throws std::runtime_error if the value is not a number or is not an integer
     */
    int64_t asInt() const { return asInteger(); }

    /**
     * @brief Get the value as a string
     * @return The string value
     * @throws std::runtime_error if the value is not a string
     */
    const std::string& asString() const;

    /**
     * @brief Returns the value as a vector of Value objects.
     *
     * If the value is a ValueArray, it returns a copy of the elements with shared_ptrs
     * dereferenced. If the value is a ChronovyanArray, it returns a copy of the elements.
     *
     * @return std::vector<Value> A vector containing the array elements
     * @throws std::runtime_error If the value is not an array or cannot be converted to one
     */
    std::vector<Value> asArray() const;

    /**
     * @brief Returns a reference to the underlying vector of ValuePtr.
     *
     * This provides direct access to the array's elements without copying.
     * The reference is only valid as long as the Value object is alive.
     *
     * @return const std::vector<ValuePtr>& Reference to the array's elements
     * @throws std::runtime_error If the value is not a ValueArray
     */
    const std::vector<ValuePtr>& asArrayPtr() const;

    /**
     * @brief Returns the value as a ValueMap (map of string to ValuePtr).
     *
     * If the value is a ValueMap, returns it directly.
     * If the value is a ChronovyanMap, converts it to a ValueMap.
     *
     * @return std::shared_ptr<ValueMap> The map as a ValueMap
     * @throws std::runtime_error If the value is not a map or cannot be converted to one
     */
    std::shared_ptr<ValueMap> asMap() const;

    /**
     * @brief Returns the value as a const ValueMap.
     *
     * Similar to asMap() but returns a const shared pointer.
     *
     * @return std::shared_ptr<const ValueMap> The map as a const ValueMap
     * @throws std::runtime_error If the value is not a map or cannot be converted to one
     */
    std::shared_ptr<const ValueMap> asConstMap() const;

    /**
     * @brief Returns the value as a std::map<std::string, Value>.
     *
     * Converts the underlying map to a standard map with string keys and Value values.
     * Handles both ValueMap and ChronovyanMap types.
     *
     * @return std::map<std::string, Value> The map as a standard string-to-Value map
     * @throws std::runtime_error If the value is not a map or cannot be converted to one
     */
    std::map<std::string, Value> asStringMap() const;

    /**
     * @brief Returns the value as a ChronovyanArray.
     *
     * If the value is already a ChronovyanArray, returns it directly.
     * If the value is a ValueArray, converts it to a new ChronovyanArray.
     *
     * @note This may involve copying the underlying data.
     *
     * @return std::shared_ptr<ChronovyanArray> The array as a ChronovyanArray
     * @throws std::runtime_error If the value cannot be converted to a ChronovyanArray
     */
    std::shared_ptr<ChronovyanArray> asChronovyanArray() const;

    /**
     * @brief Returns the value as a ChronovyanMap.
     *
     * If the value is already a ChronovyanMap, returns it directly.
     * If the value is a ValueMap, converts it to a new ChronovyanMap.
     *
     * @note This may involve copying the underlying data.
     *
     * @return std::shared_ptr<ChronovyanMap> The map as a ChronovyanMap
     * @throws std::runtime_error If the value cannot be converted to a ChronovyanMap
     */
    std::shared_ptr<ChronovyanMap> asChronovyanMap() const;

    /**
     * @brief Get the value as an instance
     * @return The instance value
     * @throws std::runtime_error if the value is not an instance
     */
    std::shared_ptr<InstanceData> asInstance() const;

    /**
     * @brief Get the value as an object
     * @return The object value
     * @throws std::runtime_error if the value is not an object
     */
    std::shared_ptr<ObjectValue> asObject() const;

    /**
     * @brief Get the value as a native function
     * @return The native function value
     * @throws std::runtime_error if the value is not a native function
     */
    NativeFunction& asNativeFunction() const;

    /**
     * @brief Get the value as a chronovyan function
     * @return The chronovyan function value
     * @throws std::runtime_error if the value is not a chronovyan function
     */
    std::shared_ptr<ChronovyanFunction> asChronovyanFunction() const;

    /**
     * @brief Set a flag on the value
     * @param flag The flag to set
     */
    void setFlag(VariableFlag flag);

    /**
     * @brief Set a flag on the value
     * @param flag The flag to set
     */
    void addFlag(VariableFlag flag) { setFlag(flag); }

    /**
     * @brief Clear a flag from the value
     * @param flag The flag to clear
     */
    void clearFlag(VariableFlag flag);

    /**
     * @brief Set the variable modifier for this value
     * @param modifier The modifier to set
     */
    void setModifier(VariableModifier modifier) { m_modifier = modifier; }

    /**
     * @brief Get the variable modifier for this value
     * @return The modifier value
     */
    VariableModifier getModifier() const { return m_modifier; }

    /**
     * @brief Check if the value has a specific flag
     * @param flag The flag to check
     * @return Whether the value has the flag
     */
    bool hasFlag(VariableFlag flag) const;

    /**
     * @brief Convert the value to a string representation
     * @return String representation of the value
     */
    std::string toString() const;

private:
    // Using std::variant to represent different possible value types
    using ValueVariant =
        std::variant<std::nullptr_t, bool, double, std::string, std::vector<ValuePtr>,
                     std::map<std::string, ValuePtr>, std::shared_ptr<InstanceData>,
                     std::shared_ptr<ObjectValue>, std::shared_ptr<NativeFunction>,
                     std::shared_ptr<ExprNode>, std::shared_ptr<ChronovyanArray>,
                     std::shared_ptr<ChronovyanMap>, std::shared_ptr<ChronovyanFunction>>;

    ValueVariant m_value;

    // Flags for the value
    std::vector<VariableFlag> m_flags;

    // Variable modifier
    VariableModifier m_modifier = VariableModifier::CONF;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_VALUE_H