---
title: Runtime Mechanics
description: Documentation for runtime mechanics
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Runtime Mechanics

## Introduction

Welcome, Weaver, to the heart of Chronovyan's temporal engine, where timelines bend and paradoxes lurk. On Chronos Prime, the runtime is the Anchor's crucible, enforcing order amid the chaos of `WEAVE_PATTERN`s. This guide unravels the mechanics of `PARADOX_LEVEL` management, paradox detection, and sandboxing, arming you to craft stable time-threads or risk a Great Unraveling.

**Purpose**: This document explains the runtime's rules for managing temporal instability, detecting conflicts, and isolating risky weaves. It provides a precise definition of how each language construct executes, how state changes propagate, and how temporal operations affect the program.

**Lore Tidbit**: The runtime hums with the Pulse of Time, a rhythm Anchors swear to protect. Its code was forged in the aftermath of the First Fracture, when a Seeker's hubris nearly unwove reality.

## Execution Model

### Program Lifecycle

A Chronovyan program executes in a well-defined sequence:

1. **Initialization**
   - Resource allocation (Aethel, Chronon)
   - PARADOX_LEVEL initialization (baseline of 0.1)
   - System stability metrics setup

2. **ANTECEDENCE Phase**
   - All declarations processed sequentially
   - WEAVE_PATTERNs defined but not executed
   - Resources are harvested and initialized
   - Chronon stream allocation
   - No execution of actual operations

3. **CONCURRENCY Phase**
   - Parallel streams execute concurrently
   - WEAVE_PATTERNs applied according to their strategies
   - Temporal operations executed
   - Resource consumption occurs
   - PARADOX_LEVEL updated continuously

4. **CONSEQUENCE Phase**
   - Paradox checks evaluated
   - Error handling through CATCH_TEMPORAL
   - Rewards determined through LOOT_TABLE
   - Timeline auditing via DEJAVU
   - Final system state established

5. **Termination**
   - Resources released or preserved
   - PARADOX_LEVEL final assessment
   - Program output finalized

### Execution Context

Each operation in Chronovyan executes within a specific context that contains:

- **Timeline**: The current execution path
- **Temporal Position**: The current point in the timeline
- **Resource State**: Available Aethel and Chronon
- **PARADOX_LEVEL**: Current system instability
- **Variable Environment**: All accessible variables
- **Anchor Points**: Available REWIND_TO destinations

Contexts can be nested (e.g., within sandboxes) with defined inheritance rules.

## PARADOX_LEVEL Management

The `PARADOX_LEVEL` (0.0–1.0) is the global gauge of timeline instability, the Weaver's constant shadow.

### Initialization

- **Base Value**: Starts at 0.1, ambient noise of Chronos Prime.  
- **Adjustments**:  
  - +0.05 per `||` stream or `BRANCH`.  
  - +0.1 for low `AETHEL_RESERVE` (< 100).  
  - -0.05 for high `CHRONON_EFFICIENCY` (> 0.9).  
  - +0.02 per nested strategy (e.g., `PARALLEL` in `SEQUENTIAL`).  
- **Example**: 2 streams, low `Aethel`, nested `CONDITIONAL` → `0.1 + 0.05*2 + 0.1 + 0.02 = 0.32`.

### Accumulation

- **Instability_Index**: Added per `WEAVE_PATTERN`:  
  - **Sequential**: `sum(Index_i)`.  
  - **Parallel**: `max(Index_i)`.  
  - **Conditional**: `sum(p_i * Index_i)`, where `p_i` is condition probability.  
- **Operation Modifiers**:  
  - `REWIND_TO`: +0.05.  
  - `FAST_FORWARD`: +0.03.  
  - `BRANCH`: +0.07.  
  - `CYCLE`: +0.01 per iteration.  
- **Conflict Penalties**:  
  - Resource Contention: 0.03 (`SEQUENTIAL`), 0.1 (`PARALLEL`), 0.05 (`CONDITIONAL`), 0.12 (nested).  
  - Temporal Interference: 0.07 (`SEQUENTIAL`), 0.15 (`PARALLEL`), 0.1 (`CONDITIONAL`), 0.1 (nested).  
  - State Divergence: 0.1 (`SEQUENTIAL`), 0.2 (`PARALLEL`), 0.15 (`CONDITIONAL`), 0.18 (nested).  

### Decay and Reduction

- **Natural Decay**: -0.01 per 5 stable ticks (no conflicts).  
- **Active Reduction**:  
  - `INFUSE`: Reduces by `Aethel_Spent * 0.001` (max 0.2).  
  - `stabilize_timeline`: Up to -0.3, costing `Aethel`.  
- **Example**:  
  ```chronovyan
  New_Risk = stabilize_timeline(Energy, 100); // Reduces PARADOX_LEVEL by 0.1
  ```

### Thresholds

- **0.0–0.3**: Stable.  
- **0.31–0.5**: Caution (`TRACE` logs, +10% `Aethel` cost).  
- **0.51–0.79**: High Risk (`NOTIFY_ANCHOR_CASTE`, restrict `BRANCH`).  
- **0.8+**: Critical (`ParadoxOverflowError`).  

**Lore Tidbit**: A `PARADOX_LEVEL` above 0.8 is a "Void Whisper," a dire omen that summons the Anchor Caste to intervene.

## Paradox Detection

The runtime vigilantly detects conflicts to prevent timeline fractures.

### Mechanism

- **Dependency Graph**: Tracks `REB::FLUX` variables, resources, and anchors.  
- **Pre-Execution**: `validate_pattern` simulates execution, flagging risks.  
- **Runtime Monitoring**: Checks resources, anchors, and states via `SNAPSHOT`s.  
- **Synchronization**: Reconciles states at strategy boundaries (e.g., end of `PARALLEL`).  

### Strategy-Specific Conflicts

- **Sequential**:  
  - Resource Dependency Violation: Consumed `LOOT` unavailable later (+0.03).  
  - Anchor Inconsistency: `REWIND_TO` invalid anchor (+0.07).  
  - State Invalidation: `FAST_FORWARD` skips critical setup (+0.1).  
- **Parallel**:  
  - Resource Contention: Concurrent `Aethel` overuse (+0.1).  
  - Temporal Interference: Overlapping `REWIND`/`BRANCH` (+0.15).  
  - State Divergence: Inconsistent `REB::FLUX` states (+0.2).  
- **Conditional**:  
  - Resource Contention: Uncertain resource needs (+0.05).  
  - Temporal Interference: Condition-driven `REWIND` (+0.1).  
  - State Divergence: Retroactive condition invalidation (+0.15).  
- **Cross-Strategy**: Nested conflicts amplify penalties (e.g., +0.12 for nested contention).  

**Example**:
```chronovyan
?! paradox_check([Stability_Field, Scout, Energy_Harvest]);
```

**Lore Tidbit**: `paradox_check` is the Weaver's Sight, a mystic scan that peers into the Void's cracks.

## Sandboxing

Sandboxing isolates nested `WEAVE_PATTERN`s to contain paradoxes, limiting their "blast radius."

### Containment Levels

- **Low-Risk** (`Instability_Index < 0.3`):  
  - Full `AETHEL_RESERVE` access.  
  - Contained failures (no parent impact).  
  - Example: Simple `Stabilization`.  
- **Medium-Risk** (0.3–0.6):  
  - 50% `AETHEL_RESERVE`, capped `Chronon`.  
  - Escalates if unhandled.  
  - Example: `CONDITIONAL` with risky conditions.  
- **High-Risk** (>0.6):  
  - 25% `AETHEL_RESERVE`, single `Chronon` stream.  
  - Always escalates, halts sandbox.  
  - Example: Nested `PARALLEL` with `BRANCH`.  

### Resource Quota

- Formula: `Quota = Parent_Aethel * (1 - Instability_Index)`.  
- Example: `Dual_Timeline` (Index=0.7) in `Master_Weave` (200 `Aethel`) gets `200 * 0.3 = 60 Aethel`.

### CATCH_TEMPORAL Integration

- **Local Handling**: Sandbox-level `CATCH_TEMPORAL` contains failures.  
- **Escalation**: Unhandled medium/high-risk paradoxes escalate to parent.  
- **Example**:
  ```chronovyan
  CATCH_TEMPORAL TimelineCorruptionAlert {
      REWIND_TO @Origin;
      NOTIFY_ANCHOR_CASTE("Nested fracture contained!");
  }
  ```

**Lore Tidbit**: Sandboxing is a "Temporal Ward," a shimmering shield woven by Anchors to cage reckless weaves.

## Variable Interaction Mechanics

### Variable Types Interaction

1. **CONF-CONF Interaction**
   - Predictable, stable outcomes
   - No PARADOX_LEVEL increase
   - Linear execution flow
   - Operations are atomic and consistent

2. **REB-REB Interaction**
   - Potential for superposition of outcomes
   - PARADOX_LEVEL increase of 0.02-0.05 per interaction
   - Non-linear effects possible
   - Operations may produce multiple potential outcomes

3. **CONF-REB Interaction**
   - CONF attempts to stabilize REB
   - REB may corrupt CONF at high PARADOX_LEVEL
   - PARADOX_LEVEL increase of 0.01-0.03
   - Outcome depends on PARADOX_LEVEL and variable flags

4. **ADAPTIVE Interaction Rules**
   - Behaves like CONF when PARADOX_LEVEL < 0.3
   - Exhibits mixed behavior between 0.3-0.7
   - Behaves like REB when PARADOX_LEVEL > 0.7
   - Adapts based on interaction partners

### Flag-Based Behavior

Different variable flags affect runtime behavior:

1. **::STATIC**
   - Value cannot change after initialization
   - Reduces PARADOX_LEVEL by 0.01 per variable
   - Exempt from temporal fluctuations
   - Behavior: Any attempt to modify raises `StaticModificationError`

2. **::FLUX** / **::VOLATILE**
   - Value can change unpredictably
   - Increases PARADOX_LEVEL by 0.02 per variable
   - Subject to quantum effects
   - Behavior: Value can spontaneously change within defined bounds when PARADOX_LEVEL > 0.5

3. **::ANCHOR**
   - Serves as a stability point for timelines
   - Can be targeted by REWIND_TO operations
   - Creates a checkpoint in the execution history
   - Behavior: When referenced by REWIND_TO, execution state reverts to the point of anchor declaration

4. **::WEAVER**
   - Can manipulate other variables' temporal properties
   - Influences timeline branching and merging
   - Higher Aethel consumption
   - Behavior: Can be used in operations that modify timeline structure

## Resource Management

### Chronons

1. **Allocation**
   - Allocated at program initialization
   - Required for all operations
   - Consumed at different rates by different operations
   - Can be harvested from CHRONON_STREAM sources

2. **Consumption Rules**
   - Basic operations: 1 Chronon
   - Control structures: 2 Chronons
   - Temporal operations: 5-20 Chronons depending on complexity
   - WEAVE_PATTERN application: Sum of contained operations + 5

3. **Depletion Effects**
   - At 20%: Operations slow by 50%
   - At 10%: Only CONF operations permitted
   - At 5%: Warning issued, PARADOX_LEVEL +0.1
   - At 0%: Program terminates with ChrononsDepletedError

### Aethel

1. **Generation**
   - CONF operations generate 0.1 Aethel per operation
   - Successful WEAVE_PATTERN execution generates 1-5 Aethel
   - HARVEST operations generate 10-50 Aethel
   - System stability above 0.8 generates 0.5 Aethel per cycle

2. **Consumption**
   - REB operations consume 1-10 Aethel
   - Temporal manipulations (REWIND_FLOW, etc.) consume 10-50 Aethel
   - PARADOX_LEVEL reduction consumes Aethel proportional to reduction amount
   - SOURCE_INFUSED operations consume 50-100 Aethel

3. **Depletion Effects**
   - At 20%: REB operations cost 2x Aethel
   - At 10%: PARADOX_LEVEL +0.2
   - At 5%: Only CONF operations permitted
   - At 0%: AethelDepletionError, forced timeline stabilization

### Resource Interaction

1. **Chronon-Aethel Exchange**
   - Chronons can be converted to Aethel at 1:0.1 ratio
   - Aethel can be converted to Chronons at 10:1 ratio
   - Exchange operations increase PARADOX_LEVEL by 0.01

## Complete Runtime Example

A weave showcasing runtime mechanics:

```chronovyan
ANTECEDENCE {
    DECLARE REB::SOURCE_INFUSED Time_Crystals : CHRONON_STREAM = HARVEST("Prime_Thread");
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift"),
        REWIND_TO @Fork
    ]);
    DECLARE CONF::STATIC Dual_Timeline : WEAVE_PATTERN = compose_pattern("Dual", 
                                                       [Stabilization, Exploration], 
                                                       "PARALLEL");
}

CONCURRENCY {
    || Stream_1: DECLARE REB::FLUX Energy : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    Risk = validate_pattern(Dual_Timeline);
    Success = apply_pattern(Dual_Timeline, System_Stabilize);
}

CONSEQUENCE {
    ?! paradox_check([Stability_Field, Scout, Energy]);
    CATCH_TEMPORAL ParadoxOverflowError {
        REWIND_TO @Origin;
        INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
        New_Risk = stabilize_timeline(Energy, 100);
    }
    DEJAVU(Dual_Timeline, Risk, "Runtime Audit");
    IF Success {
        Reward = loot_drop({ 0.95: LOOT("Legendary_Thread") });
    }
}
```

**Runtime Analysis:**
- Initial PARADOX_LEVEL: 0.1
- After Stream_1: +0.05 → 0.15
- PARALLEL composition: Higher instability risk → ~0.2
- REB::FLUX variables (Scout, Energy): +0.04 → 0.24
- REWIND_TO operation: +0.05 → 0.29
- Resource consumption monitoring throughout execution
- Paradox check prevents state inconsistencies
- CATCH_TEMPORAL provides safety against critical paradoxes
- INFUSE and stabilize_timeline actively reduce PARADOX_LEVEL
- DEJAVU performs timeline auditing for future optimization

**Lore Tidbit**: A Weaver who masters the runtime is said to hear the Void's heartbeat, a secret known only to the High Anchors. 