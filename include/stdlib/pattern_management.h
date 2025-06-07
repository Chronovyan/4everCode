/**
 * @file pattern_management.h
 * @brief Pattern management functions for the Chronovyan Standard Library
 *
 * This file contains declarations for functions that create, manipulate, and
 * apply weave patterns in Chronovyan programs. Weave patterns are the core
 * building blocks of temporal manipulation in the Chronovyan language.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_PATTERN_MANAGEMENT_H
#define CHRONOVYAN_STDLIB_PATTERN_MANAGEMENT_H

#include <functional>
#include <string>
#include <vector>

#include "../value.h"  // For Value type definitions

namespace chronovyan {
namespace stdlib {

/**
 * @brief Forward declaration for Statement
 */
class Statement;

/**
 * @brief Forward declaration for Condition
 */
class Condition;

/**
 * @brief Type alias for weave patterns
 */
class WEAVE_PATTERN {
public:
    WEAVE_PATTERN();
    WEAVE_PATTERN(const std::string& name);
    ~WEAVE_PATTERN();

    // Getters
    std::string get_name() const;
    double get_instability_index() const;
    int get_aethel_cost() const;

    // Metadata
    void set_metadata(const std::string& key, const std::string& value);
    std::string get_metadata(const std::string& key) const;

private:
    std::string name;
    double instability_index;
    int aethel_cost;
    std::vector<Statement*> operations;
    std::unordered_map<std::string, std::string> metadata;

    // Friends
    friend WEAVE_PATTERN define_pattern(const std::string& name,
                                        const std::vector<Statement*>& operations);
    friend WEAVE_PATTERN compose_pattern(const std::string& name,
                                         const std::vector<WEAVE_PATTERN>& patterns,
                                         const std::string& strategy,
                                         const std::vector<Condition*>& conditions);
    friend WEAVE_PATTERN infuse_pattern(const WEAVE_PATTERN& target, const std::string& source,
                                        const std::string& intent);
};

/**
 * @brief Creates a WEAVE_PATTERN with specified operations
 *
 * Creates a new WEAVE_PATTERN with the given name and operations. This
 * function has an Aethel cost of 10 per operation included in the pattern.
 *
 * @param name A unique identifier for the pattern
 * @param operations An array of statements/operations to include in the pattern
 * @return An immutable WEAVE_PATTERN
 *
 * Lore: This function is a sacred rite, akin to inscribing a time-scroll in the Halls of Eternity.
 *
 * Example usage:
 * @code
 * DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
 *     INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
 *     GUARD(Stability_Field > 0.6, "Low stability")
 * ]);
 * @endcode
 */
WEAVE_PATTERN define_pattern(const std::string& name, const std::vector<Statement*>& operations);

/**
 * @brief Combines patterns using specified strategy
 *
 * Combines multiple WEAVE_PATTERNs using the specified strategy (SEQUENTIAL,
 * PARALLEL, or CONDITIONAL). The instability of the resulting pattern varies
 * based on the strategy used, with nested patterns adding +0.02 to instability.
 *
 * @param name A unique identifier for the composite pattern
 * @param patterns An array of WEAVE_PATTERNs to combine
 * @param strategy The combination strategy ("SEQUENTIAL", "PARALLEL", or "CONDITIONAL")
 * @param conditions Conditions to use with the CONDITIONAL strategy (optional)
 * @return A composite WEAVE_PATTERN
 *
 * Example usage:
 * @code
 * DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = compose_pattern("Master", [Stabilization,
 * Exploration], "PARALLEL");
 * @endcode
 */
WEAVE_PATTERN compose_pattern(const std::string& name, const std::vector<WEAVE_PATTERN>& patterns,
                              const std::string& strategy,
                              const std::vector<Condition*>& conditions = {});

/**
 * @brief Executes a pattern in a given context
 *
 * Applies a WEAVE_PATTERN in the specified context. This operation adds
 * the pattern's instability index to the global PARADOX_LEVEL.
 *
 * @param pattern The WEAVE_PATTERN to apply
 * @param context The context in which to apply the pattern
 * @return true if successful, false if a paradox occurs
 *
 * Example usage:
 * @code
 * Success = apply_pattern(Master_Weave, System_Stabilize);
 * @endcode
 */
bool apply_pattern(const WEAVE_PATTERN& pattern, const WEAVE_PATTERN& context);

/**
 * @brief Simulates pattern execution to assess risk
 *
 * Validates a WEAVE_PATTERN by simulating its execution and assessing
 * the potential paradox risk. This operation has an Aethel cost of 5.
 *
 * @param pattern The WEAVE_PATTERN to validate
 * @return The estimated PARADOX_LEVEL (0.0-1.0)
 *
 * Example usage:
 * @code
 * Risk = validate_pattern(Stabilization);
 * @endcode
 */
double validate_pattern(const WEAVE_PATTERN& pattern);

/**
 * @brief Statement class for pattern operations
 */
class Statement {
public:
    virtual ~Statement() = default;
    virtual void execute() = 0;
    virtual double get_instability() const = 0;
    virtual int get_aethel_cost() const = 0;
};

/**
 * @brief Condition class for conditional patterns
 */
class Condition {
public:
    virtual ~Condition() = default;
    virtual bool evaluate() const = 0;
    virtual double get_instability() const = 0;
};

/**
 * @brief INFUSE operation for patterns
 */
class InfuseOperation : public Statement {
public:
    InfuseOperation(const std::string& target, const std::string& source,
                    const std::string& intent);
    void execute() override;
    double get_instability() const override;
    int get_aethel_cost() const override;

private:
    std::string target;
    std::string source;
    std::string intent;
};

/**
 * @brief GUARD operation for patterns
 */
class GuardOperation : public Statement {
public:
    GuardOperation(const Condition* condition, const std::string& message);
    void execute() override;
    double get_instability() const override;
    int get_aethel_cost() const override;

private:
    const Condition* condition;
    std::string message;
};

/**
 * @brief Helper function to create an INFUSE operation
 */
Statement* INFUSE(const std::string& target, const std::string& source, const std::string& intent);

/**
 * @brief Helper function to create a GUARD operation
 */
Statement* GUARD(const Condition& condition, const std::string& message);

}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_PATTERN_MANAGEMENT_H