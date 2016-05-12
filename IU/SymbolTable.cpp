#include "stdafx.h"
#include "Lexer.h"
#include "SymbolTable.h"
#include "Error.h"

using namespace std;

/**
 *
 * SymbolTable Method
 * 
 */

bool SymbolTable::isTypeDefined(string name)
{
	return classMap.find(name) != classMap.cend();
 }

bool BlockSymbolTable::isVariableDeclared(std::string name) 
{ 
	return table.find(name) != table.cend(); 
}

BlockSymbolTable* SymbolTable::addClass(std::shared_ptr<ClassNode> root) 
{
	if (!root) return nullptr;
	std::string name = root->classname.lexem;
	if (isTypeDefined(name)) {
		//throw exception
		Error::semantical_duplicate_var_error(name, root->lineno);
		return nullptr;
	}

	BlockSymbolTable* block = new BlockSymbolTable();
	block->block_type = CLASS_BLOCK;
	block->prev = nullptr;
	classMap[name] = block;
	string parentname = root->parentname;
	if (!parentname.empty() && !isTypeDefined(parentname)) {
		Error::semantical_class_undefined(parentname);
		return nullptr;
	}
	if (!parentname.empty()) {
		block->parentclass = parentname;
	}
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
	Symbol* symbol = new Symbol(node->id.lexem, node->type.lexem);
	symbol->symbol_type = OBJECT;
	symbol->lineno = node->lineno;
	scope->table[node->id.lexem] = symbol;

	return symbol;
}

BlockSymbolTable* SymbolTable::addMethodDefinition(std::shared_ptr<MethodDefinition> node, BlockSymbolTable* scope)
{
	Symbol* symbol = new Symbol(node->name.lexem, node->returntype.lexem);
	symbol->symbol_type = METHOD;
	symbol->lineno = node->lineno;
	shared_ptr<BlockStatement> blockStmt = node->block;
	BlockSymbolTable* block = addBlockScope(blockStmt, scope);
	block->block_type = METHOD_BLOCK;
	symbol->index = block->index;
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
	block->index = (int)scope->children.size();
	scope->children.push_back(block);

	auto statements = node->stmts;
	for (auto it = statements.cbegin(); it != statements.cend(); it++)
	{
		addStatement(*it, block);
	}
	return block;
}

void SymbolTable::addStatement(std::shared_ptr<Statement> stmt, BlockSymbolTable* scope)
{
	if (!stmt) return;
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
		if (!isTypeDefined(n->type.lexem)) {
			//throw exception and report an error.
			Error::semantical_undefined_type(n->type.lexem,node->lineno);
			return nullptr;
		}
		if (scope->isVariableDeclared(n->id.lexem)) {
			//throw exception and report an error.
			Error::semantical_duplicate_var_error(n->id.lexem, node->lineno);
			return nullptr;
		}
		Symbol* symbol = new Symbol(n->id.lexem, n->type.lexem);
		symbol->lineno = n->lineno;
		scope->table[symbol->id] = symbol;
		return symbol;
	}
	else if (exp->node_type == BINARY_EXP) {
		auto n = dynamic_pointer_cast<BinaryExpression>(exp);
		if (n->op.type != TK_ASSIGN) return nullptr;
		if (n->left->node_type == VAR_DECL_EXP) {
			auto var = dynamic_pointer_cast<VariableDeclareExpression>(n->left);
			if (!isTypeDefined(var->type.lexem)) {
				Error::semantical_undefined_type(var->type.lexem,var->lineno);
				return nullptr;
			}
			if (scope->isVariableDeclaredBefore(var->id.lexem)) {
				Error::semantical_duplicate_var_error(var->id.lexem, n->lineno);
				return nullptr;
			}
			Symbol* symbol = new Symbol(var->id.lexem, var->type.lexem);
			scope->table[symbol->id] = symbol;
			return symbol;
		}
	}
	return nullptr;
}

void SymbolTable::addFromWhileStmt(shared_ptr<WhileStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<BlockStatement> bs = node->block;
	BlockSymbolTable* block = addBlockScope(bs, scope);
	block->block_type = LOOP_BLOCK;
}

void SymbolTable::addFromIfStmt(shared_ptr<IfStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<BlockStatement> bs = node->block;
	BlockSymbolTable* block = addBlockScope(bs, scope);
	block->block_type = IF_BLOCK;
	if (node->elsepart) {
		BlockSymbolTable* elseblock = addBlockScope(node->elsepart, scope);
		elseblock->block_type = IF_BLOCK;
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
		if (tmp->table.find(name) != tmp->table.cend()) {
			return true;
		}
		tmp = tmp->prev;
	}
	return false;
}

//From inner scope to outer scope, find out a symbol with given name.
Symbol* BlockSymbolTable::lookupSymbolByName(std::string name)
{
	while (true) {
		if (table.find(name) != table.cend()) {
			return table[name];
		}
		if (!prev) break;
		return prev->lookupSymbolByName(name);
	}
	return nullptr;
}

/* Symbol printer */
void SymbolTable::printSymbolTable()
{
	for (auto it = classMap.cbegin(); it != classMap.cend(); ++it) {
		BlockSymbolTable::printBlockSymbolTable(it->second);
	}
}

void BlockSymbolTable::printBlockSymbolTable(BlockSymbolTable* block)
{
	auto stmtb = block->table;
	cout << "{ " << endl;
	for (auto it = stmtb.cbegin(); it != stmtb.cend(); ++it)
	{
		cout << it->second->id << " : " << it->second->type << endl;
	}
	cout << "}" << endl;
	vector<BlockSymbolTable*> children = block->children;
	for (auto child_it = children.cbegin(); child_it != children.cend(); ++child_it)
	{
		printBlockSymbolTable(*child_it);
	}
}






