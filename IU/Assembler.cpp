#include "stdafx.h"
#include "Assembler.h"
#include "ClassFormat.h"
#include <vector>
using namespace std;

void Assembler::prepare()
{
	field_descriptors["Int"] = "I";
	field_descriptors["Bool"] = "Z";
	field_descriptors["Char"] = "C";
}

void Assembler::startGen(string dir)
{
	map<string, BlockSymbolTable*> classmap = analyzer.table->classMap;
	for (auto it = classmap.cbegin(); it != classmap.cend(); it++)
	{
		string path = dir + it->first + ".class";
		current_ast = analyzer.classNodes[it->first];
		if (!current_ast) continue;
		out.open(path, ios::binary);
		genHead();
		genConstantPool(it->second);
		
		out.close();
	}
}

void Assembler::genHead()
{
	uint32_t magic = 0xCAFEBABE;
	writeInt32(magic);
	unsigned __int16 version = 0x0000;
	writeInt16(version); // write minor version
	version = 0x0033;
	writeInt16(version);
}

//extrac constant from expression
void Assembler::extractConstantFromExpression(const std::shared_ptr<Expression> exp, std::vector<cp_info*>& pool)
{
	if (!exp) return;
	switch (exp->node_type)
	{
	case LITERAL_EXP:
		literalExpConstant(dynamic_pointer_cast<LiteralExpression>(exp), pool);
		break;
	case CREATOR_EXP:
		classCreatorConstant(dynamic_pointer_cast<ClassCreatorExpression>(exp), pool);
		break;
	case BINARY_EXP:
		binaryExpConstant(dynamic_pointer_cast<BinaryExpression>(exp), pool);
		break;
	case PRAN_EXP:
		pranExpConstant(dynamic_pointer_cast<PranExpression>(exp), pool);
		break;
	case METHOD_INVOC_EXP:
		methodInvocConstant(dynamic_pointer_cast<MethodInvocationExpression>(exp), pool);
		break;
	case VAR_DECL_EXP:
		declarationConstant(dynamic_pointer_cast<VariableDeclareExpression>(exp), pool);
		break;
	}
}

void Assembler::methodInvocConstant(const std::shared_ptr<MethodInvocationExpression> node, std::vector<cp_info*>& pool)
{
	vector<shared_ptr<Expression>> arguments = node->arguments;
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++)
	{
		extractConstantFromExpression(*it, pool);
	}
}

void Assembler::declarationConstant(const std::shared_ptr<VariableDeclareExpression> node, std::vector<cp_info*>& pool)
{
	genUTF8Constant("L" + node->type.lexem, pool);
}

void Assembler::literalExpConstant(const shared_ptr<LiteralExpression> node, vector<cp_info*>& pool)
{
	if (node->token.type == TK_STR_CONST) 
	{
		genUTF8Constant(node->token.lexem, pool);
	}
}

void Assembler::classCreatorConstant(const std::shared_ptr<ClassCreatorExpression> node, std::vector<cp_info*>& pool)
{
	string type = node->name.lexem;

	vector<shared_ptr<Expression>> arguments = node->arguments;
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++) {
		extractConstantFromExpression(*it, pool);
	}
}

void Assembler::binaryExpConstant(const std::shared_ptr<BinaryExpression> node, std::vector<cp_info*>& pool)
{
	//left value should't refers to constant value.
	extractConstantFromExpression(node->right, pool);
}

void Assembler::pranExpConstant(const shared_ptr<PranExpression> node, vector<cp_info*>& pool)
{
	extractConstantFromExpression(node->exp, pool);
}

void Assembler::extractConstantFromField(const std::shared_ptr<Formal> field,
	int class_index, vector<cp_info*>& pool)
{
	string type = field->type.lexem;
	type = "L" + type;
	int type_index = genUTF8Constant(type, pool);
	int id_index = genUTF8Constant(field->id.lexem, pool);
	int nameAndType = genNameAndType(id_index, type_index, pool);
	//field->val; field value wiil be evaluated during class initialization.
	Fieldref_info* info = new Fieldref_info(0x09);
	info->class_index = class_index;
	info->name_and_type_index = nameAndType;
	pool.push_back(info);
	Field_Method_info* field_info = new Field_Method_info(0x0001, id_index, type_index);
	field_info_v.push_back(field_info);
}

void Assembler::extractConstantFromStatement(shared_ptr<Statement> stmt, vector<cp_info*>& pool)
{
	if (!stmt) return;
	switch (stmt->node_type)
	{
	case BLOCK_STMT:
		extractConstantFromBlockStatement(dynamic_pointer_cast<BlockStatement>(stmt), pool);
		break;
	case EXP_STMT:
		extractConstantFromExpStatement(dynamic_pointer_cast<ExpStatement>(stmt), pool);
		break;
	}
}

void Assembler::extractConstantFromExpStatement(shared_ptr<ExpStatement> stmt, vector<cp_info*>& pool)
{
	shared_ptr<Expression> exp = stmt->expression;
	extractConstantFromExpression(exp, pool);
}

void Assembler::extractConstantFromBlockStatement(shared_ptr<BlockStatement> stmt, vector<cp_info*>& pool)
{
	auto stmts = stmt->stmts;
	for (auto it = stmts.cbegin(); it != stmts.cend(); it++) {
		extractConstantFromStatement(*it, pool);
	}
}

void Assembler::extractConstantFromMethod(const shared_ptr<MethodDefinition> method, vector<cp_info*>& pool)
{
	//method name;
	int id_index = genUTF8Constant(method->name.lexem, pool);
	string returntype = method->returntype.lexem == "void" ? "V" : "L" + method->returntype.lexem;
	int type_index = genUTF8Constant(returntype, pool);
	vector<shared_ptr<Formal>> arguments = method->arguments;
	string method_descriptor = "(";
	for (int i = 0; i < (int)arguments.size(); i++) {
		shared_ptr<Formal> formal = arguments[i];
		method_descriptor += "L";
		method_descriptor += formal->type.lexem;
	}
	method_descriptor += ";)";
	method_descriptor += returntype;
	int descriptor_index = genUTF8Constant(method_descriptor, pool);
	int name_type_index = genNameAndType(id_index, descriptor_index, pool);
	int methodref_index = genMethodRef(cur_class_index, name_type_index);
	methodref_index_map[method->name.lexem] = methodref_index;

	shared_ptr<BlockStatement> block = method->block;
	extractConstantFromBlockStatement(block, pool);
	Field_Method_info* method_info = new Field_Method_info(0x0001, id_index, descriptor_index);
	method_info_v.push_back(method_info);
}

void Assembler::genConstantPool(BlockSymbolTable* symboltable)
{
	string classname = current_ast->classname.lexem;
	cur_class_index = genClassInfo(classname, constant_pool);
	int obj_index = genClassInfo("iu/lang/Object", constant_pool);
	vector<cp_info*> &b = constant_pool;
	//class inherits from Object. And call Object init method while creating a class.
	genMethodRef("iu/lang/Object", "<init>","()V", b);
	int init_index = genMethodRef(classname, "<init>", "()v", b);
	methodref_index_map["<init>"] = init_index;
	vector<shared_ptr<Formal>> fields = current_ast->fields;
	for (int i = 0; i < fields.size(); i++) {
		extractConstantFromField(fields[i], cur_class_index, b);
	}
	vector<shared_ptr<MethodDefinition>> methods = current_ast->methods;
	for (int i = 0; i < methods.size(); i++) {
		extractConstantFromMethod(methods[i], b);
	}
	writeConstantPool(b);
	writeInt16(0x0001); //access_flag
	writeInt16(cur_class_index); //this class
	writeInt16(obj_index); //parent class
	writeInt16(0x0000);//interface count
	writeFieldOrMethods(field_info_v);
	writeFieldOrMethods(method_info_v);
	writeInt16(0x0000);
}

int Assembler::genClassInfo(std::string classname, vector<cp_info*>& pool)
{
	int found = lookupConstantTable(0x07, classname);
	if (found != -1) return found;
	CONSTANT_Class_info* info = new CONSTANT_Class_info(0x07);
	int name_index = genUTF8Constant(classname, pool);
	info->name_index = name_index;
	pool.push_back(info);
	return (int)pool.size() - 1;
}

int Assembler::genNameAndType(int nameindex, int typeindex, std::vector<cp_info*>& pool)
{
	int found = lookupNameTypeFromConstantPool(nameindex, typeindex, pool);
	if (found != -1) return found;
	CONSTANT_NameAndType_info* info = new CONSTANT_NameAndType_info(0x0c);
	info->name_index = nameindex;
	info->descriptor_index = typeindex;
	pool.push_back(info);
	return (int)pool.size() - 1;
}

int Assembler::genNameAndType(std::string name, std::string type, std::vector<cp_info*>& pool)
{
	int nameindex = genUTF8Constant(name, pool);
	int typeindex = genUTF8Constant(type, pool);
	return genNameAndType(nameindex, typeindex, pool);	
}


int Assembler::genUTF8Constant(std::string content, vector<cp_info*>& pool) 
{
	int found = lookupUTF8FromConstantPool(content);
	if (found != -1) return found;
	CONSTANT_Utf8_info* info = new CONSTANT_Utf8_info(0x01);
	int length = (int)content.length();
	info->length = length;
	info->byte = new unsigned char[length + 1];
	for (int i = 0; i < length; i++) {
		info->byte[i] = content[i];
	}
	info->byte[length] = '\0';
	pool.push_back(info);
	return (int)pool.size() - 1;
}

/*method reference contains class info and method's name and its descriptor*/
int Assembler::genMethodRef(string classname, string id, string type, vector<cp_info*>& pool)
{
	int classindex = genClassInfo(classname, pool);
	int idindex = genUTF8Constant(id, pool);
	int typeindex = genUTF8Constant(type, pool);
	int nameAndType = genNameAndType(idindex, typeindex, pool);
	return genMethodRef(idindex, nameAndType);
}

int Assembler::genMethodRef(int classindex, int nameAndType)
{
	Methodref_info* info = new Methodref_info(0x0a);
	info->class_index = classindex;
	info->name_and_type_index = nameAndType;
	constant_pool.push_back(info);
	return (int)constant_pool.size() - 1;
}

int Assembler::lookupNameTypeFromConstantPool(__int16 name, __int16 type, std::vector<cp_info*>& pool)
{
	int index = -1;
	for (int i = 0; i < pool.size(); i++) {
		cp_info* cur = pool[i];
		if (cur->tag != 0xb0) break;
		CONSTANT_NameAndType_info* info = dynamic_cast<CONSTANT_NameAndType_info*>(cur);
		if (info->name_index != name || info->descriptor_index != type)
			continue;
		index = i;
		break;
	}
	return index;
}

int Assembler::lookupUTF8FromConstantPool(std::string target)
{
	int index = -1;
	for (int i = 0; i < constant_pool.size(); i++)
	{
		bool found = true;
		cp_info* info = constant_pool[i];
		if (info->tag != 0x01) continue;
		CONSTANT_Utf8_info* utf8 = dynamic_cast<CONSTANT_Utf8_info*>(info);
		found = isEqual(utf8->length, utf8->byte, target);
		if (found) return i;
	}
	return index;
}

int Assembler::lookupClassFromConstantPool(std::string target)
{
	for (int i = 0; i < constant_pool.size(); i++)
	{
		cp_info* info = constant_pool[i];
		if (info->tag != 0x07) continue;
		CONSTANT_Class_info* classinfo = dynamic_cast<CONSTANT_Class_info*>(info);
		CONSTANT_Utf8_info* utf8 = dynamic_cast<CONSTANT_Utf8_info*>(constant_pool[classinfo->name_index]);
		bool found = isEqual(utf8->length, utf8->byte, target);
		if (found) return i;
	}
	return -1;
}

int Assembler::lookupConstantTable(unsigned __int8 tag, std::string target)
{
	int index = -1;
	switch (tag)
	{
	case 0x07:
		index = lookupClassFromConstantPool(target);
		break;
	case 0x01:
		index = lookupUTF8FromConstantPool(target);
		break;
	}
	return index;
}
int Assembler::lookupField(string field_name)
{
	for (auto it = field_info_v.cbegin(); it != field_info_v.cend(); it++) {
		cp_info* info = constant_pool[(*it)->name_index];
		auto utf8_info = dynamic_cast<CONSTANT_Utf8_info*>(info);
		bool found = isEqual(utf8_info->length, utf8_info->byte, field_name);
		if (found) return (*it)->name_index;
	}
	return -1;
}

bool Assembler::isEqual(int size, unsigned char* bytes, string s)
{
	if (size != s.length()) return false;
	for (int j = 0; j < size; j++) {
		if (bytes[j] != s.at(j)) {
			return false;
		}
	}
	return true;
}

void Assembler::writeConstantPool(std::vector<cp_info*>& pool)
{
	writeInt16((unsigned short)pool.size());
	for (auto it = pool.cbegin(); it != pool.cend(); it++) {
		cp_info* cur = *it;
		unsigned char tag = cur->tag;
		out.write((char*)& tag, 1);
		switch (tag)
		{
		case 0x07: 
		{
			CONSTANT_Class_info* class_info = dynamic_cast<CONSTANT_Class_info*>(cur);
			writeInt16(class_info->name_index);
		}
			break;
		case 0x08:
		{
			CONSTANT_String_info* str_info = dynamic_cast<CONSTANT_String_info*>(cur);
			writeInt16(str_info->string_index);
		}
			break;
		case 0x01:
		{
			CONSTANT_Utf8_info* utf8_info = dynamic_cast<CONSTANT_Utf8_info*>(cur);
			writeInt16(utf8_info->length);
			for (int i = 0; i < utf8_info->length; i++) {
				out.write((char*)&utf8_info->byte[i], sizeof(char));
			}
		}
			break;
		case 0x0c:
		{
			CONSTANT_NameAndType_info* info = dynamic_cast<CONSTANT_NameAndType_info*>(cur);
			writeInt16(info->name_index);
			writeInt16(info->descriptor_index);
		}
		break;
		case 0x09:
		{
			Fieldref_info* info = dynamic_cast<Fieldref_info*>(cur);
			writeInt16(info->class_index);
			writeInt16(info->name_and_type_index);
		}
		break;
		case 0x0a:
		{
			Methodref_info* info = dynamic_cast<Methodref_info*>(cur);
			writeInt16(info->class_index);
			writeInt16(info->name_and_type_index);
		}
		break;
		case 0x0b:
		break;
		case 0x03:
		case 0x04:
		{
			CONSTANT_Integer_info* info = dynamic_cast<CONSTANT_Integer_info*>(cur);
			writeInt32(info->bytes);
		}
			break;
		case 0x12:
		{
			CONSTANT_InvokeDynamic_info* info = dynamic_cast<CONSTANT_InvokeDynamic_info*>(cur);
			writeInt16(info->bootstrap_method_attr_index);
			writeInt16(info->name_and_type_index);
			break;
		}
		}
	}
}

void Assembler::writeFieldOrMethods(std::vector<Field_Method_info*>& collections)
{
	writeInt16((unsigned short)collections.size());
	for (auto it = collections.cbegin(); it != collections.cend(); it++)
	{
		Field_Method_info* info = *it;
		writeInt16(info->access_flag);
		writeInt16(info->name_index);
		writeInt16(info->descriptor_index);
		writeInt16(info->attribute_count);
	}
}



//Below are visitors for Expression
void CodeGenVisitor::visit(shared_ptr<Expression> node)
{
	if (!node) return;
	switch (node->node_type)
	{
	case CREATOR_EXP:
		visit(dynamic_pointer_cast<ClassCreatorExpression>(node));
		break;
	case METHOD_INVOC_EXP:
		visit(dynamic_pointer_cast<MethodInvocationExpression>(node));
		break;
	case LITERAL_EXP:
		visit(dynamic_pointer_cast<LiteralExpression>(node));
		break;
	case PRAN_EXP:
		visit(dynamic_pointer_cast<PranExpression>(node));
		break;
	case BINARY_EXP:
		visit(dynamic_pointer_cast<BinaryExpression>(node));
		break;
	}
}

void CodeGenVisitor::visit(std::shared_ptr<VariableDeclareExpression> node)
{
	//variable_table[node->id.lexem] = max_variable;
	if (max_variable > 3) {
		//astore max_variable
	}
	else {
		//astore_max_variable;
	}
	max_variable++;
}

void CodeGenVisitor::visit(shared_ptr<LiteralExpression>(node))
{	
	Token_Type type = node->token.type;
	if (node->token.type == TK_STR_CONST) {
		int str_index = assembler.lookupUTF8FromConstantPool(node->token.lexem);
		//ldc str_index;
	}
	else if (node->token.type == TK_INT_CONST) {
		string intstr = node->token.lexem;
		int val = 0;
		sscanf_s(intstr.c_str(), "%d", &val);
		if (val >= 0 && val < 6) {
			//iconst_n iconst_0 + val
		}
		else {
			//bipush val
		}
	}
	else if (node->token.lexem == "true") {
		//iconst_1
	}
	else if (node->token.lexem == "false") {
		//iconst_0
	}
}

void CodeGenVisitor::visit(shared_ptr<BinaryExpression> node)
{
	shared_ptr<Expression> left = node->left;
	shared_ptr<Expression> right = node->right;
	Token_Type optype = node->op.type;

	if (optype == TK_ASSIGN) {
		visit(right);
		int localindex = -1;
		if (left->node_type == LITERAL_EXP) {
			shared_ptr<LiteralExpression> leftexp = dynamic_pointer_cast<LiteralExpression>(left);
			if (leftexp->token.type == TK_OBJ_ID) {
				for (int i = 0; i < local_variable.size(); i++) {
					if (leftexp->token.lexem == local_variable[i]) {
						localindex = i;
						break;
					}
				}
				if (localindex == -1) {
					//error local variable not found.
				}
				else {
					int field_index = assembler.lookupField(leftexp->token.lexem);
					if (field_index == -1) {
						//error undefined variable.
						return;
					}
					//aload_0
					//getField field_index;
				}
				localindex++;
			}
		}
		else if (left->node_type == VAR_DECL_EXP) {
			shared_ptr<VariableDeclareExpression> declare = dynamic_pointer_cast<VariableDeclareExpression>(left);
			local_variable.push_back(declare->id.lexem);
			localindex = (int)local_variable.size();
		}
		//istore_localindex;
	}
	else {
		visit(left);
		visit(right);
		switch (optype)
		{
		case TK_ADD:
			break;
		case TK_MINUS:
			break;
		case TK_MOD:
			break;
		case TK_MULTIPLY:
			break;
		case TK_GT:
			break;
		case TK_GEQ:
			break;
		case TK_LE:
			break;
		case TK_LEQ:
			break;
		case TK_EQ:
			break;
		}
	}

}


//class creator visitor
void CodeGenVisitor::visit(shared_ptr<ClassCreatorExpression> node)
{
	//initialize parent class first
	//initialize the this class and initialize its field;

	int class_index = assembler.lookupConstantTable(0x07, node->name.lexem);
	//new class_index
	//dup
	//invokespecial index_of_method.
}

//method invocation visitor
void CodeGenVisitor::visit(shared_ptr<MethodInvocationExpression> node)
{
	//search for method name and its descriptor
	vector<shared_ptr<Expression>> arguments = node->arguments;
	string name = node->name.lexem;
	int name_index = assembler.lookupConstantTable(0x01, name);
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++) {
		visit(*it);
	}
	Methodref_info* info = dynamic_cast<Methodref_info*>(assembler.constant_pool[name_index]);
	CONSTANT_NameAndType_info* nameandtype = dynamic_cast<CONSTANT_NameAndType_info*>(
		assembler.constant_pool[info->name_and_type_index]);
	CONSTANT_Utf8_info* descriptor = dynamic_cast<CONSTANT_Utf8_info*>(
		assembler.constant_pool[nameandtype->descriptor_index]);
	int i = 0;
	for (; i < descriptor->length; i++) {
		if (descriptor->byte[i] == ')') {
			break;
		}
	}

	//aload_0
	for (auto it = arguments.cbegin(); it != arguments.cend(); it++)
	{
		visit(*it);
	}
	//invokevirtual name_index
	
	char returntype = descriptor->byte[i + 1];
	switch (returntype) {
	case 'V':
		//return
		break;
	case 'I':
		//ireturn
		break;
	case 'L':
		//areturn
		break;
	}
	
	//return 

}

//below are visitors for Statement
void CodeGenVisitor::visit(shared_ptr<Statement> node)
{
	switch (node->node_type)
	{
	case IF_STMT:
		visit(dynamic_pointer_cast<IfStatement>(node));
		break;
	case WHILE_STMT:
		visit(dynamic_pointer_cast<WhileStatement>(node));
		break;
	case BLOCK_STMT:
		visit(dynamic_pointer_cast<BlockStatement>(node));
		break;
	}
}

void CodeGenVisitor::visit(std::shared_ptr<IfStatement> node)
{
	shared_ptr<Statement> statement = node->block;
	visit(statement);
	//goto byte_length;
	shared_ptr<Expression> condition = node->condition;

}

void CodeGenVisitor::visit(shared_ptr<WhileStatement> node)
{
	auto condition = node->condition;
	visit(condition);
	shared_ptr<BlockStatement> block = node->block;
	visit(block);
}

void CodeGenVisitor::visit(shared_ptr<BlockStatement> node)
{
	vector<shared_ptr<Statement>> stmts = node->stmts;
	for (auto it = stmts.cbegin(); it != stmts.cend(); it++) {
		visit(*it);
	}
}

//
void CodeGenVisitor::visit(std::shared_ptr<MethodDefinition> node)
{
	vector<shared_ptr<Formal>>arguments = node->arguments;
	local_variable.clear();
	for (shared_ptr<Formal> argument : arguments) {
		local_variable.push_back(argument->id.lexem);
		max_variable++;
	}
	shared_ptr<BlockStatement> statement = node->block;
	visit(statement);

}