#include <iomanip>
#include <sstream>

#include "compiler/IR.h"

namespace chronovyan {
namespace compiler {

std::string IRValue::toString() const {
  std::ostringstream oss;

  switch (type) {
  case IRValueType::VOID:
    oss << "void";
    break;
  case IRValueType::BOOLEAN:
    oss << (getBoolValue() ? "true" : "false");
    break;
  case IRValueType::INTEGER:
    oss << getIntValue();
    break;
  case IRValueType::FLOAT:
    oss << std::fixed << std::setprecision(4) << getFloatValue();
    break;
  case IRValueType::STRING:
    oss << "\"" << getStringValue() << "\"";
    break;
  case IRValueType::REFERENCE:
    oss << "@" << getStringValue();
    break;
  case IRValueType::FUNCTION:
    oss << "func:" << getStringValue();
    break;
  case IRValueType::RESOURCE:
    oss << "resource";
    break;
  case IRValueType::TIMELINE:
    oss << "timeline";
    break;
  case IRValueType::THREAD:
    oss << "thread";
    break;
  case IRValueType::UNKNOWN:
    oss << "unknown";
    break;
  }

  return oss.str();
}

std::string IRInstruction::toString() const {
  std::ostringstream oss;

  // Format the opcode
  switch (opcode) {
  case IROpCode::NOP:
    oss << "NOP";
    break;
  case IROpCode::LABEL:
    oss << "LABEL";
    break;
  case IROpCode::JUMP:
    oss << "JUMP";
    break;
  case IROpCode::JUMP_IF_TRUE:
    oss << "JUMP_IF_TRUE";
    break;
  case IROpCode::JUMP_IF_FALSE:
    oss << "JUMP_IF_FALSE";
    break;
  case IROpCode::CALL:
    oss << "CALL";
    break;
  case IROpCode::RETURN:
    oss << "RETURN";
    break;
  case IROpCode::LOAD_CONST:
    oss << "LOAD_CONST";
    break;
  case IROpCode::LOAD_VAR:
    oss << "LOAD_VAR";
    break;
  case IROpCode::STORE_VAR:
    oss << "STORE_VAR";
    break;
  case IROpCode::PUSH:
    oss << "PUSH";
    break;
  case IROpCode::POP:
    oss << "POP";
    break;
  case IROpCode::DUP:
    oss << "DUP";
    break;
  case IROpCode::SWAP:
    oss << "SWAP";
    break;
  case IROpCode::ADD:
    oss << "ADD";
    break;
  case IROpCode::SUB:
    oss << "SUB";
    break;
  case IROpCode::MUL:
    oss << "MUL";
    break;
  case IROpCode::DIV:
    oss << "DIV";
    break;
  case IROpCode::MOD:
    oss << "MOD";
    break;
  case IROpCode::NEG:
    oss << "NEG";
    break;
  case IROpCode::AND:
    oss << "AND";
    break;
  case IROpCode::OR:
    oss << "OR";
    break;
  case IROpCode::NOT:
    oss << "NOT";
    break;
  case IROpCode::EQ:
    oss << "EQ";
    break;
  case IROpCode::NE:
    oss << "NE";
    break;
  case IROpCode::LT:
    oss << "LT";
    break;
  case IROpCode::LE:
    oss << "LE";
    break;
  case IROpCode::GT:
    oss << "GT";
    break;
  case IROpCode::GE:
    oss << "GE";
    break;
  case IROpCode::REWIND:
    oss << "REWIND";
    break;
  case IROpCode::BRANCH:
    oss << "BRANCH";
    break;
  case IROpCode::MERGE:
    oss << "MERGE";
    break;
  case IROpCode::SNAPSHOT:
    oss << "SNAPSHOT";
    break;
  case IROpCode::CONSUME_AETHEL:
    oss << "CONSUME_AETHEL";
    break;
  case IROpCode::CONSUME_CHRONON:
    oss << "CONSUME_CHRONON";
    break;
  case IROpCode::CHECK_RESOURCES:
    oss << "CHECK_RESOURCES";
    break;
  case IROpCode::PARALLEL_BEGIN:
    oss << "PARALLEL_BEGIN";
    break;
  case IROpCode::PARALLEL_END:
    oss << "PARALLEL_END";
    break;
  case IROpCode::THREAD_ID:
    oss << "THREAD_ID";
    break;
  case IROpCode::THREAD_COUNT:
    oss << "THREAD_COUNT";
    break;
  case IROpCode::TIMELINE_BRANCH_BEGIN:
    oss << "TIMELINE_BRANCH_BEGIN";
    break;
  case IROpCode::TIMELINE_BRANCH_END:
    oss << "TIMELINE_BRANCH_END";
    break;
  case IROpCode::TIMELINE_ID:
    oss << "TIMELINE_ID";
    break;
  case IROpCode::TIMELINE_COUNT:
    oss << "TIMELINE_COUNT";
    break;
  case IROpCode::DEBUG_PRINT:
    oss << "DEBUG_PRINT";
    break;
  default:
    oss << "UNKNOWN_OPCODE";
    break;
  }

  // Format operands
  if (!operands.empty()) {
    oss << " ";
    for (size_t i = 0; i < operands.size(); ++i) {
      if (i > 0) {
        oss << ", ";
      }
      oss << operands[i].toString();
    }
  }

  return oss.str();
}

std::string IRBasicBlock::toString() const {
  std::ostringstream oss;

  // Print the label
  if (!label.empty()) {
    oss << label << ":\n";
  }

  // Print instructions
  for (const auto &instruction : instructions) {
    oss << "    " << instruction.toString() << "\n";
  }

  return oss.str();
}

std::string IRFunction::toString() const {
  std::ostringstream oss;

  // Print function signature
  oss << "function " << name << "(";
  for (size_t i = 0; i < parameters.size(); ++i) {
    if (i > 0) {
      oss << ", ";
    }
    oss << parameters[i];
  }
  oss << ") {\n";

  // Print basic blocks
  for (const auto &block : basicBlocks) {
    oss << block.toString();
  }

  oss << "}\n";
  return oss.str();
}

std::string IRProgram::toString() const {
  std::ostringstream oss;

  // Print all functions
  for (const auto &function : functions) {
    oss << function.toString() << "\n";
  }

  return oss.str();
}

} // namespace compiler
} // namespace chronovyan