---
title: Chronovyan Lexer and Parser Design
description: Documentation for lexer_parser_design.md
weight: 100
draft: true
---

# Chronovyan Lexer and Parser Design

## 1. Introduction

This document outlines the design for the lexical analyzer (lexer) and syntax analyzer (parser) components of the Chronicle Compiler. These components form the front-end of the compiler, responsible for converting Chronovyan source code into an Abstract Syntax Tree (AST) representation that can be further processed by subsequent compiler phases.

The design prioritizes:
- Clear separation of concerns between lexical and syntactic analysis
- Robust error reporting with meaningful diagnostics
- Extensibility to accommodate future language features
- Performance for handling large source files
- Maintainability through modular design

## 2. Lexical Analysis

The lexical analyzer (lexer) is responsible for converting a stream of characters into a stream of tokens. Each token represents a lexical unit in the Chronovyan language, such as keywords, identifiers, literals, and operators.

### 2.1 Token Types

The following token types will be recognized by the Chronovyan lexer:

#### 2.1.1 Keywords

```
ALLOCATE_RESOURCES  EXPEND_RESOURCES  BORROW_RESOURCES  REPAY_DEBT
DECLARE             CONST             CONF              REB
IF                  ELSE              WHILE             FOR
FUNCTION            RETURN            IMPORT            EXPORT
BRANCH_TIMELINE     MERGE_TIMELINES   REWIND_FLOW       REVERSE_FLOW
ANCHOR              WEAVER            STATIC            VOLATILE
CHRONON             AETHEL            INTEGER           FLOAT
STRING              BOOLEAN           VECTOR            MAP
```

#### 2.1.2 Operators

```
+       -       *       /       %       =       ==      !=      
<       >       <=      >=      &&      ||      !       ++      
--      +=      -=      *=      /=      %=      ::      ->
```

#### 2.1.3 Punctuation

```
{       }       (       )       [       ]       ;       :       
,       .       
```

#### 2.1.4 Literals

- Integer literals: `123`, `0`, `-456`
- Float literals: `123.456`, `.123`, `123.`
- String literals: `"Hello, world!"`, `"Embedded \"quotes\""`, `"Multiline\nstrings"`
- Boolean literals: `TRUE`, `FALSE`
- Resource literals: `10C` (Chronons), `5A` (Aethel)

#### 2.1.5 Identifiers

Identifiers start with a letter or underscore and can contain letters, digits, and underscores:
- `variable_name`
- `FunctionName`
- `_privateVar`
- `PATH_A`

### 2.2 Lexer Implementation

The lexer will be implemented using a deterministic finite automaton (DFA) approach, with the following components:

#### 2.2.1 Token Class

```cpp
struct Token {
    TokenType type;
    std::string lexeme;
    SourceLocation location;
    std::optional<std::variant<int, double, std::string, bool>> value;
};
```

#### 2.2.2 Lexer Class

```cpp
class Lexer {
public:
    Lexer(const std::string& source, const std::string& filename);
    
    Token nextToken();
    Token peekToken();
    void consumeToken();
    
    const std::vector<DiagnosticMessage>& getDiagnostics() const;
    
private:
    std::string source;
    std::string filename;
    size_t position;
    size_t line;
    size_t column;
    std::vector<DiagnosticMessage> diagnostics;
    
    char advance();
    char peek();
    char peekNext();
    bool match(char expected);
    
    Token scanToken();
    Token identifier();
    Token number();
    Token string();
    
    bool isAtEnd() const;
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    
    void skipWhitespace();
    void skipComment();
    
    void addError(const std::string& message);
};
```

#### 2.2.3 Error Handling

The lexer will report the following types of errors:

- Unterminated string literals
- Invalid numeric literals
- Invalid characters
- Invalid escape sequences in strings

Each error will include:
- Error message
- Source location (file, line, column)
- Contextual information (e.g., the problematic character or token)

### 2.3 Lexer Optimizations

- **Character Classification Tables**: Precomputed tables for fast character classification
- **Token Interning**: Reuse memory for common tokens to reduce memory usage
- **Buffered Input**: Process source code in chunks for better cache locality
- **Error Recovery**: Skip to next line or token boundary after encountering errors

## 3. Syntax Analysis

The syntax analyzer (parser) processes the stream of tokens produced by the lexer to build an Abstract Syntax Tree (AST) that represents the syntactic structure of the program.

### 3.1 Grammar Overview

The Chronovyan grammar will be defined using a context-free grammar notation. Below is a simplified subset of the grammar:

```
Program         → Declaration*
Declaration     → FunctionDecl | VariableDecl | ResourceDecl | TemporalDecl
FunctionDecl    → "FUNCTION" IDENTIFIER "(" Parameters? ")" Block
Parameters      → Parameter ("," Parameter)*
Parameter       → TypeAnnotation IDENTIFIER
VariableDecl    → ("LET" | "CONST") IDENTIFIER (":" TypeAnnotation)? "=" Expression ";"
ResourceDecl    → ResourceAlloc | ResourceExpend | ResourceBorrow
ResourceAlloc   → "ALLOCATE_RESOURCES" "{" ResourceList "}" Block
ResourceExpend  → "EXPEND_RESOURCES" "{" ResourceList "}" Block
ResourceBorrow  → "BORROW_RESOURCES" "{" ResourceList "}" Block
ResourceList    → ResourceItem ("," ResourceItem)*
ResourceItem    → ("CHRONON" | "AETHEL") ":" Expression
TemporalDecl    → BranchTimeline | MergeTimelines | RewindFlow
BranchTimeline  → "BRANCH_TIMELINE" "{" PathList "}"
PathList        → PathDecl+
PathDecl        → IDENTIFIER ":" Block
MergeTimelines  → "MERGE_TIMELINES" "(" Expression ")"
RewindFlow      → "REWIND_FLOW" "(" Expression ")" Block
Block           → "{" Declaration* "}"
Statement       → ExprStmt | IfStmt | WhileStmt | ForStmt | ReturnStmt | Block
ExprStmt        → Expression ";"
IfStmt          → "IF" "(" Expression ")" Statement ("ELSE" Statement)?
WhileStmt       → "WHILE" "(" Expression ")" Statement
ForStmt         → "FOR" "(" (VariableDecl | ExprStmt) Expression? ";" Expression? ")" Statement
ReturnStmt      → "RETURN" Expression? ";"
Expression      → Assignment
Assignment      → (Call ".")? IDENTIFIER ("=" | "+=" | "-=" | "*=" | "/=" | "%=") Assignment | LogicOr
LogicOr         → LogicAnd ("||" LogicAnd)*
LogicAnd        → Equality ("&&" Equality)*
Equality        → Comparison (("==" | "!=") Comparison)*
Comparison      → Term (("<" | ">" | "<=" | ">=") Term)*
Term            → Factor (("+" | "-") Factor)*
Factor          → Unary (("*" | "/" | "%") Unary)*
Unary           → ("!" | "-" | "++" | "--") Unary | Call
Call            → Primary ("(" Arguments? ")" | "[" Expression "]" | "." IDENTIFIER)*
Arguments       → Expression ("," Expression)*
Primary         → IDENTIFIER | Literal | "(" Expression ")" | "REVERSE_FLOW"
Literal         → NUMBER | STRING | "TRUE" | "FALSE" | ResourceLiteral
ResourceLiteral → NUMBER ("C" | "A")
TypeAnnotation  → "INTEGER" | "FLOAT" | "STRING" | "BOOLEAN" | "CHRONON" | "AETHEL" | 
                   "VECTOR" "<" TypeAnnotation ">" | "MAP" "<" TypeAnnotation "," TypeAnnotation ">"
```

### 3.2 AST Structure

The AST will be implemented as a hierarchy of node classes:

#### 3.2.1 Base Node Class

```cpp
class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void accept(ASTVisitor& visitor) = 0;
    
    const SourceLocation& getLocation() const;
    
protected:
    SourceLocation location;
};
```

#### 3.2.2 Expression Nodes

```cpp
class Expression : public ASTNode {};

class BinaryExpr : public Expression {
public:
    BinaryExpr(std::unique_ptr<Expression> left, Token op, std::unique_ptr<Expression> right);
    void accept(ASTVisitor& visitor) override;
    
    Expression* getLeft() const;
    Expression* getRight() const;
    const Token& getOperator() const;
    
private:
    std::unique_ptr<Expression> left;
    Token op;
    std::unique_ptr<Expression> right;
};

// Similar classes for: UnaryExpr, LiteralExpr, VariableExpr, CallExpr, etc.
```

#### 3.2.3 Statement Nodes

```cpp
class Statement : public ASTNode {};

class ExpressionStmt : public Statement {
public:
    ExpressionStmt(std::unique_ptr<Expression> expression);
    void accept(ASTVisitor& visitor) override;
    
    Expression* getExpression() const;
    
private:
    std::unique_ptr<Expression> expression;
};

// Similar classes for: BlockStmt, IfStmt, WhileStmt, ReturnStmt, etc.
```

#### 3.2.4 Declaration Nodes

```cpp
class Declaration : public ASTNode {};

class VariableDecl : public Declaration {
public:
    VariableDecl(Token name, Token type, std::unique_ptr<Expression> initializer, bool isConst);
    void accept(ASTVisitor& visitor) override;
    
    const Token& getName() const;
    const Token& getType() const;
    Expression* getInitializer() const;
    bool isConst() const;
    
private:
    Token name;
    Token type;
    std::unique_ptr<Expression> initializer;
    bool isConst;
};

// Similar classes for: FunctionDecl, ResourceDecl, TemporalDecl, etc.
```

#### 3.2.5 Temporal and Resource Nodes

```cpp
class ResourceExpr : public Expression {
public:
    ResourceExpr(ResourceType type, std::unique_ptr<Expression> amount);
    void accept(ASTVisitor& visitor) override;
    
    ResourceType getType() const;
    Expression* getAmount() const;
    
private:
    ResourceType type;
    std::unique_ptr<Expression> amount;
};

class BranchTimelineStmt : public Statement {
public:
    BranchTimelineStmt(std::vector<std::pair<Token, std::unique_ptr<BlockStmt>>> paths);
    void accept(ASTVisitor& visitor) override;
    
    const std::vector<std::pair<Token, BlockStmt*>>& getPaths() const;
    
private:
    std::vector<std::pair<Token, std::unique_ptr<BlockStmt>>> paths;
};

// Similar classes for: ResourceAllocStmt, ResourceExpendStmt, MergeTimelinesStmt, RewindFlowStmt, etc.
```

### 3.3 Parser Implementation

The parser will be implemented using a recursive descent approach:

```cpp
class Parser {
public:
    Parser(Lexer& lexer);
    
    std::unique_ptr<Program> parse();
    
    const std::vector<DiagnosticMessage>& getDiagnostics() const;
    
private:
    Lexer& lexer;
    Token current;
    std::vector<DiagnosticMessage> diagnostics;
    
    // Parsing methods for grammar rules
    std::unique_ptr<Declaration> declaration();
    std::unique_ptr<FunctionDecl> functionDeclaration();
    std::unique_ptr<VariableDecl> variableDeclaration();
    std::unique_ptr<Statement> statement();
    std::unique_ptr<ExpressionStmt> expressionStatement();
    std::unique_ptr<IfStmt> ifStatement();
    std::unique_ptr<WhileStmt> whileStatement();
    std::unique_ptr<ForStmt> forStatement();
    std::unique_ptr<ReturnStmt> returnStatement();
    std::unique_ptr<BlockStmt> block();
    
    std::unique_ptr<Expression> expression();
    std::unique_ptr<Expression> assignment();
    std::unique_ptr<Expression> logicOr();
    std::unique_ptr<Expression> logicAnd();
    std::unique_ptr<Expression> equality();
    std::unique_ptr<Expression> comparison();
    std::unique_ptr<Expression> term();
    std::unique_ptr<Expression> factor();
    std::unique_ptr<Expression> unary();
    std::unique_ptr<Expression> call();
    std::unique_ptr<Expression> primary();
    
    // Resource and temporal parsing methods
    std::unique_ptr<ResourceDecl> resourceDeclaration();
    std::unique_ptr<TemporalDecl> temporalDeclaration();
    std::unique_ptr<BranchTimelineStmt> branchTimeline();
    std::unique_ptr<MergeTimelinesStmt> mergeTimelines();
    std::unique_ptr<RewindFlowStmt> rewindFlow();
    
    // Helper methods
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type);
    Token advance();
    Token consume(TokenType type, const std::string& message);
    void synchronize();
    
    void addError(const std::string& message);
    void addError(const Token& token, const std::string& message);
};
```

### 3.4 Error Handling and Recovery

The parser will implement error recovery techniques to continue parsing after encountering syntax errors:

1. **Synchronization Points**: Recover at statement or declaration boundaries
2. **Skip to Next Statement**: Skip tokens until a statement delimiter (e.g., semicolon) is found
3. **Error Productions**: Define special grammar rules for common error patterns
4. **Panic Mode Recovery**: Skip tokens until a synchronization point is reached

### 3.5 Parser Optimizations

- **Lookahead Caching**: Cache lookahead tokens to avoid repeated lexer calls
- **Error Recovery**: Implement efficient error recovery to continue parsing after errors
- **AST Node Pooling**: Use memory pools for AST nodes to reduce allocation overhead
- **Context-Aware Parsing**: Use contextual information to disambiguate complex constructs

## 4. Unique Chronovyan Language Features

### 4.1 Resource Management Syntax

The lexer and parser will handle Chronovyan's resource management syntax:

```
ALLOCATE_RESOURCES {
    CHRONON: 10,
    AETHEL: 5
} {
    // Code that uses the resources
}

EXPEND_RESOURCES {
    CHRONON: 2,
    AETHEL: 1
} {
    // Code that consumes resources
}

BORROW_RESOURCES {
    CHRONON: 5
} {
    // Code that borrows resources
}
```

### 4.2 Temporal Control Flow

The parser will handle Chronovyan's temporal control flow constructs:

```
BRANCH_TIMELINE {
    PATH_A: {
        // Timeline branch A
    }
    PATH_B: {
        // Timeline branch B
    }
}

MERGE_TIMELINES(resolution_strategy)

REWIND_FLOW (condition) {
    // Code that might rewind
}

REVERSE_FLOW;
```

### 4.3 CONF/REB Variable Classification

The lexer and parser will recognize the CONF/REB variable classification modifiers:

```
DECLARE x::CONF = 42;
DECLARE y::REB = "Hello";
```

## 5. Implementation Plan

### 5.1 Phase 1: Basic Lexer (1 week)

- Implement Token class
- Implement basic Lexer class
- Support keywords, identifiers, literals, operators
- Implement basic error reporting

### 5.2 Phase 2: Complete Lexer (1 week)

- Add support for all token types
- Implement lexer optimizations
- Enhance error reporting
- Add source location tracking
- Implement lexer tests

### 5.3 Phase 3: Basic Parser (2 weeks)

- Implement ASTNode base classes
- Implement expression parsing
- Implement statement parsing
- Implement basic error recovery
- Create initial AST visitor interface

### 5.4 Phase 4: Complete Parser (2 weeks)

- Add support for declarations
- Implement resource management parsing
- Implement temporal control flow parsing
- Enhance error recovery
- Complete AST visitor implementation
- Implement parser tests

### 5.5 Phase 5: Integration and Testing (1 week)

- Integrate lexer and parser
- Implement comprehensive test suite
- Benchmark performance
- Document public API
- Create usage examples

## 6. Testing Strategy

### 6.1 Unit Tests

- Test each token type recognition
- Test lexer error reporting
- Test parser grammar rules
- Test AST node construction
- Test error recovery mechanisms

### 6.2 Integration Tests

- Test lexer and parser interaction
- Test complete program parsing
- Test AST visitor traversal
- Test error reporting end-to-end

### 6.3 Performance Tests

- Benchmark lexing and parsing speed
- Measure memory usage
- Test with large input files
- Compare against performance targets

### 6.4 Error Handling Tests

- Test recovery from common syntax errors
- Test error message quality
- Test position information accuracy
- Test error cascading prevention

## 7. API Design

### 7.1 Public API

```cpp
// Create a compiler instance
Compiler compiler;

// Parse a file
std::unique_ptr<Program> ast = compiler.parseFile("example.chron");

// Check for errors
if (compiler.hasErrors()) {
    for (const auto& error : compiler.getErrors()) {
        std::cerr << error << std::endl;
    }
    return 1;
}

// Use the AST
ASTDumper dumper;
ast->accept(dumper);
```

### 7.2 AST Visitor Interface

```cpp
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;
    
    // Expressions
    virtual void visitBinaryExpr(BinaryExpr* expr) = 0;
    virtual void visitUnaryExpr(UnaryExpr* expr) = 0;
    virtual void visitLiteralExpr(LiteralExpr* expr) = 0;
    virtual void visitVariableExpr(VariableExpr* expr) = 0;
    virtual void visitCallExpr(CallExpr* expr) = 0;
    virtual void visitResourceExpr(ResourceExpr* expr) = 0;
    
    // Statements
    virtual void visitExpressionStmt(ExpressionStmt* stmt) = 0;
    virtual void visitBlockStmt(BlockStmt* stmt) = 0;
    virtual void visitIfStmt(IfStmt* stmt) = 0;
    virtual void visitWhileStmt(WhileStmt* stmt) = 0;
    virtual void visitForStmt(ForStmt* stmt) = 0;
    virtual void visitReturnStmt(ReturnStmt* stmt) = 0;
    
    // Declarations
    virtual void visitVariableDecl(VariableDecl* decl) = 0;
    virtual void visitFunctionDecl(FunctionDecl* decl) = 0;
    
    // Resource and Temporal
    virtual void visitResourceAllocStmt(ResourceAllocStmt* stmt) = 0;
    virtual void visitResourceExpendStmt(ResourceExpendStmt* stmt) = 0;
    virtual void visitBranchTimelineStmt(BranchTimelineStmt* stmt) = 0;
    virtual void visitMergeTimelinesStmt(MergeTimelinesStmt* stmt) = 0;
    virtual void visitRewindFlowStmt(RewindFlowStmt* stmt) = 0;
    
    // Program
    virtual void visitProgram(Program* program) = 0;
};
```

## 8. Challenges and Considerations

### 8.1 Ambiguity Resolution

The Chronovyan grammar contains potential ambiguities that require careful handling:

- CONF/REB annotations vs. scope resolution operator
- Resource literals vs. identifiers with suffixes
- Timeline path labels vs. case labels

### 8.2 Error Recovery Strategy

Effective error recovery is essential for a good developer experience:

- Use synchronization points at statement boundaries
- Skip to next declaration on severe errors
- Provide contextual error messages
- Avoid error cascades

### 8.3 Performance Considerations

The lexer and parser must be efficient for large source files:

- Minimize memory allocations
- Use efficient data structures
- Implement early bailout for invalid inputs
- Optimize the lexer state machine

### 8.4 Extensibility

The design must accommodate future language features:

- Modular grammar definitions
- Extensible token and AST hierarchies
- Visitor pattern for AST traversal
- Clear separation between lexical and syntactic concerns

## 9. Conclusion

This design document outlines the approach for implementing the lexical and syntax analysis components of the Chronicle Compiler. By following this design, we can create a robust, efficient, and maintainable front-end that accurately processes Chronovyan source code while providing helpful error messages and diagnostics.

The implementation will follow the phased approach described in Section 5, with each phase building on the previous one. Comprehensive testing will ensure that the lexer and parser correctly handle all valid Chronovyan constructs and provide useful feedback for invalid code.

As the first components in the compiler pipeline, the lexer and parser set the foundation for the entire Chronicle Compiler project. A well-designed front-end will simplify the implementation of later compiler phases, such as semantic analysis, optimization, and code generation.