#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ast.h"
#include "SymbolTable.h"
/*symbol table entry*/

class AnalyzerVisitor;

class Analyzer
{
public:
	friend AnalyzerVisitor;

	SymbolTable* table;

	Analyzer();
	
	void prepare();

	BlockSymbolTable* analyze(std::shared_ptr<ClassNode>);
	
private:
	BlockSymbolTable* current_table;

	BlockSymbolTable* parent_scope;

	std::shared_ptr<ClassNode> curClassNode;

	std::map<std::string, std::shared_ptr<ClassNode>> classNodes;

	bool isEligibleForArithmetic(std::shared_ptr<Expression> node, BlockSymbolTable* scope);

	bool isEligibleForComparsion(std::shared_ptr<Expression> node, BlockSymbolTable* scope);

	bool Analyzer::isBooleanValue(std::shared_ptr<Expression> node, BlockSymbolTable* scope);

	void analyze_method(std::shared_ptr<MethodDefinition> node);

	void analyze_statement(std::shared_ptr<Statement>, BlockSymbolTable* );

	void analyze_exp_stmt(std::shared_ptr<ExpStatement> node, BlockSymbolTable* scope);

	void Analyzer::analyze_if_stmt(std::shared_ptr<IfStatement> node, BlockSymbolTable* scope);

	void Analyzer::analyze_while_stmt(std::shared_ptr<WhileStatement> node, BlockSymbolTable* scope);
	
	void Analyzer::analyze_block_stmt(std::shared_ptr<BlockStatement> node, BlockSymbolTable* scope);

	void Analyzer::analyze_binary_exp(std::shared_ptr<BinaryExpression> node, BlockSymbolTable* scope);
	

	/* determine if right value can be assigned to left object */
	bool Analyzer::canBeAssign(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right, BlockSymbolTable* scope);

	bool Analyzer::isValidLeftVal(std::shared_ptr<Expression> left);

	bool Analyzer::validateMethodInvocation(std::shared_ptr<MethodInvocationExpression> node, BlockSymbolTable* scope);
};



class AnalyzerVisitor : public IVisitor {
private:
	Analyzer* analyzer;

	BlockSymbolTable* parent_scope;

public:
	AnalyzerVisitor(Analyzer* analyzer) : analyzer(analyzer) {}

	bool AnalyzerVisitor::iseligibleForArithmetic(std::shared_ptr<Expression> node);

	void visit(std::shared_ptr<ClassNode>);

	void visit(std::shared_ptr<Formal>);
	
	void visit(std::shared_ptr<MethodDefinition>);
	
	void visit(std::shared_ptr<Statement>);
	
	void visit(std::shared_ptr<IfStatement>);
	
	void visit(std::shared_ptr<WhileStatement>);
	
	void visit(std::shared_ptr<BlockStatement>);
	
	void visit(std::shared_ptr<ExpStatement>);
	
	void visit(std::shared_ptr<Expression>);
	
	void visit(std::shared_ptr<BinaryExpression>);
};

