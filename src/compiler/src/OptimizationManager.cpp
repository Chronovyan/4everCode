#include <algorithm>
#include <iostream>

#include "compiler/OptimizationManager.h"

namespace chronovyan {
namespace compiler {

OptimizationManager::OptimizationManager()
    : level(OptimizationLevel::STANDARD) {}

void OptimizationManager::registerPass(std::unique_ptr<OptimizationPass> pass,
                                       OptimizationLevel minLevel) {
  std::string passName = pass->getName();

  // Check if a pass with this name already exists
  auto it = passIndices.find(passName);
  if (it != passIndices.end()) {
    std::cerr << "Warning: Replacing existing optimization pass '" << passName
              << "'" << std::endl;
    passes[it->second].pass = std::move(pass);
    passes[it->second].minLevel = minLevel;
    passes[it->second].enabled = true;
    passes[it->second].temporal = false;
    return;
  }

  // Add the pass to the list
  passIndices[passName] = passes.size();
  passes.push_back({std::move(pass), minLevel, true, false});
}

void OptimizationManager::registerTemporalPass(
    std::unique_ptr<OptimizationPass> pass, OptimizationLevel minLevel) {
  std::string passName = pass->getName();

  // Check if a pass with this name already exists
  auto it = passIndices.find(passName);
  if (it != passIndices.end()) {
    std::cerr << "Warning: Replacing existing optimization pass '" << passName
              << "'" << std::endl;
    passes[it->second].pass = std::move(pass);
    passes[it->second].minLevel = minLevel;
    passes[it->second].enabled = true;
    passes[it->second].temporal = true;
    return;
  }

  // Add the pass to the list
  passIndices[passName] = passes.size();
  passes.push_back({std::move(pass), minLevel, true, true});
}

void OptimizationManager::setOptimizationLevel(OptimizationLevel newLevel) {
  level = newLevel;
}

OptimizationLevel OptimizationManager::getOptimizationLevel() const {
  return level;
}

bool OptimizationManager::setPassEnabled(const std::string &passName,
                                         bool enabled) {
  auto it = passIndices.find(passName);
  if (it != passIndices.end()) {
    passes[it->second].enabled = enabled;
    return true;
  }
  return false;
}

bool OptimizationManager::isPassEnabled(const std::string &passName) const {
  auto it = passIndices.find(passName);
  if (it != passIndices.end()) {
    return passes[it->second].enabled;
  }
  return false;
}

bool OptimizationManager::optimize(AstNode *node,
                                   DiagnosticReporter &diagnostics) {
  if (!node) {
    return false;
  }

  bool optimized = false;

  // Run all enabled passes
  for (const auto &passInfo : passes) {
    if (shouldRunPass(passInfo)) {
      try {
        bool passResult = passInfo.pass->run(node, diagnostics);
        optimized = optimized || passResult;

        if (passResult) {
          // Add a diagnostic message
          SourceLocation location;
          std::string message =
              "Applied optimization pass: " + passInfo.pass->getName();
          diagnostics.addInfo(location, message);
        }
      } catch (const std::exception &e) {
        SourceLocation location;
        std::string message = "Exception in optimization pass '" +
                              passInfo.pass->getName() + "': " + e.what();
        diagnostics.addError(location, message);
      }
    }
  }

  return optimized;
}

std::vector<std::string> OptimizationManager::getRegisteredPasses() const {
  std::vector<std::string> result;
  result.reserve(passes.size());

  for (const auto &passInfo : passes) {
    result.push_back(passInfo.pass->getName());
  }

  return result;
}

bool OptimizationManager::shouldRunPass(const PassInfo &passInfo) const {
  // If the pass is disabled, don't run it
  if (!passInfo.enabled) {
    return false;
  }

  // If optimization is disabled, don't run any passes
  if (level == OptimizationLevel::NONE) {
    return false;
  }

  // If temporal optimizations are not enabled, don't run temporal passes
  if (passInfo.temporal && level != OptimizationLevel::TEMPORAL) {
    return false;
  }

  // Check if the current optimization level is high enough
  return static_cast<int>(level) >= static_cast<int>(passInfo.minLevel);
}

} // namespace compiler
} // namespace chronovyan