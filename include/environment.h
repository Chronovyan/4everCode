#ifndef CHRONOVYAN_ENVIRONMENT_H
#define CHRONOVYAN_ENVIRONMENT_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "source_location.h"
#include "value.h"

namespace chronovyan {
// Make Location a type alias for SourceLocation
using Location = SourceLocation;
}  // namespace chronovyan

namespace chronovyan {

/**
 * @class Environment
 * @brief Stores and manages variables in a scope hierarchy
 */
class Environment : public std::enable_shared_from_this<Environment> {
public:
    /**
     * @brief Create a new global environment
     */
    Environment();

    /**
     * @brief Create a new environment with an enclosing environment
     * @param enclosing The parent environment
     */
    explicit Environment(std::shared_ptr<Environment> enclosing);

    /**
     * @brief Define a new variable in this environment
     * @param name The variable name
     * @param value The variable value
     */
    void define(const std::string& name, Value value);

    /**
     * @brief Get a variable value from this environment or enclosing environments
     * @param name The variable name
     * @return The variable value
     * @throws ChronovyanRuntimeError if the variable is not defined
     */
    Value get(const std::string& name) const;

    /**
     * @brief Assign a new value to an existing variable
     * @param name The variable name
     * @param value The new value
     * @throws ChronovyanRuntimeError if the variable is not defined
     */
    void assign(const std::string& name, Value value);

    /**
     * @brief Check if a variable is defined in this environment
     * @param name The variable name
     * @return True if the variable is defined in this environment
     */
    bool contains(const std::string& name) const;

    /**
     * @brief Alias for contains() for backward compatibility
     * @param name The variable name
     * @return True if the variable is defined in this environment
     */
    bool exists(const std::string& name) const { return contains(name); }

    /**
     * @brief Get the environment where a variable is defined
     * @param name The variable name
     * @return The environment where the variable is defined, or nullptr if not found
     */
    std::shared_ptr<Environment> getEnvironmentWhere(const std::string& name) const;

    /**
     * @brief Get a reference to the value of a variable
     * @param name The variable name
     * @return A reference to the value, or nullptr if not found
     */
    std::optional<std::reference_wrapper<Value>> getReference(const std::string& name);

    /**
     * @brief Get the enclosing (parent) environment
     * @return The enclosing environment, or nullptr if this is a global environment
     */
    std::shared_ptr<Environment> getEnclosing() const;

    /**
     * @brief Clone this environment (for creating timeline branches)
     * @return A new environment with the same variables but independent storage
     */
    std::shared_ptr<Environment> clone() const;

    // Resource management methods
    bool hasEnoughAethel(double amount);
    bool hasEnoughChronon(double amount);
    void expendAethel(double amount);
    void expendChronon(double amount);
    double getAethelLevel() const;
    double getChrononLevel() const;
    void setAethelLevel(double level);
    void setChrononLevel(double level);
    double getResourceThreshold() const;
    void setResourceThreshold(double threshold);
    void logResourceIntensiveOperation(const Location& location, double cost);
    bool checkResourceAvailability(double aethelCost, double chrononCost, const Location& location);

    /**
     * @brief Get the current temporal nesting level
     * @return The nesting level (0 for global scope)
     */
    int getTemporalNestingLevel() const;

    /**
     * @brief Check if this environment has a resource insufficiency handler
     * @return True if a handler is registered
     */
    bool hasResourceInsufficiencyHandler() const;

private:
    std::unordered_map<std::string, Value> m_values;
    std::shared_ptr<Environment> m_enclosing;

    // Resource tracking
    double aethelLevel = 100.0;
    double chrononLevel = 100.0;
    double resourceThreshold = 50.0;  // Threshold for high resource usage
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_ENVIRONMENT_H