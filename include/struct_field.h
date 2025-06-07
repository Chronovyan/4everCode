#ifndef CHRONOVYAN_STRUCT_FIELD_H
#define CHRONOVYAN_STRUCT_FIELD_H

#include <memory>
#include <string>

#include "value.h"

namespace chronovyan {

// Forward declarations
class ExprNode;
class Value;

/**
 * @class StructField
 * @brief Represents a field in a struct type definition
 */
class StructField {
public:
    /**
     * @brief Construct a new struct field
     * @param name The field name
     * @param typeName The field type name
     */
    StructField(const std::string& name, const std::string& typeName)
        : m_name(name), m_typeName(typeName), m_hasDefaultValue(false), m_required(true) {}

    /**
     * @brief Construct a new struct field with a default value
     * @param name The field name
     * @param typeName The field type name
     * @param defaultValue The default value expression
     */
    StructField(const std::string& name, const std::string& typeName,
                std::unique_ptr<ExprNode> defaultValue)
        : m_name(name)
        , m_typeName(typeName)
        , m_defaultValue(std::move(defaultValue))
        , m_hasDefaultValue(true)
        , m_required(false) {}

    /**
     * @brief Copy constructor
     * @param other The struct field to copy
     */
    StructField(const StructField& other)
        : m_name(other.m_name)
        , m_typeName(other.m_typeName)
        , m_hasDefaultValue(other.m_hasDefaultValue)
        , m_required(other.m_required) {
        // Deep copy of defaultValue would go here if needed
    }

    /**
     * @brief Get the field name
     * @return The field name
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief Get the field type name
     * @return The field type name
     */
    const std::string& getTypeName() const { return m_typeName; }

    /**
     * @brief Get the default value expression
     * @return The default value expression
     */
    const ExprNode* getDefaultValue() const { return m_defaultValue.get(); }

    /**
     * @brief Check if the field has a default value
     * @return Whether the field has a default value
     */
    bool hasDefaultValue() const { return m_hasDefaultValue; }

    /**
     * @brief Check if the field is required
     * @return Whether the field is required
     */
    bool isRequired() const { return m_required; }

    /**
     * @brief Mark the field as required or optional
     * @param required Whether the field is required
     */
    void setRequired(bool required) { m_required = required; }

private:
    std::string m_name;                        // Field name
    std::string m_typeName;                    // Field type name
    std::unique_ptr<ExprNode> m_defaultValue;  // Default value expression (if any)
    bool m_hasDefaultValue;                    // Whether the field has a default value
    bool m_required;                           // Whether the field is required
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_STRUCT_FIELD_H