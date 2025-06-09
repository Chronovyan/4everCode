#include <cassert>

#include "compiler/SymbolTable.h"

namespace chronovyan {
namespace compiler {

// Scope implementation

bool Scope::addSymbol(const std::string &name, Symbol::Kind kind,
                      const TypeNode *type, bool initialized) {
  // Check if the symbol already exists in this scope
  auto it = symbols.find(name);
  if (it != symbols.end()) {
    // Symbol already exists
    return false;
  }

  // Add the symbol
  symbols.emplace(name, Symbol(kind, type, initialized));
  return true;
}

Symbol *Scope::lookupSymbol(const std::string &name) {
  // Check if the symbol exists in this scope
  auto it = symbols.find(name);
  if (it != symbols.end()) {
    return &it->second;
  }

  // If not found in this scope, check the parent scope
  if (parent) {
    return parent->lookupSymbol(name);
  }

  // Symbol not found
  return nullptr;
}

Symbol *Scope::lookupSymbolInCurrentScope(const std::string &name) {
  // Check if the symbol exists in this scope only
  auto it = symbols.find(name);
  if (it != symbols.end()) {
    return &it->second;
  }

  // Symbol not found in current scope
  return nullptr;
}

// SymbolTable implementation

SymbolTable::SymbolTable() {
  // Create the global scope
  scopes.push_back(std::make_unique<Scope>());
  currentScope = scopes.back().get();
}

void SymbolTable::enterScope() {
  // Create a new scope with the current scope as its parent
  scopes.push_back(std::make_unique<Scope>(currentScope));
  currentScope = scopes.back().get();
}

void SymbolTable::exitScope() {
  // Make sure we have a scope to exit
  assert(currentScope && currentScope->getParent());

  // Set the current scope to the parent scope
  currentScope = currentScope->getParent();
}

bool SymbolTable::addSymbol(const std::string &name, Symbol::Kind kind,
                            const TypeNode *type, bool initialized) {
  // Make sure we have a current scope
  assert(currentScope);

  // Add the symbol to the current scope
  return currentScope->addSymbol(name, kind, type, initialized);
}

Symbol *SymbolTable::lookupSymbol(const std::string &name) {
  // Make sure we have a current scope
  assert(currentScope);

  // Look up the symbol starting from the current scope
  return currentScope->lookupSymbol(name);
}

Symbol *SymbolTable::lookupSymbolInCurrentScope(const std::string &name) {
  // Make sure we have a current scope
  assert(currentScope);

  // Look up the symbol in the current scope only
  return currentScope->lookupSymbolInCurrentScope(name);
}

} // namespace compiler
} // namespace chronovyan