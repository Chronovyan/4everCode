#ifndef CHRONOVYAN_COMPILER_SYMBOL_TABLE_H
#define CHRONOVYAN_COMPILER_SYMBOL_TABLE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace chronovyan {
namespace compiler {

// Forward declarations
class TypeNode;

// Symbol representing a variable, function, or type
struct Symbol {
    // Different kinds of symbols
    enum class Kind { VARIABLE, FUNCTION, TYPE };

    // Symbol attributes
    Kind kind;
    const TypeNode* type;
    bool initialized;

    // Constructor
    Symbol(Kind kind, const TypeNode* type, bool initialized)
        : kind(kind), type(type), initialized(initialized) {}
};

// Scope representing a lexical scope (block)
class Scope {
public:
    // Constructor
    Scope(Scope* parent = nullptr) : parent(parent) {}

    // Symbol management
    bool addSymbol(const std::string& name, Symbol::Kind kind, const TypeNode* type,
                   bool initialized);
    Symbol* lookupSymbol(const std::string& name);
    Symbol* lookupSymbolInCurrentScope(const std::string& name);

    // Parent scope access
    Scope* getParent() const { return parent; }

private:
    std::unordered_map<std::string, Symbol> symbols;
    Scope* parent;
};

// SymbolTable managing all scopes
class SymbolTable {
public:
    // Constructor
    SymbolTable();

    // Scope management
    void enterScope();
    void exitScope();

    // Symbol management
    bool addSymbol(const std::string& name, Symbol::Kind kind, const TypeNode* type,
                   bool initialized);
    Symbol* lookupSymbol(const std::string& name);
    Symbol* lookupSymbolInCurrentScope(const std::string& name);

private:
    std::vector<std::unique_ptr<Scope>> scopes;
    Scope* currentScope;
};

}  // namespace compiler
}  // namespace chronovyan

#endif  // CHRONOVYAN_COMPILER_SYMBOL_TABLE_H