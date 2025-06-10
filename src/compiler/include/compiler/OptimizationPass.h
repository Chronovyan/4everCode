#pragma once

#include <memory>
#include <string>

#include "compiler/AstNode.h"
#include "compiler/AstVisitor.h"
#include "compiler/Diagnostic.h"

namespace chronovyan {
namespace compiler {

/**
 * @class OptimizationPass
 * @brief Base class for all compiler optimization passes.
 *
 * This abstract class defines the interface for optimization passes in the
 * Chronovyan compiler. Each pass should implement the `run` method to perform
 * specific optimizations on the AST. Passes can be chained together to form an
 * optimization pipeline.
 */
class OptimizationPass {
public:
  virtual ~OptimizationPass() = default;

  /**
   * @brief Get the name of the optimization pass.
   *
   * @return The name of the pass, used for logging and diagnostics.
   */
  virtual std::string getName() const = 0;

  /**
   * @brief Get the description of what the optimization pass does.
   *
   * @return A brief description of the optimization performed.
   */
  virtual std::string getDescription() const = 0;

  /**
   * @brief Run the optimization pass on an AST node.
   *
   * @param node The AST node to optimize.
   * @param diagnostics The diagnostic reporter for reporting issues.
   * @return True if any optimizations were applied, false otherwise.
   */
  virtual bool run(AstNode *node, DiagnosticReporter &diagnostics) = 0;

  /**
   * @brief Create a deep clone of this optimization pass.
   *
   * @return A unique pointer to a new instance of this pass.
   */
  virtual std::unique_ptr<OptimizationPass> clone() const = 0;
};

} // namespace compiler
} // namespace chronovyan