#pragma once
#include <string>
#include <map>
#include <memory>
#include "ast.h"

enum BLOCK_TYPE {
	CLASS_BLOCK, METHOD_BLOCK, STMT_BLOCK
};
enum SYMBOL_TYPE {
	OBJECT, METHOD
};
/* IU comes wiht several built-in types such as String, Int, IO etc
   During compilation time, we can assume these build-in types are all provided.
 */

class Symbol
{
public:
	Symbol(){}
	Symbol(std::string id, std::string type): id(id), type(type){}
	Symbol(std::string id): id(id){}
	std::string id;
	std::string type;
	SYMBOL_TYPE symbol_type;
};

class BlockSymbolTable
{
public:
	BLOCK_TYPE block_type;

	BlockSymbolTable* prev; //point to closest outer scope
	
	std::map<std::string, Symbol*> table;

	/*find out if a variable is defined in current scope and outer scopes*/
	bool isVariableDeclaredBefore(std::string name);
	
	/*find out if a variable defined in current scope*/
	bool isVariableDeclared(std::string name) { return table.find(name) == table.cend(); }
}; 

class SymbolTable
{
private:
	Symbol* SymbolTable::addExpStatement(std::shared_ptr<ExpStatement> node, BlockSymbolTable* scope);

	Symbol* SymbolTable::addFormal(std::shared_ptr<Formal> node, BlockSymbolTable* scope);

	BlockSymbolTable* addMethodDefinition(std::shared_ptr<MethodDefinition> node, BlockSymbolTable* scope);

	BlockSymbolTable* addBlockScope(std::shared_ptr<BlockStatement> node, BlockSymbolTable* scope);

	Symbol* SymbolTable::addStatement(std::shared_ptr<Statement> stmt, BlockSymbolTable* scope);

	void SymbolTable::addFromIfStmt(std::shared_ptr<IfStatement> node, BlockSymbolTable* scope);

	void addFromWhileStmt(std::shared_ptr<WhileStatement> node, BlockSymbolTable* scope);

public:
	SymbolTable() {}
	
	std::map<std::string, BlockSymbolTable*> classMap;
	
	BlockSymbolTable* currentClass;

	bool isTypeDefined(std::string name);

	BlockSymbolTable* addClass(std::shared_ptr<ClassNode>);
};

