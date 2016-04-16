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
	BlockSymbolTable* class_block = table->addClass(root); //add symbols inside this class into symbol table;
	if (!class_block) {
		return nullptr;
	}
	current_table = class_block;
	classNodes[root->classname.lexem] = root;
	curClassNode = root;
	if (root->parentname != "") { 
		if (!table->isTypeDefined(root->parentname)) {
			//throw exception
			Error::semantical_undefined_var_error(root->parentname);
			return nullptr;
		}
	}
	auto fields = root->fields;
	for (auto field_it = fields.cbegin(); field_it != fields.cend(); ++field_it)
	{
		analyze_fields(*field_it);
	}
	auto methods = root->methods;
	for (auto it = methods.cbegin(); it != methods.cend(); ++it) {
		analyze_method(*it);
	}
	return class_block;
}

void Analyzer::analyze_fields(std::shared_ptr<Formal> node)
{
	if (!table->isTypeDefined(node->type.lexem)) {
		Error::semantical_undefined_type(node->type.lexem, node->lineno);
		return;
	}
	analyze_expression(node->val, current_table);
	string val_type = deduceTypeFromExpression(node->val, current_table);
	if (node->type.lexem != val_type) {
		Error::semantical_assign_incompatible_type(node->type.lexem, val_type, node->lineno);
		return;
	}
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
		analyze_exp_stmt(dynamic_pointer_cast<ExpStatement>(node), parent_scope);
		break;
	case IF_STMT:
		analyze_if_stmt(dynamic_pointer_cast<IfStatement>(node), parent_scope);
		break;
	case WHILE_STMT:
		analyze_while_stmt(dynamic_pointer_cast<WhileStatement>(node), parent_scope);
		break;
	case BLOCK_STMT:
		analyze_block_stmt(dynamic_pointer_cast<BlockStatement>(node), parent_scope);
		break;
	}
}
void Analyzer::analyze_block_stmt(std::shared_ptr<BlockStatement> node, BlockSymbolTable* scope)
{
	auto stmts = node->stmts;
	for (auto it = stmts.cbegin(); it != stmts.cend(); ++it)
	{
		analyze_statement(*it,scope);
	}
}

void Analyzer::analyze_if_stmt(std::shared_ptr<IfStatement> node, BlockSymbolTable* scope)
{
	auto condition = node->condition;
	string ctype = deduceTypeFromExpression(node->condition, scope);
	if (ctype != "Bool") {
		Error::semantical_condition_exp(ctype, node->lineno);
		return;
	}
	analyze_block_stmt(node->block, scope);
	if (node->elsepart) {
		analyze_block_stmt(node->elsepart, scope);
	}
}

void Analyzer::analyze_while_stmt(std::shared_ptr<WhileStatement> node, BlockSymbolTable* scope)
{
	auto condition = node->condition;
	string ctype = deduceTypeFromExpression(node->condition, scope);
	if (ctype != "Bool") {
		Error::semantical_condition_exp(ctype, node->lineno);
		return;
	}
	analyze_block_stmt(node->block, scope);
}

void Analyzer::analyze_exp_stmt(std::shared_ptr<ExpStatement> node, BlockSymbolTable* scope)
{
	shared_ptr<Expression> expression = node->expression;
	switch (expression->node_type)
	{
	case BINARY_EXP:
		analyze_binary_exp(dynamic_pointer_cast<BinaryExpression>(expression), scope);
		break;
	case METHOD_INVOC_EXP:
		validateMethodInvocation(dynamic_pointer_cast<MethodInvocationExpression>(expression), scope);
		break;
	case CREATOR_EXP:
		break;
	case PRAN_EXP:
		
		break;
	}
}

void Analyzer::analyze_expression(std::shared_ptr<Expression> node, BlockSymbolTable* scope)
{
	switch (node->node_type)
	{
	case BINARY_EXP:
		analyze_binary_exp(dynamic_pointer_cast<BinaryExpression>(node), scope);
		break;
	case METHOD_INVOC_EXP:
		validateMethodInvocation(dynamic_pointer_cast<MethodInvocationExpression>(node), scope);
		break;
	case CREATOR_EXP:
		break;
	case PRAN_EXP:
	{
		auto exp = dynamic_pointer_cast<PranExpression>(node);
		analyze_expression(exp->exp, scope);
		break;
	}
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
		if (deduceTypeFromExpression(left, scope) != "Int" ||
			deduceTypeFromExpression(right, scope) != "Int") {
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
	else if (op.type == TK_EQ) {
		//TODO
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

void Analyzer::analyzer_creator(std::shared_ptr<ClassCreatorExpression> node, BlockSymbolTable* scope)
{
	if (!table->isTypeDefined(node->name.lexem)) {
		Error::semantical_undefined_type(node->name.lexem);
		return;
	}
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
		Error::semantical_method_arguments_number(node->name.lexem, arg_define.size(), arguments.size(), node->lineno);
		return false;
	}
	bool argumentsok = true;
	for (int i = 0; i < (int)arg_define.size(); i++)
	{
		shared_ptr<Formal> define = arg_define[i];
		shared_ptr<Expression> given = arguments[i];
		//type should matched
		string defined_type = define->type.lexem;
		string given_type = deduceTypeFromExpression(given, scope);
		if (defined_type != given_type) {
			argumentsok = false;
			Error::semantical_method_argument_incompatibal(m->name.lexem, defined_type, given_type, node->lineno);
		}
	}
	return argumentsok;
}

bool Analyzer::canBeAssign(shared_ptr<Expression> left, shared_ptr<Expression> right, BlockSymbolTable* scope)
{
	if (!isValidLeftVal(left)) {
		return false;
	}
	string ltype = deduceTypeFromExpression(left, scope);
	string rtype = deduceTypeFromExpression(right, scope);
	return ltype == rtype;
}

string Analyzer::deduceTypeFromExpression(std::shared_ptr<Expression> node, BlockSymbolTable* scope)
{
	if (node->node_type == LITERAL_EXP) {
		auto exp = dynamic_pointer_cast<LiteralExpression>(node);
		Token token = exp->token;
		if (token.type == TK_INT_CONST) {
			return "Int";
		}
		else if (token.type == TK_STR_CONST) {
			return "String";
		}
		else if (token.lexem == "true" || token.lexem == "false") {
			return "Bool";
		}
		else if (token.type == TK_OBJ_ID) {
			Symbol* symbol = scope->lookupSymbolByName(token.lexem);
			if (symbol) {
				return symbol->type;
			}
			Error::semantical_undefined_var_error(token.lexem);
			return ""; // symbol is null;
		}
		return "";
	}
	else if (node->node_type == METHOD_INVOC_EXP) {
		auto exp = dynamic_pointer_cast<MethodInvocationExpression>(node);
		string name = exp->name.lexem;
		//lookup symbol table for method with this name and then check its return type.
		if (!current_table->isVariableDeclared(name)) {
			Error::semantical_method_undefined_error(name);
			return "";
		}
		Symbol* symbol = scope->lookupSymbolByName(name);
		if (symbol) {
			return symbol->type;
		}
		Error::semantical_undefined_var_error(name);
		return "";
	}
	else if (node->node_type == CREATOR_EXP) {
		auto exp = dynamic_pointer_cast<ClassCreatorExpression>(node);
		return exp->name.lexem;
	}
	else if (node->node_type == PRAN_EXP) {
		auto exp = dynamic_pointer_cast<PranExpression>(node);
		return deduceTypeFromExpression(exp->exp, scope);
	}
	else if (node->node_type == BINARY_EXP) {
		auto exp = dynamic_pointer_cast<BinaryExpression>(node);
		string lefttype = deduceTypeFromExpression(exp->left, scope);
		string righttype = deduceTypeFromExpression(exp->right, scope);
		Token op = exp->op;
		if (op.type == TK_ADD || op.type == TK_MINUS || op.type == TK_DIVID
			|| op.type == TK_MULTIPLY || op.type == TK_MOD) {
			if (lefttype == "Int" && righttype == "Int") 
				return "Int";
			Error::semantical_operator_incompitable(op.lexem);
			return "";
		}
		else if (op.type == TK_LE || op.type == TK_LEQ
			|| op.type == TK_GT || op.type == TK_GEQ || op.type == TK_EQ) {
			if (lefttype == "Int" && righttype == "Int") 
				return "Bool";
			Error::semantical_operator_incompitable(op.lexem);
			return "";
		}
		else if (op.type == TK_ASSIGN) {
			return "";
		}
		return "";
	}
}