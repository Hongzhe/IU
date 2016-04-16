#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ast.h"
#include "SymbolTable.h"


class Analyzer
{
public:

	SymbolTable* table;

	Analyzer();
	
	void prepare();

	BlockSymbolTable* analyze(std::shared_ptr<ClassNode>);
	
private:
	BlockSymbolTable* current_table;

	BlockSymbolTable* parent_scope;

	std::shared_ptr<ClassNode> curClassNode;

	std::map<std::string, std::shared_ptr<ClassNode>> classNodes;

	//deduce type from expression
	std::string deduceTypeFromExpression(std::shared_ptr<Expression> node, BlockSymbolTable* scope);
	
	void analyze_field(std::shared_ptr<Formal> node);

	void analyze_method(std::shared_ptr<MethodDefinition> node);

	void analyze_statement(std::shared_ptr<Statement>, BlockSymbolTable* );

	void analyze_exp_stmt(std::shared_ptr<ExpStatement> node, BlockSymbolTable* scope);

	void analyze_if_stmt(std::shared_ptr<IfStatement> node, BlockSymbolTable* scope);

	void analyze_while_stmt(std::shared_ptr<WhileStatement> node, BlockSymbolTable* scope);
	
	void analyze_block_stmt(std::shared_ptr<BlockStatement> node, BlockSymbolTable* scope);

	void analyze_binary_exp(std::shared_ptr<BinaryExpression> node, BlockSymbolTable* scope);
	
	void analyze_expression(std::shared_ptr<Expression> node, BlockSymbolTable* scope);

	void analyzer_creator(std::shared_ptr<ClassCreatorExpression> node, BlockSymbolTable* scope);
	
	/* determine if right value can be assigned to left object */
	bool canBeAssign(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, BlockSymbolTable* scope);

	bool isValidLeftValForAssignment(std::shared_ptr<Expression> left);

	bool validateMethodInvocation(std::shared_ptr<MethodInvocationExpression> node, BlockSymbolTable* scope);
};


