---
title: Chronicle Bytecode Format Specification
description: Documentation for chronicle_bytecode_format.md
weight: 100
draft: true
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronicle Bytecode Format Specification

## 1. Introduction

The Chronicle Bytecode Format is the target output of the Chronicle Compiler, designed to efficiently represent [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime") programs in a format suitable for execution by the Chronicle Virtual Machine (ChronVM). This bytecode format is optimized for the unique temporal semantics and resource management features of the Chronovyan language, enabling efficient execution while preserving the language's temporal safety guarantees.

This document specifies the structure, instruction set, and encoding of the Chronicle Bytecode Format, providing a reference for both compiler developers and runtime implementers.

## 2. File Format

### 2.1 File Extension and Magic Number

Chronicle Bytecode files use the `.chronx` file extension. Each file begins with the magic number`0x4348524E` ("CHRN" in ASCII) to identify it as a Chronicle Bytecode file.

### 2.2 Overall Structure

A Chronicle Bytecode file consists of the following sections:
```tex
----------------+

    | Header         
----------------+

    | Constant Pool  
----------------+

    | Type Pool      
----------------+

    | Resource Pool  
----------------+

    | Code Section   
----------------+

    | Metadata       
----------------+

    | Debug Info     | (Optional
----------------+
```text

    ### 2.3 Header

    The header contains essential information about the bytecode file:
```text

    struct ChronicleHeader {

        uint32_t magic;              // Magic number: 0x4348524E ("CHRN")

        uint16_t major_version;      // Major bytecode format version

        uint16_t minor_version;      // Minor bytecode format version

        uint32_t flags;              // Bytecode flags

        uint32_t constant_pool_size; // Number of entries in constant pool

        uint32_t type_pool_size;     // Number of entries in type pool

        uint32_t resource_pool_size; // Number of entries in resource pool

        uint32_t code_size;          // Size of code section in bytes

        uint32_t entry_point;        // Offset to main entry point

        uint32_t checksum;           // CRC32 checksum of the bytecode

    }
```chronoscript

    The `flags` field can contain the following bit flags:`0x0001`: Contains debug information`0x0002`: Uses temporal features`0x0004`: Has custom resource optimizations`0x0008`: Contains CONF/REB variable interactions`0x0010`: Requires specific ChronVM version

    ### 2.4 Constant Pool

    The constant pool contains all constant values used in the program:

     Integers

     Floating-point numbers

     Strings

     Boolean values

     Temporal values (\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)c)h)r)o)n)o)n)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)c)h)r)o)n)o)n)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)c)h)r)o)n)o)n))))) "The fundamental unit) of) time) in) Chronovyan"") constants, \1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l))))) "The energy) that) powers) temporal) operations"") constants)

     Reference to other constants (arrays, vectors, maps)

    Each constant pool entry has the following structure:
```text

    struct ConstantPoolEntry {

        uint8_t type;     // Type of the constant

        uint8_t flags;    // Flags for the constant

        uint16_t length;  // Length of the constant data in bytes

        uint8_t data[];   // Variable-length data for the constant

    }
```text

    ### 2.5 Type Pool

    The type pool contains definitions for all types used in the program:

     Primitive types

     Complex types (vectors, maps)

     User-defined types

     Function types

    Each type pool entry has the following structure:
```text

    struct TypePoolEntry {

        uint8_t type_kind;    // Kind of type (primitive, vector, map, etc.)

        uint8_t flags;        // Type flags (CONF/REB, etc.)

        uint16_t name_index;  // Index into constant pool for type name

        uint32_t size;        // Size of the type in bytes

        uint16_t fields_count; // Number of fields (for complex types)

        uint16_t fields[];    // Indices into the type pool for field types

    }
```text

    ### 2.6 Resource Pool

    The resource pool contains information about resource usage throughout the program:

     Chronon allocations and expenditures

     Aethel allocations and expenditures

     Temporal debt operations

     Resource optimization hints

    Each resource pool entry has the following structure:
```text

    struct ResourcePoolEntry {

        uint8_t resource_type;   // Type of resource (Chronon, Aethel)

        uint8_t operation;       // Operation (allocate, expend, borrow, etc.)

        uint16_t amount_index;   // Index into constant pool for amount

        uint32_t code_offset;    // Offset in code section where used

    }
```text

    ### 2.7 Code Section

    The code section contains the actual bytecode instructions for the program. Instructions are variable-length, with each instruction consisting of an opcode followed by zero or more operands.

    ### 2.8 Metadata

    The metadata section contains additional information about the program:

     Module name

     Import/export information

     Author information

     Compilation information

     Platform compatibility information

    ### 2.9 Debug Information (Optional)

    If present, the debug information section contains:

     Source file mappings

     Line number tables

     Local variable information

     Stack trace information

    ## 3. Instruction Set

    The Chronicle Bytecode instruction set is designed to efficiently represent Chronovyan programs. Instructions are categorized by their functionality:

    ### 3.1 Instruction Format

    Each instruction has the following format:
```tex
--------+----------+

    | Opcode | Operands 
--------+----------+

      1 byte   Variable
```text

    The opcode is a single byte that identifies the instruction. The number and type of operands depend on the opcode.

    ### 3.2 Basic Instructions

    #### 3.2.1 Stack Operations

     `nop`: No operation`push <const_pool_index>`: Push constant onto stack`pop`: Pop value from stack`dup`: Duplicate top stack value`swap`: Swap top two stack values

    #### 3.2.2 Local Variable Operations

     `load <var_index>`: Load local variable onto stack`store <var_index>`: Store stack value into local variable`inc_local <var_index>`: Increment local variable`dec_local <var_index>`: Decrement local variable

    #### 3.2.3 Arithmetic Operations

     `add`: Add top two stack values`sub`: Subtract top stack value from second stack value`mul`: Multiply top two stack values`div`: Divide second stack value by top stack value`mod`: Modulo operation on top two stack values`neg`: Negate top stack value

    #### 3.2.4 Logical Operations

     `and`: Logical AND of top two stack values`or`: Logical OR of top two stack values`not`: Logical NOT of top stack value`xor`: Logical XOR of top two stack values

    #### 3.2.5 Comparison Operations

     `eq`: Equal comparison`ne`: Not equal comparison`lt`: Less than comparison`le`: Less than or equal comparison`gt`: Greater than comparison`ge`: Greater than or equal comparison

    #### 3.2.6 Control Flow

     `jmp <offset>`: Unconditional jump`jmp_if <offset>`: Jump if top of stack is true`jmp_if_not <offset>`: Jump if top of stack is false`call <function_index>`: Call function`ret`: Return from function`throw <exception_index>`: Throw exception

    ### 3.3 Temporal Instructions

    #### 3.3.1 Resource Management

     `alloc_chronons <amount_index>`: Allocate Chronons`alloc_\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l))))) <amount_index>`: Allocate Aethel`expend_chronons <amount_index>`: Expend Chronons`expend_\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l))))) <amount_index>`: Expend Aethel`check_resources`: Check if resources are available

    #### 3.3.2 Timeline Operations

     `branch_timeline <path_count>`: Branch timeline into multiple paths`merge_timelines <strategy_index>`: Merge timelines with resolution strategy`rewind_flow <target_offset>`: Rewind execution flow`create_anchor <name_index>`: Create temporal anchor`jump_to_anchor <name_index>`: Jump to temporal anchor

    #### 3.3.3 Temporal Debt Operations

     `borrow_chronons <amount_index>`: Borrow Chronons`borrow_\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l))))) <amount_index>`: Borrow Aethel`repay_chronon_debt <amount_index>`: Repay Chronon debt`repay_aethel_debt <amount_index>`: Repay Aethel debt`advance_debt_cycle`: Advance temporal debt cycle

    ### 3.4 Type Operations

    #### 3.4.1 Object Operations

     `new <type_index>`: Create new object`get_field <field_index>`: Get field from object`set_field <field_index>`: Set field in object`instance_of <type_index>`: Check if object is instance of type

    #### 3.4.2 Array/Vector Operations

     `new_array <type_index>`: Create new array`array_length`: Get array length`array_load`: Load from array`array_store`: Store into array

    #### 3.4.3 Map Operations

     `new_map <key_type_index> <value_type_index>`: Create new map`map_put`: Put key-value pair into map`map_get`: Get value from map by key`map_contains`: Check if map contains key`map_remove`: Remove key-value pair from map

    ### 3.5 CONF/REB Instructions

    #### 3.5.1 Variable Classification

     `mark_conf <var_index>`: Mark variable as CONF`mark_reb <var_index>`: Mark variable as REB`check_classification <var_index>`: Check variable classification

    #### 3.5.2 \1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x))))) "A temporal inconsistency) that) must) be) resolved"") Prevention

     `check_\1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x)))))`: Check for potential \1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x)))))`resolve_\1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x))))) <strategy_index>`: Apply \1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x))))) resolution strategy

    ## 4. Bytecode Encoding

    ### 4.1 Numeric Encoding

     Unsigned integers are encoded in little-endian format

     Signed integers use two's complement representation

     Floating-point numbers use IEEE 754 format

    ### 4.2 String Encoding

    Strings are encoded in UTF-8 format, preceded by their length.

    ### 4.3 Instruction Encoding

    Instructions are encoded as an opcode byte followed by operand bytes. The number and size of operands depend on the opcode.

    #### 4.3.1 Operand Types

     `u8`: Unsigned 8-bit integer`u16`: Unsigned 16-bit integer`u32`: Unsigned 32-bit integer`i8`: Signed 8-bit integer`i16`: Signed 16-bit integer`i32`: Signed 32-bit integer`index`: Index into a pool (constant, type, function)`offset`: Jump offset relative to current instruction

    #### 4.3.2 Operand Encoding Examples

     `push <const_pool_index>`: Opcode (0x10) followed by a 16-bit index`jmp <offset>`: Opcode (0x50) followed by a 16-bit signed offset`call <function_index>`: Opcode (0x58) followed by a 16-bit index

    ### 4.4 Constant Pool Encoding

    Constants in the constant pool are encoded based on their type:

     Integer: 4 or 8 bytes in little-endian format

     Float: 4 or 8 bytes in IEEE 754 format

     String: Length prefix followed by UTF-8 bytes

     Boolean: Single byte (0 for false, 1 for true)

     Chronon/Aethel: Specialized encoding for temporal values

    ## 5. Execution Model

    ### 5.1 Stack-Based Execution

    The ChronVM uses a stack-based execution model. Most operations pop their operands from the stack and push their results back onto the stack.

    ### 5.2 Resource Accounting

    The ChronVM maintains resource counters for Chronons and Aethel. Resource allocation and expenditure instructions update these counters, and the VM checks resource availability before executing resource-intensive operations.

    ### 5.3 Timeline Management

    The ChronVM maintains a timeline stack to track the current execution path and timeline branches. Timeline operations manipulate this stack to implement Chronovyan's temporal semantics.

    ### 5.4 CONF/REB Variable Tracking

    The ChronVM tracks the classification of variables as CONF or REB and enforces the corresponding constraints to prevent paradoxes.

    ## 6. Examples

    ### 6.1 Simple Resource Expenditure

    ####

    ``text

        EXPEND_RESOURCES {

            \1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3: 10,

            \1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3: 5

        } {

            LET x = 42;

            PRINT(x);

        }
```text

    ####

    ``text

        // Constant pool:

        // #1: Integer 10 (Aethel amount)

        // #2: Integer 5 (Chronon amount)

        // #3: Integer 42 (x value)

        // #4: String "x" (variable name)

        // Code:

        push #1                  // Push Aethel amount

        expend_\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l)))))            // Expend Aethel

        push #2                  // Push Chronon amount

        expend_chronons          // Expend Chronons

        check_resources          // Check if resources are available

        jmp_if_not insufficient_resources

        // Main block

        push #3                  // Push 42

        store 0                  // Store in local variable 0 (x)

        load 0                   // Load x

        invoke_print             // Print x

        jmp end

        insufficient_resources:  // Handle insufficient resources

        // ...

        end:

        ret                      // Return
```chronoscript

    ### 6.2 Temporal Loop

    ####

    ``text

        REWIND_FLOW (i < 10) {

            LET i = i + 1;

            IF (i == 5) {

                REVERSE_FLOW;

            }

        }
```text

    ####

    ``text

        // Constant pool:

        // #1: Integer 0 (initial i)

        // #2: Integer 10 (comparison value)

        // #3: Integer 1 (increment)

        // #4: Integer 5 (condition value)

        // Initialize

        push #1              // Push 0

        store 0              // Store in i (local variable 0)

        loop_start:

        load 0               // Load i

        push #2              // Push 10

        lt                   // i < 10

        jmp_if_not loop_end  // Exit if not true

        // Loop body

        load 0               // Load i

        push #3              // Push 1

        add                  // i + 1

        store 0              // Store result back in i

        // Check condition

        load 0               // Load i

        push #4              // Push 5

        eq                   // i == 5

        jmp_if_not continue  // Skip if not true

        // Reverse flow

        rewind_flow loop_start

        continue:

        jmp loop_start       // Next iteration

        loop_end:

        ret                  // Return
```text

    ### 6.3 Timeline Branching

    ####

    ``text

        BRANCH_TIMELINE {

            PATH_A: {

                LET result = "Timeline A";

            }

            PATH_B: {

                LET result = "Timeline B";

            }

        }
```text

    ####

    ``text

        // Constant pool:

        // #1: String "Timeline A"

        // #2: String "Timeline B"

        // #3: Integer 2 (path count)

        // Branch timeline

        push #3                  // Push path count (2)

        branch_timeline          // Create timeline branches

        jmp_table [path_a, path_b, merge_point]

        path_a:

        push #1                  // Push "Timeline A"

        store 0                  // Store in result (local variable 0)

        jmp merge_point

        path_b:

        push #2                  // Push "Timeline B"

        store 0                  // Store in result (local variable 0)

        jmp merge_point

        merge_point:

        merge_timelines          // Merge timelines

        // Continue execution

        ret

```text

## 7. Validation

The ChronVM validates bytecode before execution to ensure:

 Correctness: Bytecode follows the format specification

 Resource consistency: Resource allocations and expenditures are balanced

 Timeline consistency: Timeline branches and merges are properly matched

 CONF/REB consistency: No paradoxes can occur from variable interactions

 Type safety: Operations are performed on compatible types

## 8. Future Extensions

The Chronicle Bytecode Format is designed to be extensible to accommodate future language features:

 Advanced temporal patterns

 Parallel timeline execution

 Custom resource types

 Extended type system

 Interoperability with other languages

 Performance optimizations

 Security features

## 9. Conclusion

The Chronicle Bytecode Format provides an efficient representation of Chronovyan programs for execution by the ChronVM. Its design emphasizes the unique features of the Chronovyan language, particularly its temporal semantics, resource management, and CONF/REB variable classification. By providing a well-defined bytecode format, we enable consistent execution of Chronovyan programs across different implementations of the ChronVM.

As the Chronovyan language evolves, the Chronicle Bytecode Format will be extended to support new features while maintaining backward compatibility with existing bytecode files. This evolution will be guided by the principles of efficiency, safety, and expressiveness that are central to the Chronovyan language design.