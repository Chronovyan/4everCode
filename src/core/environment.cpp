#include <iostream>
#include <stdexcept>

#include "environment.h"
#include "error_handler.h"

namespace chronovyan {

Environment::Environment() : m_enclosing(nullptr) {
    // Initialize global environment
}

Environment::Environment(std::shared_ptr<Environment> enclosing)
    : m_enclosing(std::move(enclosing)) {
    // Initialize local environment with enclosing scope
}

void Environment::define(const std::string& name, Value value) {
    // Define a new variable or update existing variable in current scope
    m_values[name] = std::move(value);
}

Value Environment::get(const std::string& name) const {
    // Look up variable in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return it->second;
    }

    // If not found and we have an enclosing environment, look there
    if (m_enclosing) {
        return m_enclosing->get(name);
    }

    // Not found in any enclosing scope
    throw ChronovyanRuntimeError("Undefined variable '" + name + "'", SourceLocation());
}

void Environment::assign(const std::string& name, Value value) {
    // Try to assign in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        // Check for STATIC flag - cannot reassign static variables
        if (it->second.hasFlag(VariableFlag::STATIC)) {
            throw ChronovyanRuntimeError("Cannot reassign static variable '" + name + "'",
                                         SourceLocation());
        }

        it->second = std::move(value);
        return;
    }

    // If not found and we have an enclosing environment, try there
    if (m_enclosing) {
        m_enclosing->assign(name, std::move(value));
        return;
    }

    // Not found in any enclosing scope
    throw ChronovyanRuntimeError("Cannot assign to undefined variable '" + name + "'",
                                 SourceLocation());
}

bool Environment::contains(const std::string& name) const {
    return m_values.find(name) != m_values.end();
}

std::shared_ptr<Environment> Environment::getEnvironmentWhere(const std::string& name) const {
    // Check if the variable is in this environment
    if (contains(name)) {
        return const_cast<Environment*>(this)->shared_from_this();
    }

    // If not, check the enclosing environment
    if (m_enclosing) {
        return m_enclosing->getEnvironmentWhere(name);
    }

    // Not found anywhere
    return nullptr;
}

std::optional<std::reference_wrapper<Value>> Environment::getReference(const std::string& name) {
    // Look up variable in current scope
    auto it = m_values.find(name);
    if (it != m_values.end()) {
        return std::optional<std::reference_wrapper<Value>>(it->second);
    }

    // If not found and we have an enclosing environment, look there
    if (m_enclosing) {
        return m_enclosing->getReference(name);
    }

    // Not found in any enclosing scope
    return std::nullopt;
}

std::shared_ptr<Environment> Environment::getEnclosing() const { return m_enclosing; }

std::shared_ptr<Environment> Environment::clone() const {
    // Create a new environment with the same enclosing environment
    auto cloned = std::make_shared<Environment>(m_enclosing);

    // Copy all values
    for (const auto& [name, value] : m_values) {
        cloned->m_values[name] = value;
    }

    return cloned;
}

// Resource management methods
bool Environment::hasEnoughAethel(double amount) { return aethelLevel >= amount; }

bool Environment::hasEnoughChronon(double amount) { return chrononLevel >= amount; }

void Environment::expendAethel(double amount) {
    aethelLevel -= amount;
    if (aethelLevel < 0) {
        aethelLevel = 0;
    }
}

void Environment::expendChronon(double amount) {
    chrononLevel -= amount;
    if (chrononLevel < 0) {
        chrononLevel = 0;
    }
}

double Environment::getAethelLevel() const { return aethelLevel; }

double Environment::getChrononLevel() const { return chrononLevel; }

void Environment::setAethelLevel(double level) { aethelLevel = level; }

void Environment::setChrononLevel(double level) { chrononLevel = level; }

double Environment::getResourceThreshold() const { return resourceThreshold; }

void Environment::setResourceThreshold(double threshold) { resourceThreshold = threshold; }

void Environment::logResourceIntensiveOperation(const Location& location, double cost) {
    std::cout << "WARNING: Resource-intensive operation at " << location.getFilename() << ":"
              << location.line << ":" << location.column << " (cost: " << cost
              << " - threshold: " << resourceThreshold << ")" << std::endl;
}

bool Environment::checkResourceAvailability(double aethelCost, double chrononCost,
                                            const Location& location) {
    bool hasEnough = true;

    // Check for sufficient aethel
    if (aethelCost > 0 && !hasEnoughAethel(aethelCost)) {
        hasEnough = false;
    }

    // Check for sufficient chronon
    if (chrononCost > 0 && !hasEnoughChronon(chrononCost)) {
        hasEnough = false;
    }

    // If we have enough resources, expend them
    if (hasEnough) {
        if (aethelCost > 0) {
            expendAethel(aethelCost);
        }

        if (chrononCost > 0) {
            expendChronon(chrononCost);
        }

        // Log if this is a resource-intensive operation
        double combinedCost = aethelCost + chrononCost;
        if (combinedCost > resourceThreshold) {
            logResourceIntensiveOperation(location, combinedCost);
        }
    }

    return hasEnough;
}

int Environment::getTemporalNestingLevel() const {
    // Calculate nesting level recursively
    // Global environment (no enclosing) has level 0
    if (!m_enclosing) {
        return 0;
    }

    // Add 1 to the enclosing environment's level
    return 1 + m_enclosing->getTemporalNestingLevel();
}

bool Environment::hasResourceInsufficiencyHandler() const {
    // Check if this environment has a handler for resource insufficiency
    // For now, we'll implement a simple check to see if a specific handler function exists
    return contains("onResourceInsufficiency") ||
           (m_enclosing && m_enclosing->hasResourceInsufficiencyHandler());
}

}  // namespace chronovyan