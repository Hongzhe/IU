#include "stdafx.h"
#include "Analyzer.h"
#include "Error.h"
#include "Parser.h"
#include <memory>
#include <vector>

using namespace std;
Analyzer::Analyzer() {
	table = new SymbolTable();
	prepare();
}

void Analyzer::prepare()
{	
	//supposed we already have String and Int modules ready
	BlockSymbolTable* stringblock = new BlockSymbolTable();
	BlockSymbolTable* intblock = new BlockSymbolTable();
	table->classMap["String"] = stringblock;
	table->classMap["Int"] = intblock;
}


//start to analyze a class
BlockSymbolTable* Analyzer::analyze(std::shared_ptr<ClassNode> root)
{
	BlockSymbolTable* class_block = table->addClass(root); //add symbols of this class into symbol table;
	if (!class_block) {
		return nullptr;
	}
	current_table = class_block;
	classNodes[root->classname.lexem] = root;
	curClassNode = root;
	if (root->parentname != "") { //parent class is not defined.
		if (!table->isTypeDefined(root->parentname)) {
			//throw exception
			Error::semantical_undefined_var_error(root->parentname);
			return nullptr;
		}
	}
	return class_block;
}

void Analyzer::analyze_method(std::shared_ptr<MethodDefinition> node)
{
	shared_ptr<BlockStatement> blockstmt = node->block;
	vector<shared_ptr<Statement>> stmts = blockstmt->stmts;
	Symbol* symbol = current_table->table[node->name.lexem];
	BlockSymbolTable* scope = current_table->children[symbol->index];
	parent_scope = scope;
	//analyze stmts.
	for (auto it = stmts.cbegin(); it != stmts.cend(); ++it)
	{
		analyze_statement(*it, scope);
	}
}

void Analyzer::analyze_statement(std::shared_ptr<Statement> node, BlockSymbolTable* parent_scope)
{
	if (!node) return;
	switch (node->node_type)
	{
	case EXP_STMT:
		
		break;
	}
}

void Analyzer::analyze_exp_stmt(std::shared_ptr<ExpStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<Expression> expression = node->expression;
	if (expression->node_type == BINARY_EXP) {
		auto exp = dynamic_pointer_cast<BinaryExpression>(expression);
		analyze_binary_exp(exp, scope);
	}
}

void Analyzer::analyze_binary_exp(std::shared_ptr<BinaryExpression> node, BlockSymbolTable* scope)
{
	shared_ptr<Expression> left = node->left;
	shared_ptr<Expression> right = node->right;
	Token op = node->op;
	if (op.type == TK_ADD || op.type == TK_MINUS || op.type == TK_DIVID
		|| op.type == TK_MULTIPLY || op.type == TK_MOD || op.type == TK_LE || op.type == TK_LEQ
		|| op.type == TK_GT || op.type == TK_GEQ) {
		if (!isEligibleForArithmetic(left, scope) ||
			!isEligibleForArithmetic(right, scope)) {
			Error::semantical_operator_incompitable(op.lexem);
			return;
		}
	}
	else if (op.type == TK_ASSIGN) {
		if (!canBeAssign(left, right, scope)) {
			Error::semantical_operator_incompitable(op.lexem);
			return;
		}
	}
}

/*Only OBJ_ID can be on the left hand side.*/
bool Analyzer::isValidLeftVal(std::shared_ptr<Expression> left) 
{
	if (left->node_type == LITERAL_EXP)
	{
		auto node = std::dynamic_pointer_cast<LiteralExpression>(left);
		Token token = node->token;
		if (token.type == TK_STR_CONST ||
			token.type == TK_INT_CONST || token.lexem == "true"
			|| token.lexem == "false") {
			return false;
		}
	}
	else {
		return false;
	}
	return true;
}

bool Analyzer::validateMethodInvocation(shared_ptr<MethodInvocationExpression> node, BlockSymbolTable* scope)
{
	Symbol* symbol = scope->lookupSymbolByName(node->name.lexem);
	if (!symbol) return false;
	vector<shared_ptr<Expression>> arguments = node->arguments;
	//find out method definition
	auto methods = curClassNode->methods;
	shared_ptr<MethodDefinition> m;
	for (auto it = methods.cbegin(); it != methods.cend(); ++it)
	{
		if ((*it)->name.lexem == node->name.lexem) {
			m = *it;
			break;
		}
	}
	vector<shared_ptr<Formal>> arg_define = m->arguments;
	if (arguments.size() != arg_define.size()) {
		Error::semantical_method_arguments_number(node->name.lexem, arg_define.size(), arguments.size());
		return false;
	}
	for (int i = 0; i < arg_define.size(); i++)
	{
		shared_ptr<Formal> define = arg_define[i];
		shared_ptr<Expression> given = arguments[i];
		//type should matched

	}
}

bool Analyzer::canBeAssign(shared_ptr<Expression> left, shared_ptr<Expression> right, BlockSymbolTable* scope)
{
	if (!isValidLeftVal(left)) {
		return false;
	}
	auto lnode = dynamic_pointer_cast<LiteralExpression>(left);
	string lid = lnode->token.lexem;
	//looking for type of left val.
	Symbol* left_symbol = scope->lookupSymbolByName(lnode->token.lexem);
	if (!left_symbol) {
		Error::semantical_undefined_var_error(lnode->token.lexem);
		return false;
	}
	//now let's check the right value.
	if (right->node_type == CREATOR_EXP) {
		auto node = dynamic_pointer_cast<ClassCreatorExpression>(right);
		string classname = node->name.lexem;
		if (left_symbol->type != classname) {
			return false;
		}
	}
	else if (right->node_type == METHOD_INVOC_EXP) {
		auto node = dynamic_pointer_cast<MethodInvocationExpression>(right);
		Symbol* symbol = scope->lookupSymbolByName(node->name.lexem);
		if (!symbol) {
			Error::semantical_method_undefined_error(node->name.lexem);
			return false;
		}
		if (left_symbol->type != symbol->type) {
			return false;
		}
	}
	return true;
}

bool Analyzer::isEligibleForArithmetic(std::shared_ptr<Expression> node, BlockSymbolTable* scope)
{
	if (node->node_type == LITERAL_EXP) {
		auto exp = dynamic_pointer_cast<LiteralExpression>(node);
		Token token = exp->token;
		if (token.type == TK_INT_CONST) {
			return true;
		}
		else if (token.type == TK_OBJ_ID) {
			//lookup table  TODO
			Symbol* symbol = scope->table[token.lexem];
			if (symbol->type == "Int") {
				return true;
			}
		}
	}
	else if (node->node_type == METHOD_INVOC_EXP)
	{
		auto exp = dynamic_pointer_cast<MethodInvocationExpression>(node);
		string name = exp->name.lexem;
		//lookup symbol table for method with this name and then check its return type.
		if (!current_table->isVariableDeclared(name)) {
			Error::semantical_method_undefined_error(name);
		}
		Symbol* symbol = current_table->table[name];
		if (symbol->type == "Int") {
			return true;
		}
	}
	return false;
}