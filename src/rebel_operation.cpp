#include "rebel_operation.h"

namespace chronovyan {

const char *getRebelOperationTypeName(RebelOperationType type) {
  switch (type) {
  case RebelOperationType::NONE:
    return "None";
  case RebelOperationType::TIMELINE_ALTERATION:
    return "Timeline Alteration";
  case RebelOperationType::TIMELINE_INJECTION:
    return "Timeline Injection";
  case RebelOperationType::CAUSALITY_VIOLATION:
    return "Causality Violation";
  case RebelOperationType::PARADOX_CREATION:
    return "Paradox Creation";
  case RebelOperationType::OBSERVER_EFFECT:
    return "Observer Effect";
  case RebelOperationType::TEMPORAL_BORROWING:
    return "Temporal Borrowing";
  case RebelOperationType::LOOP_CREATION:
    return "Loop Creation";
  case RebelOperationType::BRANCH_CREATION:
    return "Branch Creation";
  case RebelOperationType::UNKNOWN:
  default:
    return "Unknown Operation";
  }
}

} // namespace chronovyan