#include "stdafx.h"
#include "Lexer.h"
#include "SymbolTable.h"

using namespace std;

/**
 *
 * SymbolTable Method
 * 
 */

bool SymbolTable::isTypeDefined(string name)
{
	return classMap.find(name) == classMap.cend();
}

BlockSymbolTable* SymbolTable::addClass(std::shared_ptr<ClassNode> root) 
{
	if (!root) return nullptr;
	std::string name = root->classname.lexem;
	if (!isTypeDefined(name)) {
		//throw exception
		return nullptr;
	}
	BlockSymbolTable* block = new BlockSymbolTable();
	block->block_type = CLASS_BLOCK;
	block->prev = nullptr;
	classMap[name] = block;

	auto fields = root->fields;
	for (auto it_fields = fields.cbegin(); it_fields != fields.cend(); it_fields++)
	{
		addFormal(*it_fields, block);
	}
	auto methods = root->methods;
	for (auto it_methods = methods.cbegin(); it_methods != methods.cend(); it_methods++)
	{
		addMethodDefinition(*it_methods, block);
	}

	return block;

}

Symbol* SymbolTable::addFormal(shared_ptr<Formal> node, BlockSymbolTable* scope)
{
	if (!node) return nullptr;
	Symbol* symbol = new Symbol();
	symbol->id = node->id.lexem;
	symbol->type = node->type.lexem;
	symbol->symbol_type = OBJECT;
	scope->table[node->id.lexem] = symbol;

	return symbol;
}

BlockSymbolTable* SymbolTable::addMethodDefinition(std::shared_ptr<MethodDefinition> node, BlockSymbolTable* scope)
{
	Symbol* symbol = new Symbol();
	symbol->id = node->name.lexem;
	symbol->type = node->returntype.lexem;
	symbol->symbol_type = METHOD;
	
	shared_ptr<BlockStatement> blockStmt = node->block;
	BlockSymbolTable* block = addBlockScope(blockStmt, scope);
	block->block_type = METHOD_BLOCK;
	
	auto arguments = node->arguments;
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++) {
		addFormal(*it, block);
	}
	scope->table[symbol->id] = symbol;
	return block;
}

BlockSymbolTable* SymbolTable::addBlockScope(std::shared_ptr<BlockStatement> node, BlockSymbolTable* scope)
{
	BlockSymbolTable* block = new BlockSymbolTable();
	block->prev = scope;
	auto statements = node->stmts;
	for (auto it = statements.cbegin(); it != statements.cend(); it++)
	{
		addStatement(*it, block);
	}
	return block;
}

Symbol* SymbolTable::addStatement(std::shared_ptr<Statement> stmt, BlockSymbolTable* scope)
{
	if (!stmt) return nullptr;
	switch (stmt->node_type)
	{
	case EXP_STMT:
		addExpStatement(dynamic_pointer_cast<ExpStatement>(stmt), scope);
		break;
	case IF_STMT:
		addFromIfStmt(dynamic_pointer_cast<IfStatement>(stmt), scope);
		break;
	case WHILE_STMT:
		addFromWhileStmt(dynamic_pointer_cast<WhileStatement>(stmt), scope);
		break;
	}
}

Symbol* SymbolTable::addExpStatement(shared_ptr<ExpStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<Expression> exp = node->expression;
	if (exp->node_type == VAR_DECL_EXP) {
		auto n = dynamic_pointer_cast<VariableDeclareExpression>(exp);
		if (isTypeDefined(n->type.lexem)) {
			//throw exception and report an error.
			return nullptr;
		}

		if (scope->isVariableDeclared(n->id.lexem)) {
			//throw exception and report an error.
			return nullptr;
		}
		Symbol* symbol = new Symbol(n->id.lexem, n->type.lexem);
		return symbol;
	}
	return nullptr;
}

void SymbolTable::addFromWhileStmt(shared_ptr<WhileStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<BlockStatement> bs = node->block;
	addBlockScope(bs, scope);
}

void SymbolTable::addFromIfStmt(shared_ptr<IfStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<BlockStatement> bs = node->block;
	addBlockScope(bs, scope);
	if (node->elsepart) {
		addBlockScope(node->elsepart, scope);
	}
}

/*   
 *
 * BlockSymbolTable Methods
 *
 */

bool BlockSymbolTable::isVariableDeclaredBefore(std::string name)
{
	auto tmp = this;
	while (tmp) {
		if (tmp->table.find(name) == tmp->table.cend()) {
			return true;
		}
		tmp = tmp->prev;
	}
	return false;
}





