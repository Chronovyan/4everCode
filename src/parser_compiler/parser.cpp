#include <sstream>
#include <unordered_map>
#include <unordered_set>

#include "error_handler.h"
#include "parser.h"

namespace chronovyan {

Parser::Parser(std::shared_ptr<Lexer> lexer) : m_lexer(std::move(lexer)) {
  // Initialize the parser state
  advance(); // Get the first token
}

Parser::~Parser() {
  // Clean up any resources
}

void Parser::advance() {
  m_previous = m_current;
  m_current = m_lexer->nextToken();
}

bool Parser::match(TokenType type) {
  if (check(type)) {
    advance();
    return true;
  }
  return false;
}

bool Parser::check(TokenType type) { return m_current.type == type; }

void Parser::consume(TokenType type, const std::string &message) {
  if (check(type)) {
    advance();
    return;
  }

  // Special case for missing semicolons - just issue a warning and continue
  if (type == TokenType::SEMICOLON) {
    ErrorHandler::getInstance().reportWarning(
        m_current.location,
        "Missing semicolon; automatic semicolon insertion applied.");
    return;
  }

  ErrorHandler::getInstance().reportError(m_current.location, message);
  throw std::runtime_error("Parse error: " + message);
}

void Parser::synchronize() {
  advance();

  while (m_current.type != TokenType::EOF_TOKEN) {
    if (m_previous.type == TokenType::SEMICOLON) {
      return;
    }

    switch (m_current.type) {
    case TokenType::IF:
    case TokenType::FOR_CHRONON:
    case TokenType::WHILE_EVENT:
    case TokenType::DECLARE:
    case TokenType::REWIND_FLOW:
    case TokenType::BRANCH_TIMELINE:
    case TokenType::MERGE_TIMELINES:
    case TokenType::DEFINE_TYPE:
    case TokenType::TEMPORAL_PROGRAM:
      return;
    default:
      break;
    }

    advance();
  }
}

std::unique_ptr<ProgramNode> Parser::parse() {
  std::vector<std::unique_ptr<StmtNode>> statements;

  try {
    // Parse statements until end of file
    while (!check(TokenType::EOF_TOKEN)) {
      try {
        // Special case for 'DEFINE_TYPE' and 'LET' at the beginning of a line
        if (check(TokenType::IDENTIFIER)) {
          if (m_current.lexeme == "DEFINE_TYPE") {
            // This is a type definition
            advance(); // Consume 'DEFINE_TYPE'
            auto typeDef = parseTypeDefinition();
            statements.push_back(std::move(typeDef));
            continue; // Continue to next statement
          } else if (m_current.lexeme == "LET") {
            // This is a variable declaration
            advance(); // Consume 'LET'
            auto varDecl = parseVarDeclaration();
            statements.push_back(std::move(varDecl));
            continue; // Continue to next statement
          }
        }

        // Regular statement parsing
        statements.push_back(parseStatement());
      } catch (const std::exception &e) {
        // Report the error
        ErrorHandler::getInstance().reportError(
            m_current.location, std::string("Parser error: ") + e.what());

        // Synchronize to the next statement
        synchronize();
      }
    }
  } catch (const std::exception &e) {
    // Handle any uncaught errors
    ErrorHandler::getInstance().reportError(
        m_current.location,
        std::string("Unrecoverable parser error: ") + e.what());
  }

  return std::make_unique<ProgramNode>(std::move(statements));
}

std::unique_ptr<StmtNode> Parser::parseStatement() {
  if (match(TokenType::PRINT)) {
    return parsePrintStatement();
  } else if (match(TokenType::LEFT_BRACE)) {
    return parseBlock();
  } else if (match(TokenType::IF)) {
    return parseIfStatement();
  } else if (match(TokenType::WHILE_EVENT)) {
    return parseWhileEventLoop();
  } else if (match(TokenType::FOR_CHRONON)) {
    return parseForChrononLoop();
  } else if (match(TokenType::RETURN)) {
    return parseReturnStatement();
  } else if (match(TokenType::BREAK_CHRONON)) {
    return parseBreakChrononStatement();
  } else if (match(TokenType::CONTINUE_WEAVE)) {
    return parseContinueWeaveStatement();
  } else if (match(TokenType::REWIND_FLOW)) {
    return parseRewindFlowLoop();
  } else if (match(TokenType::REVERSE_FLOW)) {
    return parseReverseFlowStatement();
  } else if (match(TokenType::TEMPORAL_LOOP)) {
    return parseTemporalLoop();
  } else if (match(TokenType::PARALLEL_EXECUTION)) {
    return parseParallelExecution();
  } else if (match(TokenType::BRANCH_TIMELINE)) {
    return parseBranchTimeline();
  } else if (match(TokenType::HALT_REWIND)) {
    return parseHaltRewindStatement();
  } else if (match(TokenType::STABILIZE_TIMELINE)) {
    return parseStabilizeTimelineStatement();
  } else if (match(TokenType::PREVENT_MODIFICATION)) {
    return parsePreventModificationStatement();
  } else if (match(TokenType::VAR)) {
    return parseVarDeclaration();
  } else if (match(TokenType::ATTEMPT_WEAVE)) {
    return parseAttemptWeave();
  } else if (match(TokenType::EXPEND_RESOURCES)) {
    return parseExpendResourcesStatement();
  } else {
    return parseExpressionStatement();
  }
}

std::unique_ptr<StmtNode> Parser::parseExpressionStatement() {
  // Special case for handling keywords as identifiers at the start of an
  // expression statement
  if (check(TokenType::IDENTIFIER)) {
    if (m_current.lexeme == "DEFINE_TYPE") {
      advance(); // Consume "DEFINE_TYPE"
      auto typeDef = parseTypeDefinition();
      return std::move(typeDef);
    }
  }

  auto expr = parseExpression();

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");

  return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parsePrintStatement() {
  auto expr = parseExpression();

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expect ';' after print statement.");

  return std::make_unique<ExprStmtNode>(std::move(expr));
}

std::unique_ptr<StmtNode> Parser::parseReturnStatement() {
  Token keyword = m_previous;
  std::unique_ptr<ExprNode> value = nullptr;

  if (!check(TokenType::SEMICOLON)) {
    value = parseExpression();
  }

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expect ';' after return statement.");

  return std::make_unique<ReturnStmtNode>(keyword, std::move(value));
}

std::unique_ptr<IfStmtNode> Parser::parseIfStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  auto condition = parseExpression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  auto thenBranch = parseStatement();
  std::unique_ptr<StmtNode> elseBranch = nullptr;

  if (match(TokenType::ELSE)) {
    elseBranch = parseStatement();
  }

  return std::make_unique<IfStmtNode>(
      std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::unique_ptr<ExprNode> Parser::parseExpression() { return parseEquality(); }

std::unique_ptr<ExprNode> Parser::parseEquality() {
  auto expr = parseComparison();

  while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL)) {
    Token op = m_previous;
    auto right = parseComparison();
    expr =
        std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<ExprNode> Parser::parseComparison() {
  auto expr = parseTerm();

  while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
         match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL)) {
    Token op = m_previous;
    auto right = parseTerm();
    expr =
        std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<ExprNode> Parser::parseTerm() {
  auto expr = parseFactor();

  while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
    Token op = m_previous;
    auto right = parseFactor();
    expr =
        std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<ExprNode> Parser::parseFactor() {
  auto expr = parseUnary();

  while (match(TokenType::STAR) || match(TokenType::SLASH) ||
         match(TokenType::PERCENT)) {
    Token op = m_previous;
    auto right = parseUnary();
    expr =
        std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
  }

  return expr;
}

std::unique_ptr<ExprNode> Parser::parseUnary() {
  if (match(TokenType::MINUS) || match(TokenType::BANG)) {
    Token op = m_previous;
    auto right = parseUnary();
    return std::make_unique<UnaryExprNode>(op, std::move(right));
  }

  return parsePrimary();
}

std::unique_ptr<StmtNode> Parser::parseFunctionDeclaration() {
  // Expect function name
  consume(TokenType::IDENTIFIER, "Expect function name after 'func'.");
  std::string name = m_previous.lexeme;
  consume(TokenType::LEFT_PAREN, "Expect '(' after function name.");
  // For now, no parameters
  consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
  auto body = parseBlock();
  return std::make_unique<FunctionDeclStmtNode>(name, std::move(body));
}

std::unique_ptr<BlockStmtNode> Parser::parseBlock() {
  std::vector<std::unique_ptr<StmtNode>> statements;
  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    statements.push_back(parseStatement());
  }
  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return std::make_unique<BlockStmtNode>(std::move(statements));
}

std::unique_ptr<ExprNode> Parser::parsePrimary() {
  if (match(TokenType::FALSE)) {
    return std::make_unique<LiteralExprNode>(m_previous.location, false);
  }
  if (match(TokenType::TRUE)) {
    return std::make_unique<LiteralExprNode>(m_previous.location, true);
  }

  // Check for numeric literals
  if (match(TokenType::INTEGER_LITERAL)) {
    return std::make_unique<LiteralExprNode>(
        m_previous.location,
        static_cast<int64_t>(std::stoll(m_previous.lexeme)));
  }
  if (match(TokenType::FLOAT_LITERAL)) {
    return std::make_unique<LiteralExprNode>(m_previous.location,
                                             std::stod(m_previous.lexeme));
  }

  // Check for string literals
  if (match(TokenType::STRING_LITERAL)) {
    return std::make_unique<LiteralExprNode>(m_previous.location,
                                             m_previous.lexeme);
  }

  // Handle identifiers
  if (check(TokenType::IDENTIFIER)) {
    // Check if the current token is 'DEFINE_TYPE', which should be handled at
    // the statement level
    if (m_current.lexeme == "DEFINE_TYPE") {
      ErrorHandler::getInstance().reportError(
          m_current.location,
          "DEFINE_TYPE should be used as a statement, not in an expression.");
      throw std::runtime_error("Parse error");
    }

    // Check for special case of "var" which should be a declaration, not an
    // expression
    if (m_current.lexeme == "var") {
      ErrorHandler::getInstance().reportError(
          m_current.location, "Unexpected 'var' in expression context. 'var' "
                              "should be used for variable declarations.");
      throw std::runtime_error("Parse error");
    }

    // Otherwise, handle as normal variable
    advance();
    return std::make_unique<VariableExprNode>(m_previous.location,
                                              m_previous.lexeme);
  }

  if (match(TokenType::LEFT_PAREN)) {
    auto expr = parseExpression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<GroupingExprNode>(std::move(expr));
  }

  if (match(TokenType::LEFT_BRACE)) {
    return parseObjectLiteral();
  }

  if (match(TokenType::LEFT_BRACKET)) {
    return parseArrayLiteral();
  }

  // Error case
  ErrorHandler::getInstance().reportError(
      m_current.location, "Expect expression. Got '" + m_current.lexeme + "'.");
  throw std::runtime_error("Parse error");
}

LootTable Parser::parseLootTable() {
  LootTable lootTable;

  consume(TokenType::LEFT_BRACE, "Expect '{' to start loot table.");

  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Parse loot entry: "loot_id": probability

    if (!check(TokenType::STRING_LITERAL)) {
      ErrorHandler::getInstance().reportError(
          m_current.location, "Expected loot ID as string literal.");
      break;
    }

    // Get the loot ID
    std::string lootId = m_current.lexeme;
    advance(); // Consume the string literal

    consume(TokenType::COLON, "Expect ':' after loot ID.");

    // Get the probability
    if (!check(TokenType::FLOAT_LITERAL) &&
        !check(TokenType::INTEGER_LITERAL)) {
      ErrorHandler::getInstance().reportError(
          m_current.location, "Expected probability as number.");
      break;
    }

    double probability;
    if (check(TokenType::FLOAT_LITERAL)) {
      probability = std::stod(m_current.lexeme);
    } else {
      probability = static_cast<double>(std::stoi(m_current.lexeme));
    }

    advance(); // Consume the number

    // Look up the loot in the registry
    auto loot = LootRegistry::getInstance().getLootById(lootId);
    if (!loot) {
      ErrorHandler::getInstance().reportError(m_previous.location,
                                              "Unknown loot ID: " + lootId);
    } else {
      // Add to the loot table
      lootTable.addLoot(loot, probability);
    }

    // Allow trailing comma
    if (match(TokenType::COMMA)) {
      continue;
    }
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after loot table entries.");

  return lootTable;
}

std::unique_ptr<TemporalProgramNode> Parser::parseTemporalProgram() {
  // Consume the TEMPORAL_PROGRAM token
  consume(TokenType::TEMPORAL_PROGRAM, "Expected 'temporal_program' keyword.");

  // Get the location of the start of the program
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'temporal_program'.");

  // Parse sections
  std::map<TemporalSectionType, std::unique_ptr<TemporalSectionNode>> sections;

  // Track required sections for error reporting
  bool hasVariables = false;
  bool hasInitialization = false;
  bool hasExecution = false;
  bool hasCleanup = false;

  // Parse sections until closing brace
  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Determine the section type based on the current token
    if (match(TokenType::NAME)) {
      auto section = parseMetadataSection(TemporalSectionType::NAME);
      sections[TemporalSectionType::NAME] = std::move(section);
    } else if (match(TokenType::TYPE)) {
      auto section = parseMetadataSection(TemporalSectionType::TYPE);
      sections[TemporalSectionType::TYPE] = std::move(section);
    } else if (match(TokenType::AUTHOR)) {
      auto section = parseMetadataSection(TemporalSectionType::AUTHOR);
      sections[TemporalSectionType::AUTHOR] = std::move(section);
    } else if (match(TokenType::VERSION)) {
      auto section = parseMetadataSection(TemporalSectionType::VERSION);
      sections[TemporalSectionType::VERSION] = std::move(section);
    } else if (match(TokenType::DESCRIPTION)) {
      auto section = parseMetadataSection(TemporalSectionType::DESCRIPTION);
      sections[TemporalSectionType::DESCRIPTION] = std::move(section);
    } else if (match(TokenType::RESOURCES_REQUIRED)) {
      auto section =
          parseMetadataSection(TemporalSectionType::RESOURCES_REQUIRED);
      sections[TemporalSectionType::RESOURCES_REQUIRED] = std::move(section);
    } else if (match(TokenType::TIMELINE_BEHAVIOR)) {
      auto section =
          parseMetadataSection(TemporalSectionType::TIMELINE_BEHAVIOR);
      sections[TemporalSectionType::TIMELINE_BEHAVIOR] = std::move(section);
    } else if (match(TokenType::STABILITY_TARGET)) {
      auto section =
          parseMetadataSection(TemporalSectionType::STABILITY_TARGET);
      sections[TemporalSectionType::STABILITY_TARGET] = std::move(section);
    } else if (match(TokenType::DEFAULT_FLOW)) {
      auto section = parseMetadataSection(TemporalSectionType::DEFAULT_FLOW);
      sections[TemporalSectionType::DEFAULT_FLOW] = std::move(section);
    } else if (match(TokenType::PARADOX_HANDLING)) {
      auto section =
          parseMetadataSection(TemporalSectionType::PARADOX_HANDLING);
      sections[TemporalSectionType::PARADOX_HANDLING] = std::move(section);
    } else if (match(TokenType::VARIABLES)) {
      auto section = parseVariablesSection();
      sections[TemporalSectionType::VARIABLES] = std::move(section);
      hasVariables = true;
    } else if (match(TokenType::INITIALIZATION_PHASE)) {
      auto section = parseInitializationSection();
      sections[TemporalSectionType::INITIALIZATION_PHASE] = std::move(section);
      hasInitialization = true;
    } else if (match(TokenType::EXECUTION_WEAVE)) {
      auto section = parseExecutionWeaveSection();
      sections[TemporalSectionType::EXECUTION_WEAVE] = std::move(section);
      hasExecution = true;
    } else if (match(TokenType::CLEANUP_PHASE)) {
      auto section = parseCleanupSection();
      sections[TemporalSectionType::CLEANUP_PHASE] = std::move(section);
      hasCleanup = true;
    } else {
      // Unexpected section
      ErrorHandler::getInstance().reportError(
          m_current.location, "Unexpected section in temporal_program block.");
      throw std::runtime_error("Parse error");
    }
  }

  // Consume closing brace
  consume(TokenType::RIGHT_BRACE,
          "Expected '}' to close temporal_program block.");

  // Check for required sections
  if (!hasVariables) {
    ErrorHandler::getInstance().reportWarning(
        location, "Missing 'variables' section in temporal_program block.");
  }
  if (!hasInitialization) {
    ErrorHandler::getInstance().reportWarning(
        location,
        "Missing 'initialization_phase' section in temporal_program block.");
  }
  if (!hasExecution) {
    ErrorHandler::getInstance().reportError(
        location, "Missing required 'execution_weave' section in "
                  "temporal_program block.");
    throw std::runtime_error("Parse error: Missing required section");
  }
  if (!hasCleanup) {
    ErrorHandler::getInstance().reportWarning(
        location, "Missing 'cleanup_phase' section in temporal_program block.");
  }

  // Create and return the TemporalProgramNode
  return std::make_unique<TemporalProgramNode>(location, std::move(sections));
}

std::unique_ptr<TemporalSectionNode>
Parser::parseMetadataSection(TemporalSectionType sectionType) {
  // Get the location of the start of the section
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after section keyword.");

  // For metadata sections, we expect a string or identifier
  std::unique_ptr<ExprNode> content;

  if (check(TokenType::STRING_LITERAL)) {
    advance();
    content = std::make_unique<LiteralExprNode>(
        location, LiteralExprNode::LiteralValue(m_previous.lexeme));
  } else if (check(TokenType::IDENTIFIER)) {
    advance();
    content = std::make_unique<VariableExprNode>(m_previous.lexeme);
  } else {
    // For version, allow numeric literals
    if (sectionType == TemporalSectionType::VERSION &&
        (check(TokenType::INTEGER_LITERAL) ||
         check(TokenType::FLOAT_LITERAL))) {
      advance();
      if (m_previous.type == TokenType::INTEGER_LITERAL) {
        int64_t value = std::stoll(m_previous.lexeme);
        content = std::make_unique<LiteralExprNode>(
            location, LiteralExprNode::LiteralValue(value));
      } else {
        double value = std::stod(m_previous.lexeme);
        content = std::make_unique<LiteralExprNode>(
            location, LiteralExprNode::LiteralValue(value));
      }
    } else {
      // Unexpected token
      ErrorHandler::getInstance().reportError(
          m_current.location, "Expected string, identifier, or numeric literal "
                              "in metadata section.");
      throw std::runtime_error("Parse error");
    }
  }

  // Expect closing brace
  consume(TokenType::RIGHT_BRACE, "Expected '}' to close section.");

  // Create and return the TemporalSectionNode
  return std::make_unique<TemporalSectionNode>(location, sectionType,
                                               std::move(content));
}

std::unique_ptr<TemporalSectionNode> Parser::parseVariablesSection() {
  // Get the location of the start of the section
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'variables'.");

  // Parse variable declarations
  std::vector<std::unique_ptr<StmtNode>> declarations;

  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Parse CONF and REB variable declarations
    if (match(TokenType::CONF)) {
      if (match(TokenType::VAR)) {
        declarations.push_back(
            parseModifiedVarDeclaration(VariableModifier::CONF));
      } else {
        // Unexpected token after CONF
        ErrorHandler::getInstance().reportError(m_current.location,
                                                "Expected 'var' after 'CONF'.");
        throw std::runtime_error("Parse error");
      }
    } else if (match(TokenType::REB)) {
      if (match(TokenType::VAR)) {
        declarations.push_back(
            parseModifiedVarDeclaration(VariableModifier::REB));
      } else {
        // Unexpected token after REB
        ErrorHandler::getInstance().reportError(m_current.location,
                                                "Expected 'var' after 'REB'.");
        throw std::runtime_error("Parse error");
      }
    } else if (match(TokenType::DECLARE)) {
      declarations.push_back(parseDeclareConstant());
    } else {
      // Unexpected token in variables section
      ErrorHandler::getInstance().reportError(
          m_current.location,
          "Expected 'CONF var', 'REB var', or 'DECLARE' in variables section.");
      throw std::runtime_error("Parse error");
    }
  }

  // Consume closing brace
  consume(TokenType::RIGHT_BRACE, "Expected '}' to close variables section.");

  // Create a block statement for the variable declarations
  auto block = std::make_unique<BlockStmtNode>(std::move(declarations));

  // Create and return the TemporalSectionNode
  return std::make_unique<TemporalSectionNode>(
      location, TemporalSectionType::VARIABLES, std::move(block));
}

std::unique_ptr<TemporalSectionNode> Parser::parseInitializationSection() {
  // Get the location of the start of the section
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'initialization_phase'.");

  // Parse statements
  std::vector<std::unique_ptr<StmtNode>> statements;

  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    statements.push_back(parseStatement());
  }

  // Consume closing brace
  consume(TokenType::RIGHT_BRACE,
          "Expected '}' to close initialization_phase section.");

  // Create a block statement for the statements
  auto block = std::make_unique<BlockStmtNode>(std::move(statements));

  // Create and return the TemporalSectionNode
  return std::make_unique<TemporalSectionNode>(
      location, TemporalSectionType::INITIALIZATION_PHASE, std::move(block));
}

std::unique_ptr<TemporalSectionNode> Parser::parseExecutionWeaveSection() {
  // Get the location of the start of the section
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'execution_weave'.");

  // Parse statements
  std::vector<std::unique_ptr<StmtNode>> statements;

  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    statements.push_back(parseStatement());
  }

  // Consume closing brace
  consume(TokenType::RIGHT_BRACE,
          "Expected '}' to close execution_weave section.");

  // Create a block statement for the statements
  auto block = std::make_unique<BlockStmtNode>(std::move(statements));

  // Create and return the TemporalSectionNode
  return std::make_unique<TemporalSectionNode>(
      location, TemporalSectionType::EXECUTION_WEAVE, std::move(block));
}

std::unique_ptr<TemporalSectionNode> Parser::parseCleanupSection() {
  // Get the location of the start of the section
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'cleanup_phase'.");

  // Parse statements
  std::vector<std::unique_ptr<StmtNode>> statements;

  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    statements.push_back(parseStatement());
  }

  // Consume closing brace
  consume(TokenType::RIGHT_BRACE,
          "Expected '}' to close cleanup_phase section.");

  // Create a block statement for the statements
  auto block = std::make_unique<BlockStmtNode>(std::move(statements));

  // Create and return the TemporalSectionNode
  return std::make_unique<TemporalSectionNode>(
      location, TemporalSectionType::CLEANUP_PHASE, std::move(block));
}

std::unique_ptr<VariableDeclStmtNode> Parser::parseVarDeclaration() {
  // Expect variable name
  consume(TokenType::IDENTIFIER, "Expect variable name after 'var' or 'LET'.");
  std::string name = m_previous.lexeme;

  // Parse the type if specified
  std::unique_ptr<TypeNode> type = nullptr;
  if (match(TokenType::COLON)) {
    auto sharedType = parseType();
    if (sharedType) {
      type = std::make_unique<TypeNode>(sharedType->getLocation(),
                                        sharedType->getName());
    }
  }

  // Optional initializer
  std::unique_ptr<ExprNode> initializer = nullptr;
  if (match(TokenType::EQUAL)) {
    initializer = parseExpression();
  }

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");

  // Create a simple variable declaration (unmodified)
  return std::make_unique<VariableDeclStmtNode>(
      m_current.location, name, std::move(type),
      VariableModifier::CONF, // Use CONF as the default modifier since NONE
                              // doesn't exist
      std::vector<VariableFlag>(), std::move(initializer));
}

std::unique_ptr<VariableDeclStmtNode>
Parser::parseModifiedVarDeclaration(VariableModifier modifier) {
  // Get the location of the start of the declaration
  SourceLocation location = m_previous.location;

  // Expect identifier
  consume(TokenType::IDENTIFIER, "Expected variable name.");
  std::string name = m_previous.lexeme;

  // Expect colon and type
  consume(TokenType::COLON, "Expected ':' after variable name.");
  auto sharedType = parseType();
  std::unique_ptr<TypeNode> type = nullptr;
  if (sharedType) {
    type = std::make_unique<TypeNode>(sharedType->getLocation(),
                                      sharedType->getName());
  }

  // Optional initializer
  std::unique_ptr<ExprNode> initializer;
  if (match(TokenType::EQUAL)) {
    initializer = parseExpression();
  }

  // Parse variable flags
  std::vector<VariableFlag> flags = parseVariableFlags();

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

  // Create and return the VariableDeclStmtNode
  return std::make_unique<VariableDeclStmtNode>(location, name, std::move(type),
                                                modifier, std::move(flags),
                                                std::move(initializer));
}

std::unique_ptr<DeclareConstantNode> Parser::parseDeclareConstant() {
  // Get the location of the start of the declaration
  SourceLocation location = m_previous.location;

  // Expect CONF
  consume(TokenType::CONF, "Expected 'CONF' after 'DECLARE'.");

  // Parse flags
  std::vector<VariableFlag> flags = parseVariableFlags();

  // Expect identifier
  consume(TokenType::IDENTIFIER, "Expected constant name.");
  std::string name = m_previous.lexeme;

  // Expect colon and type
  consume(TokenType::COLON, "Expected ':' after constant name.");
  auto sharedType = parseType();
  std::unique_ptr<TypeNode> type = nullptr;
  if (sharedType) {
    type = std::make_unique<TypeNode>(sharedType->getLocation(),
                                      sharedType->getName());
  }

  // Required initializer
  consume(TokenType::EQUAL, "Expected '=' after type in constant declaration.");
  auto initializer = parseExpression();

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expected ';' after constant declaration.");

  // Create and return the DeclareConstantNode
  return std::make_unique<DeclareConstantNode>(location, name, std::move(type),
                                               std::move(flags),
                                               std::move(initializer));
}

std::vector<VariableFlag> Parser::parseVariableFlags() {
  std::vector<VariableFlag> flags;

  // Parse zero or more ::FLAG sequences
  while (match(TokenType::DOUBLE_COLON)) {
    // Expect a flag token
    if (check(TokenType::STATIC) || check(TokenType::VOLATILE) ||
        check(TokenType::ANCHOR) || check(TokenType::WEAVER) ||
        check(TokenType::FLUX) || check(TokenType::ECHO)) {
      advance();
      flags.push_back(tokenToVariableFlag(m_previous));
    } else {
      // Unexpected token after ::
      ErrorHandler::getInstance().reportError(
          m_current.location, "Expected variable flag after '::'.");
      throw std::runtime_error("Parse error");
    }
  }

  return flags;
}

VariableFlag Parser::tokenToVariableFlag(const Token &token) {
  if (token.type == TokenType::STATIC)
    return VariableFlag::STATIC;
  if (token.type == TokenType::VOLATILE)
    return VariableFlag::VOLATILE;
  if (token.type == TokenType::ANCHOR)
    return VariableFlag::ANCHOR;
  if (token.type == TokenType::WEAVER)
    return VariableFlag::WEAVER;
  if (token.type == TokenType::FLUX)
    return VariableFlag::FLUX;
  if (token.type == TokenType::ECHO)
    return VariableFlag::ECHO;

  // This should never happen if the caller checks the token type first
  throw std::runtime_error("Invalid variable flag: " + token.lexeme);
}

std::shared_ptr<TypeNode> Parser::parseType() {
  // Parse a type name or expression
  consume(TokenType::IDENTIFIER, "Expect type name.");
  std::string typeName = m_previous.lexeme;
  SourceLocation location = m_previous.location;

  // Check for generic type parameters
  if (match(TokenType::LESS)) {
    // This is a generic type with type arguments
    std::vector<std::shared_ptr<TypeNode>> typeArgs;

    // Parse at least one type argument
    typeArgs.push_back(parseType());

    // Parse additional type arguments
    while (match(TokenType::COMMA)) {
      typeArgs.push_back(parseType());
    }

    consume(TokenType::GREATER, "Expect '>' after type arguments.");

    // Create a string representation of the type expression for validation
    std::stringstream ss;
    ss << typeName << "<";
    for (size_t i = 0; i < typeArgs.size(); ++i) {
      if (i > 0) {
        ss << ", ";
      }
      ss << typeArgs[i]->getName();
    }
    ss << ">";

    // Create a type node with the full generic type expression
    return std::make_shared<TypeNode>(location, ss.str());
  }

  // Check for array types
  if (match(TokenType::LEFT_BRACKET)) {
    consume(TokenType::RIGHT_BRACKET, "Expect ']' after '['.");

    // Create a string representation of the array type
    std::string arrayTypeName = typeName + "[]";

    return std::make_shared<TypeNode>(location, arrayTypeName);
  }

  // Check for nullable types
  if (match(TokenType::QUESTION)) {
    // Create a string representation of the nullable type
    std::string nullableTypeName = typeName + "?";

    return std::make_shared<TypeNode>(location, nullableTypeName);
  }

  // Simple type name
  return std::make_shared<TypeNode>(location, typeName);
}

std::unique_ptr<ForChrononStmtNode> Parser::parseForChrononLoop() {
  // Get the location of the start of the loop
  SourceLocation location = m_previous.location;

  // Expect opening parenthesis
  consume(TokenType::LEFT_PAREN, "Expected '(' after 'FOR_CHRONON'.");

  // Parse iterator variable
  consume(TokenType::IDENTIFIER, "Expected iterator variable name.");
  std::string iterator = m_previous.lexeme;

  // Expect colon
  consume(TokenType::COLON, "Expected ':' after iterator variable.");

  // Parse range start expression
  auto rangeStart = parseExpression();

  // Expect 'to' or range separator
  consume(TokenType::IDENTIFIER, "Expected 'to' after range start expression.");
  if (m_previous.lexeme != "to") {
    ErrorHandler::getInstance().reportError(
        m_previous.location, "Expected 'to' after range start expression.");
    throw std::runtime_error("Parse error");
  }

  // Parse range end expression
  auto rangeEnd = parseExpression();

  // Parse optional step expression
  std::unique_ptr<ExprNode> stepExpr;
  if (match(TokenType::IDENTIFIER) && m_previous.lexeme == "step") {
    stepExpr = parseExpression();
  }

  // Expect closing parenthesis
  consume(TokenType::RIGHT_PAREN,
          "Expected ')' after FOR_CHRONON loop parameters.");

  // Parse loop body
  std::unique_ptr<BlockStmtNode> body;
  if (match(TokenType::LEFT_BRACE)) {
    // Body is a block statement
    std::vector<std::unique_ptr<StmtNode>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
      statements.push_back(parseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after loop body.");
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  } else {
    // Body is a single statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the ForChrononStmtNode
  return std::make_unique<ForChrononStmtNode>(
      location, iterator, std::move(rangeStart), std::move(rangeEnd),
      std::move(stepExpr), std::move(body));
}

std::unique_ptr<WhileEventStmtNode> Parser::parseWhileEventLoop() {
  // Get the location of the start of the loop
  SourceLocation location = m_previous.location;

  // Expect opening parenthesis
  consume(TokenType::LEFT_PAREN, "Expected '(' after 'WHILE_EVENT'.");

  // Parse condition expression
  auto condition = parseExpression();

  // Expect closing parenthesis
  consume(TokenType::RIGHT_PAREN, "Expected ')' after WHILE_EVENT condition.");

  // Parse loop body
  std::unique_ptr<BlockStmtNode> body;
  if (match(TokenType::LEFT_BRACE)) {
    // Body is a block statement
    std::vector<std::unique_ptr<StmtNode>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
      statements.push_back(parseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after loop body.");
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  } else {
    // Body is a single statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the WhileEventStmtNode
  return std::make_unique<WhileEventStmtNode>(location, std::move(condition),
                                              std::move(body));
}

std::unique_ptr<RewindFlowStmtNode> Parser::parseRewindFlowLoop() {
  // Get the location of the start of the loop
  SourceLocation location = m_previous.location;

  // Expect opening parenthesis
  consume(TokenType::LEFT_PAREN, "Expected '(' after 'REWIND_FLOW'.");

  // Parse condition expression
  auto condition = parseExpression();

  // Parse optional max rewinds
  std::unique_ptr<ExprNode> maxRewinds;
  if (match(TokenType::COMMA)) {
    // There's a max rewinds parameter
    maxRewinds = parseExpression();
  }

  // Expect closing parenthesis
  consume(TokenType::RIGHT_PAREN, "Expected ')' after REWIND_FLOW parameters.");

  // Parse loop body
  std::unique_ptr<BlockStmtNode> body;
  if (match(TokenType::LEFT_BRACE)) {
    // Body is a block statement
    std::vector<std::unique_ptr<StmtNode>> statements;
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
      statements.push_back(parseStatement());
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' after loop body.");
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  } else {
    // Body is a single statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    body = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the RewindFlowStmtNode
  return std::make_unique<RewindFlowStmtNode>(
      location, std::move(condition), std::move(maxRewinds), std::move(body));
}

std::unique_ptr<AttemptWeaveStmtNode> Parser::parseAttemptWeave() {
  // Get the location of the start of the statement
  SourceLocation location = m_previous.location;

  // Parse try block (ATTEMPT_WEAVE block)
  consume(TokenType::LEFT_BRACE, "Expected '{' after 'ATTEMPT_WEAVE'.");
  auto tryBlock = parseBlock();

  // Parse dissonance handlers (ON_DISSONANCE blocks)
  std::vector<std::unique_ptr<DissonanceHandlerNode>> handlers;
  while (match(TokenType::ON_DISSONANCE)) {
    handlers.push_back(parseDissonanceHandler());
  }

  // Parse optional finally block (ENSURE_HARMONY block)
  std::unique_ptr<BlockStmtNode> finallyBlock;
  if (match(TokenType::ENSURE_HARMONY)) {
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'ENSURE_HARMONY'.");
    finallyBlock = parseBlock();
  }

  // Create and return the AttemptWeaveStmtNode
  return std::make_unique<AttemptWeaveStmtNode>(location, std::move(tryBlock),
                                                std::move(handlers),
                                                std::move(finallyBlock));
}

std::unique_ptr<DissonanceHandlerNode> Parser::parseDissonanceHandler() {
  // Get the location of the start of the handler
  SourceLocation location = m_previous.location;

  // Expect opening parenthesis
  consume(TokenType::LEFT_PAREN, "Expected '(' after 'ON_DISSONANCE'.");

  // Parse optional dissonance type
  SourceLocation handlerLocation = m_previous.location;
  std::unique_ptr<TypeNode> dissonanceType = nullptr;
  std::string errorVar;

  if (check(TokenType::IDENTIFIER) || check(TokenType::INT) ||
      check(TokenType::FLOAT) || check(TokenType::BOOLEAN) ||
      check(TokenType::STRING) || check(TokenType::CHRONON) ||
      check(TokenType::EVENT) || check(TokenType::ENTITY) ||
      check(TokenType::PATTERN) || check(TokenType::WEAVE) ||
      check(TokenType::THREAD) || check(TokenType::RESOURCE)) {
    // This is a dissonance type
    auto sharedType = parseType();
    if (sharedType) {
      dissonanceType = std::make_unique<TypeNode>(sharedType->getLocation(),
                                                  sharedType->getName());
    }

    // Expect comma after type
    consume(TokenType::COMMA, "Expected ',' after dissonance type.");
  }

  // Parse error variable
  consume(TokenType::IDENTIFIER, "Expected error variable name.");
  errorVar = m_previous.lexeme;

  // Expect closing parenthesis
  consume(TokenType::RIGHT_PAREN,
          "Expected ')' after ON_DISSONANCE parameters.");

  // Parse handler body
  consume(TokenType::LEFT_BRACE,
          "Expected '{' after ON_DISSONANCE parameters.");
  auto body = parseBlock();

  // Create and return the DissonanceHandlerNode
  return std::make_unique<DissonanceHandlerNode>(
      location, std::move(dissonanceType), errorVar, std::move(body));
}

std::unique_ptr<BreakChrononStmtNode> Parser::parseBreakChrononStatement() {
  // Get the location of the statement
  SourceLocation location = m_previous.location;

  // Expect semicolon
  consume(TokenType::SEMICOLON, "Expected ';' after BREAK_CHRONON statement.");

  // Create and return the BreakChrononStmtNode
  return std::make_unique<BreakChrononStmtNode>(location);
}

std::unique_ptr<ContinueWeaveStmtNode> Parser::parseContinueWeaveStatement() {
  // Get the location of the statement
  SourceLocation location = m_previous.location;

  // Expect semicolon
  consume(TokenType::SEMICOLON, "Expected ';' after CONTINUE_WEAVE statement.");

  // Create and return the ContinueWeaveStmtNode
  return std::make_unique<ContinueWeaveStmtNode>(location);
}

std::unique_ptr<ReverseFlowStmtNode> Parser::parseReverseFlowStatement() {
  // Get the location of the statement
  SourceLocation location = m_previous.location;

  // Expect semicolon
  consume(TokenType::SEMICOLON, "Expected ';' after REVERSE_FLOW statement.");

  // Create and return the ReverseFlowStmtNode
  return std::make_unique<ReverseFlowStmtNode>(location);
}

std::unique_ptr<ExpendResourcesStmtNode>
Parser::parseExpendResourcesStatement() {
  // Get the location of the statement
  SourceLocation location = m_previous.location;

  // Expect opening parenthesis
  consume(TokenType::LEFT_PAREN, "Expected '(' after EXPEND_RESOURCES.");

  // Parse resource cost expressions
  std::unique_ptr<ExprNode> aethelCostExpr = nullptr;
  std::unique_ptr<ExprNode> chrononCostExpr = nullptr;

  // Parse the first resource cost
  if (match(TokenType::AETHEL)) {
    // Expect colon
    consume(TokenType::COLON, "Expected ':' after AETHEL.");

    // Parse expression
    aethelCostExpr = parseExpression();

    // Check for comma indicating another resource cost
    if (match(TokenType::COMMA)) {
      // Parse Chronon cost
      consume(TokenType::CHRONONS, "Expected 'CHRONONS' after comma.");
      consume(TokenType::COLON, "Expected ':' after CHRONONS.");
      chrononCostExpr = parseExpression();
    }
  } else if (match(TokenType::CHRONONS)) {
    // Expect colon
    consume(TokenType::COLON, "Expected ':' after CHRONONS.");

    // Parse expression
    chrononCostExpr = parseExpression();

    // Check for comma indicating another resource cost
    if (match(TokenType::COMMA)) {
      // Parse Aethel cost
      consume(TokenType::AETHEL, "Expected 'AETHEL' after comma.");
      consume(TokenType::COLON, "Expected ':' after AETHEL.");
      aethelCostExpr = parseExpression();
    }
  } else {
    // Unexpected token
    ErrorHandler::getInstance().reportError(
        m_current.location, "Expected 'AETHEL' or 'CHRONONS' after '('.");
    throw std::runtime_error("Parse error");
  }

  // Expect closing parenthesis
  consume(TokenType::RIGHT_PAREN, "Expected ')' after resource costs.");

  // Parse main block
  consume(TokenType::LEFT_BRACE, "Expected '{' after resource costs.");
  auto mainBlock = parseBlock();

  // Check for optional insufficient resources handler
  std::unique_ptr<ResourceInsufficiencyHandlerNode> insufficientHandlerBlock =
      nullptr;
  if (match(TokenType::ON_INSUFFICIENT_RESOURCES)) {
    insufficientHandlerBlock = parseResourceInsufficiencyHandler();
  }

  // Create and return the ExpendResourcesStmtNode
  return std::make_unique<ExpendResourcesStmtNode>(
      location, std::move(aethelCostExpr), std::move(chrononCostExpr),
      std::move(mainBlock), std::move(insufficientHandlerBlock));
}

std::unique_ptr<ResourceInsufficiencyHandlerNode>
Parser::parseResourceInsufficiencyHandler() {
  // Get the location of the handler
  SourceLocation location = m_previous.location;

  // Expect opening brace
  consume(TokenType::LEFT_BRACE,
          "Expected '{' after ON_INSUFFICIENT_RESOURCES.");

  // Parse the handler body
  auto body = parseBlock();

  // Create and return the ResourceInsufficiencyHandlerNode
  return std::make_unique<ResourceInsufficiencyHandlerNode>(
      location,
      "", // Add an empty error variable name for now
      std::move(body));
}

std::unique_ptr<TypeDefStmtNode> Parser::parseTypeDefinition() {
  // Store the location of the DEFINE_TYPE token
  SourceLocation location = m_previous.location;

  // Parse the type name
  consume(TokenType::IDENTIFIER, "Expect type name after 'DEFINE_TYPE'.");
  std::string typeName = m_previous.lexeme;

  // Parse any type parameters (for generics)
  std::vector<std::string> typeParameters;
  if (match(TokenType::LESS)) {
    typeParameters = parseTypeParameters();
  }

  // Check for optional AS token
  if (match(TokenType::AS)) {
    // AS token is present, proceed with type kind
  }

  // Check which kind of type definition this is
  std::unique_ptr<TypeDefStmtNode> result;
  if (match(TokenType::STRUCT)) {
    result = parseStructDefinition(location, typeName, typeParameters);
  } else if (match(TokenType::ENUM)) {
    result = parseEnumDefinition(location, typeName, typeParameters);
  } else if (match(TokenType::UNION)) {
    result = parseUnionDefinition(location, typeName, typeParameters);
  } else if (match(TokenType::ALIAS)) {
    return parseAliasDefinition(
        location, typeName,
        typeParameters); // This already consumes a semicolon
  } else {
    // Default to struct if no specific type is provided
    ErrorHandler::getInstance().reportError(
        m_current.location, "Expected a type kind (STRUCT, ENUM, UNION, or "
                            "ALIAS) after type name.");
    throw std::runtime_error("Parse error");
  }

  // Consume semicolon - this will now issue a warning if missing but won't
  // throw
  consume(TokenType::SEMICOLON, "Expect ';' after type definition.");

  return result;
}

std::vector<std::string> Parser::parseTypeParameters() {
  std::vector<std::string> parameters;

  // Parse at least one parameter
  consume(TokenType::IDENTIFIER, "Expect type parameter name.");
  parameters.push_back(m_previous.lexeme);

  // Parse additional parameters
  while (match(TokenType::COMMA)) {
    consume(TokenType::IDENTIFIER, "Expect type parameter name after ','.");
    parameters.push_back(m_previous.lexeme);
  }

  // Close the parameter list
  consume(TokenType::GREATER, "Expect '>' after type parameters.");

  return parameters;
}

std::unique_ptr<TypeDefStmtNode>
Parser::parseStructDefinition(const SourceLocation &location,
                              const std::string &name,
                              const std::vector<std::string> &typeParameters) {
  // Parse the struct body
  consume(TokenType::LEFT_BRACE, "Expect '{' after 'STRUCT'.");

  std::vector<StructField> fields;

  // Parse fields until we reach the closing brace
  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Parse field name
    consume(TokenType::IDENTIFIER, "Expect field name.");
    std::string fieldName = m_previous.lexeme;

    // Parse field type
    consume(TokenType::COLON, "Expect ':' after field name.");
    auto fieldType = parseType();

    // Parse default value if present
    std::unique_ptr<ExprNode> defaultValue = nullptr;
    if (match(TokenType::EQUAL)) {
      defaultValue = parseExpression();
    }

    // Create the field
    fields.emplace_back(fieldName, fieldType->getName(),
                        std::move(defaultValue));

    // Expect a semicolon or comma after each field
    if (!match(TokenType::SEMICOLON) && !match(TokenType::COMMA)) {
      ErrorHandler::getInstance().reportError(
          m_current.location, "Expect ';' or ',' after field definition.");
      throw std::runtime_error("Parse error");
    }
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after struct body.");

  // Create and return the struct type definition
  return std::make_unique<TypeDefStmtNode>(location, name, fields,
                                           typeParameters);
}

std::unique_ptr<TypeDefStmtNode>
Parser::parseEnumDefinition(const SourceLocation &location,
                            const std::string &name,
                            const std::vector<std::string> &typeParameters) {
  // Parse the enum body
  consume(TokenType::LEFT_BRACE, "Expect '{' after 'ENUM'.");

  std::vector<EnumVariant> variants;

  // Parse variants until we reach the closing brace
  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Parse variant name
    consume(TokenType::IDENTIFIER, "Expect variant name.");
    std::string variantName = m_previous.lexeme;

    // Parse value if present
    std::unique_ptr<ExprNode> value = nullptr;
    if (match(TokenType::EQUAL)) {
      value = parseExpression();
    }

    // Create the variant
    variants.emplace_back(variantName, std::move(value));

    // Expect a comma after each variant except the last one
    if (!check(TokenType::RIGHT_BRACE)) {
      consume(TokenType::COMMA, "Expect ',' after variant.");
    } else {
      // Optional trailing comma
      match(TokenType::COMMA);
    }
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after enum body.");

  // Create and return the enum type definition
  return std::make_unique<TypeDefStmtNode>(location, name, variants,
                                           typeParameters);
}

std::unique_ptr<TypeDefStmtNode>
Parser::parseUnionDefinition(const SourceLocation &location,
                             const std::string &name,
                             const std::vector<std::string> &typeParameters) {
  // Parse the union body
  consume(TokenType::LEFT_BRACE, "Expect '{' after 'UNION'.");

  std::vector<std::shared_ptr<TypeNode>> unionTypes;

  // Parse types until we reach the closing brace
  while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::EOF_TOKEN)) {
    // Parse type
    auto type = parseType();
    unionTypes.push_back(std::move(type));

    // Expect a comma after each type except the last one
    if (!check(TokenType::RIGHT_BRACE)) {
      consume(TokenType::COMMA, "Expect ',' after type.");
    } else {
      // Optional trailing comma
      match(TokenType::COMMA);
    }
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after union body.");

  // Create and return the union type definition
  return std::make_unique<TypeDefStmtNode>(location, name, unionTypes,
                                           typeParameters);
}

std::unique_ptr<TypeDefStmtNode>
Parser::parseAliasDefinition(const SourceLocation &location,
                             const std::string &name,
                             const std::vector<std::string> &typeParameters) {
  // Parse the aliased type
  consume(TokenType::EQUAL, "Expect '=' after 'ALIAS'.");
  auto aliasedType = parseType();

  // Expect a semicolon after the alias definition
  consume(TokenType::SEMICOLON, "Expect ';' after alias definition.");

  // Create and return the alias type definition
  return std::make_unique<TypeDefStmtNode>(
      location, name, std::shared_ptr<TypeNode>(std::move(aliasedType)),
      typeParameters);
}

// Implementation of parseObjectLiteral
std::unique_ptr<ExprNode> Parser::parseObjectLiteral() {
  // Since we're not fully implementing object literals now, report an error
  ErrorHandler::getInstance().reportError(
      m_previous.location, "Object literals are not fully supported yet.");
  throw std::runtime_error("Parse error");
}

// Implementation of parseArrayLiteral
std::unique_ptr<ExprNode> Parser::parseArrayLiteral() {
  // Since we're not fully implementing array literals now, report an error
  ErrorHandler::getInstance().reportError(
      m_previous.location, "Array literals are not fully supported yet.");
  throw std::runtime_error("Parse error");
}

std::unique_ptr<TemporalLoopStmtNode> Parser::parseTemporalLoop() {
  // Store start location for error reporting
  SourceLocation startLoc = previous().location;

  // Parse duration expression (optional)
  std::unique_ptr<ExprNode> durationExpr = nullptr;
  if (match(TokenType::DURATION)) {
    consume(TokenType::LEFT_PAREN,
            "Expected '(' after DURATION in TEMPORAL_LOOP.");
    durationExpr = parseExpression();
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after duration expression in TEMPORAL_LOOP.");
  }

  // Parse condition expression
  consume(TokenType::LEFT_PAREN, "Expected '(' after TEMPORAL_LOOP.");
  std::unique_ptr<ExprNode> conditionExpr = parseExpression();
  consume(TokenType::RIGHT_PAREN,
          "Expected ')' after condition in TEMPORAL_LOOP.");

  // Parse variance expression (optional)
  std::unique_ptr<ExprNode> varianceExpr = nullptr;
  if (match(TokenType::VARIANCE)) {
    consume(TokenType::LEFT_PAREN,
            "Expected '(' after VARIANCE in TEMPORAL_LOOP.");
    varianceExpr = parseExpression();
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after variance expression in TEMPORAL_LOOP.");
  }

  // Parse loop body
  std::unique_ptr<BlockStmtNode> blockBody;
  if (match(TokenType::LEFT_BRACE)) {
    blockBody = parseBlock();
  } else {
    // If it's not a block, we need to create a block containing the single
    // statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    blockBody = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the TEMPORAL_LOOP node
  return std::make_unique<TemporalLoopStmtNode>(
      startLoc, std::move(durationExpr), std::move(conditionExpr),
      std::move(varianceExpr), std::move(blockBody));
}

std::unique_ptr<StmtNode> Parser::parseHaltRewindStatement() {
  SourceLocation location = previous().location;
  consume(TokenType::SEMICOLON, "Expected ';' after HALT_REWIND statement.");
  return std::make_unique<HaltRewindStmtNode>(location);
}

std::unique_ptr<StmtNode> Parser::parseStabilizeTimelineStatement() {
  SourceLocation location = previous().location;
  std::unique_ptr<ExprNode> intensity = nullptr;

  // Check for optional intensity parameter
  if (!check(TokenType::SEMICOLON)) {
    intensity = parseExpression();
  }

  consume(TokenType::SEMICOLON,
          "Expected ';' after STABILIZE_TIMELINE statement.");
  return std::make_unique<StabilizeTimelineStmtNode>(location,
                                                     std::move(intensity));
}

std::unique_ptr<StmtNode> Parser::parsePreventModificationStatement() {
  SourceLocation location = previous().location;
  std::unique_ptr<ExprNode> scope = nullptr;

  // Check for optional scope parameter
  if (!check(TokenType::SEMICOLON)) {
    scope = parseExpression();
  }

  consume(TokenType::SEMICOLON,
          "Expected ';' after PREVENT_MODIFICATION statement.");
  return std::make_unique<PreventModificationStmtNode>(location,
                                                       std::move(scope));
}

// Implementation for PARALLEL_EXECUTION parsing
std::unique_ptr<ParallelExecutionStmtNode> Parser::parseParallelExecution() {
  // Store start location for error reporting
  SourceLocation startLoc = previous().location;

  // Parse thread count expression (optional)
  std::unique_ptr<ExprNode> threadCountExpr = nullptr;
  if (match(TokenType::THREADS)) {
    consume(TokenType::LEFT_PAREN,
            "Expected '(' after THREADS in PARALLEL_EXECUTION.");
    threadCountExpr = parseExpression();
    consume(
        TokenType::RIGHT_PAREN,
        "Expected ')' after thread count expression in PARALLEL_EXECUTION.");
  }

  // Parse condition expression
  consume(TokenType::LEFT_PAREN, "Expected '(' after PARALLEL_EXECUTION.");
  std::unique_ptr<ExprNode> conditionExpr = parseExpression();
  consume(TokenType::RIGHT_PAREN,
          "Expected ')' after condition in PARALLEL_EXECUTION.");

  // Parse synchronization points (optional)
  std::vector<std::unique_ptr<ExprNode>> syncPoints;
  while (match(TokenType::SYNC_POINT)) {
    consume(TokenType::LEFT_PAREN,
            "Expected '(' after SYNC_POINT in PARALLEL_EXECUTION.");
    syncPoints.push_back(parseExpression());
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after sync point expression in PARALLEL_EXECUTION.");
  }

  // Parse loop body
  std::unique_ptr<BlockStmtNode> blockBody;
  if (match(TokenType::LEFT_BRACE)) {
    blockBody = parseBlock();
  } else {
    // If it's not a block, we need to create a block containing the single
    // statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    blockBody = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the PARALLEL_EXECUTION node
  return std::make_unique<ParallelExecutionStmtNode>(
      startLoc, std::move(threadCountExpr), std::move(conditionExpr),
      std::move(blockBody), std::move(syncPoints));
}

// Implementation for BRANCH_TIMELINE parsing
std::unique_ptr<BranchTimelineStmtNode> Parser::parseBranchTimeline() {
  // Store start location for error reporting
  SourceLocation startLoc = previous().location;

  // Parse branch count expression (optional)
  std::unique_ptr<ExprNode> branchCountExpr = nullptr;
  if (match(TokenType::LEFT_PAREN)) {
    branchCountExpr = parseExpression();
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after branch count expression in BRANCH_TIMELINE.");
  }

  // Parse branch identifier (optional)
  std::unique_ptr<ExprNode> identifierExpr = nullptr;
  if (match(TokenType::IDENTIFIER) && previous().lexeme == "id") {
    consume(TokenType::LEFT_PAREN, "Expected '(' after id in BRANCH_TIMELINE.");
    identifierExpr = parseExpression();
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after identifier expression in BRANCH_TIMELINE.");
  }

  // Parse merge strategy (optional)
  std::unique_ptr<ExprNode> mergeStrategyExpr = nullptr;
  if (match(TokenType::IDENTIFIER) && previous().lexeme == "merge_strategy") {
    consume(TokenType::LEFT_PAREN,
            "Expected '(' after merge_strategy in BRANCH_TIMELINE.");
    mergeStrategyExpr = parseExpression();
    consume(TokenType::RIGHT_PAREN,
            "Expected ')' after merge strategy expression in BRANCH_TIMELINE.");
  }

  // Parse body
  std::unique_ptr<BlockStmtNode> blockBody;
  if (match(TokenType::LEFT_BRACE)) {
    blockBody = parseBlock();
  } else {
    // If it's not a block, we need to create a block containing the single
    // statement
    auto stmt = parseStatement();
    std::vector<std::unique_ptr<StmtNode>> statements;
    statements.push_back(std::move(stmt));
    blockBody = std::make_unique<BlockStmtNode>(std::move(statements));
  }

  // Create and return the BRANCH_TIMELINE node
  return std::make_unique<BranchTimelineStmtNode>(
      startLoc, std::move(branchCountExpr), std::move(identifierExpr),
      std::move(mergeStrategyExpr), std::move(blockBody));
}

Token Parser::previous() const { return m_previous; }

} // namespace chronovyan
