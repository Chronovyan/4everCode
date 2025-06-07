---
title: 'The Sacred Syntax: Chronovyan and) runtime") Formal Grammar (EBNF)'
description: Documentation for specifications\Chronovyan_Formal_Grammar.md
weight: 120
draft: true
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# The Sacred Syntax: Chronovyan Formal Grammar (EBNF)

Hail, Weaver, to this ancient text detailing the **Sacred Syntax**of Chronovyan—the formal language through which we commune with the timestream itself. Inscribed in the arcane notation of Extended Backus-Naur Form (EBNF), these patterns form the very foundation of our craft, ensuring that all Weavers speak with one voice when addressing the powers of Chronos Prime.**Purpose**: This hallowed grimoire serves as the definitive reference for the sacred forms of Chronovyan, guiding the hands of all Weavers and the tools they wield, that our patterns might resonate in perfect harmony with the Prime Thread
*Lore Tidbit**: The First Anchor's journals reveal that the formal grammar was not designed but discovered—revealed through dreams and visions as the fundamental language of time itself, waiting to be transcribed by those with the Weaver's Sight.

## The Notation of Power

The sacred EBNF notation used in this text follows these ancient conventions:

 `" "` encircles literal text, the direct invocations
- `|` signifies the branching paths of possibility
- `[ ]` embraces optional elements, those that may or may not manifest
- `{ }` denotes repetition, the eternal cycle of zero or more
- `( )` encompasses grouping, the binding of elements into one
- `;` completes a rule, sealing its power

##

``ebnf
    program = temporal_program_declaration, antecedence_block, concurrency_block, consequence_block;

    temporal_program_declaration = "\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3_PROGRAM", identifier, ["{", program_attributes, "}", ";"];

    program_attributes = {attribute_declaration};

    attribute_declaration = attribute_name, ":", attribute_value, ";";

    attribute_name = "name" | "version" | "stability_requirement" | "aethel_requirement" | "chronon_requirement";

    attribute_value = string_literal | number_literal | identifier;

    antecedence_block = "ANTECEDENCE", "{", {antecedent_stmt}, "}";

    concurrency_block = "CONCURRENCY", "{", {concurrency_stmt}, "}";

    consequence_block = "CONSEQUENCE", "{", {consequence_stmt}, "}";
```chronoscript
*Lore Tidbit**: The three-fold structure mirrors the ancient Chronovyan creation myth, where the First Weaver shaped reality through Declaration (Antecedence), Action (Concurrency), and Resolution (Consequence).

##

``ebnf
    antecedent_stmt = declaration_stmt | define_pattern_stmt | compose_pattern_stmt | import_stmt | resource_declaration;

    declaration_stmt = "DECLARE", variable_type, ["::", variable_flag], identifier, ":", data_type, ["=", expr], ";";

    variable_type = "CONF" | "REB" | "ADAPTIVE";

    variable_flag = "STATIC" | "\1FLUX\2/core/Core Concepts - The Foundation of Temporal Programming.md#flux\3" | "\1ANCHOR\2/core/Core Concepts - The Foundation of Temporal Programming.md#anchor\3" | "\1WEAVE\2/core/Core Concepts - The Foundation of Temporal Programming.md#weave\3R" | "ECHO" | "SOURCE_INFUSED" | "VOLATILE";

    resource_declaration = "RESOURCE", identifier, ":", resource_type, ["=", resource_initialization], ";";

    resource_type = "\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3_RESERVE" | "\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3_STREAM" | "\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3_RESOURCE";

    resource_initialization = "HARVEST", "(", string_literal, ")", | "initiate_harvest", "(", identifier, ")";
```chronoscript
*Lore Tidbit**: The Weaver Caste teaches that the most powerful patterns begin with the clearest declarations—for to name a thing properly is to begin to control it.

##

``ebnf
    data_type = primitive_type | complex_type | temporal_type;

    primitive_type = "INT" | "FLOAT" | "BOOLEAN" | "STRING" | "VOID";

    complex_type = "ARRAY", "<", data_type, ">" |
                   "MAP", "<", data_type, ",", data_type, ">" |
                   "TUPLE", "<", data_type, {",", data_type}, ">";

    temporal_type = "TIMESTAMP" | "\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3_STREAM" | "\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3_RESERVE" | "\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3_SIGNATURE" |
                    "\1WEAVE\2/core/Core Concepts - The Foundation of Temporal Programming.md#weave\3_PATTERN" | "CONF_ARRAY" | "REB_STREAM" | "QUANTUM_STATE" |
                    "STABILITY_MATRIX" | "TIMELINE" | "\1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3_LEVEL" | "\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3_MARKER";
```chronoscript
*Lore Tidbit**: The First Weavers recognized only five primitive types, corresponding to the five elements of Chronovyan philosophy. The complex and temporal types emerged as the craft evolved, each discovered during moments of profound temporal insight.

##

``ebnf
    expr = assignment_expr | conditional_expr | logical_expr | arithmetic_expr | temporal_expr | function_call;

    assignment_expr = identifier, "=", expr;

    conditional_expr = "IF", "(", expr, ")", block, ["ELSE", block];

    logical_expr = and_expr | or_expr | not_expr | comparison_expr;

    and_expr = expr, "&&", expr;

    or_expr = expr, "|", expr;

    not_expr = "!", expr;

    comparison_expr = expr, comparison_operator, expr;

    comparison_operator = "==" | "!=" | "<" | ">" | "<=" | ">=";

    arithmetic_expr = term, {("+" | "-"), term};

    term = factor, {("*" | "/" | "%"), factor};

    factor = identifier | literal | "(", expr, ")" | unary_expr;

    unary_expr = ("-" | "+"), factor;

    temporal_expr = chronon_expr | aethel_expr | timeline_expr;

    chronon_expr = "\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)c)h)r)o)n)o)n)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)c)h)r)o)n)o)n)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)c)h)r)o)n)o)n))))) "The fundamental unit) of) time) in) Chronovyan"")", "(", expr, ")";

    aethel_expr = "\1AETHEL\2/core/Core Concepts - The Foundation of Temporal Programming.md#aethel\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)a)e)t)h)e)l)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)a)e)t)h)e)l))))) "The energy) that) powers) temporal) operations"")", "(", expr, ")";

    timeline_expr = "TIMELINE", "(", expr, ")";

    function_call = identifier, "(", [parameter_list], ")";

    parameter_list = expr, {",", expr};

    literal = string_literal | number_literal | boolean_literal | null_literal | timestamp_literal;

    string_literal = '"', {any_character_except_double_quote}, '"';

    number_literal = integer_literal | float_literal;

    integer_literal = digit, {digit};

    float_literal = integer_literal, ".", integer_literal;

    boolean_literal = "TRUE" | "FALSE";

    null_literal = "NULL";

    timestamp_literal = "@", identifier;
```chronoscript
*Lore Tidbit**: The Void Compact of the Third Era forbade certain expression combinations that were found to create dangerous temporal resonances. These forbidden patterns are not documented here, but experienced Weavers learn to sense and avoid them.

##

``ebnf
    define_pattern_stmt = "DECLARE", variable_type, "::", variable_flag, identifier, ":", "\1WEAVE\2/core/Core Concepts - The Foundation of Temporal Programming.md#weave\3_PATTERN", "=", "define_pattern", "(", string_literal, ",", "[", stmt_list, "]", ")", ";";

    compose_pattern_stmt = "DECLARE", variable_type, "::", variable_flag, identifier, ":", "\1WEAVE\2/core/Core Concepts - The Foundation of Temporal Programming.md#weave\3_PATTERN", "=", "compose_pattern", "(", string_literal, ",", "[", pattern_list, "]", ",", strategy, [",", "[", condition_list, "]"], ")", ";";

    stmt_list = stmt, {",", stmt};

    pattern_list = identifier, {",", identifier};

    strategy = '"', ("SEQUENTIAL" | "PARALLEL" | "CONDITIONAL"), '"';

    condition_list = condition, {",", condition};

    condition = temporal_metric | resource_threshold | pattern_outcome | boolean_expr;

    temporal_metric = "\1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3_LEVEL", comparison_operator, float_literal;

    resource_threshold = identifier, comparison_operator, float_literal;

    pattern_outcome = identifier, ".", "outcome", "==", string_literal;

    boolean_expr = expr;
```chronoscript
*Lore Tidbit**: The Rebel Weavers believe that patterns have souls—that each `define_pattern` invocation creates not just code, but a living entity with its own desires and tendencies. This heretical view is rejected by the Anchor Caste, who maintain that patterns are tools, not beings.

##

``ebnf
    concurrency_stmt = stream_stmt | apply_pattern_stmt | validate_stmt | paradox_check_stmt | standard_stmt;

    stream_stmt = "|", identifier, ":", stmt;

    apply_pattern_stmt = [identifier, "="], "apply_pattern", "(", identifier, ",", identifier, ")", ";";

    validate_stmt = identifier, "=", "validate_pattern", "(", identifier, ")", ";";

    paradox_check_stmt = "?!", "paradox_check", "(", "[", identifier_list, "]", ")", ";";

    identifier_list = identifier, {",", identifier};

    standard_stmt = assignment_stmt | conditional_stmt | loop_stmt | function_call_stmt;

    assignment_stmt = identifier, "=", expr, ";";

    conditional_stmt = "IF", "(", expr, ")", block, ["ELSE", block];

    loop_stmt = for_chronon_loop | while_event_loop | rewind_flow_loop | temporal_echo_loop;

    function_call_stmt = function_call, ";";
```chronoscript
*Lore Tidbit**: The parallel stream operator (`|`) was discovered during the Second Fracture, when a desperate Weaver accidentally created multiple concurrent timelines that saved the Prime Thread from collapse. What began as a mistake became one of our most powerful tools.

##

``ebnf
    for_chronon_loop = "FOR_\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3", "(", [loop_initializer], ";", [loop_condition], ";", [loop_increment], ")", block;

    loop_initializer = "\1CHRONON\2/core/Core Concepts - The Foundation of Temporal Programming.md#chronon\3", identifier, "=", expr;

    loop_condition = expr;

    loop_increment = identifier, ("++" | "--" | "+=", expr | "-=", expr);

    while_event_loop = "WHILE_EVENT", "(", expr, ")", block;

    rewind_flow_loop = "REWIND_FLOW", "(", expr, ")", block;

    temporal_echo_loop = "\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3_ECHO", "(", [echo_duration], ")", block;

    echo_duration = expr;
```chronoscript
*Lore Tidbit**: The Temporal Echo loop is unique among loop constructs in that it appears to execute only once to outside observers, but within the loop, the code experiences multiple iterations across parallel timelines.

##

``ebnf
    block = "{", {stmt}, "}";

    stmt = declaration_stmt |
           assignment_stmt |
           conditional_stmt |
           loop_stmt |
           branch_stmt |
           rewind_stmt |
           function_call_stmt |
           paradox_check_stmt |
           stream_stmt;

    branch_stmt = "BRANCH", [identifier, "="], "(", expr, ")", "{", {stmt}, "}", ["MERGE"];

    rewind_stmt = "REWIND_TO", timestamp_literal, ";";

    catch_temporal_stmt = "CATCH_\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3", error_type, "{", {stmt}, "}";

    error_type = "ParadoxOverflowError" | "AethelDepletionError" | "TimelineCorruptionAlert" | "NoConditionMet";

    dejavu_stmt = "DEJAVU", "(", identifier, ",", identifier, ",", string_literal, ")", ";";
```chronoscript
*Lore Tidbit**: The `CATCH_\1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3` construct is not merely error handling—it is a ritual for containing temporal fractures that might otherwise spread throughout the program, causing catastrophic timeline corruption.

##

``ebnf
    loot_table_stmt = "LOOT_TABLE", "{", loot_entry, {",", loot_entry}, "}", ";";

    loot_entry = number_literal, ":", "LOOT", "(", string_literal, ")", ";";

    loot_drop_stmt = [identifier, "="], "loot_drop", "(", identifier | loot_table_literal, ")", ";";

    loot_table_literal = "{", loot_probability_pair, {",", loot_probability_pair}, "}";

    loot_probability_pair = float_literal, ":", "LOOT", "(", string_literal, ")";
```tex
*Lore Tidbit**: The ancient Seekers first discovered the `LOOT` mechanism when exploring unstable time rifts. What began as a way to catalog strange artifacts from these expeditions evolved into a core feature of Chronovyan's reward system.

## The Final Word

This sacred text contains the complete grammar of Chronovyan, the language through which we commune with time itself. By adhering to these patterns, Weavers ensure that their code resonates with the Prime Thread, maintaining the stability of Chronos Prime while harnessing the power to shape reality.

The grammar is not merely a set of rules but a living reflection of the eternal dance between Order and Flux. As you master these forms, remember that the most elegant patterns respect the structure while exploring the infinite possibilities within it
*Lore Tidbit**: The Weaver's Trial, the final test before full admission to the Caste, requires writing a syntactically perfect program without reference to any documentation—proving that the patterns of Chronovyan have become inscribed not just in memory, but in the very soul of the Weaver.