/**
 * @file pattern_management.cpp
 * @brief Implementation of pattern management functions for the Chronovyan Standard Library
 *
 * This file contains the implementation of functions that create, manipulate, and
 * apply weave patterns in Chronovyan programs. Weave patterns are the core building
 * blocks of temporal manipulation in the Chronovyan language.
 *
 * @copyright Chronovyan Project
 */

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <stdexcept>
#include <unordered_map>

#include "stdlib/paradox_management.h"
#include "stdlib/pattern_management.h"
#include "stdlib/resource_management.h"

namespace chronovyan {
namespace stdlib {

// Forward declaration for external functions
extern double get_paradox_level();  // From paradox_management.cpp

// Global state for pattern management
namespace {
std::mutex pattern_mutex;
bool initialized = false;

// Registry of all created patterns
std::unordered_map<std::string, WEAVE_PATTERN> pattern_registry;

// Random generator for validation and application
std::mt19937 rng{std::random_device{}()};
std::uniform_real_distribution<double> success_distribution(0.0, 1.0);
}  // namespace

// Initialize the pattern management subsystem
bool initialize_pattern_management() {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (initialized) {
        return true;
    }

    try {
        // No default patterns to register

        initialized = true;
        std::cout << "Pattern management subsystem initialized." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize pattern management subsystem: " << e.what() << std::endl;
        return false;
    }
}

// Shutdown the pattern management subsystem
bool shutdown_pattern_management() {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (!initialized) {
        return true;
    }

    try {
        // Clean up resources
        pattern_registry.clear();

        initialized = false;
        std::cout << "Pattern management subsystem shut down." << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to shut down pattern management subsystem: " << e.what() << std::endl;
        return false;
    }
}

// Implementation of WEAVE_PATTERN class

WEAVE_PATTERN::WEAVE_PATTERN() : name("unnamed_pattern"), instability_index(0.0), aethel_cost(0) {}

WEAVE_PATTERN::WEAVE_PATTERN(const std::string& name)
    : name(name), instability_index(0.1), aethel_cost(5) {
    // Default pattern has minimal instability and cost
}

WEAVE_PATTERN::~WEAVE_PATTERN() {
    // Clean up operations
    for (auto op : operations) {
        delete op;
    }
    operations.clear();
}

std::string WEAVE_PATTERN::get_name() const { return name; }

double WEAVE_PATTERN::get_instability_index() const { return instability_index; }

int WEAVE_PATTERN::get_aethel_cost() const { return aethel_cost; }

void WEAVE_PATTERN::set_metadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string WEAVE_PATTERN::get_metadata(const std::string& key) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return "";
}

// Implementation of define_pattern function
WEAVE_PATTERN define_pattern(const std::string& name, const std::vector<Statement*>& operations) {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (!initialized) {
        std::cerr << "Pattern management subsystem not initialized." << std::endl;
        return WEAVE_PATTERN(name);  // Return a minimal pattern
    }

    try {
        // Check if a pattern with this name already exists
        if (pattern_registry.find(name) != pattern_registry.end()) {
            std::cerr << "Pattern with name '" << name << "' already exists." << std::endl;
            return WEAVE_PATTERN(name);  // Return a minimal pattern
        }

        // Create a new pattern
        WEAVE_PATTERN pattern;
        pattern.name = name;

        // Calculate instability and cost based on operations
        double total_instability = 0.0;
        int total_cost = 0;

        for (const auto& op : operations) {
            // Create a deep copy of each operation
            Statement* op_copy = nullptr;

            // Determine the type of operation and create the appropriate copy
            InfuseOperation* infuse = dynamic_cast<InfuseOperation*>(op);
            GuardOperation* guard = dynamic_cast<GuardOperation*>(op);

            if (infuse) {
                op_copy = new InfuseOperation(*infuse);
            } else if (guard) {
                op_copy = new GuardOperation(*guard);
            } else {
                // Unknown operation type, skip it
                std::cerr << "Unknown operation type in pattern definition." << std::endl;
                continue;
            }

            // Add the copy to the pattern's operations
            pattern.operations.push_back(op_copy);

            // Update instability and cost
            total_instability += op->get_instability();
            total_cost += op->get_aethel_cost();
        }

        // Set the pattern's properties
        pattern.instability_index = total_instability;
        pattern.aethel_cost = total_cost;

        // Register the pattern
        pattern_registry[name] = pattern;

        std::cout << "Defined pattern '" << name << "' with " << operations.size()
                  << " operations, instability " << total_instability << ", cost " << total_cost
                  << std::endl;

        return pattern;
    } catch (const std::exception& e) {
        std::cerr << "Pattern definition failed: " << e.what() << std::endl;
        return WEAVE_PATTERN(name);  // Return a minimal pattern
    }
}

// Implementation of compose_pattern function
WEAVE_PATTERN compose_pattern(const std::string& name, const std::vector<WEAVE_PATTERN>& patterns,
                              const std::string& strategy,
                              const std::vector<Condition*>& conditions) {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (!initialized) {
        std::cerr << "Pattern management subsystem not initialized." << std::endl;
        return WEAVE_PATTERN(name);  // Return a minimal pattern
    }

    try {
        // Check if a pattern with this name already exists
        if (pattern_registry.find(name) != pattern_registry.end()) {
            std::cerr << "Pattern with name '" << name << "' already exists." << std::endl;
            return WEAVE_PATTERN(name);  // Return a minimal pattern
        }

        // Create a new pattern
        WEAVE_PATTERN composite;
        composite.name = name;

        // Calculate instability and cost based on strategy and component patterns
        double total_instability = 0.0;
        int total_cost = 0;

        // Apply strategy-specific calculations
        if (strategy == "SEQUENTIAL") {
            // Sequential patterns: sum of component instabilities
            for (const auto& pattern : patterns) {
                total_instability += pattern.instability_index;
                total_cost += pattern.aethel_cost;
            }
        } else if (strategy == "PARALLEL") {
            // Parallel patterns: max instability + 10% for each additional pattern
            if (!patterns.empty()) {
                // Find max instability
                double max_instability = 0.0;
                for (const auto& pattern : patterns) {
                    max_instability = std::max(max_instability, pattern.instability_index);
                    total_cost += pattern.aethel_cost;
                }

                // Add base instability
                total_instability = max_instability;

                // Add 10% for each additional pattern
                if (patterns.size() > 1) {
                    total_instability += (patterns.size() - 1) * 0.1;
                }
            }
        } else if (strategy == "CONDITIONAL") {
            // Conditional patterns: weighted average of component instabilities
            // Plus additional instability for condition evaluation
            if (!patterns.empty()) {
                double weighted_instability = 0.0;
                for (const auto& pattern : patterns) {
                    weighted_instability += pattern.instability_index / patterns.size();
                    total_cost +=
                        pattern.aethel_cost / 2;  // Reduced cost since not all will execute
                }

                // Add base weighted instability
                total_instability = weighted_instability;

                // Add instability for conditions (if provided)
                for (const auto& cond : conditions) {
                    total_instability += cond->get_instability();
                }
            }
        } else {
            // Unknown strategy
            std::cerr << "Unknown composition strategy: " << strategy << std::endl;
            return WEAVE_PATTERN(name);  // Return a minimal pattern
        }

        // Add additional instability for nested patterns (0.02 per pattern)
        total_instability += 0.02 * patterns.size();

        // Set the pattern's properties
        composite.instability_index = total_instability;
        composite.aethel_cost = total_cost;

        // Set metadata for the composite pattern
        composite.set_metadata("composition_strategy", strategy);
        composite.set_metadata("component_count", std::to_string(patterns.size()));

        // Register the pattern
        pattern_registry[name] = composite;

        std::cout << "Composed pattern '" << name << "' with " << patterns.size()
                  << " components using " << strategy << " strategy, instability "
                  << total_instability << ", cost " << total_cost << std::endl;

        return composite;
    } catch (const std::exception& e) {
        std::cerr << "Pattern composition failed: " << e.what() << std::endl;
        return WEAVE_PATTERN(name);  // Return a minimal pattern
    }
}

// Implementation of apply_pattern function
bool apply_pattern(const WEAVE_PATTERN& pattern, const WEAVE_PATTERN& context) {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (!initialized) {
        std::cerr << "Pattern management subsystem not initialized." << std::endl;
        return false;
    }

    try {
        // Calculate success probability based on pattern and context instability
        double combined_instability = pattern.get_instability_index();

        // Context can reduce instability if it's a stabilizing context
        std::string context_type = context.get_metadata("context_type");
        if (context_type == "stabilizing") {
            combined_instability *= 0.8;  // 20% reduction
        } else if (context_type == "destabilizing") {
            combined_instability *= 1.2;  // 20% increase
        }

        // Success probability is inverse of instability
        double success_prob = 1.0 - combined_instability;

        // Ensure probability is in valid range
        success_prob = std::max(0.1, std::min(0.9, success_prob));

        // Determine success based on probability
        bool success = (success_distribution(rng) < success_prob);

        // Apply Aethel cost regardless of success
        std::cout << "Applied pattern '" << pattern.get_name() << "' in context '"
                  << context.get_name() << "' with Aethel cost " << pattern.get_aethel_cost()
                  << std::endl;

        if (success) {
            std::cout << "Pattern application successful (probability: " << success_prob << ")"
                      << std::endl;
        } else {
            std::cout << "Pattern application failed (probability: " << success_prob << ")"
                      << std::endl;

            // Increase paradox level on failure
            std::cout << "WARNING: Failed pattern application increases PARADOX_LEVEL by "
                      << combined_instability << std::endl;
        }

        return success;
    } catch (const std::exception& e) {
        std::cerr << "Pattern application failed: " << e.what() << std::endl;
        return false;
    }
}

// Implementation of validate_pattern function
double validate_pattern(const WEAVE_PATTERN& pattern) {
    std::lock_guard<std::mutex> lock(pattern_mutex);
    if (!initialized) {
        std::cerr << "Pattern management subsystem not initialized." << std::endl;
        return 1.0;  // Maximum risk
    }

    try {
        // Calculate risk based on pattern instability
        double risk = pattern.get_instability_index();

        // Apply some randomness to the risk assessment
        double risk_variation = 0.9 + (success_distribution(rng) * 0.2);  // 0.9 to 1.1
        risk *= risk_variation;

        // Ensure risk is in valid range
        risk = std::max(0.0, std::min(1.0, risk));

        // Apply a small Aethel cost for validation
        std::cout << "Validated pattern '" << pattern.get_name() << "' with Aethel cost 5"
                  << std::endl;
        std::cout << "Estimated PARADOX_LEVEL: " << risk << std::endl;

        return risk;
    } catch (const std::exception& e) {
        std::cerr << "Pattern validation failed: " << e.what() << std::endl;
        return 1.0;  // Maximum risk
    }
}

// Implementation of operation classes

InfuseOperation::InfuseOperation(const std::string& target, const std::string& source,
                                 const std::string& intent)
    : target(target), source(source), intent(intent) {}

void InfuseOperation::execute() {
    std::cout << "Executing INFUSE operation: " << target << " with " << source << " for " << intent
              << std::endl;
    // Actual implementation would call infuse_pattern
}

double InfuseOperation::get_instability() const {
    // Base instability plus target-specific modifier
    double base_instability = 0.05;

    // Unstable targets increase instability
    if (target == "Temporal_Rift" || source == "Temporal_Rift") {
        base_instability += 0.1;
    }

    return base_instability;
}

int InfuseOperation::get_aethel_cost() const {
    // Base cost for INFUSE operation
    return 20;
}

GuardOperation::GuardOperation(const Condition* condition, const std::string& message)
    : condition(condition), message(message) {}

void GuardOperation::execute() {
    bool result = condition->evaluate();
    std::cout << "Executing GUARD operation with condition result: " << (result ? "true" : "false")
              << std::endl;

    if (!result) {
        std::cout << "GUARD condition failed: " << message << std::endl;
    }
}

double GuardOperation::get_instability() const {
    // Guard operations have low instability
    return 0.02 + condition->get_instability();
}

int GuardOperation::get_aethel_cost() const {
    // Base cost for GUARD operation
    return 5;
}

// Helper functions for creating operations

Statement* INFUSE(const std::string& target, const std::string& source, const std::string& intent) {
    return new InfuseOperation(target, source, intent);
}

Statement* GUARD(const Condition& condition, const std::string& message) {
    return new GuardOperation(&condition, message);
}

}  // namespace stdlib
}  // namespace chronovyan