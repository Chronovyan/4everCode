#ifndef CHRONOVYAN_COMPILER_PROVING_GROUNDS_PARADOX_DETECTOR_H
#define CHRONOVYAN_COMPILER_PROVING_GROUNDS_PARADOX_DETECTOR_H

#include <memory>
#include <string>
#include <vector>

#include "compiler/DiagnosticReporter.h"
#include "compiler/debugger/DejaVuDebugger.h"

namespace chronovyan {
namespace compiler {
namespace proving_grounds {

/**
 * @class ParadoxDetector
 * @brief Detects temporal paradoxes in Chronovyan programs.
 *
 * The ParadoxDetector analyzes the state of a program to detect potential
 * temporal paradoxes, such as timeline inconsistencies, resource violations,
 * or causality violations.
 */
class ParadoxDetector {
public:
    /**
     * @brief Constructor
     * @param diagnostics Diagnostic reporter for reporting errors
     */
    explicit ParadoxDetector(DiagnosticReporter& diagnostics);

    /**
     * @brief Destructor
     */
    ~ParadoxDetector();

    /**
     * @brief Paradox structure representing a detected paradox
     */
    struct Paradox {
        std::string description;  ///< Description of the paradox
        int severity;             ///< Severity of the paradox (1-10, where 10 is most severe)
        SourceLocation location;  ///< Source location where the paradox occurred
    };

    /**
     * @brief Detect paradoxes in the current program state
     * @param debugger The debugger containing the program state
     * @return A vector of detected paradoxes
     */
    std::vector<Paradox> detectParadoxes(const debugger::DejaVuDebugger& debugger) const;

    /**
     * @brief Enable or disable specific paradox detection rules
     * @param ruleName The name of the rule to enable or disable
     * @param enabled Whether the rule should be enabled or disabled
     */
    void setRuleEnabled(const std::string& ruleName, bool enabled);

    /**
     * @brief Check if a specific paradox detection rule is enabled
     * @param ruleName The name of the rule to check
     * @return True if the rule is enabled, false otherwise
     */
    bool isRuleEnabled(const std::string& ruleName) const;

    /**
     * @brief Get a list of all paradox detection rules
     * @return A vector of rule names
     */
    std::vector<std::string> getRules() const;

    /**
     * @brief Set the threshold for considering a paradox as severe
     * @param threshold The threshold value (1-10)
     */
    void setSeverityThreshold(int threshold);

    /**
     * @brief Get the threshold for considering a paradox as severe
     * @return The threshold value
     */
    int getSeverityThreshold() const;

private:
    DiagnosticReporter& diagnostics;
    std::unordered_map<std::string, bool> ruleEnabled;
    int severityThreshold;

    /**
     * @brief Detect resource violation paradoxes
     * @param debugger The debugger containing the program state
     * @param paradoxes Vector to store detected paradoxes
     */
    void detectResourceViolations(const debugger::DejaVuDebugger& debugger,
                                  std::vector<Paradox>& paradoxes) const;

    /**
     * @brief Detect timeline inconsistency paradoxes
     * @param debugger The debugger containing the program state
     * @param paradoxes Vector to store detected paradoxes
     */
    void detectTimelineInconsistencies(const debugger::DejaVuDebugger& debugger,
                                       std::vector<Paradox>& paradoxes) const;

    /**
     * @brief Detect causality violation paradoxes
     * @param debugger The debugger containing the program state
     * @param paradoxes Vector to store detected paradoxes
     */
    void detectCausalityViolations(const debugger::DejaVuDebugger& debugger,
                                   std::vector<Paradox>& paradoxes) const;

    /**
     * @brief Detect variable state inconsistency paradoxes
     * @param debugger The debugger containing the program state
     * @param paradoxes Vector to store detected paradoxes
     */
    void detectVariableStateInconsistencies(const debugger::DejaVuDebugger& debugger,
                                            std::vector<Paradox>& paradoxes) const;

    /**
     * @brief Detect thread synchronization paradoxes
     * @param debugger The debugger containing the program state
     * @param paradoxes Vector to store detected paradoxes
     */
    void detectThreadSynchronizationParadoxes(const debugger::DejaVuDebugger& debugger,
                                              std::vector<Paradox>& paradoxes) const;
};

}  // namespace proving_grounds
}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_PROVING_GROUNDS_PARADOX_DETECTOR_H