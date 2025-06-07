#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>

#include "compiler/ChronFileGenerator.h"

namespace chronovyan {
namespace compiler {

// BytecodeInstruction implementation

BytecodeInstruction::BytecodeInstruction(BytecodeNodeType type) : type(type), id(0), hasId(false) {}

BytecodeInstruction::BytecodeInstruction(BytecodeNodeType type, uint32_t id)
    : type(type), id(id), hasId(true) {}

BytecodeInstruction& BytecodeInstruction::addField(const std::string& name,
                                                   const BytecodeValue& value) {
    fields.emplace_back(name, value);
    return *this;
}

BytecodeInstruction& BytecodeInstruction::addChild(uint32_t childId) {
    children.push_back(childId);
    return *this;
}

BytecodeInstruction& BytecodeInstruction::withId(uint32_t nodeId) {
    id = nodeId;
    hasId = true;
    return *this;
}

void BytecodeInstruction::encode(std::vector<uint8_t>& bytecode) const {
    // Add the node type
    bytecode.push_back(static_cast<uint8_t>(type));

    // Add the node ID if present
    if (hasId) {
        bytecode.push_back(static_cast<uint8_t>((id >> 24) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>((id >> 16) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>((id >> 8) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>(id & 0xFF));
    }

    // Encode fields as a string
    std::string fieldsData;
    for (const auto& [name, value] : fields) {
        if (!fieldsData.empty()) {
            fieldsData += ":";
        }

        fieldsData += name + "=";

        if (std::holds_alternative<std::monostate>(value)) {
            fieldsData += "null";
        } else if (std::holds_alternative<bool>(value)) {
            fieldsData += std::get<bool>(value) ? "true" : "false";
        } else if (std::holds_alternative<int32_t>(value)) {
            fieldsData += std::to_string(std::get<int32_t>(value));
        } else if (std::holds_alternative<double>(value)) {
            fieldsData += std::to_string(std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            fieldsData += std::get<std::string>(value);
        } else if (std::holds_alternative<uint32_t>(value)) {
            fieldsData += std::to_string(std::get<uint32_t>(value));
        }
    }

    // Add data length
    uint32_t dataLength = static_cast<uint32_t>(fieldsData.size());
    bytecode.push_back(static_cast<uint8_t>((dataLength >> 24) & 0xFF));
    bytecode.push_back(static_cast<uint8_t>((dataLength >> 16) & 0xFF));
    bytecode.push_back(static_cast<uint8_t>((dataLength >> 8) & 0xFF));
    bytecode.push_back(static_cast<uint8_t>(dataLength & 0xFF));

    // Add data
    for (char c : fieldsData) {
        bytecode.push_back(static_cast<uint8_t>(c));
    }

    // Add children
    for (uint32_t childId : children) {
        bytecode.push_back(static_cast<uint8_t>((childId >> 24) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>((childId >> 16) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>((childId >> 8) & 0xFF));
        bytecode.push_back(static_cast<uint8_t>(childId & 0xFF));
    }
}

// ChronFileGenerator implementation

ChronFileGenerator::ChronFileGenerator() : nextNodeId(1) {}

bool ChronFileGenerator::generate(ProgramNode* programNode,
                                  const std::filesystem::path& outputPath) {
    if (!programNode) {
        return false;
    }

    // Clear state
    bytecode.clear();
    instructions.clear();
    symbolTable = SymbolTable();
    nodeIds.clear();
    nextNodeId = 1;

    // Generate header
    generateHeader();

    // Generate bytecode
    visitProgramNode(programNode);

    // Encode all instructions into bytecode
    encodeInstructions();

    // Write to file
    return writeFile(outputPath);
}

bool ChronFileGenerator::writeFile(const std::filesystem::path& outputPath) {
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open output file: " << outputPath << std::endl;
        return false;
    }

    outFile.write(reinterpret_cast<const char*>(bytecode.data()), bytecode.size());
    outFile.close();

    return true;
}

uint32_t ChronFileGenerator::getNextNodeId() { return nextNodeId++; }

void ChronFileGenerator::registerNodeId(const AstNode* node, uint32_t id) { nodeIds[node] = id; }

uint32_t ChronFileGenerator::getNodeId(const AstNode* node) const {
    auto it = nodeIds.find(node);
    if (it != nodeIds.end()) {
        return it->second;
    }

    // This should never happen if the AST is properly visited
    std::cerr << "Error: Node ID not found" << std::endl;
    return 0;
}

void ChronFileGenerator::addInstruction(const BytecodeInstruction& instruction) {
    instructions.push_back(instruction);
}

void ChronFileGenerator::generateHeader() {
    // Add magic number to identify Chronovyan bytecode files
    // "CHRON" in ASCII
    bytecode.push_back(0x43);  // 'C'
    bytecode.push_back(0x48);  // 'H'
    bytecode.push_back(0x52);  // 'R'
    bytecode.push_back(0x4F);  // 'O'
    bytecode.push_back(0x4E);  // 'N'

    // Add version number (0.1.0)
    bytecode.push_back(0x00);
    bytecode.push_back(0x01);
    bytecode.push_back(0x00);
}

void ChronFileGenerator::encodeInstructions() {
    for (const auto& instruction : instructions) {
        instruction.encode(bytecode);
    }
}

void ChronFileGenerator::visitProgramNode(ProgramNode* node) {
    uint32_t programId = getNextNodeId();
    registerNodeId(node, programId);

    // Create program node instruction
    BytecodeInstruction programInstr(BytecodeNodeType::PROGRAM, programId);

    // Add number of declarations as a field
    programInstr.addField("numDeclarations", static_cast<int32_t>(node->getDeclarations().size()));

    addInstruction(programInstr);

    // Visit all declarations
    for (const auto& decl : node->getDeclarations()) {
        if (decl) {
            decl->accept(*this);

            // Add child ID to the program node
            uint32_t declId = getNodeId(decl.get());
            programInstr.addChild(declId);
        }
    }
}

void ChronFileGenerator::visitBasicTypeNode(BasicTypeNode* node) {
    uint32_t typeId = getNextNodeId();
    registerNodeId(node, typeId);

    BytecodeInstruction typeInstr(BytecodeNodeType::BASIC_TYPE, typeId);
    typeInstr.addField("name", node->getToken().getLexeme());

    addInstruction(typeInstr);
}

void ChronFileGenerator::visitLiteralExprNode(LiteralExprNode* node) {
    uint32_t literalId = getNextNodeId();
    registerNodeId(node, literalId);

    BytecodeInstruction literalInstr(BytecodeNodeType::LITERAL, literalId);

    // Encode different literal types
    if (!node->getToken().hasValue()) {
        literalInstr.addField("value", std::monostate{});
    } else {
        const auto& value = node->getValue();

        if (std::holds_alternative<int>(value)) {
            literalInstr.addField("type", std::string("int"));
            literalInstr.addField("value", static_cast<int32_t>(std::get<int>(value)));
        } else if (std::holds_alternative<double>(value)) {
            literalInstr.addField("type", std::string("float"));
            literalInstr.addField("value", std::get<double>(value));
        } else if (std::holds_alternative<std::string>(value)) {
            literalInstr.addField("type", std::string("string"));
            literalInstr.addField("value", std::get<std::string>(value));
        } else if (std::holds_alternative<bool>(value)) {
            literalInstr.addField("type", std::string("bool"));
            literalInstr.addField("value", std::get<bool>(value));
        } else {
            literalInstr.addField("value", std::monostate{});
        }
    }

    addInstruction(literalInstr);
}

void ChronFileGenerator::visitVariableExprNode(VariableExprNode* node) {
    uint32_t varId = getNextNodeId();
    registerNodeId(node, varId);

    BytecodeInstruction varInstr(BytecodeNodeType::VARIABLE, varId);
    varInstr.addField("name", node->getName().getLexeme());

    addInstruction(varInstr);
}

void ChronFileGenerator::visitUnaryExprNode(UnaryExprNode* node) {
    uint32_t unaryId = getNextNodeId();
    registerNodeId(node, unaryId);

    // Generate code for the operand
    node->getRight().accept(*this);
    uint32_t rightId = getNodeId(&node->getRight());

    BytecodeInstruction unaryInstr(BytecodeNodeType::UNARY, unaryId);
    unaryInstr.addField("operator", node->getOperator().getLexeme());
    unaryInstr.addChild(rightId);

    addInstruction(unaryInstr);
}

void ChronFileGenerator::visitBinaryExprNode(BinaryExprNode* node) {
    uint32_t binaryId = getNextNodeId();
    registerNodeId(node, binaryId);

    // Generate code for the operands
    node->getLeft().accept(*this);
    node->getRight().accept(*this);

    uint32_t leftId = getNodeId(&node->getLeft());
    uint32_t rightId = getNodeId(&node->getRight());

    BytecodeInstruction binaryInstr(BytecodeNodeType::BINARY, binaryId);
    binaryInstr.addField("operator", node->getOperator().getLexeme());
    binaryInstr.addChild(leftId);
    binaryInstr.addChild(rightId);

    addInstruction(binaryInstr);
}

void ChronFileGenerator::visitCallExprNode(CallExprNode* node) {
    uint32_t callId = getNextNodeId();
    registerNodeId(node, callId);

    // Generate code for the callee
    node->getCallee().accept(*this);
    uint32_t calleeId = getNodeId(&node->getCallee());

    BytecodeInstruction callInstr(BytecodeNodeType::CALL, callId);
    callInstr.addField("numArgs", static_cast<int32_t>(node->getArguments().size()));
    callInstr.addChild(calleeId);

    // Generate code for arguments and add IDs
    for (const auto& arg : node->getArguments()) {
        arg->accept(*this);
        uint32_t argId = getNodeId(arg.get());
        callInstr.addChild(argId);
    }

    addInstruction(callInstr);
}

void ChronFileGenerator::visitAssignExprNode(AssignExprNode* node) {
    uint32_t assignId = getNextNodeId();
    registerNodeId(node, assignId);

    // Generate code for the value
    node->getValue().accept(*this);
    uint32_t valueId = getNodeId(&node->getValue());

    BytecodeInstruction assignInstr(BytecodeNodeType::ASSIGN, assignId);
    assignInstr.addField("name", node->getName().getLexeme());
    assignInstr.addChild(valueId);

    addInstruction(assignInstr);
}

void ChronFileGenerator::visitVarDeclNode(VarDeclNode* node) {
    uint32_t varDeclId = getNextNodeId();
    registerNodeId(node, varDeclId);

    BytecodeInstruction varDeclInstr(BytecodeNodeType::VAR_DECL, varDeclId);
    varDeclInstr.addField("name", node->getName().getLexeme());

    // Add type information
    if (node->getType()) {
        node->getType()->accept(*this);
        uint32_t typeId = getNodeId(node->getType());
        varDeclInstr.addChild(typeId);
    } else {
        varDeclInstr.addField("hasType", false);
    }

    // Add initializer if present
    if (node->getInitializer()) {
        node->getInitializer()->accept(*this);
        uint32_t initId = getNodeId(node->getInitializer());
        varDeclInstr.addChild(initId);
    } else {
        varDeclInstr.addField("hasInit", false);
    }

    addInstruction(varDeclInstr);
}

void ChronFileGenerator::visitStmtDeclNode(StmtDeclNode* node) {
    uint32_t stmtDeclId = getNextNodeId();
    registerNodeId(node, stmtDeclId);

    // Generate code for the statement
    node->getStmt().accept(*this);
    uint32_t stmtId = getNodeId(&node->getStmt());

    BytecodeInstruction stmtDeclInstr(BytecodeNodeType::STMT_DECL, stmtDeclId);
    stmtDeclInstr.addChild(stmtId);

    addInstruction(stmtDeclInstr);
}

void ChronFileGenerator::visitBlockStmtNode(BlockStmtNode* node) {
    uint32_t blockId = getNextNodeId();
    registerNodeId(node, blockId);

    BytecodeInstruction blockInstr(BytecodeNodeType::BLOCK, blockId);
    blockInstr.addField("numStmts", static_cast<int32_t>(node->getStatements().size()));

    // Generate code for statements
    for (const auto& stmt : node->getStatements()) {
        if (stmt) {
            stmt->accept(*this);
            uint32_t stmtId = getNodeId(stmt.get());
            blockInstr.addChild(stmtId);
        }
    }

    addInstruction(blockInstr);
}

void ChronFileGenerator::visitExprStmtNode(ExprStmtNode* node) {
    uint32_t exprStmtId = getNextNodeId();
    registerNodeId(node, exprStmtId);

    // Generate code for the expression
    node->getExpression().accept(*this);
    uint32_t exprId = getNodeId(&node->getExpression());

    BytecodeInstruction exprStmtInstr(BytecodeNodeType::EXPR_STMT, exprStmtId);
    exprStmtInstr.addChild(exprId);

    addInstruction(exprStmtInstr);
}

void ChronFileGenerator::visitIfStmtNode(IfStmtNode* node) {
    uint32_t ifStmtId = getNextNodeId();
    registerNodeId(node, ifStmtId);

    // Generate code for the condition
    node->getCondition().accept(*this);
    uint32_t condId = getNodeId(&node->getCondition());

    BytecodeInstruction ifStmtInstr(BytecodeNodeType::IF_STMT, ifStmtId);
    ifStmtInstr.addChild(condId);

    // Generate code for the then branch
    node->getThenBranch().accept(*this);
    uint32_t thenId = getNodeId(&node->getThenBranch());
    ifStmtInstr.addChild(thenId);

    // Generate code for the else branch if present
    if (node->getElseBranch()) {
        node->getElseBranch()->accept(*this);
        uint32_t elseId = getNodeId(node->getElseBranch());
        ifStmtInstr.addField("hasElse", true);
        ifStmtInstr.addChild(elseId);
    } else {
        ifStmtInstr.addField("hasElse", false);
    }

    addInstruction(ifStmtInstr);
}

void ChronFileGenerator::visitWhileStmtNode(WhileStmtNode* node) {
    uint32_t whileStmtId = getNextNodeId();
    registerNodeId(node, whileStmtId);

    // Generate code for the condition
    node->getCondition().accept(*this);
    uint32_t condId = getNodeId(&node->getCondition());

    BytecodeInstruction whileStmtInstr(BytecodeNodeType::WHILE_STMT, whileStmtId);
    whileStmtInstr.addChild(condId);

    // Generate code for the body
    node->getBody().accept(*this);
    uint32_t bodyId = getNodeId(&node->getBody());
    whileStmtInstr.addChild(bodyId);

    addInstruction(whileStmtInstr);
}

void ChronFileGenerator::visitResourceStmtNode(ResourceStmtNode* node) {
    uint32_t resourceStmtId = getNextNodeId();
    registerNodeId(node, resourceStmtId);

    BytecodeInstruction resourceStmtInstr(BytecodeNodeType::RESOURCE_STMT, resourceStmtId);

    // Add resource operation type
    resourceStmtInstr.addField("operation", static_cast<int32_t>(node->getOperation()));

    // Add chronons expression if present
    if (const ExprNode* chronons = node->getChronons()) {
        // Need a non-const pointer for accept call
        ExprNode* nonConstChronons = const_cast<ExprNode*>(chronons);
        nonConstChronons->accept(*this);
        uint32_t chrononsId = getNodeId(chronons);
        resourceStmtInstr.addField("hasChronons", true);
        resourceStmtInstr.addChild(chrononsId);
    } else {
        resourceStmtInstr.addField("hasChronons", false);
    }

    // Add aethel expression if present
    if (const ExprNode* aethel = node->getAethel()) {
        // Need a non-const pointer for accept call
        ExprNode* nonConstAethel = const_cast<ExprNode*>(aethel);
        nonConstAethel->accept(*this);
        uint32_t aethelId = getNodeId(aethel);
        resourceStmtInstr.addField("hasAethel", true);
        resourceStmtInstr.addChild(aethelId);
    } else {
        resourceStmtInstr.addField("hasAethel", false);
    }

    // Generate code for the body
    node->getBody().accept(*this);
    uint32_t bodyId = getNodeId(&node->getBody());
    resourceStmtInstr.addChild(bodyId);

    addInstruction(resourceStmtInstr);
}

void ChronFileGenerator::visitParallelExecutionStmtNode(ParallelExecutionStmtNode* node) {
    uint32_t parallelId = getNextNodeId();
    registerNodeId(node, parallelId);

    // Assuming we need to add PARALLEL_EXECUTION to BytecodeNodeType enum
    BytecodeInstruction parallelInstr(
        static_cast<BytecodeNodeType>(0x24),
        parallelId);  // Using 0x24 as temp value for PARALLEL_EXECUTION
    parallelInstr.addField("numThreads",
                           static_cast<int32_t>(1));  // Assuming thread count is 1 for now

    // Process thread count expression
    node->getThreadCount().accept(*this);
    uint32_t threadCountId = getNodeId(&node->getThreadCount());
    parallelInstr.addChild(threadCountId);

    addInstruction(parallelInstr);

    // Visit body
    node->getBody().accept(*this);
    uint32_t bodyId = getNodeId(&node->getBody());
    parallelInstr.addChild(bodyId);
}

void ChronFileGenerator::visitBranchTimelineStmtNode(BranchTimelineStmtNode* node) {
    uint32_t branchId = getNextNodeId();
    registerNodeId(node, branchId);

    // Assuming we need to add BRANCH_TIMELINE to BytecodeNodeType enum
    BytecodeInstruction branchInstr(static_cast<BytecodeNodeType>(0x25),
                                    branchId);  // Using 0x25 as temp value for BRANCH_TIMELINE

    // Process branch count expression
    node->getBranchCount().accept(*this);
    uint32_t branchCountId = getNodeId(&node->getBranchCount());
    branchInstr.addField("branchCount", static_cast<int32_t>(1));  // Default to 1 branch
    branchInstr.addChild(branchCountId);

    addInstruction(branchInstr);

    // Visit the branch body
    node->getBody().accept(*this);
    uint32_t bodyId = getNodeId(&node->getBody());
    branchInstr.addChild(bodyId);
}

}  // namespace compiler
}  // namespace chronovyan