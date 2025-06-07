#pragma once

#include <filesystem>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "compiler/AstNode.h"
#include "compiler/AstVisitor.h"
#include "compiler/SymbolTable.h"

namespace chronovyan {
namespace compiler {

/**
 * @brief Enumeration of node types in bytecode.
 */
enum class BytecodeNodeType : uint8_t {
    // Program Structure
    PROGRAM = 0x01,
    BLOCK = 0x02,

    // Declarations
    VAR_DECL = 0x10,
    STMT_DECL = 0x11,

    // Statements
    EXPR_STMT = 0x20,
    IF_STMT = 0x21,
    WHILE_STMT = 0x22,
    RESOURCE_STMT = 0x23,
    PARALLEL_EXECUTION = 0x24,
    BRANCH_TIMELINE = 0x25,

    // Expressions
    LITERAL = 0x30,
    VARIABLE = 0x31,
    UNARY = 0x32,
    BINARY = 0x33,
    CALL = 0x34,
    ASSIGN = 0x35,

    // Types
    BASIC_TYPE = 0x40
};

/**
 * @brief Type representing a value that can be encoded in bytecode.
 */
using BytecodeValue = std::variant<std::monostate,  // No value
                                   bool,            // Boolean value
                                   int32_t,         // Integer value
                                   double,          // Floating-point value
                                   std::string,     // String value
                                   uint32_t         // Node ID reference
                                   >;

/**
 * @class BytecodeInstruction
 * @brief Represents a single instruction in the bytecode.
 *
 * This class encapsulates the logic for encoding different types of instructions
 * and their associated data into the bytecode buffer.
 */
class BytecodeInstruction {
public:
    /**
     * @brief Create a simple instruction with just a type.
     *
     * @param type The node type.
     */
    explicit BytecodeInstruction(BytecodeNodeType type);

    /**
     * @brief Create an instruction with a type and an ID.
     *
     * @param type The node type.
     * @param id The node ID.
     */
    BytecodeInstruction(BytecodeNodeType type, uint32_t id);

    /**
     * @brief Add a field to the instruction.
     *
     * @param name The field name.
     * @param value The field value.
     * @return This instruction for chaining.
     */
    BytecodeInstruction& addField(const std::string& name, const BytecodeValue& value);

    /**
     * @brief Add a child node reference to the instruction.
     *
     * @param childId The ID of the child node.
     * @return This instruction for chaining.
     */
    BytecodeInstruction& addChild(uint32_t childId);

    /**
     * @brief Add a node ID to the instruction.
     *
     * @param id The node ID.
     * @return This instruction for chaining.
     */
    BytecodeInstruction& withId(uint32_t id);

    /**
     * @brief Encode the instruction into the bytecode buffer.
     *
     * @param bytecode The bytecode buffer to append to.
     */
    void encode(std::vector<uint8_t>& bytecode) const;

private:
    BytecodeNodeType type;
    uint32_t id;
    bool hasId;
    std::vector<std::pair<std::string, BytecodeValue>> fields;
    std::vector<uint32_t> children;
};

/**
 * @class ChronFileGenerator
 * @brief Generates .chron files from Chronovyan AST.
 *
 * This class is responsible for generating .chron files, which are a compiled
 * format specific to the Chronovyan language. Unlike the CodeGenerator which produces
 * C code, this generates a more optimized binary format that can be directly executed
 * by the Chronovyan interpreter or runtime.
 */
class ChronFileGenerator : public AstVisitor {
public:
    /**
     * @brief Default constructor.
     */
    ChronFileGenerator();

    /**
     * @brief Generates a .chron file from a ProgramNode.
     *
     * @param programNode The AST root node.
     * @param outputPath The path to write the .chron file to.
     * @return True if generation was successful, false otherwise.
     */
    bool generate(ProgramNode* programNode, const std::filesystem::path& outputPath);

    // AstVisitor interface implementation
    void visitProgramNode(ProgramNode* node) override;
    void visitBasicTypeNode(BasicTypeNode* node) override;
    void visitLiteralExprNode(LiteralExprNode* node) override;
    void visitVariableExprNode(VariableExprNode* node) override;
    void visitUnaryExprNode(UnaryExprNode* node) override;
    void visitBinaryExprNode(BinaryExprNode* node) override;
    void visitCallExprNode(CallExprNode* node) override;
    void visitAssignExprNode(AssignExprNode* node) override;
    void visitVarDeclNode(VarDeclNode* node) override;
    void visitExprStmtNode(ExprStmtNode* node) override;
    void visitBlockStmtNode(BlockStmtNode* node) override;
    void visitIfStmtNode(IfStmtNode* node) override;
    void visitWhileStmtNode(WhileStmtNode* node) override;
    void visitResourceStmtNode(ResourceStmtNode* node) override;
    void visitStmtDeclNode(StmtDeclNode* node) override;
    void visitParallelExecutionStmtNode(ParallelExecutionStmtNode* node) override;
    void visitBranchTimelineStmtNode(BranchTimelineStmtNode* node) override;

protected:
    /**
     * @brief Get a new unique node ID.
     *
     * @return A new unique node ID.
     */
    uint32_t getNextNodeId();

    /**
     * @brief Register a node ID mapping.
     *
     * @param node The AST node.
     * @param id The assigned ID.
     */
    void registerNodeId(const AstNode* node, uint32_t id);

    /**
     * @brief Get the ID for a node.
     *
     * @param node The AST node.
     * @return The node's ID.
     */
    uint32_t getNodeId(const AstNode* node) const;

    /**
     * @brief Add a bytecode instruction to the instruction list.
     *
     * @param instruction The instruction to add.
     */
    void addInstruction(const BytecodeInstruction& instruction);

private:
    /**
     * @brief Write a binary file containing the compiled bytecode.
     *
     * @param outputPath The path to write the .chron file to.
     * @return True if the file was written successfully, false otherwise.
     */
    bool writeFile(const std::filesystem::path& outputPath);

    /**
     * @brief Generate the header for the bytecode file.
     */
    void generateHeader();

    /**
     * @brief Encode all instructions into the bytecode buffer.
     */
    void encodeInstructions();

    // Bytecode buffer
    std::vector<uint8_t> bytecode;

    // List of instructions
    std::vector<BytecodeInstruction> instructions;

    // Symbol table for tracking variable and function declarations
    SymbolTable symbolTable;

    // Map to store IDs of nodes for reference
    std::unordered_map<const AstNode*, uint32_t> nodeIds;

    // Next ID to assign to a node
    uint32_t nextNodeId;
};

}  // namespace compiler
}  // namespace chronovyan