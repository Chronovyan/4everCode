#ifndef CHRONOVYAN_OBJECT_VALUE_H
#define CHRONOVYAN_OBJECT_VALUE_H

#include <map>
#include <memory>
#include <string>

namespace chronovyan {

// Forward declarations
class Value;

/**
 * @class ObjectValue
 * @brief Represents an object value in the Chronovyan language
 *
 * An object value is a collection of named properties, similar to a JavaScript object.
 */
class ObjectValue {
public:
    /**
     * @brief Default constructor
     */
    ObjectValue() = default;

    /**
     * @brief Set a property on the object
     * @param name The property name
     * @param value The property value
     */
    void setProperty(const std::string& name, Value value);

    /**
     * @brief Get a property from the object
     * @param name The property name
     * @return The property value, or a nil value if the property doesn't exist
     */
    Value getProperty(const std::string& name) const;

    /**
     * @brief Check if the object has a property
     * @param name The property name
     * @return Whether the object has the property
     */
    bool hasProperty(const std::string& name) const;

    /**
     * @brief Get all properties
     * @return A map of all properties
     */
    const std::map<std::string, Value>& getProperties() const { return m_properties; }

private:
    std::map<std::string, Value> m_properties;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_OBJECT_VALUE_H