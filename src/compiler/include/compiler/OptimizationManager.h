#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/Diagnostic.h"
#include "compiler/OptimizationPass.h"

namespace chronovyan {
namespace compiler {

/**
 * @enum OptimizationLevel
 * @brief Defines the optimization levels available in the compiler.
 */
enum class OptimizationLevel {
  NONE,     ///< No optimizations (useful for debugging)
  BASIC,    ///< Basic optimizations (safe, minimal impact on compilation time)
  STANDARD, ///< Standard optimizations (good balance between performance and
            ///< compile time)
  AGGRESSIVE, ///< Aggressive optimizations (may increase compilation time
              ///< significantly)
  TEMPORAL    ///< Temporal-specific optimizations (focus on optimizing temporal
              ///< operations)
};

/**
 * @class OptimizationManager
 * @brief Manages and runs optimization passes on the AST.
 *
 * This class is responsible for registering and executing optimization passes
 * based on the selected optimization level and specific options.
 */
class OptimizationManager {
public:
  /**
   * @brief Default constructor.
   */
  OptimizationManager();

  /**
   * @brief Register a standard optimization pass.
   *
   * @param pass The optimization pass to register.
   * @param level The minimum optimization level at which to run this pass.
   */
  void registerPass(std::unique_ptr<OptimizationPass> pass,
                    OptimizationLevel level = OptimizationLevel::STANDARD);

  /**
   * @brief Register a temporal optimization pass.
   *
   * @param pass The optimization pass to register.
   * @param level The minimum optimization level at which to run this pass.
   */
  void
  registerTemporalPass(std::unique_ptr<OptimizationPass> pass,
                       OptimizationLevel level = OptimizationLevel::TEMPORAL);

  /**
   * @brief Set the optimization level.
   *
   * @param level The optimization level to use.
   */
  void setOptimizationLevel(OptimizationLevel level);

  /**
   * @brief Get the current optimization level.
   *
   * @return The current optimization level.
   */
  OptimizationLevel getOptimizationLevel() const;

  /**
   * @brief Enable or disable a specific optimization pass.
   *
   * @param passName The name of the pass to enable or disable.
   * @param enabled Whether the pass should be enabled.
   * @return True if the pass was found and its state was changed, false
   * otherwise.
   */
  bool setPassEnabled(const std::string &passName, bool enabled);

  /**
   * @brief Check if a specific optimization pass is enabled.
   *
   * @param passName The name of the pass to check.
   * @return True if the pass is enabled, false otherwise.
   */
  bool isPassEnabled(const std::string &passName) const;

  /**
   * @brief Run all enabled optimization passes on an AST node.
   *
   * @param node The AST node to optimize.
   * @param diagnostics The diagnostic reporter for reporting issues.
   * @return True if any optimizations were applied, false otherwise.
   */
  bool optimize(AstNode *node, DiagnosticReporter &diagnostics);

  /**
   * @brief Get a list of all registered passes.
   *
   * @return A vector of pass names.
   */
  std::vector<std::string> getRegisteredPasses() const;

private:
  struct PassInfo {
    std::unique_ptr<OptimizationPass> pass;
    OptimizationLevel minLevel;
    bool enabled;
    bool temporal;
  };

  OptimizationLevel level;
  std::vector<PassInfo> passes;
  std::unordered_map<std::string, size_t> passIndices;

  /**
   * @brief Check if a pass should be run based on the current settings.
   *
   * @param passInfo The pass information.
   * @return True if the pass should be run, false otherwise.
   */
  bool shouldRunPass(const PassInfo &passInfo) const;
};

} // namespace compiler
} // namespace chronovyan