#pragma once

// Include the main Diagnostic.h file which already defines DiagnosticReporter
#include "compiler/Diagnostic.h"

namespace chronovyan {
namespace compiler {

/**
 * @brief Factory function to create a default diagnostic reporter
 * @return A new diagnostic reporter instance
 */
std::unique_ptr<DiagnosticReporter> createDefaultDiagnosticReporter();

}  // namespace compiler
}  // namespace chronovyan