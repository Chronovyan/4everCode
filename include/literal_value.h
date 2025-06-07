#ifndef CHRONOVYAN_LITERAL_VALUE_H
#define CHRONOVYAN_LITERAL_VALUE_H

#include <stdint.h>

#include <memory>
#include <string>
#include <variant>

namespace chronovyan {

/**
 * @class LiteralValue
 * @brief Represents a literal value in the Chronovyan AST
 *
 * The LiteralValue class is a variant type that can hold different types of literal values,
 * including null, boolean, number, and string types that can appear in the source code.
 */
class LiteralValue {
public:
    // Variant type to hold the different possible values
    using ValueVariant = std::variant<std::monostate,  // Null/nil value
                                      bool,            // Boolean value
                                      int64_t,         // Integer value
                                      double,          // Floating-point value
                                      std::string      // String value
                                      >;

    /**
     * @brief Default constructor creates a nil value
     */
    LiteralValue() : value(std::monostate{}) {}

    /**
     * @brief Construct a boolean value
     * @param value The boolean value
     */
    explicit LiteralValue(bool value) : value(value) {}

    /**
     * @brief Construct an integer value
     * @param value The integer value
     */
    explicit LiteralValue(int64_t value) : value(value) {}

    /**
     * @brief Construct a floating-point value
     * @param value The floating-point value
     */
    explicit LiteralValue(double value) : value(value) {}

    /**
     * @brief Construct a string value
     * @param value The string value
     */
    explicit LiteralValue(const std::string& value) : value(value) {}
    explicit LiteralValue(const char* value) : value(std::string(value)) {}

    /**
     * @brief Check if the value is nil
     * @return Whether the value is nil
     */
    bool isNil() const { return std::holds_alternative<std::monostate>(value); }

    /**
     * @brief Check if the value is a boolean
     * @return Whether the value is a boolean
     */
    bool isBoolean() const { return std::holds_alternative<bool>(value); }

    /**
     * @brief Check if the value is an integer
     * @return Whether the value is an integer
     */
    bool isInteger() const { return std::holds_alternative<int64_t>(value); }

    /**
     * @brief Check if the value is a floating-point number
     * @return Whether the value is a floating-point number
     */
    bool isFloat() const { return std::holds_alternative<double>(value); }

    /**
     * @brief Check if the value is a number (integer or float)
     * @return Whether the value is a number
     */
    bool isNumber() const { return isInteger() || isFloat(); }

    /**
     * @brief Check if the value is a string
     * @return Whether the value is a string
     */
    bool isString() const { return std::holds_alternative<std::string>(value); }

    /**
     * @brief Get the value as a boolean
     * @return The boolean value
     * @throws std::bad_variant_access if the value is not a boolean
     */
    bool asBoolean() const { return std::get<bool>(value); }

    /**
     * @brief Get the value as an integer
     * @return The integer value
     * @throws std::bad_variant_access if the value is not an integer
     */
    int64_t asInteger() const { return std::get<int64_t>(value); }

    /**
     * @brief Get the value as a floating-point number
     * @return The floating-point value
     * @throws std::bad_variant_access if the value is not a floating-point number
     */
    double asFloat() const { return std::get<double>(value); }

    /**
     * @brief Get the value as a number
     * @return The numeric value
     * @throws std::bad_variant_access if the value is not a number
     */
    double asNumber() const {
        if (isInteger()) {
            return static_cast<double>(asInteger());
        }
        return asFloat();
    }

    /**
     * @brief Get the value as a string
     * @return The string value
     * @throws std::bad_variant_access if the value is not a string
     */
    const std::string& asString() const { return std::get<std::string>(value); }

    /**
     * @brief Get the underlying variant value
     * @return The variant value
     */
    const ValueVariant& getValue() const { return value; }

private:
    ValueVariant value;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_LITERAL_VALUE_H