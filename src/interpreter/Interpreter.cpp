#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "../include/ast_nodes.h"
#include "../include/environment.h"
#include "../include/error_handler.h"
#include "../include/interpreter.h"
#include "../include/interpreter/StatementVisitor.h"
#include "../include/loot_system.h"
#include "../include/value_converter.h"
#include "multi_dimensional_vector.h"
#include "vector_type.h"

namespace chronovyan {

// Forward declarations
class ReturnException;

// Add ReturnException class for handling return statements
class ReturnException : public std::runtime_error {
public:
    ReturnException(const Value& val)
        : std::runtime_error("Return statement encountered"), value(val) {}

    Value value;
};

// Class representing a Chronovyan function
class ChronovyanFunction {
public:
    ChronovyanFunction(const FunctionDeclStmtNode* declaration) : m_declaration(declaration) {}

    Value call(Interpreter& interpreter, const std::vector<Value>& arguments) const {
        // Create a new environment for the function execution
        auto environment = std::make_shared<Environment>(interpreter.getGlobals());

        // Bind parameters to arguments
        const auto& params = m_declaration->getParams();
        for (size_t i = 0; i < params.size() && i < arguments.size(); i++) {
            environment->define(params[i], arguments[i]);
        }

        // Execute the function body with the new environment
        auto previousEnv = interpreter.getEnvironment();
        interpreter.setEnvironment(environment);

        try {
            // Execute the function body
            interpreter.execute(m_declaration->getBody());
        } catch (ReturnException& returnException) {
            // Restore the previous environment
            interpreter.setEnvironment(previousEnv);
            return returnException.value;
        }

        // Restore the previous environment
        interpreter.setEnvironment(previousEnv);

        // If no return statement was executed, return nil
        return Value();
    }

    const FunctionDeclStmtNode* getDeclaration() const { return m_declaration; }

private:
    const FunctionDeclStmtNode* m_declaration;
};

Interpreter::Interpreter()
    : m_globals(std::make_shared<Environment>())
    , m_environment(m_globals)
    , m_optimizer(std::make_shared<ResourceOptimizer>(nullptr))
    , m_debt_tracker(std::make_shared<TemporalDebtTracker>())
    , m_visualization(std::make_shared<ResourceVisualization>(nullptr))
    , m_typeSystem(std::make_shared<CustomTypeSystem>())
    , m_userTypeSystem(std::make_shared<UserTypeSystem>())
    , m_lootSystem(std::make_shared<LootSystem>())
    , m_isReturning(false)
    , m_breakChronon_flag(false)
    , m_continueWeave_flag(false)
    , m_reverseFlow_flag(false)
    , m_haltRewind_flag(false)
    , m_preventModification_flag(false)
    , m_readOnlyMode(false)
    , m_stabilizationFactor(0.0)
    , m_flowDirection(1)
    , m_inParallelExecution(false)
    , m_currentThreadId(0)
    , m_totalThreads(1)
    , m_aethel(1000.0)
    , m_chronons(1000.0) {
    initializeGlobals();
    defineNativeFunctions();
    defineTemporalFunctions();
    defineTypeNativeFunctions();
    defineUserTypeNativeFunctions();
}

Value Interpreter::interpret(const ProgramNode& program) {
    try {
        visitProgram(program);
        // After all functions are defined, call main()
        Value mainFunc = m_globals->get("main");
        if (mainFunc.isChronovyanFunction()) {
            m_lastValue = mainFunc.asChronovyanFunction()->call(*this, {});
        }
        return m_lastValue;
    } catch (const ChronovyanException& e) {
        // Already handled by the error system
        return Value();  // Return nil
    } catch (const std::exception& e) {
        // Unexpected error
        ErrorHandler::getInstance().reportError(SourceLocation(),
                                                "Runtime error: " + std::string(e.what()));
        return Value();  // Return nil
    }
}

void Interpreter::execute(const StmtNode& stmt) { stmt.accept(*this); }

Value Interpreter::evaluate(const ExprNode& expr) {
    expr.accept(*this);
    return m_lastValue;
}

std::shared_ptr<TemporalRuntime> Interpreter::getRuntime() const { return m_runtime; }

std::shared_ptr<ResourceOptimizer> Interpreter::getResourceOptimizer() const { return m_optimizer; }

std::shared_ptr<TemporalDebtTracker> Interpreter::getTemporalDebtTracker() const {
    return m_debt_tracker;
}

std::shared_ptr<CustomTypeSystem> Interpreter::getTypeSystem() const { return m_typeSystem; }

std::shared_ptr<UserTypeSystem> Interpreter::getUserTypeSystem() const { return m_userTypeSystem; }

std::shared_ptr<Environment> Interpreter::getGlobals() const { return m_globals; }

std::shared_ptr<Environment> Interpreter::getEnvironment() const { return m_environment; }

Value Interpreter::getLastValue() const { return m_lastValue; }

std::shared_ptr<ResourceVisualization> Interpreter::getResourceVisualization() const {
    return m_visualization;
}

// Visitor methods for expressions

void Interpreter::visitLiteralExpr(const LiteralExprNode& expr) {
    auto value = expr.getValue();

    if (std::holds_alternative<int64_t>(value)) {
        m_lastValue = Value(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        m_lastValue = Value(std::get<double>(value));
    } else if (std::holds_alternative<std::string>(value)) {
        m_lastValue = Value(std::get<std::string>(value));
    } else if (std::holds_alternative<bool>(value)) {
        m_lastValue = Value(std::get<bool>(value));
    } else {
        // Should never happen
        m_lastValue = Value();  // nil
    }
}

void Interpreter::visitVariableExpr(const VariableExprNode& expr) {
    m_lastValue = lookUpVariable(expr.getName(), expr.getLocation());
}

void Interpreter::visitUnaryExpr(const UnaryExprNode& expr) {
    Value right = evaluate(expr.getRight());

    switch (expr.getOperator().type) {
        case TokenType::MINUS:
            m_lastValue = negate(right);
            break;
        case TokenType::BANG:
            m_lastValue = logicalNot(right);
            break;
        default:
            // Invalid operator for unary expression
            throw std::runtime_error("Invalid operator for unary expression");
    }
}

void Interpreter::visitBinaryExpr(const BinaryExprNode& expr) {
    Value left = evaluate(expr.getLeft());
    Value right = evaluate(expr.getRight());

    switch (expr.getOperator().type) {
        case TokenType::PLUS:
            m_lastValue = add(left, right);
            break;
        case TokenType::MINUS:
            m_lastValue = subtract(left, right);
            break;
        case TokenType::STAR:
            m_lastValue = multiply(left, right);
            break;
        case TokenType::SLASH:
            m_lastValue = divide(left, right);
            break;
        case TokenType::PERCENT:
            m_lastValue = modulo(left, right);
            break;
        case TokenType::EQUAL_EQUAL:
            m_lastValue = Value(areEqual(left, right));
            break;
        case TokenType::BANG_EQUAL:
            m_lastValue = Value(!areEqual(left, right));
            break;
        case TokenType::LESS:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() < right.asFloat());
            } else {
                throw std::runtime_error("Less than operator requires numeric operands");
            }
            break;

        case TokenType::LESS_EQUAL:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() <= right.asFloat());
            } else {
                throw std::runtime_error("Less than or equal operator requires numeric operands");
            }
            break;

        case TokenType::GREATER:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() > right.asFloat());
            } else {
                throw std::runtime_error("Greater than operator requires numeric operands");
            }
            break;

        case TokenType::GREATER_EQUAL:
            if (left.isNumeric() && right.isNumeric()) {
                m_lastValue = Value(left.asFloat() >= right.asFloat());
            } else {
                throw std::runtime_error(
                    "Greater than or equal operator requires numeric operands");
            }
            break;

        default:
            // Should never happen if the parser is correct
            throw std::runtime_error("Unknown binary operator");
    }

    // Update paradox level based on CONF/REB interaction
    updateParadoxLevel(left, right, expr.getOperator().type);
}

void Interpreter::visitGroupingExpr(const GroupingExprNode& expr) {
    m_lastValue = evaluate(expr.getExpression());
}

void Interpreter::visitAssignExpr(const AssignExprNode& expr) {
    Value value = evaluate(expr.getValue());

    // Handle variable assignment
    m_environment->assign(expr.getName(), value);
    m_lastValue = value;
}

void Interpreter::visitCallExpr(const CallExprNode& expr) {
    // Evaluate the callee
    Value callee = evaluate(expr.getCallee());

    // Evaluate all arguments
    std::vector<Value> arguments;
    for (const auto& arg : expr.getArguments()) {
        arguments.push_back(evaluate(*arg));
    }

    // Check if the callee is a function
    if (callee.isNativeFunction()) {
        // Call the native function with the arguments
        m_lastValue = callee.asNativeFunction().call(*this, arguments);
    } else {
        // Report an error if the callee is not callable
        ErrorHandler::getInstance().reportError(expr.getLocation(),
                                                "Can only call native functions at this time");
    }
}

void Interpreter::visitProgram(const ProgramNode& program) {
    for (const auto& stmt : program.getStatements()) {
        execute(*stmt);
    }
}

// Visitor methods for statements

void Interpreter::visitExprStmt(const ExprStmtNode& stmt) {
    m_lastValue = evaluate(stmt.getExpression());
}

void Interpreter::visitBlockStmt(const BlockStmtNode& stmt) {
    executeBlock(stmt, std::make_shared<Environment>(m_environment));
}

void Interpreter::visitVariableDeclStmt(const VariableDeclStmtNode& stmt) {
    Value value;

    if (stmt.hasInitializer()) {
        value = evaluate(stmt.getInitializer());
    } else {
        // Default value based on type
        // For now, just use nil
        value = Value();
    }

    // Set variable modifiers and flags
    value.setModifier(stmt.getModifier());
    for (const auto& flag : stmt.getFlags()) {
        value.addFlag(flag);
    }

    // Define the variable in the current environment
    m_environment->define(stmt.getName(), value);
}

void Interpreter::visitIfStmt(const IfStmtNode& stmt) {
    Value condition = evaluate(stmt.getCondition());

    if (condition.asBoolean()) {
        execute(stmt.getThenBranch());
    } else if (stmt.hasElseBranch()) {
        execute(stmt.getElseBranch());
    }
}

void Interpreter::visitTemporalOpStmt(const TemporalOpStmtNode& stmt) {
    switch (stmt.getOpType()) {
        case TemporalOpType::FOR_CHRONON:
            executeForChronon(stmt);
            break;
        case TemporalOpType::WHILE_EVENT:
            executeWhileEvent(stmt);
            break;
        case TemporalOpType::REWIND_FLOW:
            executeRewindFlow(stmt);
            break;
        case TemporalOpType::BRANCH_TIMELINE:
            executeBranchTimeline(stmt);
            break;
        case TemporalOpType::MERGE_TIMELINES:
            executeMergeTimelines(stmt);
            break;
        case TemporalOpType::TEMPORAL_ECHO_LOOP:
            executeTemporalEchoLoop(stmt);
            break;
        default:
            throw ChronovyanRuntimeError("Unknown temporal operation", stmt.getLocation());
    }
}

void Interpreter::visitFunctionDeclStmt(const FunctionDeclStmtNode& stmt) {
    // Store the function in the global environment
    m_globals->define(stmt.getName(), Value(std::make_shared<ChronovyanFunction>(&stmt)));
}

void Interpreter::visitReturnStmt(const ReturnStmtNode& stmt) {
    Value value;
    if (stmt.hasValue()) {
        value = evaluate(stmt.getValue());
    }

    // Set the return value and flag
    m_returnValues.push(value);
    m_isReturning = true;
}

// Visitor methods for other nodes

void Interpreter::visitType(const TypeNode& type) {
    // Set the value to NIL by default, implementing visitor method
    m_lastValue = Value();  // Nil value
}

void Interpreter::visitTemporalProgram(const TemporalProgramNode& program) {
    // Simple stub implementation - can be expanded later
    m_lastValue = Value();  // Nil value
}

void Interpreter::visitTemporalSection(const TemporalSectionNode& section) {
    // Placeholder implementation to be completed
    // This stub avoids the calls to undefined functions
    std::cout << "Temporal section encountered (stub implementation)" << std::endl;
}

// Helper methods for temporal section execution
void Interpreter::executeTemporalLoop(const TemporalSectionNode& section) {
    // Execute the loop body until a stopping condition is met
    bool continueLoop = true;
    int maxIterations = 1000;  // Safety limit to prevent infinite loops
    int iteration = 0;

    if (section.getType() != TemporalSectionType::EXECUTION_WEAVE) {
        ErrorHandler::getInstance().reportError(
            section.getLocation(),
            "Attempted to execute a temporal loop on a section that is not an EXECUTION_WEAVE.");
        return;
    }

    const auto* bodyContent = dynamic_cast<const BlockStmtNode*>(&section.getContent());
    if (!bodyContent) {
        ErrorHandler::getInstance().reportError(
            section.getLocation(),
            "Content of EXECUTION_WEAVE section is not a valid block statement for temporal loop.");
        return;
    }

    while (continueLoop && iteration < maxIterations) {
        // Execute the loop body
        executeBlock(*bodyContent, m_environment);

        // Check if we should continue the loop
        // For example, stop if a break statement was encountered
        if (m_isBreaking) {
            m_isBreaking = false;  // Reset the flag
            continueLoop = false;
        }

        // Increment the iteration counter
        iteration++;

        // Check if we hit the safety limit
        if (iteration >= maxIterations) {
            ErrorHandler::getInstance().reportWarning(
                section.getLocation(),
                "Temporal loop hit maximum iteration limit and was forcibly terminated");
        }
    }
}

void Interpreter::registerEventHandler(const TemporalSectionNode& section) {
    // Register this section as an event handler for its associated event
    std::string eventName;

    if (section.getType() == TemporalSectionType::NAME) {
        const auto* nameContentNode = dynamic_cast<const LiteralExprNode*>(&section.getContent());
        if (nameContentNode) {
            const auto& literalVal = nameContentNode->getValue();
            if (std::holds_alternative<std::string>(literalVal)) {
                eventName = std::get<std::string>(literalVal);
            } else {
                ErrorHandler::getInstance().reportError(
                    section.getLocation(),
                    "Event name content in temporal NAME section is not a string literal.");
                return;
            }
        } else {
            ErrorHandler::getInstance().reportError(
                section.getLocation(),
                "Content of temporal NAME section is not a literal expression for event name.");
            return;
        }
    } else {
        // It might not be an error to call this on other section types,
        // but it won't register an event handler if it's not a NAME section.
        // Depending on desired behavior, a warning or error could be logged here.
        if (m_debugLogging) {
            std::cout << "Attempted to register event handler for a non-NAME section. Type: "
                      << static_cast<int>(section.getType()) << std::endl;
        }
        return;
    }

    // Store the event handler in the runtime system
    m_eventHandlers[eventName] = &section;

    if (m_debugLogging) {
        std::cout << "Registered event handler for '" << eventName << "'" << std::endl;
    }
}

// Helper methods

void Interpreter::executeBlock(const BlockStmtNode& block,
                               std::shared_ptr<Environment> environment) {
    std::shared_ptr<Environment> previous = m_environment;

    try {
        m_environment = environment;

        for (const auto& stmt : block.getStatements()) {
            execute(*stmt);
        }
    } catch (...) {
        m_environment = previous;
        throw;
    }

    m_environment = previous;
}

Value Interpreter::lookUpVariable(const std::string& name, const SourceLocation& location) {
    try {
        return m_environment->get(name);
    } catch (const std::exception& e) {
        ErrorHandler::getInstance().reportError(location, "Undefined variable '" + name + "'");
        throw;
    }
}

// Placeholder implementations for temporal operations

void Interpreter::executeForChronon([[maybe_unused]] const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeWhileEvent([[maybe_unused]] const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeRewindFlow(const TemporalOpStmtNode& stmt) {
    // Simplified implementation until TemporalOpStmtNode attributes are fully implemented
    // Track resource usage for the REWIND_FLOW operation
    double cost = calculateTemporalResourceCost(TemporalOperation::REWIND_FLOW);
    consumeResources(cost);

    // Log the operation if in debug mode
    if (m_debugLogging) {
        std::cout << "Executing REWIND_FLOW operation with cost: " << cost << std::endl;
    }

    // Set the rewind flow flag
    m_reverseFlow_flag = true;
    m_flowDirection = -1;  // Reverse the flow direction

    // Execute the body with the flow reversed
    // (Body execution would go here once the TemporalOpStmtNode is updated to support it)
}

void Interpreter::executeBranchTimeline([[maybe_unused]] const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeMergeTimelines([[maybe_unused]] const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::executeTemporalEchoLoop([[maybe_unused]] const TemporalOpStmtNode& stmt) {
    // Placeholder implementation
}

void Interpreter::defineNativeFunctions() {
    // Define basic print function
    m_globals->define(
        "print",
        Value(std::make_shared<NativeFunction>(
            []([[maybe_unused]] Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.empty()) {
                    std::cout << "***** OUTPUT: nil *****" << std::endl;
                } else {
                    std::cout << "***** OUTPUT: " << args[0].toString() << " *****" << std::endl;
                }
                return Value();  // Return nil
            },
            1  // Arity (number of arguments)
            )));

    // Define clock function to get current time
    m_globals->define("clock", Value(std::make_shared<NativeFunction>(
                                   []([[maybe_unused]] Interpreter& interpreter,
                                      [[maybe_unused]] const std::vector<Value>& args) -> Value {
                                       auto now = std::chrono::system_clock::now();
                                       auto milliseconds =
                                           std::chrono::duration_cast<std::chrono::milliseconds>(
                                               now.time_since_epoch())
                                               .count();
                                       return Value(static_cast<double>(milliseconds) / 1000.0);
                                   },
                                   0  // Arity
                                   )));

    // Define the chrono.core module
    auto chronoCore = std::make_shared<ObjectValue>();

    // Add now() function to chrono.core
    chronoCore->setProperty(
        "now", Value(std::make_shared<NativeFunction>(
                   []([[maybe_unused]] Interpreter& interpreter,
                      [[maybe_unused]] const std::vector<Value>& args) -> Value {
                       auto now = std::chrono::system_clock::now();
                       auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                                               now.time_since_epoch())
                                               .count();
                       return Value(static_cast<int64_t>(milliseconds));
                   },
                   0  // Arity
                   )));

    // Add the chrono.core module to globals
    auto chrono = std::make_shared<ObjectValue>();
    chrono->setProperty("core", Value(chronoCore));
    m_globals->define("chrono", Value(chrono));

    // Define the math module
    auto math = std::make_shared<ObjectValue>();

    // Add random() function to math
    math->setProperty("random", Value(std::make_shared<NativeFunction>(
                                    []([[maybe_unused]] Interpreter& interpreter,
                                       [[maybe_unused]] const std::vector<Value>& args) -> Value {
                                        // Return a random number between 0 and 1
                                        return Value(static_cast<double>(std::rand()) / RAND_MAX);
                                    },
                                    0  // Arity
                                    )));

    // Add floor() function to math
    math->setProperty(
        "floor",
        Value(std::make_shared<NativeFunction>(
            []([[maybe_unused]] Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.empty() || !args[0].isNumeric()) {
                    ErrorHandler::getInstance().reportError(SourceLocation(),
                                                            "floor() requires a numeric argument");
                    return Value();  // nil
                }

                return Value(static_cast<int64_t>(std::floor(args[0].asFloat())));
            },
            1  // Arity
            )));

    // Add the math module to globals
    m_globals->define("math", Value(math));

    // Add resource optimization functions

    // GET_CHRONONS_LEVEL function
    m_globals->define(
        "GET_CHRONONS_LEVEL",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& /*args*/) -> Value {
                return Value(static_cast<int64_t>(interpreter.getRuntime()->getChrononsLevel()));
            },
            0  // No arguments
            )));

    // GET_AETHEL_LEVEL function
    m_globals->define(
        "GET_AETHEL_LEVEL",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& /*args*/) -> Value {
                return Value(static_cast<int64_t>(interpreter.getRuntime()->getAethelLevel()));
            },
            0  // No arguments
            )));

    // OPTIMIZE_CHRONONS function
    m_globals->define("OPTIMIZE_CHRONONS",
                      Value(std::make_shared<NativeFunction>(
                          [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                              if (args.empty() || !args[0].isString()) {
                                  ErrorHandler::getInstance().reportError(
                                      SourceLocation(),
                                      "OPTIMIZE_CHRONONS() requires an operation ID string");
                                  return Value(false);
                              }

                              std::string operationId = args[0].asString();
                              double optimizationFactor =
                                  interpreter.getResourceOptimizer()->optimizeChronons(operationId);

                              return Value(optimizationFactor);
                          },
                          1  // Arity
                          )));

    // OPTIMIZE_AETHEL function
    m_globals->define("OPTIMIZE_AETHEL",
                      Value(std::make_shared<NativeFunction>(
                          [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                              if (args.empty() || !args[0].isString()) {
                                  ErrorHandler::getInstance().reportError(
                                      SourceLocation(),
                                      "OPTIMIZE_AETHEL() requires a timeline ID string");
                                  return Value(false);
                              }

                              std::string timelineId = args[0].asString();
                              double optimizationFactor =
                                  interpreter.getResourceOptimizer()->optimizeAethel(timelineId);

                              return Value(optimizationFactor);
                          },
                          1  // Arity
                          )));

    // ANALYZE_RESOURCE_USAGE function
    m_globals->define(
        "ANALYZE_RESOURCE_USAGE",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, [[maybe_unused]] const std::vector<Value>& args) -> Value {
                auto analysis = interpreter.getResourceOptimizer()->analyzeResourceUsage();

                // Convert the analysis to a Chronovyan map
                std::map<std::string, Value> resultMap;
                for (const auto& [key, value] : analysis) {
                    resultMap[key] = Value(value);
                }

                return Value(std::make_shared<ChronovyanMap>(resultMap));
            },
            0  // Arity
            )));

    // PREDICT_RESOURCE_NEEDS function
    m_globals->define(
        "PREDICT_RESOURCE_NEEDS",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.empty() || !args[0].isNumeric()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "PREDICT_RESOURCE_NEEDS() requires a steps parameter (integer)");
                    return Value();
                }

                int steps = static_cast<int>(args[0].asInteger());
                auto predictions = interpreter.getResourceOptimizer()->predictResourceNeeds(steps);

                // Convert predictions to a Chronovyan array of maps
                std::vector<Value> resultArray;
                for (const auto& [chronons, aethel] : predictions) {
                    std::map<std::string, Value> predictionMap;
                    predictionMap["chronons"] = Value(static_cast<int64_t>(chronons));
                    predictionMap["aethel"] = Value(static_cast<int64_t>(aethel));

                    resultArray.push_back(Value(std::make_shared<ChronovyanMap>(predictionMap)));
                }

                return Value(std::make_shared<ChronovyanArray>(resultArray));
            },
            1  // Arity
            )));

    // Add temporal debt management functions

    // BORROW_CHRONONS function
    m_globals->define("BORROW_CHRONONS",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              if (args.size() < 2) {
                                  return Value(false);
                              }

                              double amount =
                                  args[0].isInteger() ? args[0].asInteger() : args[0].asFloat();
                              std::string operation = args[1].asString();
                              bool critical = args.size() >= 3 ? args[2].asBoolean() : false;

                              bool success =
                                  m_debt_tracker->borrowChronons(amount, operation, critical);
                              if (success) {
                                  m_visualization->takeSnapshot();
                              }

                              return Value(success);
                          },
                          2  // Minimum 2 arguments required (amount, operation)
                          )));

    // BORROW_AETHEL function
    m_globals->define("BORROW_AETHEL",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              if (args.size() < 2) {
                                  return Value(false);
                              }

                              double amount =
                                  args[0].isInteger() ? args[0].asInteger() : args[0].asFloat();
                              std::string operation = args[1].asString();
                              bool critical = args.size() >= 3 ? args[2].asBoolean() : false;

                              bool success =
                                  m_debt_tracker->borrowAethel(amount, operation, critical);
                              if (success) {
                                  m_visualization->takeSnapshot();
                              }

                              return Value(success);
                          },
                          2  // Minimum 2 arguments required (amount, operation)
                          )));

    // REPAY_CHRONONS function
    m_globals->define("REPAY_CHRONONS_DEBT",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              if (args.size() < 1) {
                                  return Value(0.0);
                              }

                              double amount =
                                  args[0].isInteger() ? args[0].asInteger() : args[0].asFloat();
                              std::string operation = args.size() >= 2 ? args[1].asString() : "";

                              double repaid = m_debt_tracker->repayChronons(amount, operation);
                              m_visualization->takeSnapshot();

                              return Value(repaid);
                          },
                          1  // Minimum 1 argument required (amount)
                          )));

    // REPAY_AETHEL function
    m_globals->define("REPAY_AETHEL_DEBT",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              if (args.size() < 1) {
                                  return Value(0.0);
                              }

                              double amount =
                                  args[0].isInteger() ? args[0].asInteger() : args[0].asFloat();
                              std::string operation = args.size() >= 2 ? args[1].asString() : "";

                              double repaid = m_debt_tracker->repayAethel(amount, operation);
                              m_visualization->takeSnapshot();

                              return Value(repaid);
                          },
                          1  // Minimum 1 argument required (amount)
                          )));

    // ADVANCE_DEBT_CYCLE function
    m_globals->define(
        "ADVANCE_CYCLE",
        Value(std::make_shared<NativeFunction>(
            [this]([[maybe_unused]] Interpreter& interpreter,
                   const std::vector<Value>& /*args*/) -> Value {
                m_debt_tracker->advanceCycle();
                m_visualization->takeSnapshot();  // Take a snapshot after advancing the cycle
                return Value();                   // Return nil
            },
            0  // 0 means variable arity (optional parameters)
            )));

    // GET_DEBT_SUMMARY function
    m_globals->define(
        "GET_DEBT_SUMMARY",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& /*args*/) -> Value {
                auto debt_tracker = interpreter.getTemporalDebtTracker();

                auto map = std::make_shared<ChronovyanMap>();
                map->set("total_chronon_debt", Value(debt_tracker->getTotalChrononsDebt()));
                map->set("total_aethel_debt", Value(debt_tracker->getTotalAethelDebt()));

                // Get debt counts
                auto all_debts = debt_tracker->getAllDebts();
                int chronon_debt_count = 0;
                int aethel_debt_count = 0;
                int critical_debt_count = 0;

                for (const auto& debt : all_debts) {
                    if (debt.chronon_debt > 0)
                        chronon_debt_count++;
                    if (debt.aethel_debt > 0)
                        aethel_debt_count++;
                    if (debt.critical)
                        critical_debt_count++;
                }

                map->set("chronon_debt_count", Value(static_cast<int64_t>(chronon_debt_count)));
                map->set("aethel_debt_count", Value(static_cast<int64_t>(aethel_debt_count)));
                map->set("critical_debts", Value(static_cast<int64_t>(critical_debt_count)));
                map->set("debt_limit", Value(debt_tracker->getDebtLimit()));
                map->set("stability", Value(debt_tracker->getStability()));
                map->set("past_due_count",
                         Value(static_cast<int64_t>(debt_tracker->getPastDueDebts().size())));

                return Value(map);
            },
            0  // No arguments
            )));

    // Define the chrono.simulation module
    auto simulation = std::make_shared<ObjectValue>();

    // Add Environment class constructor to chrono.simulation
    simulation->setProperty(
        "Environment", Value(std::make_shared<NativeFunction>(
                           []([[maybe_unused]] Interpreter& interpreter,
                              [[maybe_unused]] const std::vector<Value>& args) -> Value {
                               auto environment = std::make_shared<ObjectValue>();

                               // Add methods to the environment object
                               environment->setProperty(
                                   "set_time_delta",
                                   Value(std::make_shared<NativeFunction>(
                                       []([[maybe_unused]] Interpreter& interpreter,
                                          const std::vector<Value>& args) -> Value {
                                           if (args.size() < 2 || !args[1].isNumeric()) {
                                               ErrorHandler::getInstance().reportError(
                                                   SourceLocation(),
                                                   "set_time_delta() requires a numeric argument");
                                               return Value();  // nil
                                           }

                                           // In a real implementation, we would set the time delta
                                           std::cout << "Setting time delta to "
                                                     << args[1].asFloat() << std::endl;
                                           return Value(true);
                                       },
                                       2  // Arity (self + argument)
                                       )));

                               return Value(environment);
                           },
                           0  // Arity
                           )));

    chrono->setProperty("simulation", Value(simulation));

    // Resource visualization functions
    m_globals->define("GENERATE_RESOURCE_DASHBOARD",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->generateDashboard(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_CURRENT_STATE",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeCurrentState(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_RESOURCE_TRENDS",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              int steps = 10;  // Default value
                              VisualizationFormat format = VisualizationFormat::TEXT;

                              if (args.size() >= 1 && (args[0].isInteger() || args[0].isFloat())) {
                                  steps = static_cast<int>(args[0].isInteger() ? args[0].asInteger()
                                                                               : args[0].asFloat());
                              }

                              if (args.size() >= 2 && args[1].isString()) {
                                  std::string formatStr = args[1].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeTrends(steps, format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_OPTIMIZATION_OPPORTUNITIES",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(
                                  m_visualization->visualizeOptimizationOpportunities(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_DEBT_STATUS",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeDebtStatus(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("TAKE_RESOURCE_SNAPSHOT",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 [[maybe_unused]] const std::vector<Value>& /*args*/) -> Value {
                              [[maybe_unused]] auto snapshot = m_visualization->takeSnapshot();
                              return Value();  // Return nil
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define(
        "ADVANCE_CYCLE",
        Value(std::make_shared<NativeFunction>(
            [this]([[maybe_unused]] Interpreter& interpreter,
                   const std::vector<Value>& /*args*/) -> Value {
                m_debt_tracker->advanceCycle();
                m_visualization->takeSnapshot();  // Take a snapshot after advancing the cycle
                return Value();                   // Return nil
            },
            0  // 0 means variable arity (optional parameters)
            )));

    // Add console output functions
    m_globals->define(
        "PRINT_CONSOLE",
        Value(std::make_shared<NativeFunction>(
            []([[maybe_unused]] Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.empty()) {
                    std::cout << "[CONSOLE OUTPUT]: nil" << std::endl;
                } else {
                    std::cout << "[CONSOLE OUTPUT]: " << args[0].toString() << std::endl;
                }
                return Value();  // Return nil
            },
            1  // Arity
            )));

    m_globals->define(
        "OUTPUT_LOG",
        Value(std::make_shared<NativeFunction>(
            []([[maybe_unused]] Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.empty()) {
                    std::cerr << "[LOG OUTPUT]: nil" << std::endl;
                } else {
                    std::cerr << "[LOG OUTPUT]: " << args[0].toString() << std::endl;
                }
                return Value();  // Return nil
            },
            1  // Arity
            )));

    // Add file output logging
    m_globals->define(
        "FILE_LOG",
        Value(std::make_shared<NativeFunction>(
            []([[maybe_unused]] Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                std::ofstream logFile("chronovyan_output.log", std::ios::app);
                if (logFile.is_open()) {
                    if (args.empty()) {
                        logFile << "[FILE LOG]: nil" << std::endl;
                    } else {
                        logFile << "[FILE LOG]: " << args[0].toString() << std::endl;
                    }
                    logFile.close();
                }
                return Value();  // Return nil
            },
            1  // Arity
            )));
}

void Interpreter::updateParadoxLevel([[maybe_unused]] const Value& left,
                                     [[maybe_unused]] const Value& right,
                                     [[maybe_unused]] TokenType operation) {
    // Placeholder for paradox management
}

Value Interpreter::handleVariableInteraction([[maybe_unused]] const Value& left,
                                             [[maybe_unused]] const Value& right,
                                             [[maybe_unused]] TokenType operation) {
    // Placeholder for CONF/REB interaction
    return Value();
}

// Implementation for new TypeDefStmt visitor
void Interpreter::visitTypeDefStmt(const TypeDefStmtNode& stmt) {
    const std::string& typeName = stmt.getName();
    CustomTypeKind kind;

    // Determine the type kind based on the statement properties
    if (stmt.isStruct()) {
        kind = CustomTypeKind::STRUCT;
    } else if (stmt.isEnum()) {
        kind = CustomTypeKind::ENUM;
    } else if (stmt.isUnion()) {
        kind = CustomTypeKind::UNION;
    } else if (stmt.isAlias()) {
        kind = CustomTypeKind::ALIAS;
    } else {
        // Default to struct
        kind = CustomTypeKind::STRUCT;
    }

    // Collect properties from the type definition
    std::map<std::string, Value> properties;

    // For structs, collect field definitions
    if (stmt.isStruct()) {
        auto fieldsMap = std::make_shared<ChronovyanMap>();

        if (stmt.getFields().empty()) {
            std::stringstream ss;
            ss << "Struct type '" << typeName << "' has no fields defined";
            ErrorHandler::getInstance().reportWarning(stmt.getLocation(), ss.str());
        }

        for (const auto& field : stmt.getFields()) {
            // Validate field name
            if (field.getName().empty()) {
                std::stringstream ss;
                ss << "Field in struct '" << typeName << "' has no name";
                throw std::runtime_error(ss.str());
            }

            // Validate field type
            if (field.getTypeName().empty()) {
                std::stringstream ss;
                ss << "Field '" << field.getName() << "' in struct '" << typeName
                   << "' has no type specified";
                throw std::runtime_error(ss.str());
            }

            // Create a map for each field with type and default value
            auto fieldProperties = std::make_shared<ChronovyanMap>();
            fieldProperties->set("type", Value(field.getTypeName()));

            // Add default value if present
            if (field.hasDefaultValue()) {
                try {
                    fieldProperties->set("default", evaluate(*field.getDefaultValue()));
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Error evaluating default value for field '" << field.getName()
                       << "' in struct '" << typeName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
                }
            }

            // Add the field to the fields map
            fieldsMap->set(field.getName(), Value(fieldProperties));
        }

        properties["fields"] = Value(fieldsMap);
        properties["primitive"] = Value(false);
        properties["container"] = Value(true);
    }

    // For enums, collect variant definitions
    else if (stmt.isEnum()) {
        auto variantsMap = std::make_shared<ChronovyanMap>();

        if (stmt.getVariants().empty()) {
            std::stringstream ss;
            ss << "Enum type '" << typeName << "' has no variants defined";
            ErrorHandler::getInstance().reportWarning(stmt.getLocation(), ss.str());
        }

        // Start with 0 for auto-incrementing enum values
        int64_t nextValue = 0;

        for (const auto& variant : stmt.getVariants()) {
            // Validate variant name
            if (variant.getName().empty()) {
                std::stringstream ss;
                ss << "Variant in enum '" << typeName << "' has an empty name";
                ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
                continue;
            }

            Value variantValue;

            if (variant.hasExprValue()) {
                variantValue = evaluate(variant.getExprValue());
            }

            // Add the variant to the variants map
            variantsMap->set(variant.getName(), variantValue);
        }

        properties["variants"] = Value(variantsMap);
        properties["primitive"] = Value(false);
        properties["enum"] = Value(true);
    }

    // For unions, collect type names
    else if (stmt.isUnion()) {
        auto typesList = std::make_shared<ChronovyanArray>();

        if (stmt.getUnionTypes().empty()) {
            std::stringstream ss;
            ss << "Union type '" << typeName << "' has no member types defined";
            ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
        }

        std::set<std::string> uniqueTypes;  // To check for duplicates

        for (const auto& type : stmt.getUnionTypes()) {
            std::string memberTypeName = type->getName();

            // Validate member type name
            if (memberTypeName.empty()) {
                ErrorHandler::getInstance().reportError(stmt.getLocation(),
                                                        "Member type in union has an empty name");
                continue;
            }

            // Check for duplicate member types
            if (uniqueTypes.find(memberTypeName) != uniqueTypes.end()) {
                std::stringstream ss;
                ss << "Duplicate member type '" << memberTypeName << "' in union '" << typeName
                   << "'";
                ErrorHandler::getInstance().reportWarning(stmt.getLocation(), ss.str());
                continue;
            }

            uniqueTypes.insert(memberTypeName);
            typesList->push(Value(memberTypeName));
        }

        properties["types"] = Value(typesList);
        properties["primitive"] = Value(false);
        properties["union"] = Value(true);
    }

    // For aliases, store the aliased type
    else if (stmt.isAlias()) {
        std::string aliasedTypeName;

        if (stmt.getAliasedType() == nullptr) {
            std::stringstream ss;
            ss << "Alias type '" << typeName << "' has no target type defined";
            ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
            aliasedTypeName = "nil";  // Fallback
        } else {
            aliasedTypeName = stmt.getAliasedType()->getName();

            // Validate aliased type name
            if (aliasedTypeName.empty()) {
                std::stringstream ss;
                ss << "Alias type '" << typeName << "' references an empty type name";
                ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
                aliasedTypeName = "nil";  // Fallback
            }
        }

        properties["aliasedType"] = Value(aliasedTypeName);
        properties["primitive"] = Value(false);
        properties["alias"] = Value(true);
    }

    // Add type parameters for generic types
    if (stmt.isGeneric()) {
        auto paramsList = std::make_shared<ChronovyanArray>();

        std::set<std::string> uniqueParams;  // To check for duplicates

        for (const auto& param : stmt.getTypeParameters()) {
            // Validate type parameter name
            if (param.empty()) {
                ErrorHandler::getInstance().reportError(stmt.getLocation(),
                                                        "Type parameter has an empty name");
                continue;
            }

            // Check for duplicate type parameters
            if (uniqueParams.find(param) != uniqueParams.end()) {
                std::stringstream ss;
                ss << "Duplicate type parameter '" << param << "' in type '" << typeName << "'";
                ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
                continue;
            }

            uniqueParams.insert(param);
            paramsList->push(Value(param));
        }

        properties["typeParameters"] = Value(paramsList);
        properties["generic"] = Value(true);
    }

    // Check for reserved type names
    if (typeName == "nil" || typeName == "bool" || typeName == "int" || typeName == "float" ||
        typeName == "string" || typeName == "Array" || typeName == "Map" ||
        typeName == "Function" || typeName == "Object" || typeName == "Any") {
        std::stringstream ss;
        ss << "'" << typeName << "' is a reserved type name and cannot be redefined";
        ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
        return;
    }

    // Create the type definition
    auto typeDef = std::make_shared<CustomTypeDefinition>(typeName, kind, properties,
                                                          stmt.getTypeParameters());

    // Register the type with the custom type system
    bool success = m_typeSystem->registerType(typeDef);

    if (success) {
        std::cout << "Registered type: " << typeDef->toString() << std::endl;
    } else {
        std::stringstream ss;
        ss << "Failed to register type '" << typeName << "'";
        ErrorHandler::getInstance().reportError(stmt.getLocation(), ss.str());
    }
}

// Define type-related native functions
void Interpreter::defineTypeNativeFunctions() {
    // TYPE_OF function - returns the type name of a value
    m_globals->define("TYPE_OF",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              if (args.empty()) {
                                  ErrorHandler::getInstance().reportError(
                                      SourceLocation(), "TYPE_OF() requires a value parameter");
                                  return Value("nil");
                              }

                              const Value& value = args[0];

                              // Return the type name based on the value type
                              if (value.isNil()) {
                                  return Value("nil");
                              } else if (value.isBoolean()) {
                                  return Value("bool");
                              } else if (value.isInteger()) {
                                  return Value("int");
                              } else if (value.isFloat()) {
                                  return Value("float");
                              } else if (value.isString()) {
                                  return Value("string");
                              } else if (value.isArray()) {
                                  return Value("Array");
                              } else if (value.isMap()) {
                                  return Value("Map");
                              } else if (value.isNativeFunction() || value.isChronovyanFunction()) {
                                  return Value("Function");
                              } else if (value.isObject()) {
                                  // Check if the object has a type definition
                                  auto obj = value.asObject();
                                  if (obj->hasProperty("__type")) {
                                      return obj->getProperty("__type");
                                  } else {
                                      return Value("Object");
                                  }
                              } else {
                                  return Value("unknown");
                              }
                          },
                          1  // Arity
                          )));

    // IS_TYPE function - checks if a value is of a specific type
    m_globals->define(
        "IS_TYPE", Value(std::make_shared<NativeFunction>(
                       [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                           if (args.size() < 2) {
                               ErrorHandler::getInstance().reportError(
                                   SourceLocation(),
                                   "IS_TYPE() requires two parameters: a value and a type name");
                               return Value(false);
                           }

                           if (!args[1].isString()) {
                               std::stringstream ss;
                               ss << "IS_TYPE() second parameter must be a string type name, got "
                                  << args[1].getTypeName();
                               ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                               return Value(false);
                           }

                           const Value& value = args[0];
                           const std::string& typeName = args[1].asString();

                           // Check if typeName is empty
                           if (typeName.empty()) {
                               ErrorHandler::getInstance().reportError(
                                   SourceLocation(), "IS_TYPE() requires a non-empty type name");
                               return Value(false);
                           }

                           // Check if the type exists for custom types
                           if (!typeName.empty() && typeName != "nil" && typeName != "bool" &&
                               typeName != "int" && typeName != "float" && typeName != "string" &&
                               typeName != "Array" && typeName != "Map" && typeName != "Function" &&
                               typeName != "Object" && typeName != "Any" &&
                               !interpreter.getTypeSystem()->isTypeRegistered(typeName)) {
                               std::stringstream ss;
                               ss << "Unknown type '" << typeName << "' in IS_TYPE() check";
                               ErrorHandler::getInstance().reportWarning(SourceLocation(),
                                                                         ss.str());
                               // Still continue with the check, it will return false
                           }

                           // Check if the value conforms to the specified type
                           return Value(interpreter.getTypeSystem()->checkType(value, typeName));
                       },
                       2  // Arity
                       )));

    // CREATE_STRUCT function - creates a new struct instance
    m_globals->define(
        "CREATE_STRUCT",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.size() < 1) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "CREATE_STRUCT() requires at least a type name parameter");
                    return Value();
                }

                if (!args[0].isString()) {
                    std::stringstream ss;
                    ss << "CREATE_STRUCT() first parameter must be a string type name, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                const std::string& typeName = args[0].asString();

                // Check if typeName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "CREATE_STRUCT() requires a non-empty type name");
                    return Value();
                }

                // Check if the type exists
                if (!interpreter.getTypeSystem()->isTypeRegistered(typeName)) {
                    std::stringstream ss;
                    ss << "Cannot create struct: Type '" << typeName << "' is not defined";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the type definition
                auto typeDef = interpreter.getTypeSystem()->getTypeDefinition(typeName);

                // Check if it's a struct
                if (typeDef->getKind() != CustomTypeKind::STRUCT) {
                    std::stringstream ss;
                    ss << "Cannot create struct: Type '" << typeName
                       << "' is not a struct type (it's a "
                       << CustomTypeKindToString(typeDef->getKind()) << ")";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Create a map for field values
                std::map<std::string, Value> fieldValues;

                // If provided, use the field values from the second argument
                if (args.size() > 1) {
                    if (!args[1].isMap()) {
                        std::stringstream ss;
                        ss << "CREATE_STRUCT() second parameter must be a map of field values, got "
                           << args[1].getTypeName();
                        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                        // Continue with empty field values
                    } else {
                        auto fieldMap = args[1].asMap();

                        // Iterate through the provided fields
                        for (const auto& [key, value] : fieldMap) {
                            fieldValues[key] = value;
                        }
                    }
                }

                // Get the struct fields by accessing properties directly
                if (!typeDef->hasProperty("fields")) {
                    std::stringstream ss;
                    ss << "Invalid struct definition for '" << typeName
                       << "': missing fields property";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                Value fieldsValue = typeDef->getProperty("fields");

                if (!fieldsValue.isMap()) {
                    std::stringstream ss;
                    ss << "Invalid struct definition for '" << typeName
                       << "': fields property is not a map";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Create an object to represent the struct instance
                auto obj = std::make_shared<ObjectValue>();

                // Set the type property
                obj->setProperty("__type", Value(typeName));

                // Collect missing required fields
                std::vector<std::string> missingRequiredFields;

                // Set all fields with default values or provided values
                for (const auto& [fieldName, fieldProps] : fieldsValue.asMap()) {
                    if (!fieldProps.isMap()) {
                        std::stringstream ss;
                        ss << "Invalid field definition for '" << fieldName << "' in struct '"
                           << typeName << "': field properties not a map";
                        ErrorHandler::getInstance().reportWarning(SourceLocation(), ss.str());
                        continue;
                    }

                    auto fieldPropsMap = fieldProps.asMap();

                    // Check if the field is required
                    bool isRequired = false;
                    if (fieldPropsMap.find("required") != fieldPropsMap.end()) {
                        isRequired = fieldPropsMap.at("required").asBoolean();
                    }

                    // If the field has a value provided, use it
                    if (fieldValues.find(fieldName) != fieldValues.end()) {
                        // Type check the provided value if possible
                        if (fieldPropsMap.find("type") != fieldPropsMap.end()) {
                            std::string fieldType = fieldPropsMap.at("type").asString();
                            if (!interpreter.getTypeSystem()->checkType(fieldValues[fieldName],
                                                                        fieldType)) {
                                std::stringstream ss;
                                ss << "Type mismatch for field '" << fieldName << "' in struct '"
                                   << typeName << "': expected " << fieldType << ", got "
                                   << fieldValues[fieldName].getTypeName();
                                ErrorHandler::getInstance().reportWarning(SourceLocation(),
                                                                          ss.str());
                                // Still set the value despite the type mismatch
                            }
                        }
                        obj->setProperty(fieldName, fieldValues[fieldName]);
                    }
                    // Otherwise, if it has a default value, use that
                    else if (fieldPropsMap.find("default") != fieldPropsMap.end()) {
                        obj->setProperty(fieldName, fieldPropsMap.at("default"));
                    }
                    // Otherwise, use a default value based on the type
                    else if (fieldPropsMap.find("type") != fieldPropsMap.end()) {
                        std::string fieldType = fieldPropsMap.at("type").asString();

                        // Track missing required fields
                        if (isRequired) {
                            missingRequiredFields.push_back(fieldName);
                        }

                        // Set default value based on type
                        if (fieldType == "int" || fieldType == "INT") {
                            obj->setProperty(fieldName, Value(static_cast<int64_t>(0)));
                        } else if (fieldType == "float" || fieldType == "FLOAT") {
                            obj->setProperty(fieldName, Value(0.0));
                        } else if (fieldType == "bool" || fieldType == "BOOLEAN") {
                            obj->setProperty(fieldName, Value(false));
                        } else if (fieldType == "string" || fieldType == "STRING") {
                            obj->setProperty(fieldName, Value(""));
                        } else {
                            obj->setProperty(fieldName, Value());
                        }
                    }
                    // Last resort: nil
                    else {
                        // Track missing required fields
                        if (isRequired) {
                            missingRequiredFields.push_back(fieldName);
                        }
                        obj->setProperty(fieldName, Value());
                    }
                }

                // Report missing required fields
                if (!missingRequiredFields.empty()) {
                    std::stringstream ss;
                    ss << "Missing required fields for struct '" << typeName << "': ";
                    for (size_t i = 0; i < missingRequiredFields.size(); ++i) {
                        if (i > 0)
                            ss << ", ";
                        ss << missingRequiredFields[i];
                    }
                    ErrorHandler::getInstance().reportWarning(SourceLocation(), ss.str());
                }

                return Value(obj);
            },
            1,  // Minimum arity
            2   // Maximum arity
            )));

    // CREATE_ENUM_VALUE function - creates a value of an enum type
    m_globals->define(
        "CREATE_ENUM_VALUE",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                if (args.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "CREATE_ENUM_VALUE() requires two parameters: a type name and a variant "
                        "name");
                    return Value();
                }

                if (!args[0].isString() || !args[1].isString()) {
                    std::stringstream ss;
                    ss << "CREATE_ENUM_VALUE() parameters must be strings, got "
                       << args[0].getTypeName() << " and " << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                const std::string& typeName = args[0].asString();
                const std::string& variantName = args[1].asString();

                // Check if typeName or variantName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "CREATE_ENUM_VALUE() requires a non-empty type name");
                    return Value();
                }

                if (variantName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "CREATE_ENUM_VALUE() requires a non-empty variant name");
                    return Value();
                }

                // Check if the type exists
                if (!interpreter.getTypeSystem()->isTypeRegistered(typeName)) {
                    std::stringstream ss;
                    ss << "Cannot create enum value: Type '" << typeName << "' is not defined";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the type definition
                auto typeDef = interpreter.getTypeSystem()->getTypeDefinition(typeName);

                // Check if it's an enum
                if (typeDef->getKind() != CustomTypeKind::ENUM) {
                    std::stringstream ss;
                    ss << "Cannot create enum value: Type '" << typeName
                       << "' is not an enum type (it's a "
                       << CustomTypeKindToString(typeDef->getKind()) << ")";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the enum variants by accessing properties directly
                if (!typeDef->hasProperty("variants")) {
                    std::stringstream ss;
                    ss << "Invalid enum definition for '" << typeName
                       << "': missing variants property";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                Value variantsValue = typeDef->getProperty("variants");

                if (!variantsValue.isMap()) {
                    std::stringstream ss;
                    ss << "Invalid enum definition for '" << typeName
                       << "': variants property is not a map";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Check if the variant exists
                if (variantsValue.asMap().find(variantName) == variantsValue.asMap().end()) {
                    std::stringstream ss;
                    ss << "Unknown variant '" << variantName << "' in enum type '" << typeName
                       << "'. Available variants: ";

                    const auto& elements = variantsValue.asMap();
                    size_t count = 0;
                    for (const auto& [key, _] : elements) {
                        if (count++ > 0)
                            ss << ", ";
                        if (count > 5) {
                            ss << "...and " << (elements.size() - 5) << " more";
                            break;
                        }
                        ss << key;
                    }

                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Create an object to represent the enum value
                auto obj = std::make_shared<ObjectValue>();

                // Set the type and variant properties
                obj->setProperty("__type", Value(typeName));
                obj->setProperty("__variant", Value(variantName));
                obj->setProperty("__value", variantsValue.asMap().at(variantName));

                return Value(obj);
            },
            2  // Arity
            )));
}

void Interpreter::initializeGlobals() {
    // Define resource visualization functions
    m_globals->define("TAKE_RESOURCE_SNAPSHOT",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 [[maybe_unused]] const std::vector<Value>& /*args*/) -> Value {
                              [[maybe_unused]] auto snapshot = m_visualization->takeSnapshot();
                              return Value();  // Return nil
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_CURRENT_STATE",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeCurrentState(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_RESOURCE_TRENDS",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              int steps = 10;  // Default value
                              VisualizationFormat format = VisualizationFormat::TEXT;

                              if (args.size() >= 1 && (args[0].isInteger() || args[0].isFloat())) {
                                  steps = static_cast<int>(args[0].isInteger() ? args[0].asInteger()
                                                                               : args[0].asFloat());
                              }

                              if (args.size() >= 2 && args[1].isString()) {
                                  std::string formatStr = args[1].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeTrends(steps, format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_OPTIMIZATION_OPPORTUNITIES",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(
                                  m_visualization->visualizeOptimizationOpportunities(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("VISUALIZE_DEBT_STATUS",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->visualizeDebtStatus(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define("GENERATE_RESOURCE_DASHBOARD",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 const std::vector<Value>& args) -> Value {
                              VisualizationFormat format = VisualizationFormat::TEXT;
                              if (args.size() >= 1 && args[0].isString()) {
                                  std::string formatStr = args[0].asString();
                                  if (formatStr == "JSON")
                                      format = VisualizationFormat::JSON;
                                  else if (formatStr == "CSV")
                                      format = VisualizationFormat::CSV;
                                  else if (formatStr == "COMPACT")
                                      format = VisualizationFormat::COMPACT;
                              }

                              return Value(m_visualization->generateDashboard(format));
                          },
                          0  // 0 means variable arity (optional parameters)
                          )));

    m_globals->define(
        "CLEAR_VISUALIZATION_HISTORY",
        Value(std::make_shared<NativeFunction>(
            [](Interpreter& interpreter, [[maybe_unused]] const std::vector<Value>& args) -> Value {
                interpreter.getResourceVisualization()->clearHistory();
                return Value();
            },
            0  // Arity
            )));
}

void Interpreter::setEnvironment(std::shared_ptr<Environment> environment) {
    m_environment = environment;
}

// Implementation of new visitor methods for temporal control flow

void Interpreter::visitBreakChrononStmt(const BreakChrononStmtNode& node) {
    // Set the flag to indicate that a BREAK_CHRONON statement was encountered
    m_breakChronon_flag = true;

    // For debugging/tracing purposes, you can add a message
    // std::cout << "BREAK_CHRONON executed at " << node.getLocation().toString() << std::endl;
}

void Interpreter::visitContinueWeaveStmt(const ContinueWeaveStmtNode& node) {
    // Set the flag to indicate that a CONTINUE_WEAVE statement was encountered
    m_continueWeave_flag = true;

    // For debugging/tracing purposes, you can add a message
    // std::cout << "CONTINUE_WEAVE executed at " << node.getLocation().toString() << std::endl;
}

void Interpreter::visitReverseFlowStmt(const ReverseFlowStmtNode& node) {
    // Set the flag to indicate that a REVERSE_FLOW statement was encountered
    m_reverseFlow_flag = true;

    // Invert the flow direction
    m_flowDirection = -m_flowDirection;

    // For debugging/tracing purposes, you can add a message
    // std::cout << "REVERSE_FLOW executed at " << node.getLocation().toString()
    //           << ", new direction: " << (m_flowDirection > 0 ? "forward" : "backward") <<
    //           std::endl;
}

void Interpreter::visitHaltRewindStmt(const HaltRewindStmtNode& stmt) {
    // Log the operation if in debug mode
    if (m_debugLogging) {
        std::cout << "Executing HALT_REWIND at " << stmt.getLocation().toString() << std::endl;
    }

    // Set the flag to halt the rewind operation
    m_haltRewind_flag = true;

    // Track resource usage for this operation
    trackResourceUsage(10.0, 5.0, "HALT_REWIND");
}

void Interpreter::visitStabilizeTimelineStmt(const StabilizeTimelineStmtNode& stmt) {
    // Log the operation if in debug mode
    if (m_debugLogging) {
        std::cout << "Executing STABILIZE_TIMELINE at " << stmt.getLocation().toString()
                  << std::endl;
    }

    double intensity = 0.5;  // Default intensity if not specified

    // Evaluate the intensity expression if provided
    if (stmt.hasIntensity()) {
        Value intensityValue = evaluate(stmt.getIntensity());

        if (intensityValue.isNumeric()) {
            intensity = intensityValue.asFloat();

            // Clamp intensity between 0.0 and 1.0
            intensity = std::max(0.0, std::min(1.0, intensity));
        } else {
            throw std::runtime_error("Stabilization intensity must be a number");
        }
    }

    // Set the stabilization factor
    m_stabilizationFactor = intensity;

    // Track resource usage (scales with intensity)
    trackResourceUsage(20.0 * intensity, 15.0 * intensity, "STABILIZE_TIMELINE");

    // Log the intensity if in debug mode
    if (m_debugLogging) {
        std::cout << "Timeline stabilization set to intensity: " << intensity << std::endl;
    }
}

void Interpreter::visitPreventModificationStmt(const PreventModificationStmtNode& stmt) {
    // Log the operation if in debug mode
    if (m_debugLogging) {
        std::cout << "Executing PREVENT_MODIFICATION at " << stmt.getLocation().toString()
                  << std::endl;
    }

    std::string scope = "local";  // Default scope

    // Evaluate the scope expression if provided
    if (stmt.hasScope()) {
        Value scopeValue = evaluate(stmt.getScope());

        if (scopeValue.getType() == Value::Type::STRING) {
            scope = scopeValue.asString();

            // Validate scope
            if (scope != "local" && scope != "global" && scope != "timeline") {
                throw std::runtime_error(
                    "Invalid scope for PREVENT_MODIFICATION. Must be 'local', 'global', or "
                    "'timeline'.");
            }
        } else {
            throw std::runtime_error("Scope must be a string");
        }
    }

    // Set the prevent modification flag
    m_preventModification_flag = true;

    // Enable read-only mode
    m_readOnlyMode = true;

    // Calculate resource costs based on scope
    double aethelCost = 15.0;
    double chrononCost = 10.0;

    if (scope == "global") {
        aethelCost *= 2.0;
        chrononCost *= 2.0;
    } else if (scope == "timeline") {
        aethelCost *= 3.0;
        chrononCost *= 3.0;
    }

    // Track resource usage
    trackResourceUsage(aethelCost, chrononCost, "PREVENT_MODIFICATION");

    // Log the scope if in debug mode
    if (m_debugLogging) {
        std::cout << "Modification prevention enabled with scope: " << scope << std::endl;
    }
}

void Interpreter::trackResourceUsage(double aethel, double chronons, const std::string& operation) {
    // Check if we have enough resources
    if (m_aethelAvailable < aethel || m_chrononsAvailable < chronons) {
        // Not enough resources
        throw std::runtime_error("Insufficient resources for operation: " + operation);
    }

    // Update available resources
    m_aethelAvailable -= aethel;
    m_chrononsAvailable -= chronons;

    // Log resource usage if debug mode is enabled
    if (m_debugLogging || m_debugMode) {  // Use both flags for compatibility
        std::cout << "Resource usage - Operation: " << operation << " | Aethel: " << aethel
                  << " | Chronons: " << chronons << std::endl;
        std::cout << "Remaining resources - Aethel: " << m_aethelAvailable
                  << " | Chronons: " << m_chrononsAvailable << std::endl;
    }

    // Check for resource depletion
    if (m_aethelAvailable < 0 || m_chrononsAvailable < 0) {
        throw std::runtime_error("Insufficient resources: Operation " + operation +
                                 " depleted available resources");
    }

    // Update visualization (commented out until visualization is fully implemented)
    // if (m_visualization) {
    //     m_visualization->recordResourceUsage(operation, aethel, chronons);
    // }
}

// Implement resource tracking method
// REMOVED: Duplicate implementation of trackResourceUsage was here (BUILD-005 fix)

// Implementation of visitor methods for resource management

void Interpreter::visitExpendResourcesStmt(const ExpendResourcesStmtNode& node) {
    double aethelCost = 0.0;
    double chrononCost = 0.0;

    // Use precalculated costs if available
    if (node.hasCalculatedCosts()) {
        aethelCost = node.getCachedAethelCost();
        chrononCost = node.getCachedChrononCost();
    } else {
        // Otherwise, evaluate expressions at runtime
        if (node.hasAethelCost()) {
            Value aethelValue = evaluate(node.getAethelCostExpr());
            aethelCost = getNumericValue(aethelValue);
        }

        if (node.hasChrononCost()) {
            Value chrononValue = evaluate(node.getChrononCostExpr());
            chrononCost = getNumericValue(chrononValue);
        }
    }

    // Check if we have enough resources
    bool hasEnoughResources =
        m_environment->checkResourceAvailability(aethelCost, chrononCost, node.getLocation());

    if (hasEnoughResources) {
        // Execute the main block
        executeBlock(node.getMainBlock(), m_environment);
    } else if (node.hasInsufficientHandler()) {
        // Execute the insufficient handler block
        visitResourceInsufficiencyHandler(node.getInsufficientHandler());
    } else {
        // Default handling when no handler is provided
        throw ChronovyanRuntimeError("Insufficient resources: required " +
                                         std::to_string(aethelCost) + " aethel and " +
                                         std::to_string(chrononCost) + " chronon",
                                     node.getLocation());
    }
}

void Interpreter::visitResourceInsufficiencyHandler(
    const ResourceInsufficiencyHandlerNode& handler) {
    // Implementation of resource insufficiency handler
    m_lastValue = Value();  // Set to Nil instead of Object()

    // (Handler body execution would go here once the ResourceInsufficiencyHandlerNode is updated to
    // support it)
}

// Implementations for ForChrononStmt

void Interpreter::visitForChrononStmt(const ForChrononStmtNode& node) {
    // Get the loop iterator name
    std::string iteratorName = node.getIterator();

    // Evaluate the range start and end expressions
    Value startValue = evaluate(node.getRangeStart());
    Value endValue = evaluate(node.getRangeEnd());

    // Evaluate the optional step expression, default to 1
    Value stepValue = Value(static_cast<int64_t>(1));
    if (node.hasStep()) {
        stepValue = evaluate(node.getStepExpr());
    }

    // Convert all values to appropriate types
    if (!startValue.isNumeric() || !endValue.isNumeric() || !stepValue.isNumeric()) {
        ErrorHandler::getInstance().reportError(
            node.getLocation(), "FOR_CHRONON loop requires numeric range and step values");
        return;
    }

    // Create a new environment for the loop body
    auto loopEnv = std::make_shared<Environment>(m_environment);

    // Reset control flow flags
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;
    m_reverseFlow_flag = false;
    m_flowDirection = 1;  // Start in forward direction

    // Get loop bounds and step
    double start = startValue.asFloat();
    double end = endValue.asFloat();
    double step = stepValue.asFloat() * m_flowDirection;  // Apply direction to step

    // Define the iterator variable
    loopEnv->define(iteratorName, Value(start));

    // Execute the loop
    while ((step > 0 && loopEnv->get(iteratorName).asFloat() <= end) ||
           (step < 0 && loopEnv->get(iteratorName).asFloat() >= end)) {
        // Execute the loop body with the loop environment
        executeBlock(node.getBody(), loopEnv);

        // Check for control flow interruptions
        if (m_breakChronon_flag) {
            // Reset the flag and exit the loop
            m_breakChronon_flag = false;
            break;
        }

        if (m_continueWeave_flag) {
            // Reset the flag and continue to the next iteration
            m_continueWeave_flag = false;
            // Skip the rest of the loop body
        }

        if (m_reverseFlow_flag) {
            // The flow direction has been changed by REVERSE_FLOW
            // Update the step to reflect the new direction
            step = std::abs(step) * m_flowDirection;
            m_reverseFlow_flag = false;
        }

        // Update the iterator for the next iteration
        double currentValue = loopEnv->get(iteratorName).asFloat();
        loopEnv->assign(iteratorName, Value(currentValue + step));
    }
}

// Implementations for WhileEventStmt

void Interpreter::visitWhileEventStmt(const WhileEventStmtNode& node) {
    // Reset control flow flags
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;

    // Execute the loop
    while (true) {
        // Evaluate the condition
        Value conditionValue = evaluate(node.getCondition());

        if (!conditionValue.isBoolean()) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "WHILE_EVENT condition must evaluate to a boolean value");
            return;
        }

        // Exit if the condition is false
        if (!conditionValue.asBool()) {
            break;
        }

        // Execute the loop body
        executeBlock(node.getBody(), m_environment);

        // Check for control flow interruptions
        if (m_breakChronon_flag) {
            // Reset the flag and exit the loop
            m_breakChronon_flag = false;
            break;
        }

        if (m_continueWeave_flag) {
            // Reset the flag and continue to the next iteration
            m_continueWeave_flag = false;
            // Skip the rest of the loop body
        }
    }
}

// Implementations for RewindFlowStmt

void Interpreter::visitRewindFlowStmt(const RewindFlowStmtNode& stmt) {
    // Initialize control flow flags - save old values to restore later
    bool oldBreakChronon = m_breakChronon_flag;
    bool oldContinueWeave = m_continueWeave_flag;
    bool oldReverseFlow = m_reverseFlow_flag;
    bool oldHaltRewind = m_haltRewind_flag;  // Save current HALT_REWIND state
    bool oldPreventModification =
        m_preventModification_flag;         // Save current PREVENT_MODIFICATION state
    bool oldReadOnlyMode = m_readOnlyMode;  // Track previous read-only state
    double oldStabilizationFactor = m_stabilizationFactor;  // Track previous stabilization

    // Reset control flags for this REWIND_FLOW operation
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;
    m_reverseFlow_flag = false;
    m_haltRewind_flag = false;           // Initialize the HALT_REWIND flag
    m_preventModification_flag = false;  // Initialize the PREVENT_MODIFICATION flag
    m_readOnlyMode = false;              // Start with modifications allowed
    m_stabilizationFactor = 0.0;         // Start with no stabilization

    // Log operation start if in debug mode
    if (m_debugLogging || m_debugMode) {
        std::cout << "Starting REWIND_FLOW at " << stmt.getLocation().toString() << std::endl;
    }

    // Check if we have a maximum rewinds limit
    int maxRewinds = -1;  // -1 means no limit
    if (stmt.hasMaxRewinds()) {
        Value maxRewindsValue = evaluate(stmt.getMaxRewinds());

        if (maxRewindsValue.getType() != Value::Type::NUMBER || !maxRewindsValue.isInteger()) {
            throw std::runtime_error("Maximum rewinds must be a number");
        }

        maxRewinds = static_cast<int>(maxRewindsValue.asFloat());
        if (maxRewinds < 0) {
            throw std::runtime_error("Maximum rewinds cannot be negative");
        }

        // Log max rewinds if in debug mode
        if (m_debugLogging || m_debugMode) {
            std::cout << "REWIND_FLOW maximum rewinds set to: " << maxRewinds << std::endl;
        }
    }

    // Track number of rewinds
    int rewindCount = 0;
    bool paradoxRiskIncreased = false;

    // Calculate initial resource cost based on the temporal scope
    double baseCost = calculateTemporalResourceCost(TemporalOperation::REWIND_FLOW);

    // Rewind flow execution
    while (true) {
        // Check if we've reached the maximum number of rewinds
        if (maxRewinds >= 0 && rewindCount >= maxRewinds) {
            if (m_debugLogging || m_debugMode) {
                std::cout << "REWIND_FLOW reached maximum rewinds: " << maxRewinds << std::endl;
            }
            break;
        }

        // Evaluate the condition
        Value conditionValue = evaluate(stmt.getCondition());
        if (conditionValue.getType() != Value::Type::BOOLEAN) {
            throw std::runtime_error("REWIND_FLOW condition must be a boolean");
        }

        if (!conditionValue.asBool()) {
            if (m_debugLogging || m_debugMode) {
                std::cout << "REWIND_FLOW condition evaluated to false, exiting loop" << std::endl;
            }
            break;
        }

        // Calculate resource cost for this iteration, affected by stabilization factor
        double iterationCost = baseCost;

        // Apply stabilization factor - higher stabilization reduces resource cost but increases
        // paradox risk
        if (m_stabilizationFactor > 0.0) {
            // Reduce cost based on stabilization
            iterationCost *= (1.0 - (m_stabilizationFactor * 0.5));

            // But increase paradox risk
            if (!paradoxRiskIncreased && m_stabilizationFactor > 0.3) {
                if (m_debugLogging || m_debugMode) {
                    std::cout << "REWIND_FLOW stabilization factor " << m_stabilizationFactor
                              << " increases paradox risk" << std::endl;
                }
                paradoxRiskIncreased = true;
            }
        }

        // Consume resources for this rewind operation
        trackResourceUsage(iterationCost * 0.6, iterationCost * 0.4,
                           "REWIND_FLOW iteration " + std::to_string(rewindCount + 1));

        // Apply read-only mode if PREVENT_MODIFICATION is active
        if (m_preventModification_flag) {
            m_readOnlyMode = true;
            if (m_debugLogging || m_debugMode) {
                std::cout << "REWIND_FLOW in read-only mode due to PREVENT_MODIFICATION"
                          << std::endl;
            }
        }

        // Execute the body with current control flags
        executeBlock(stmt.getBody(), m_environment);

        // Check for control flow interruption
        if (m_breakChronon_flag) {
            if (m_debugLogging || m_debugMode) {
                std::cout << "REWIND_FLOW exited due to BREAK_CHRONON" << std::endl;
            }
            break;
        }

        if (m_continueWeave_flag) {
            m_continueWeave_flag = false;
            rewindCount++;
            continue;
        }

        // Check for HALT_REWIND flag
        if (m_haltRewind_flag) {
            if (m_debugLogging || m_debugMode) {
                std::cout << "REWIND_FLOW halted due to HALT_REWIND control mechanism" << std::endl;
            }
            break;
        }

        rewindCount++;

        // Log iteration completion if in debug mode
        if (m_debugLogging || m_debugMode) {
            std::cout << "REWIND_FLOW completed iteration " << rewindCount;
            if (m_stabilizationFactor > 0.0) {
                std::cout << " with stabilization factor " << m_stabilizationFactor;
            }
            std::cout << std::endl;
        }
    }

    // Log operation completion if in debug mode
    if (m_debugLogging || m_debugMode) {
        std::cout << "REWIND_FLOW completed after " << rewindCount << " iterations" << std::endl;
    }

    // Restore control flow flags
    m_breakChronon_flag = oldBreakChronon;
    m_continueWeave_flag = oldContinueWeave;
    m_reverseFlow_flag = oldReverseFlow;
    m_haltRewind_flag = oldHaltRewind;
    m_preventModification_flag = oldPreventModification;
    m_readOnlyMode = oldReadOnlyMode;
    m_stabilizationFactor = oldStabilizationFactor;
}

// Implementation for AttemptWeaveStmt

void Interpreter::visitAttemptWeaveStmt(const AttemptWeaveStmtNode& node) {
    try {
        // Execute the try block
        executeBlock(node.getTryBlock(), m_environment);
    } catch (const std::exception& e) {
        // Get the exception message
        std::string errorMessage = e.what();

        // Look for a matching handler
        bool handlerFound = false;

        for (const auto& handler : node.getHandlers()) {
            // TODO: Implement type checking for dissonance handlers
            // For now, just use the first handler

            // Create a new environment for the handler
            auto handlerEnv = std::make_shared<Environment>(m_environment);

            // Bind the error variable
            handlerEnv->define(handler->getErrorVar(), Value(errorMessage));

            // Execute the handler body
            executeBlock(handler->getBody(), handlerEnv);

            handlerFound = true;
            break;
        }

        // If no handler was found, re-throw the exception
        if (!handlerFound) {
            throw;
        }
    }

    // Execute the finally block if present
    if (node.hasFinallyBlock()) {
        executeBlock(node.getFinallyBlock(), m_environment);
    }
}

void Interpreter::visitDissonanceHandler(const DissonanceHandlerNode& node) {
    // This method should not be called directly
    // Handlers are executed from visitAttemptWeaveStmt

    // If called directly, just execute the handler body
    executeBlock(node.getBody(), m_environment);
}

// Define temporal-related native functions
void Interpreter::defineTemporalFunctions() {
    // CURRENT_TIMESTAMP function - returns the current timestamp
    m_globals->define("CURRENT_TIMESTAMP",
                      Value(std::make_shared<NativeFunction>(
                          [this]([[maybe_unused]] Interpreter& interpreter,
                                 [[maybe_unused]] const std::vector<Value>& args) -> Value {
                              return Value(static_cast<int64_t>(
                                  0));  // Placeholder, should use m_runtime->getCurrentTimestamp()
                          },
                          0  // Arity
                          )));
}

double Interpreter::getNumericValue(const Value& value) {
    if (value.isInteger()) {
        return static_cast<double>(value.asInteger());
    } else if (value.isFloat()) {
        return value.asFloat();
    } else {
        throw ChronovyanRuntimeError("Expected a number, but got " + value.getTypeName(),
                                     SourceLocation());
    }
}

// Define user type system native functions
void Interpreter::defineUserTypeNativeFunctions() {
    // DEFINE_STRUCT function - defines a new struct type
    m_globals->define(
        "DEFINE_STRUCT",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(10);

                if (args.size() < 1) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "DEFINE_STRUCT() requires at least a type name parameter");
                    return Value(false);
                }

                if (!args[0].isString()) {
                    std::stringstream ss;
                    ss << "DEFINE_STRUCT() first parameter must be a string type name, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                const std::string& typeName = args[0].asString();

                // Check if typeName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_STRUCT() requires a non-empty type name");
                    return Value(false);
                }

                // Create a struct builder
                auto builder = m_userTypeSystem->createTypeBuilder(typeName, UserTypeKind::STRUCT);

                // If provided, add fields from the second argument
                if (args.size() > 1) {
                    if (!args[1].isMap()) {
                        std::stringstream ss;
                        ss << "DEFINE_STRUCT() second parameter must be a map of field "
                              "definitions, got "
                           << args[1].getTypeName();
                        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                        return Value(false);
                    }

                    auto fieldsMap = args[1].asMap();

                    // Iterate through the provided fields
                    for (const auto& [fieldName, fieldProps] : fieldsMap) {
                        // If it's a simple type specification
                        if (fieldProps.isString()) {
                            // Use the most specific constructor to avoid ambiguity
                            builder.addField(fieldName, fieldProps.asString());
                        }
                        // If it's a map, it contains detailed field properties
                        else if (fieldProps.isMap()) {
                            auto propsMap = fieldProps.asMap();

                            // Get type (required)
                            if (propsMap.find("type") == propsMap.end() ||
                                !propsMap.at("type").isString()) {
                                std::stringstream ss;
                                ss << "Field '" << fieldName
                                   << "' must have a string 'type' property";
                                ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                                continue;
                            }

                            std::string fieldType = propsMap.at("type").asString();

                            // Get default value (optional)
                            Value defaultValue;
                            bool hasDefault = false;

                            if (propsMap.find("default") != propsMap.end()) {
                                defaultValue = propsMap.at("default");
                                hasDefault = true;
                            }

                            // Get required flag (optional, defaults to false if there's a default
                            // value)
                            bool isRequired = false;
                            if (propsMap.find("required") != propsMap.end() &&
                                propsMap.at("required").isBoolean()) {
                                isRequired = propsMap.at("required").asBoolean();
                            } else if (!hasDefault) {
                                // If no required flag and no default, assume required
                                isRequired = true;
                            }

                            // Add the field with all properties
                            if (hasDefault) {
                                // Use the most specific constructor to avoid ambiguity
                                builder.addField(fieldName, fieldType);
                            } else {
                                // Use the most specific constructor to avoid ambiguity
                                builder.addField(fieldName, fieldType);
                            }
                        } else {
                            std::stringstream ss;
                            ss << "Field '" << fieldName
                               << "' definition must be a string type or a properties map, got "
                               << fieldProps.getTypeName();
                            ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                        }
                    }
                }

                // Check for temporal support in the third argument
                bool temporal = false;
                if (args.size() > 2 && args[2].isBoolean()) {
                    temporal = args[2].asBoolean();
                }

                // Set temporal support
                builder.withTemporalSupport(temporal);

                // Build and register the type
                try {
                    builder.register_();
                    return Value(true);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to define struct type '" << typeName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }
            },
            1, 3  // Min and max arity
            )));

    // DEFINE_ENUM function - defines a new enum type
    m_globals->define(
        "DEFINE_ENUM",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(5);

                if (args.size() < 1) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_ENUM() requires at least a type name parameter");
                    return Value(false);
                }

                if (!args[0].isString()) {
                    std::stringstream ss;
                    ss << "DEFINE_ENUM() first parameter must be a string type name, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                const std::string& typeName = args[0].asString();

                // Check if typeName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_ENUM() requires a non-empty type name");
                    return Value(false);
                }

                // Create an enum builder
                auto builder = m_userTypeSystem->createTypeBuilder(typeName, UserTypeKind::ENUM);

                // If provided, add enum values from the second argument
                if (args.size() > 1) {
                    if (!args[1].isMap()) {
                        std::stringstream ss;
                        ss << "DEFINE_ENUM() second parameter must be a map of enum values, got "
                           << args[1].getTypeName();
                        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                        return Value(false);
                    }

                    auto valuesMap = args[1].asMap();

                    // Iterate through the provided values
                    for (const auto& [valueName, valueData] : valuesMap) {
                        // If valueData is an integer, it's an explicit value
                        if (valueData.isInteger()) {
                            // For now, just use addVariant without a value
                            // The advanced implementation with ExprNode would require more work
                            builder.addVariant(valueName);
                        }
                        // Otherwise, add without explicit value (auto-assign)
                        else {
                            builder.addVariant(valueName);
                        }
                    }
                }

                // Build and register the type
                try {
                    builder.register_();
                    return Value(true);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to define enum type '" << typeName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }
            },
            1, 2  // Min and max arity
            )));

    // DEFINE_UNION function - defines a new union type
    m_globals->define(
        "DEFINE_UNION",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(7);

                if (args.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "DEFINE_UNION() requires a type name and member types array");
                    return Value(false);
                }

                if (!args[0].isString()) {
                    std::stringstream ss;
                    ss << "DEFINE_UNION() first parameter must be a string type name, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                if (!args[1].isArray()) {
                    std::stringstream ss;
                    ss << "DEFINE_UNION() second parameter must be an array of member types, got "
                       << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                const std::string& typeName = args[0].asString();

                // Check if typeName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_UNION() requires a non-empty type name");
                    return Value(false);
                }

                // Create a union builder
                auto builder = m_userTypeSystem->createTypeBuilder(typeName, UserTypeKind::UNION);

                // Process member types
                auto membersArray = args[1].asArray();

                if (membersArray.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_UNION() requires at least two member types");
                    return Value(false);
                }

                std::vector<std::string> memberTypes;
                for (size_t i = 0; i < membersArray.size(); i++) {
                    auto memberType = membersArray.at(i);
                    if (!memberType.isString()) {
                        std::stringstream ss;
                        ss << "Union member type must be a string, got "
                           << memberType.getTypeName();
                        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                        continue;
                    }

                    memberTypes.push_back(memberType.asString());
                }

                // Add the member types to the union
                builder.asUnion();
                for (const auto& memberType : memberTypes) {
                    builder.addField("_member_" + memberType, memberType);
                }

                // Build and register the type
                try {
                    builder.register_();
                    return Value(true);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to define union type '" << typeName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }
            },
            2  // Arity
            )));

    // DEFINE_ALIAS function - defines a new type alias
    m_globals->define(
        "DEFINE_ALIAS",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(3);

                if (args.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_ALIAS() requires a type name and target type");
                    return Value(false);
                }

                if (!args[0].isString() || !args[1].isString()) {
                    std::stringstream ss;
                    ss << "DEFINE_ALIAS() parameters must be strings, got " << args[0].getTypeName()
                       << " and " << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                const std::string& aliasName = args[0].asString();
                const std::string& targetType = args[1].asString();

                // Check if names are empty
                if (aliasName.empty() || targetType.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "DEFINE_ALIAS() requires non-empty type names");
                    return Value(false);
                }

                // Create an alias builder
                auto builder = m_userTypeSystem->createTypeBuilder(aliasName, UserTypeKind::ALIAS);

                // No direct method to set the alias target in TypeBuilder API
                // We'll need to handle this through registration

                // Build and register the type
                try {
                    builder.register_();
                    return Value(true);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to define type alias '" << aliasName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }
            },
            2  // Arity
            )));

    // CREATE_INSTANCE function - creates an instance of a user-defined type
    m_globals->define(
        "CREATE_INSTANCE",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(5);

                if (args.size() < 1) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "CREATE_INSTANCE() requires at least a type name parameter");
                    return Value();
                }

                if (!args[0].isString()) {
                    std::stringstream ss;
                    ss << "CREATE_INSTANCE() first parameter must be a string type name, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                const std::string& typeName = args[0].asString();

                // Check if typeName is empty
                if (typeName.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "CREATE_INSTANCE() requires a non-empty type name");
                    return Value();
                }

                // Check if we have initial values
                std::map<std::string, Value> initialValues;
                if (args.size() > 1) {
                    if (!args[1].isMap()) {
                        std::stringstream ss;
                        ss << "CREATE_INSTANCE() second parameter must be a map of initial values, "
                              "got "
                           << args[1].getTypeName();
                        ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    } else {
                        auto valuesMap = args[1].asMap();
                        for (const auto& [key, value] : valuesMap) {
                            initialValues[key] = value;
                        }
                    }
                }

                // Create the instance
                try {
                    auto instancePtr = m_userTypeSystem->createInstance(typeName, initialValues);
                    if (instancePtr) {
                        return *instancePtr;  // Dereference to get the actual Value
                    } else {
                        return Value();  // Return empty Value if nullptr
                    }
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to create instance of type '" << typeName << "': " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }
            },
            1, 2  // Min and max arity
            )));

    // GET_INSTANCE_FIELD function - gets a field from a type instance
    m_globals->define(
        "GET_INSTANCE_FIELD",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(1);

                if (args.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "GET_INSTANCE_FIELD() requires an instance and a field name");
                    return Value();
                }

                if (!args[1].isString()) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_FIELD() second parameter must be a string field name, got "
                       << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                const std::string& fieldName = args[1].asString();

                // Check if instance is valid
                if (!m_userTypeSystem->isValidInstance(args[0])) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_FIELD() first parameter must be a valid type instance, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the field value
                try {
                    return m_userTypeSystem->getInstanceField(args[0], fieldName);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to get field '" << fieldName << "' from instance: " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }
            },
            2  // Arity
            )));

    // SET_INSTANCE_FIELD function - sets a field in a type instance
    m_globals->define(
        "SET_INSTANCE_FIELD",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(2);

                if (args.size() < 3) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "SET_INSTANCE_FIELD() requires an instance, a field name, and a value");
                    return Value(false);
                }

                if (!args[1].isString()) {
                    std::stringstream ss;
                    ss << "SET_INSTANCE_FIELD() second parameter must be a string field name, got "
                       << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                const std::string& fieldName = args[1].asString();
                const Value& newValue = args[2];

                // Check if instance is valid
                if (!m_userTypeSystem->isValidInstance(args[0])) {
                    std::stringstream ss;
                    ss << "SET_INSTANCE_FIELD() first parameter must be a valid type instance, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }

                // Set the field value
                try {
                    bool isTemporal = m_userTypeSystem->isInstanceTemporal(args[0]);

                    // If the instance is temporal, record the change
                    if (isTemporal) {
                        // Get current cycle
                        uint64_t currentCycle =
                            static_cast<uint64_t>(m_runtime->getCycleNumber());  // MODIFIED

                        // Update history
                        m_userTypeSystem->setInstanceFieldAt(args[0], fieldName, newValue,
                                                             currentCycle);
                    } else {
                        // Regular update
                        m_userTypeSystem->setInstanceField(args[0], fieldName, newValue);
                    }

                    return Value(true);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to set field '" << fieldName << "' in instance: " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value(false);
                }
            },
            3  // Arity
            )));

    // GET_INSTANCE_HISTORY function - gets the history of a temporal instance
    m_globals->define(
        "GET_INSTANCE_HISTORY",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(5);

                if (args.empty()) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(), "GET_INSTANCE_HISTORY() requires an instance parameter");
                    return Value();
                }

                // Check if instance is valid
                if (!m_userTypeSystem->isValidInstance(args[0])) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_HISTORY() parameter must be a valid type instance, got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Check if instance is temporal
                if (!m_userTypeSystem->isInstanceTemporal(args[0])) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_HISTORY() can only be used with temporal instances";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the history
                try {
                    return m_userTypeSystem->getInstanceHistory(args[0]);
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to get instance history: " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }
            },
            1  // Arity
            )));

    // GET_INSTANCE_AT_CYCLE function - gets an instance at a specific cycle
    m_globals->define(
        "GET_INSTANCE_AT_CYCLE",
        Value(std::make_shared<NativeFunction>(
            [this](Interpreter& interpreter, const std::vector<Value>& args) -> Value {
                // Track resource usage
                m_runtime->consumeChronons(3);

                if (args.size() < 2) {
                    ErrorHandler::getInstance().reportError(
                        SourceLocation(),
                        "GET_INSTANCE_AT_CYCLE() requires an instance and a cycle parameter");
                    return Value();
                }

                if (!args[1].isInteger()) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_AT_CYCLE() second parameter must be an integer cycle, got "
                       << args[1].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                int64_t cycle = args[1].asInteger();

                // Check if cycle is valid
                if (cycle < 0) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_AT_CYCLE() cycle must be non-negative, got " << cycle;
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Check if instance is valid
                if (!m_userTypeSystem->isValidInstance(args[0])) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_AT_CYCLE() first parameter must be a valid type instance, "
                          "got "
                       << args[0].getTypeName();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Check if instance is temporal
                if (!m_userTypeSystem->isInstanceTemporal(args[0])) {
                    std::stringstream ss;
                    ss << "GET_INSTANCE_AT_CYCLE() can only be used with temporal instances";
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }

                // Get the instance at the specified cycle
                try {
                    return m_userTypeSystem->getInstanceAtCycle(args[0],
                                                                static_cast<uint64_t>(cycle));
                } catch (const std::exception& e) {
                    std::stringstream ss;
                    ss << "Failed to get instance at cycle " << cycle << ": " << e.what();
                    ErrorHandler::getInstance().reportError(SourceLocation(), ss.str());
                    return Value();
                }
            },
            2  // Arity
            )));
}

// Implementation for TEMPORAL_LOOP construct
void Interpreter::visitTemporalLoopStmt(const TemporalLoopStmtNode& node) {
    // Reset control flow flags
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;
    m_reverseFlow_flag = false;
    m_flowDirection = 1;  // Start in forward direction

    // Determine duration if specified
    double totalDuration = -1;  // -1 means unlimited
    if (node.hasDuration()) {
        Value durationValue = evaluate(node.getDuration());
        if (!durationValue.isNumeric()) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "TEMPORAL_LOOP duration must be a numeric value");
            return;
        }
        totalDuration = durationValue.asFloat();
    }

    // Track the accumulated duration
    double accumulatedDuration = 0.0;

    // Get variance if specified
    double variance = 0.0;
    if (node.hasVariance()) {
        Value varianceValue = evaluate(node.getVariance());
        if (!varianceValue.isNumeric()) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "TEMPORAL_LOOP variance must be a numeric value");
            return;
        }
        variance = varianceValue.asFloat();
    }

    // Execute the loop
    while (true) {
        // Check if we've exceeded the total duration
        if (totalDuration >= 0 && accumulatedDuration >= totalDuration) {
            break;
        }

        // Evaluate the condition
        if (node.hasCondition()) {
            Value conditionValue = evaluate(node.getCondition());
            if (!conditionValue.isBoolean()) {
                ErrorHandler::getInstance().reportError(
                    node.getLocation(), "TEMPORAL_LOOP condition must evaluate to a boolean value");
                return;
            }

            // Exit if the condition is false
            if (!conditionValue.asBool()) {
                break;
            }
        }

        // Execute the loop body
        execute(node.getBody());

        // Check for control flow interruptions
        if (m_breakChronon_flag) {
            // Reset the flag and exit the loop
            m_breakChronon_flag = false;
            break;
        }

        if (m_continueWeave_flag) {
            // Reset the flag and continue to the next iteration
            m_continueWeave_flag = false;
        }

        if (m_reverseFlow_flag) {
            // The flow direction has been changed by REVERSE_FLOW
            m_reverseFlow_flag = false;
            m_flowDirection *= -1;  // Toggle direction
        }

        // Update accumulated duration based on variance
        // If variance is specified, apply a random fluctuation to the duration increment
        double durationIncrement = 1.0;  // Default increment
        if (variance > 0) {
            // Apply random variance between -variance and +variance
            double randomFactor =
                ((double)std::rand() / RAND_MAX) * 2 - 1;  // Random between -1 and 1
            durationIncrement = 1.0 + (randomFactor * variance);
            if (durationIncrement < 0)
                durationIncrement = 0.1;  // Ensure positive increment
        }

        // Apply flow direction to duration increment
        durationIncrement *= m_flowDirection;

        // Update the accumulated duration
        accumulatedDuration += durationIncrement;
    }
}

// Helper method to calculate temporal resource cost based on operation type
double Interpreter::calculateTemporalResourceCost(TemporalOperation operation) {
    switch (operation) {
        case TemporalOperation::FOR_CHRONON:
            return 1.0;
        case TemporalOperation::WHILE_EVENT:
            return 2.0;
        case TemporalOperation::REWIND_FLOW:
            return 3.5;
        case TemporalOperation::BRANCH_TIMELINE:
            return 5.0;  // Higher cost due to timeline branching
        case TemporalOperation::MERGE_TIMELINES:
            return 4.5;
        case TemporalOperation::TEMPORAL_ECHO_LOOP:
            return 3.0;
        case TemporalOperation::TEMPORAL_LOOP:
            return 3.0;
        case TemporalOperation::PARALLEL_EXECUTION:
            return 4.0;
        default:
            return 1.0;
    }
}

// Helper method to consume resources and handle insufficiency
void Interpreter::consumeResources(double amount) {
    // Check if we have enough resources
    if (m_aethelAvailable < amount) {
        // Not enough resources
        if (m_environment->hasResourceInsufficiencyHandler()) {
            // Execute resource insufficiency handler
            // TODO: Implement this
        } else {
            throw std::runtime_error("Insufficient aethel resources for temporal operation");
        }
    }

    // Consume resources
    m_aethelAvailable -= amount;
}

// This code is no longer needed since we're not using these enum values
/*
void executeTemporalLoopStmt(const TemporalLoopStmtNode& stmt) {
    // Evaluate the condition
    Value conditionValue = evaluate(stmt.getCondition());

    if (conditionValue.getType() != Value::Type::BOOLEANEAN) {
        throw std::runtime_error("Temporal loop condition must be a boolean");
    }

    // ... existing code ...
}
*/

// Add missing arithmetic and logical operation methods

Value Interpreter::negate(const Value& operand) {
    if (operand.getType() == Value::Type::NUMBER) {
        return Value(-operand.asNumber());
    }
    throw std::runtime_error("Operand must be a number.");
}

Value Interpreter::logicalNot(const Value& operand) {
    return Value(!operand.isBoolean() ? false : !operand.asBoolean());
}

Value Interpreter::add(const Value& left, const Value& right) {
    if (left.getType() == Value::Type::NUMBER && right.getType() == Value::Type::NUMBER) {
        return Value(left.asNumber() + right.asNumber());
    }
    if (left.isString() || right.isString()) {
        // String concatenation
        return Value(left.asString() + right.asString());
    }
    throw std::runtime_error("Operands must be two numbers or at least one string.");
}

Value Interpreter::subtract(const Value& left, const Value& right) {
    if (left.getType() == Value::Type::NUMBER && right.getType() == Value::Type::NUMBER) {
        return Value(left.asNumber() - right.asNumber());
    }
    throw std::runtime_error("Operands must be numbers.");
}

Value Interpreter::multiply(const Value& left, const Value& right) {
    if (left.getType() == Value::Type::NUMBER && right.getType() == Value::Type::NUMBER) {
        return Value(left.asNumber() * right.asNumber());
    }
    throw std::runtime_error("Operands must be numbers.");
}

Value Interpreter::divide(const Value& left, const Value& right) {
    if (left.getType() == Value::Type::NUMBER && right.getType() == Value::Type::NUMBER) {
        if (right.asNumber() == 0) {
            throw std::runtime_error("Division by zero.");
        }
        return Value(left.asNumber() / right.asNumber());
    }
    throw std::runtime_error("Operands must be numbers.");
}

Value Interpreter::modulo(const Value& left, const Value& right) {
    if (left.getType() == Value::Type::NUMBER && right.getType() == Value::Type::NUMBER) {
        if (right.asNumber() == 0) {
            throw std::runtime_error("Modulo by zero.");
        }
        return Value(std::fmod(left.asNumber(), right.asNumber()));
    }
    throw std::runtime_error("Operands must be numbers.");
}

bool Interpreter::areEqual(const Value& left, const Value& right) {
    // Handle nil case
    if (left.isNil() && right.isNil())
        return true;
    if (left.isNil() || right.isNil())
        return false;

    // Handle different types
    if (left.getType() != right.getType())
        return false;

    // Handle specific types
    switch (left.getType()) {
        case Value::Type::BOOLEAN:
            return left.asBoolean() == right.asBoolean();
        case Value::Type::NUMBER:
            return left.asNumber() == right.asNumber();
        case Value::Type::STRING:
            return left.asString() == right.asString();
        case Value::Type::NATIVE_FUNCTION:
        case Value::Type::CHRONOVYAN_FUNCTION:
            return false;  // Functions are never equal
        default:
            return false;
    }
}

void Interpreter::visitParallelExecutionStmt(const ParallelExecutionStmtNode& node) {
    // Reset control flow flags
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;

    // Calculate the number of threads
    int numThreads = 2;  // Default to 2 threads if not specified
    if (node.hasThreadCount()) {
        Value threadCountValue = evaluate(node.getThreadCount());
        if (!threadCountValue.isNumeric()) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "PARALLEL_EXECUTION thread count must be a numeric value");
            return;
        }
        numThreads = static_cast<int>(threadCountValue.asFloat());

        // Validate thread count
        if (numThreads < 1) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "PARALLEL_EXECUTION thread count must be at least 1");
            numThreads = 1;
        } else if (numThreads > 16) {
            // Arbitrary limit to prevent excessive resource usage
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "PARALLEL_EXECUTION thread count limited to 16");
            numThreads = 16;
        }
    }

    // Track resource usage for the PARALLEL_EXECUTION operation
    // More threads consume more resources
    double cost = calculateTemporalResourceCost(TemporalOperation::PARALLEL_EXECUTION) * numThreads;
    consumeResources(cost);

    // Set parallel execution flag
    bool oldInParallelExecution = m_inParallelExecution;
    m_inParallelExecution = true;
    m_totalThreads = numThreads;

    // Initialize thread environments
    m_threadEnvironments.clear();
    for (int i = 0; i < numThreads; i++) {
        m_threadEnvironments.push_back(std::make_shared<Environment>(m_environment));
    }

    // Execute the parallel block for each thread
    for (int i = 0; i < numThreads; i++) {
        // Skip execution for threads that don't satisfy the condition
        if (node.hasCondition()) {
            m_currentThreadId = i;
            Value conditionValue = evaluate(node.getCondition());
            if (!conditionValue.asBoolean()) {
                continue;  // Skip this thread
            }
        }

        try {
            // Execute the parallel block
            executeParallelBlock(node.getBody(), i, numThreads);
        } catch (const std::exception& e) {
            // Log error but continue with other threads
            std::string errorMsg = "Error in thread " + std::to_string(i) + ": " + e.what();
            ErrorHandler::getInstance().reportError(node.getLocation(), errorMsg);
        }
    }

    // Restore the original environment and flags
    m_inParallelExecution = oldInParallelExecution;
    m_currentThreadId = 0;
    m_totalThreads = 1;
    m_threadEnvironments.clear();
}

void Interpreter::executeParallelBlock(const BlockStmtNode& block, int threadId, int totalThreads) {
    // Save the current environment
    std::shared_ptr<Environment> previousEnv = m_environment;

    try {
        // Use the thread-specific environment
        m_environment = m_threadEnvironments[threadId];

        // Define THREAD_ID in the environment
        m_environment->define("THREAD_ID", Value(static_cast<int64_t>(threadId)));
        m_environment->define("TOTAL_THREADS", Value(static_cast<int64_t>(totalThreads)));

        // Execute the block statements
        for (const auto& stmt : block.getStatements()) {
            execute(*stmt);
        }
    } catch (const std::exception& e) {
        // Log the error but continue execution
        ErrorHandler::getInstance().reportError(block.getLocation(),
                                                "Error in parallel execution thread " +
                                                    std::to_string(threadId) + ": " + e.what());
    }

    // Restore the previous environment
    m_environment = previousEnv;
}

void Interpreter::reset() {
    // Reset the environment
    m_globals = std::make_shared<Environment>();
    m_environment = m_globals;

    // Reset control flow flags
    m_isReturning = false;
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;
    m_reverseFlow_flag = false;
    m_flowDirection = 1;
    m_haltRewind_flag = false;
    m_preventModification_flag = false;
    m_readOnlyMode = false;
    m_stabilizationFactor = 0.0;

    // Reset parallel execution state
    m_inParallelExecution = false;
    m_currentThreadId = 0;
    m_totalThreads = 1;
    m_threadEnvironments.clear();

    // Reset resources
    m_aethel = 1000.0;
    m_chronons = 1000.0;

    // Reinitialize
    initializeGlobals();
    defineNativeFunctions();
    defineTemporalFunctions();
    defineTypeNativeFunctions();
    defineUserTypeNativeFunctions();
}

void Interpreter::visitBranchTimelineStmt(const BranchTimelineStmtNode& node) {
    // Reset control flow flags
    m_breakChronon_flag = false;
    m_continueWeave_flag = false;

    // Calculate the number of branches
    int branchCount = 1;  // Default to 1 branch if not specified
    if (node.hasBranchCount()) {
        Value branchCountValue = evaluate(node.getBranchCount());
        if (!branchCountValue.isNumeric()) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "BRANCH_TIMELINE branch count must be a numeric value");
            return;
        }
        branchCount = static_cast<int>(branchCountValue.asFloat());

        // Validate branch count
        if (branchCount < 1) {
            ErrorHandler::getInstance().reportError(
                node.getLocation(), "BRANCH_TIMELINE branch count must be at least 1");
            branchCount = 1;
        } else if (branchCount > 10) {
            // Arbitrary limit to prevent excessive resource usage
            ErrorHandler::getInstance().reportError(node.getLocation(),
                                                    "BRANCH_TIMELINE branch count limited to 10");
            branchCount = 10;
        }
    }

    // Track resource usage for the BRANCH_TIMELINE operation
    // More branches consume more resources
    double cost = calculateTemporalResourceCost(TemporalOperation::BRANCH_TIMELINE) * branchCount;
    consumeResources(cost);

    // Get branch identifier if specified
    std::string branchPrefix = "branch_";
    if (node.hasIdentifier()) {
        Value identifierValue = evaluate(node.getIdentifier());
        branchPrefix = identifierValue.toString() + "_";
    }

    // Get merge strategy if specified
    std::string mergeStrategy = "latest";  // Default strategy
    if (node.hasMergeStrategy()) {
        Value mergeStrategyValue = evaluate(node.getMergeStrategy());
        mergeStrategy = mergeStrategyValue.toString();
    }

    // Create vector of branch IDs for later merging
    std::vector<std::string> branchIds;

    // Create and execute each branch
    for (int i = 0; i < branchCount; i++) {
        // Generate a unique branch ID
        std::string branchId = branchPrefix + std::to_string(i);
        branchIds.push_back(branchId);

        // Create a timeline snapshot
        if (m_runtime) {
            m_runtime->createTimelineSnapshot();
        }

        // Create a new environment for the branch
        auto branchEnv = std::make_shared<Environment>(m_environment);

        // Add branch-specific variables to the environment
        branchEnv->define("BRANCH_ID", Value(static_cast<int64_t>(i)));
        branchEnv->define("TOTAL_BRANCHES", Value(static_cast<int64_t>(branchCount)));
        branchEnv->define("BRANCH_NAME", Value(branchId));

        // Execute the branch body in the branch environment
        std::shared_ptr<Environment> previousEnv = m_environment;
        try {
            m_environment = branchEnv;
            execute(node.getBody());
        } catch (const std::exception& e) {
            // Log error but continue with other branches
            std::string errorMsg = "Error in branch " + branchId + ": " + e.what();
            ErrorHandler::getInstance().reportError(node.getLocation(), errorMsg);
        }
        m_environment = previousEnv;
    }

    // Merge timelines if multiple branches were created
    if (branchCount > 1 && m_runtime) {
        m_runtime->mergeTimelines(branchIds);

        // Increase paradox level based on number of branches merged
        if (m_runtime) {
            m_runtime->increaseParadoxLevel(branchCount - 1);
        }
    }
}

std::shared_ptr<LootSystem> Interpreter::getLootSystem() const { return m_lootSystem; }

}  // namespace chronovyan
