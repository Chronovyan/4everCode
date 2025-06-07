#ifndef CHRONOVYAN_ENUM_VARIANT_H
#define CHRONOVYAN_ENUM_VARIANT_H

#include <memory>
#include <string>
#include <variant>

namespace chronovyan {

// Forward declarations
class ExprNode;

/**
 * @class EnumVariant
 * @brief Represents a variant in an enum type
 */
class EnumVariant {
public:
    /**
     * @brief Default constructor creates a variant without a value
     */
    EnumVariant() : m_hasValue(false) {}

    /**
     * @brief Construct a variant with a string value
     * @param value The string value
     */
    explicit EnumVariant(const std::string& value) : m_value(value), m_hasValue(true) {}

    /**
     * @brief Construct a variant with an integer value
     * @param value The integer value
     */
    explicit EnumVariant(int64_t value) : m_value(value), m_hasValue(true) {}

    /**
     * @brief Construct a variant with a floating-point value
     * @param value The floating-point value
     */
    explicit EnumVariant(double value) : m_value(value), m_hasValue(true) {}

    /**
     * @brief Construct a variant with a boolean value
     * @param value The boolean value
     */
    explicit EnumVariant(bool value) : m_value(value), m_hasValue(true) {}

    /**
     * @brief Construct a variant with a name and an expression value
     * @param name The variant name
     * @param value The expression value
     */
    EnumVariant(const std::string& name, std::unique_ptr<ExprNode> value)
        : m_name(name), m_exprValue(std::move(value)), m_hasValue(true) {}

    /**
     * @brief Copy constructor - creates a deep copy of the ExprNode if present
     * @param other The EnumVariant to copy from
     */
    EnumVariant(const EnumVariant& other)
        : m_name(other.m_name), m_value(other.m_value), m_hasValue(other.m_hasValue) {
        // Only clone the expression value if it exists
        if (other.m_exprValue) {
            // We'll handle this specially in the implementation file - for now, just set to nullptr
            // This will need to be implemented with a proper deep clone once ExprNode is available
            m_exprValue = nullptr;
        }
    }

    /**
     * @brief Copy assignment operator - creates a deep copy of the ExprNode if present
     * @param other The EnumVariant to copy from
     * @return Reference to this object
     */
    EnumVariant& operator=(const EnumVariant& other) {
        if (this != &other) {
            m_name = other.m_name;
            m_value = other.m_value;
            m_hasValue = other.m_hasValue;

            // Only clone the expression value if it exists
            if (other.m_exprValue) {
                // We'll handle this specially in the implementation file - for now, just set to
                // nullptr This will need to be implemented with a proper deep clone once ExprNode
                // is available
                m_exprValue = nullptr;
            } else {
                m_exprValue = nullptr;
            }
        }
        return *this;
    }

    /**
     * @brief Check if the variant has a value
     * @return Whether the variant has a value
     */
    bool hasValue() const { return m_hasValue; }

    /**
     * @brief Get the name of the variant
     * @return The variant name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Check if the variant has an expression value
     * @return Whether the variant has an expression value
     */
    bool hasExprValue() const { return m_exprValue != nullptr; }

    /**
     * @brief Get the expression value
     * @return The expression value
     */
    const ExprNode& getExprValue() const { return *m_exprValue; }

    /**
     * @brief Get the value as a string
     * @return The string value, or empty if not a string
     */
    std::string asString() const {
        if (auto* val = std::get_if<std::string>(&m_value)) {
            return *val;
        }
        return "";
    }

    /**
     * @brief Get the value as an integer
     * @return The integer value, or 0 if not an integer
     */
    int64_t asInteger() const {
        if (auto* val = std::get_if<int64_t>(&m_value)) {
            return *val;
        }
        return 0;
    }

    /**
     * @brief Get the value as a double
     * @return The double value, or 0.0 if not a double
     */
    double asDouble() const {
        if (auto* val = std::get_if<double>(&m_value)) {
            return *val;
        }
        return 0.0;
    }

    /**
     * @brief Get the value as a boolean
     * @return The boolean value, or false if not a boolean
     */
    bool asBoolean() const {
        if (auto* val = std::get_if<bool>(&m_value)) {
            return *val;
        }
        return false;
    }

    /**
     * @brief Check if the value is a string
     * @return Whether the value is a string
     */
    bool isString() const { return std::holds_alternative<std::string>(m_value); }

    /**
     * @brief Check if the value is an integer
     * @return Whether the value is an integer
     */
    bool isInteger() const { return std::holds_alternative<int64_t>(m_value); }

    /**
     * @brief Check if the value is a double
     * @return Whether the value is a double
     */
    bool isDouble() const { return std::holds_alternative<double>(m_value); }

    /**
     * @brief Check if the value is a boolean
     * @return Whether the value is a boolean
     */
    bool isBoolean() const { return std::holds_alternative<bool>(m_value); }

private:
    std::string m_name;
    std::variant<std::string, int64_t, double, bool> m_value;
    std::unique_ptr<ExprNode> m_exprValue;
    bool m_hasValue;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_ENUM_VARIANT_H