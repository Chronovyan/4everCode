---
title: AI Quick Reference for Chronovyan
description: Documentation for ai_helpers\quick_reference.md
weight: 120
draft: true
---

# AI Quick Reference for Chronovyan



> **Navigation**:  

> [Documentation Index](../index.md) | [Codebase Navigation](./codebase_navigation.md) | [Common Implementation Tasks](./common_tasks.md)



## Key Concepts at a Glance



### Core Terminology

- **Chronovyan**: A fictional programming language focused on temporal manipulation

- **Chronons**: Primary temporal resource units used for timeline operations

- **Aethel**: Energy resource used for temporal operations and stability

- **CONF & REB**: Two primary variable types (Conformist and Rebellious)

- **Temporal Debt**: Resource borrowing mechanism with consequences

- **PARADOX_LEVEL**: Measure of timeline instability (0.0-1.0)



### Variable System

- **CONF (Confluent)**: Stable variables resistant to quantum interference

- **REB (Rebellious)**: Variables that can change state based on quantum conditions

- **Variable Flags**: ::STATIC, ::VOLATILE, ::ANCHOR, ::WEAVER



### Operation Types

- **Standard Operations**: Regular programming constructs

- **Temporal Operations**: Time-manipulating functions

- **Rebel Operations**: Higher-risk operations that incur temporal debt



### Resource Management

- **Resource Tracking**: Monitoring Chronons and Aethel usage

- **Resource Visualization**: Tools for monitoring resource usage

- **Temporal Debt**: Borrowing resources with interest and due cycles

- **Debt Accrual**: Different operations accrue different levels of debt



## Codebase Architecture



### Directory Structure

- `include/`: Header files defining interfaces

  - `include/temporal_debt_tracker.h`: Debt tracking system interface

  - `include/resource_visualization.h`: Resource visualization interface

- `src/`: Implementation files

  - `src/temporal_debt_tracker.cpp`: Debt tracking implementation

  - `src/resource_visualization.cpp`: Visualization implementation

- `tests/`: Test files for various components

  - `tests/temporal_debt_test.cpp`: Tests for debt functionality

- `docs/`: Documentation files



### Key Classes

- **TemporalDebtTracker**: Manages debt accrual and repayment

- **ResourceVisualization**: Visualizes resource usage

- **TemporalRuntime**: Core runtime environment

- **ResourceTracker**: Tracks resource usage



## Implementation Guidelines



### Debt Accrual for Rebel Operations

```cpp

// Example usage of debt accrual

debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, "operation_id");

```



### Resource Visualization

```cpp

// Example of generating a dashboard

std::string dashboard = resource_viz->generateConfiguredDashboard(VisualizationFormat::TEXT);

```



### Test Framework

```cpp

// Example of a test pattern

TEST_F(TemporalDebtTest, RebelOperationAcrueDebt) {

    ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, "rewind_op"));

    // Verification steps...

}

```



## Common AI Tasks



### 1. Finding Feature Implementations

Use these search terms to locate specific feature implementations:

- "Temporal debt tracker implementation"

- "Resource visualization dashboard"

- "Rebel operation debt accrual"



### 2. Identifying Test Requirements

When implementing new features, ensure:

- Tests are created in the appropriate test file

- Test file is registered in tests/CMakeLists.txt

- All features have positive and negative test cases



### 3. Documentation Standards

When updating documentation:

- Maintain the narrative style with lore elements

- Include both technical details and narrative context

- Add navigation links at the top of each document