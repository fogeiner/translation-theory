#ifndef PARSER_H
#define PARSER_H

#include <cassert>
#include <string>
#include <vector>

#include "Tokenizer.h"
#include "Logger.h"

#include <map>
#include <list>
#include <algorithm>

#include <iostream>

#define ASSERT_TYPE(X,Y) assert(dynamic_cast<X>(Y) != NULL)

class ParserException : public std::exception {
	private:
		std::string _msg;
	public:

		ParserException(std::string msg) : _msg(msg) {
		}

		~ParserException() throw () {
		}

		const char *what() throw () {
			return _msg.c_str();
		}
};

class Function {
	private:
		std::string _type;
		std::string _name;
		std::map<std::string, std::string> _types;
		std::map<std::string, int> _offsets;

		int _max_parameters_offset;
		int _max_local_variable_offset;

		bool isVariableUnique(std::string id) {
			TRACE;
			return (_offsets.find(id) == _offsets.end());
		}
	public:
		Function(std::string type, std::string name):
			_type(type),
			_name(name),
			// 4 bytes for return address
			// 4 bytes for the saved ebp
			_max_parameters_offset(8),
			_max_local_variable_offset(-4)
	{TRACE;}

		void addParameter(std::string type, std::string id) {
			TRACE;
			if (!isVariableUnique(id)) {
				throw ParserException(fmt("Variable is already defined: %s", id.c_str()));
			}
			_types.insert(std::make_pair<std::string, std::string> (id, type));
			// set offset!
			_offsets.insert(std::make_pair<std::string, int>(id, _max_parameters_offset));
			_max_parameters_offset += 4;
		}

		void addLocalVariable(std::string type, std::string id) {
			TRACE;

			if (!isVariableUnique(id)) {
				throw ParserException(fmt("Variable is already defined: %s", id.c_str()));
			}
			_types.insert(std::make_pair<std::string, std::string> (id, type));
			// set offset!
			_offsets.insert(std::make_pair<std::string, int>(id, _max_local_variable_offset));
			_max_local_variable_offset -= 4;
		}

		int getVariableOffset(std::string id) {
			TRACE;

			if (_types.find(id) == _types.end() &&
					_offsets.find(id) == _offsets.end()) {
				throw ParserException(
						fmt("Variable %s is not defined",
							id.c_str()));
			}
			assert(_types.find(id) != _types.end());
			assert(_offsets.find(id) != _offsets.end());

			return _offsets[id];
		}

		std::string getType() const {
			TRACE;
			return _type;
		}

		std::string getName() const {
			TRACE;
			return _name;
		}

		int getInputParametersCount() {
			TRACE;

			int count = 0;
			for (std::map<std::string, int>::iterator it = _offsets.begin();
					it != _offsets.end(); ++it) {
				std::string id = it->first;
				int offset = it->second;
				if (offset > 0) {
					count = count + 1;
				}
			}
			return count;
		}

		bool isEqualInterface(Function *other) {
			TRACE;
			// for now it's enough that there are the
			// same amount of parameters passed in
			int parametersCount = getInputParametersCount();
			int otherParametersCount = other->getInputParametersCount();
			DEBUG(fmt("%d vs %d", parametersCount, otherParametersCount));
			return parametersCount == otherParametersCount;
		}

		std::string getEndLabel() const {
			return fmt(".%send", _name.c_str());
		}
};

/**
 * Static program class saves all the declared functions (as well as declarations)
 * Functions should be allocated on the heap and memory should not be deleted from outside
 * I.e. this class is very tightly coupled with the Function class.
 * WARNING: after adding function or declaration getFunction!
 */
class Program {
	private:
		static std::map<std::string, Function *> _functions;
		// true if the function is declaration; false otherwise
		static std::map<std::string, bool> _declarationMask;
	public:
		static void addDeclaration(std::string id, Function *function) {
			TRACE;
			if (_declarationMask.find(id) != _declarationMask.end()) {

				Function *saved = _functions[id];
				if (saved->isEqualInterface(function) == false) {
					throw ParserException(fmt("Function %s is already declared with the other interface", id.c_str()));
				}

				bool savedDeclaration = _declarationMask[id];

				// declaration should not replace function
				if (savedDeclaration == true) {
					delete saved;
					_functions[id] = function;
					_declarationMask[id] = true;
				} else {
					delete function;
				}

			} else {
				_functions[id] = function;
				_declarationMask[id] = true;
			}
		}

		static void addFunction(std::string id, Function *function) {
			TRACE;
			// if the function is already declared -- ok;
			// should check that the amount of arguments is the same
			// and that their types are equal
			if (_declarationMask.find(id) != _declarationMask.end()) {
				Function *saved = _functions[id];

				if (saved->isEqualInterface(function) == false) {
					throw ParserException(fmt("Function %s is already declared with the other interface", id.c_str()));
				}
				bool savedDeclaration = _declarationMask[id];
				if (savedDeclaration) {
					// replace declaration with the real function
					delete saved;
					_functions[id] = function;
					_declarationMask[id] = false;
				} else {
					// can have function defined multiple times
					throw ParserException(fmt("Function %s is already defined", id.c_str()));
				}
			} else {
				_functions[id] = function;
				_declarationMask[id] = false;
			}
		}

		static Function *getFunction(std::string id) {
			TRACE;

			if (_functions.find(id) != _functions.end()) {
				return _functions[id];
			} else {
				return NULL;
			}
		}
};

class Node;
std::string buildXMLTree(Node *root, int level = 0);


#define PARSER_EXPECTED(expected) \
	ParserException(\
			fmt("Failed on %d:%d: expected %s", \
				_tokenizer->getLineNumber(), \
				_tokenizer->getLinePosition(), \
				_tokenizer->getTokenDescription(expected).c_str()))

#define PARSER_ILLEGAL \
	ParserException(\
			fmt("Failed on %d:%d: illegal token %s", \
				_tokenizer->getLineNumber(), \
				_tokenizer->getLinePosition(), \
				_tokenizer->getTokenDescription(_tokenizer->getToken()).c_str()))

class Node {
	private:
		std::string _tag;
		std::vector<Node*> _children;

		virtual std::string _getDefaultXMLTag() const = 0;
	public:
		typedef std::vector<Node *>::iterator node_iterator;

		Node(std::string tag = "") :
			_tag(tag) {
			}

		std::string getTag() const {
			return _tag;
		}

		void addChild(Node *node) {
			_children.push_back(node);
		}

		node_iterator begin() {
			return _children.begin();
		}

		node_iterator end() {
			return _children.end();
		}

		int childrenCount() const {
			return _children.size();
		}

		Node *get(int index) const {
			return _children[index];
		}

		void clear() {
			_children.clear();
		}

		virtual ~Node() {
			for (Node::node_iterator it = this->begin();
					it != this->end(); ++it) {
				Node *node = *it;
				delete node;
			}
		}

		virtual std::string XMLStart() const {
			std::string XMLTag = _getDefaultXMLTag(),
				tag = getTag(),
				xml;

			if (tag.length() == 0) {
				xml = "<" + XMLTag + ">";
			} else {
				xml = "<" + XMLTag + ">" + tag + "</" + XMLTag + ">";
			}

			return xml;
		}

		virtual std::string XMLEnd() const {
			std::string XMLTag = _getDefaultXMLTag(),
				tag = getTag(),
				xml;

			if (tag.length() == 0) {
				xml = "</" + XMLTag + ">";
			} else {
				xml = "";
			}

			return xml;
		}

		virtual std::string generate(Function *context) {
			return "<" + _getDefaultXMLTag() + ">\n"; 
		}
};


class TypeNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "type";
		}
	public:
		TypeNode(std::string tag): Node(tag) {}
};


class IdNode: public Node {
	public:
		IdNode(std::string id):
			Node(id) {}
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "id";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert(childrenCount() == 0);

			std::string id = getTag();
			int offset = context->getVariableOffset(id);
			std::string code;
			code += fmt(
					"# id %s\n"
					"    pushl %d(%%ebp)\n",
					id.c_str(), offset);
			return code;
		}

};

class FuncargsNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "funcargs";
		}
};

class FuncargNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "funarg";
		}
};


class StatementsNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "statements";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			std::string code;
			assert(context != NULL);
			for (int i = 0; i < childrenCount(); ++i) {
				code += get(i)->generate(context);
			}
			return code;
		}
};

class FuncdefNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "funcdef";
		}

	public:
		virtual std::string generate(Function *context_outer) {
			TRACE;

			assert(context_outer == NULL);
			// 0 type_int -- return type (by now int only)
			ASSERT_TYPE(TypeNode*, get(0));
			std::string type = get(0)->getTag();
			assert("int" == type);
			// 1 id
			ASSERT_TYPE(IdNode*, get(1));
			std::string id = get(1)->getTag();

			Function *context = new Function(type, id);

			// 2 funcargs -> funcarg*
			ASSERT_TYPE(FuncargsNode*, get(2));
			for(int i = 0; i < get(2)->childrenCount(); ++i) {
				Node *child = get(2)->get(i);
				ASSERT_TYPE(FuncargNode*, child);
				ASSERT_TYPE(TypeNode*, child->get(0));
				ASSERT_TYPE(IdNode*, child->get(1));
				std::string type = child->get(0)->getTag();
				assert("int" == type);
				std::string id = child->get(1)->getTag();

				context->addParameter(type, id);
			}

			// 3 statements or none if it was a function declaration
			if (childrenCount() == 4) {
				ASSERT_TYPE(StatementsNode*, get(3));
				Program::addFunction(id, context);

				std::string code;
				code += fmt(
						".globl %s\n"
						"%s:\n"
						"    pushl %%ebp\n"
						"    movl %%esp, %%ebp\n",
						id.c_str(), id.c_str());

				code += get(3)->generate(context);

				code += fmt(
						"# epilogue\n"
						"%s:\n"
						"    movl %%ebp, %%esp\n"
						"    popl %%ebp\n"
						"    ret\n",
						context->getEndLabel().c_str());
				return code;
			} else {
				// declaration produces no code but saves meta-information
				Program::addDeclaration(id, context);
				return "";
			}
		}
};

class ProgramNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "program";
		}

		virtual std::string generate(Function *context) {
			TRACE;

			assert(context == NULL);

			std::string code;
			code += fmt(
					".READFORMAT:\n"
				    "    .string \"%%d\"\n"
					".PRINTFORMAT:\n"
					"    .string \"%%d\\n\"\n"
					);

			for(node_iterator it = this->begin();
					it != this->end(); ++it) {
				Node *child = *it;
				ASSERT_TYPE(FuncdefNode*, child);
				code += child->generate(NULL);
			}

			return code;
		}
};



class ReadNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "read";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);

			ASSERT_TYPE(IdNode*, get(0));
			std::string id = get(0)->getTag();

			int offset = context->getVariableOffset(id);

			std::string code;
			code += fmt(
					"# read %s\n"
					"    leal %d(%%ebp), %%eax\n"
					"    pushl %%eax\n"
					"    pushl $.READFORMAT\n"
					"    call scanf\n"
					"    addl $8, %%esp\n",
					id.c_str(), offset);

							
			return code;
		}
};


class AtomNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "atom";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert(childrenCount() == 1);

			std::string code;
			code += fmt(
					"# atom\n"
					);
			code += get(0)->generate(context);

			return code;
		}
};

class multNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "mult";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			
			std::string code;
			code += fmt(
					"# multNode\n"
					);
			if (childrenCount() == 1) {
				ASSERT_TYPE(AtomNode*, get(0));
				code += get(0)->generate(context);
			} else {
				ASSERT_TYPE(AtomNode*, get(0));
				code += get(0)->generate(context);
				// MultMultNode, ModMultNode, DivMultNode
				code += get(1)->generate(context);
			}

			
			return code;
		}
};


class termNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "term";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));

			std::string code;
			code += fmt(
					"# termNode\n"
					);
			if (childrenCount() == 1) {
				ASSERT_TYPE(multNode*, get(0));
				code += get(0)->generate(context);
			} else {
				ASSERT_TYPE(multNode*, get(0));
				code += get(0)->generate(context);
				// MultMultNode, ModMultNode, DivMultNode
				code += get(1)->generate(context);
			}
			
			return code;
		}
};

class ExpressionNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "expression";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));

			std::string code;
			code += fmt(
					"# expression\n"
					);
			if (childrenCount() == 1) {
				ASSERT_TYPE(termNode*, get(0));
				code += get(0)->generate(context);
			} else {
				ASSERT_TYPE(termNode*, get(0));
				code += get(0)->generate(context);
				// PlusTermNode or MinusTermNode
				code += get(1)->generate(context);
			} 

			return code;
		}
};

class ReturnNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "return";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert(childrenCount() == 1);
			ASSERT_TYPE(ExpressionNode*, get(0));

			std::string code;
			code += fmt(
					"# return\n"
					);
			code += get(0)->generate(context);
			// the result of the expression on the top of the stack
			// no need to move anything

			code += fmt(
					"    popl %%eax\n"
					"    jmp %s\n"
					"    push $.PRINTFORMAT\n"
					"    call printf\n"
					"    subl $8, %%esp\n",
					context->getEndLabel().c_str());

			return code;
		}
};

class PrintNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "print";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert(childrenCount() == 1);
			ASSERT_TYPE(ExpressionNode*, get(0));

			std::string code;
			code += fmt(
					"# print\n"
					);
			code += get(0)->generate(context);
			// the result of the expression on the top of the stack
			// no need to move anything
			code += fmt(
					"    push $.PRINTFORMAT\n"
					"    call printf\n"
					"    subl $8, %%esp\n"
					);

			return code;
		}
};

class NegationNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "negation";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1));
			ASSERT_TYPE(AtomNode*, get(0));

			std::string code;
			code += fmt(
					"# negation\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%eax\n"
					"    imull $-1, %%eax\n"
					"    pushl %%eax\n"
					);

			return code;
		}
};

class PlusTermNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "plusterm";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			ASSERT_TYPE(termNode*, get(0));

			std::string code;
			code += fmt(
					"# plusterm\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%eax\n"
					"    popl %%ecx\n"
					"    addl %%eax, %%ecx\n"
					"    pushl %%ecx\n"
					);

			if (childrenCount() == 2) {
				code += get(1)->generate(context);
			}

			return code;
		}
};

class MinusTermNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "minusterm";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			ASSERT_TYPE(termNode*, get(0));

			std::string code;
			code += fmt(
					"# minusterm\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%eax\n"
					"    popl %%ecx\n"
					"    subl %%eax, %%ecx\n"
					"    pushl %%ecx\n"
					);

			if (childrenCount() == 2) {
				code += get(1)->generate(context);
			}

			return code;
		}
};

class MultMultNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "multmult";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			ASSERT_TYPE(AtomNode*, get(0));

			std::string code;
			code += fmt(
					"# multmult\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%eax\n"
					"    popl %%ecx\n"
					"    imull %%eax, %%ecx\n"
					"    pushl %%ecx\n"
					);

			if (childrenCount() == 2) {
				code += get(1)->generate(context);
			}

			return code;
		}
};

class ModMultNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "modmult";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			ASSERT_TYPE(AtomNode*, get(0));

			std::string code;
			code += fmt(
					"# modmult\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%ecx\n"
					"    popl %%eax\n"
					"    movl %%eax, %%edx\n"
					"    sarl $31, %%edx\n"
					"    idivl %%ecx\n"
					"    pushl %%edx\n"
					);

			if (childrenCount() == 2) {
				code += get(1)->generate(context);
			}

			return code;
		}
};

class DivMultNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "divmult";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			assert((childrenCount() == 1) || (childrenCount() == 2));
			ASSERT_TYPE(AtomNode*, get(0));

			std::string code;
			code += fmt(
					"# divmult\n"
					);
			code += get(0)->generate(context);
			code += fmt(
					"    popl %%ecx\n"
					"    popl %%eax\n"
					"    movl %%eax, %%edx\n"
					"    sarl $31, %%edx\n"
					"    idivl %%ecx\n"
					"    pushl %%eax\n"
					);

			if (childrenCount() == 2) {
				code += get(1)->generate(context);
			}

			return code;
		}
};

class IntegerNode: public Node {
	public:
		IntegerNode(std::string integer): Node(integer) {}
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "integer";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);
			std::string code;
			code = fmt(
					"    pushl $%s\n",
					getTag().c_str()
					);

			return code;
		}
};

class AssignmentNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "assignment";
		}
	public:
		virtual std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);

			ASSERT_TYPE(IdNode*, get(0));
			ASSERT_TYPE(ExpressionNode*, get(1));

			std::string id = get(0)->getTag();
			int offset = context->getVariableOffset(id);

			std::string code;
			code += get(1)->generate(context);
			code += fmt(
					"# saving result of expression to %s\n"
					"    popl %%eax\n"
					"    movl %%eax, %d(%%ebp)\n",
					id.c_str(), offset);

			return code;
		}
};

class DeclarationNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "declaration";
		}
	public:
		std::string generate(Function *context) {
			TRACE;

			assert(context != NULL);

			ASSERT_TYPE(TypeNode*, get(0));
			ASSERT_TYPE(IdNode*, get(1));

			std::string type = get(0)->getTag();
			std::string id = get(1)->getTag();

			context->addLocalVariable(type, id);
			
			int offset = context->getVariableOffset(id);

			std::string code;
			code += fmt(
					"# declaration %s %s offset %d\n"
					"    subl $4, %%esp\n",
					type.c_str(), id.c_str(), offset);

			return code;
		}
};

class IfNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "if";
		}
};

class WhileNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "while";
		}
};

class FuncallNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "funcall";
		}
};

class FuncallargNode: public Node {
	private:
		std::string _getDefaultXMLTag() const {
			return "funcallarg";
		}
};

class BexpressionNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "bexpression";
		}
};

class BdisjNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "bdisj";
		}
};

class BDisjNode: public Node {
	private:;
		virtual std::string _getDefaultXMLTag() const {
			return "bDisj";
		}
};

class BconjNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "bconj";
		}
};

class BConjNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "bConj";
		}
};

class BAtomNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "batom";
		}
};

class CmpNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "--- auxiliary cmp node ---";
		}
};

class CmpLessNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmpless";
		}
};

class CmpGreaterNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmpgreater";
		}
};

class CmpLessOrEqualNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmplessorequal";
		}
};

class CmpGreaterOrEqualNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmpgreaterorequal";
		}
};

class CmpEqualNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmpequal";
		}
};

class CmpNotEqualNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "cmpnotequal";
		}
};

class TrueNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "true";
		}
};

class FalseNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "false";
		}
};

class NotNode: public Node {
	private:
		virtual std::string _getDefaultXMLTag() const {
			return "not";
		}
};


class Parser {
	private:
		Tokenizer *_tokenizer;
		Node *_root;

		bool match(Tokenizer::ValueType type) {
			if (_tokenizer->getToken() == type) {
				DEBUG(fmt("Matched %s", _tokenizer->getTag().c_str()));
				return true;
			}
			return false;
		}

	public:
		std::string getXMLTree() {
			return ::buildXMLTree(_root);
		}

		std::string generate() {
			return _root->generate(NULL);
		}

		Parser(Tokenizer *tokenizer):
			_tokenizer(tokenizer) {
				TRACE;
				assert(_tokenizer != NULL);

				_tokenizer->nextToken();
				parseProgram(&_root);

				if (!match(Tokenizer::T_EOF)) {
					throw PARSER_EXPECTED(Tokenizer::T_EOF);
				}
			}

		void parseProgram(Node **node) {
			TRACE;
			*node = new ProgramNode();
			parseFuncdefs(*node);
		}

		void parseFuncdefs(Node *node) {
			TRACE;
			if (match(Tokenizer::T_DEF)) {
				parseFuncdef(node);
				parseFuncdefs(node);
			}
			// eps is ok
		}

		void parseFuncdef(Node *node) {
			TRACE;
			if (match(Tokenizer::T_DEF)) {
				_tokenizer->nextToken();

				Node *node_funcdef = new FuncdefNode();
				parseType(node_funcdef);
				parseId(node_funcdef);

				Node *node_funcargs = new FuncargsNode();
				parseFunargs(node_funcargs);
				node_funcdef->addChild(node_funcargs);

				if (match(Tokenizer::T_ENDDEF)) {
					// with no body; just declaration
					_tokenizer->nextToken();
					node->addChild(node_funcdef);
				} else if (match(Tokenizer::T_COLON)) {
					// with body;
					_tokenizer->nextToken();

					Node *node_statements = new StatementsNode();
					parseStatements(node_statements);
					node_funcdef->addChild(node_statements);

					if (!match(Tokenizer::T_ENDDEF)) {
						throw PARSER_EXPECTED(Tokenizer::T_ENDDEF);
					}
					_tokenizer->nextToken();

					node->addChild(node_funcdef);
				} else {
					throw PARSER_ILLEGAL;
				}
			} else {
				PARSER_EXPECTED(Tokenizer::T_DEF);
			}
		}

		void parseType(Node *node) {
			TRACE;
			if (match(Tokenizer::T_TYPE_INT)) {
				node->addChild(new TypeNode("int"));
				_tokenizer->nextToken();
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseId(Node *node) {
			TRACE;
			if (match(Tokenizer::T_ID)) {
				node->addChild(new IdNode(_tokenizer->getTag()));
				_tokenizer->nextToken();
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_ID);
			}
		}

		void parseFunargs(Node *node) {
			TRACE;

			if (match(Tokenizer::T_TYPE_INT)) {
				Node *node_funcarg = new FuncargNode();
				parseType(node_funcarg);
				parseId(node_funcarg);
				node->addChild(node_funcarg);
				parseFunargsrest(node);
			}
			// eps
		}

		void parseFunargsrest(Node *node) {
			TRACE;

			if (match(Tokenizer::T_COMMA)) {
				_tokenizer->nextToken();
				Node *node_funcarg = new FuncargNode();
				parseType(node_funcarg);
				parseId(node_funcarg);
				node->addChild(node_funcarg);
				parseFunargsrest(node);
			}
			// eps
		}

		void parseStatements(Node *node) {
			TRACE;

			if (match(Tokenizer::T_WHILE)) {
				parseWhile(node);
				parseStatements(node);
			} else if (match(Tokenizer::T_IF)) {
				parseIf(node);
				parseStatements(node);
			} else if (match(Tokenizer::T_ID)) {
				parseAssignment(node);
				if (!match(Tokenizer::T_SEMICOLON)) {
					throw PARSER_EXPECTED(Tokenizer::T_SEMICOLON);
				}
				_tokenizer->nextToken();
				parseStatements(node);
			} else if (match(Tokenizer::T_TYPE_INT)) {
				parseDeclaration(node);
				if (!match(Tokenizer::T_SEMICOLON)) {
					throw PARSER_EXPECTED(Tokenizer::T_SEMICOLON);
				}
				_tokenizer->nextToken();
				parseStatements(node);
			} else if (match(Tokenizer::T_RETURN)) {
				parseReturn(node);
				if (!match(Tokenizer::T_SEMICOLON)) {
					throw PARSER_EXPECTED(Tokenizer::T_SEMICOLON);
				}
				_tokenizer->nextToken();
				parseStatements(node);
			} else if (match(Tokenizer::T_PRINT)) {
				parsePrint(node);
				if (!match(Tokenizer::T_SEMICOLON)) {
					throw PARSER_EXPECTED(Tokenizer::T_SEMICOLON);
				}
				_tokenizer->nextToken();
				parseStatements(node);
			} else if (match(Tokenizer::T_READ)) {
				parseRead(node);
				if (!match(Tokenizer::T_SEMICOLON)) {
					throw PARSER_EXPECTED(Tokenizer::T_SEMICOLON);
				}
				_tokenizer->nextToken();
				parseStatements(node);
			}
			// eps
		}

		void parseDeclaration(Node *node) {
			TRACE;
			if (match(Tokenizer::T_TYPE_INT)) {
				Node *node_declaration = new DeclarationNode();
				parseType(node_declaration);
				parseId(node_declaration);
				node->addChild(node_declaration);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseReturn(Node *node) {
			TRACE;
			if (match(Tokenizer::T_RETURN)) {
				_tokenizer->nextToken();
				Node *node_return = new ReturnNode();
				parseExpression(node_return);
				node->addChild(node_return);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_RETURN);
			}
		}

		void parsePrint(Node *node) {
			TRACE;
			if (match(Tokenizer::T_PRINT)) {
				_tokenizer->nextToken();
				Node *node_print = new PrintNode();
				parseExpression(node_print);
				node->addChild(node_print);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_PRINT);
			}
		}

		void parseRead(Node *node) {
			TRACE;
			if (match(Tokenizer::T_READ)) {
				_tokenizer->nextToken();
				Node *node_read = new ReadNode();
				parseId(node_read);
				node->addChild(node_read);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_READ);
			}
		}

		void parseAssignment(Node *node) {
			TRACE;
			if (match(Tokenizer::T_ID)) {
				Node *node_assignment = new AssignmentNode();
				parseId(node_assignment);
				if (!match(Tokenizer::T_ASSIGNMENT)) {
					throw PARSER_EXPECTED(Tokenizer::T_ASSIGNMENT);
				}
				_tokenizer->nextToken();
				parseExpression(node_assignment);
				node->addChild(node_assignment);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_ID);
			}
		}

		void parseWhile(Node *node) { // TODO:
			TRACE;
			if (match(Tokenizer::T_WHILE)) {
				_tokenizer->nextToken();

				Node *node_while = new WhileNode();
				// parseBexpression
				parseBexpression(node_while);
				// match do
				if (!match(Tokenizer::T_DO)) {
					throw PARSER_EXPECTED(Tokenizer::T_DO);
				}
				_tokenizer->nextToken();

				// parseStatements
				Node *node_statements = new StatementsNode();
				parseStatements(node_statements);
				node_while->addChild(node_statements);
				// match done
				if (!match(Tokenizer::T_DONE)) {
					throw PARSER_EXPECTED(Tokenizer::T_DONE);
				}
				_tokenizer->nextToken();

				node->addChild(node_while);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_WHILE);
			}
		}

		void parseIf(Node *node) { // TODO:
			TRACE;
			if (match(Tokenizer::T_IF)) {
				_tokenizer->nextToken();
				Node *node_if = new IfNode();
				// parseBexpression
				parseBexpression(node_if);

				// match then
				if (!match(Tokenizer::T_THEN)) {
					throw PARSER_EXPECTED(Tokenizer::T_THEN);
				}
				_tokenizer->nextToken();

				Node *node_then_statements = new StatementsNode();
				// parseStatements
				parseStatements(node_then_statements);
				node_if->addChild(node_then_statements);

				Node *node_else_statements = new StatementsNode();
				if (match(Tokenizer::T_ELSE)) {
					// else if T_ELSE -> parseStatements, match T_FI
					_tokenizer->nextToken();
					parseStatements(node_else_statements);

					if (!match(Tokenizer::T_FI)) {
						throw PARSER_EXPECTED(Tokenizer::T_FI);
					}
					_tokenizer->nextToken();

					node_if->addChild(node_else_statements);
				} else if (match(Tokenizer::T_FI)) {
					// if T_FI -> return
					_tokenizer->nextToken();
					node_if->addChild(node_else_statements);
				} else {
					// else throw ILLEGAL
					throw PARSER_ILLEGAL;
				}

				node->addChild(node_if);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_IF);
			}
		}

		bool isbAtomStart() {
			bool result = false
				|| isAtomStart()
				|| match(Tokenizer::T_NOT)
				|| match(Tokenizer::T_OPENING_SBRACKET)
				|| match(Tokenizer::T_TRUE)
				|| match(Tokenizer::T_FALSE);

			return result;
		}

		bool isAtomStart() {
			bool result = false
				|| match(Tokenizer::T_ID)
				|| match(Tokenizer::T_INTEGER)
				|| match(Tokenizer::T_OPENING_RBRACKET)
				|| match(Tokenizer::T_PLUS)
				|| match(Tokenizer::T_MINUS)
				|| match(Tokenizer::T_OPENING_CBRACKET);

			return result;
		}

		void parseFuncall(Node *node) {
			TRACE;

			if (match(Tokenizer::T_OPENING_CBRACKET)) {
				_tokenizer->nextToken();
				Node *node_funcall = new FuncallNode();
				parseId(node_funcall);
				parseFuncallargs(node_funcall);
				if (!match(Tokenizer::T_CLOSING_CBRACKET)) {
					throw PARSER_EXPECTED(Tokenizer::T_CLOSING_CBRACKET);
				}
				_tokenizer->nextToken();
				node->addChild(node_funcall);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_OPENING_CBRACKET);
			}
		}

		void parseFuncallargs(Node *node) {
			TRACE;

			if (isAtomStart()) {
				Node *node_funcallarg = new FuncallargNode();
				parseExpression(node_funcallarg);
				node->addChild(node_funcallarg);
				parseFuncallargsrest(node);
			}
			// eps
		}

		void parseFuncallargsrest(Node *node) {
			TRACE;

			if (match(Tokenizer::T_COMMA)) {
				_tokenizer->nextToken();
				Node *node_expression = new ExpressionNode();
				parseExpression(node_expression);
				node->addChild(node_expression);
				parseFuncallargsrest(node);
			}
			// eps
		}

		void parseBexpression(Node *node) {
			TRACE;

			if (isbAtomStart()) {
				Node *node_bexpression = new BexpressionNode();
				parsebdisj(node_bexpression);
				if (match(Tokenizer::T_OR)) {
					parsebDisj(node_bexpression);
				}
				node->addChild(node_bexpression);
			} else {
				throw PARSER_ILLEGAL;
			}
		}


		void parsebdisj(Node *node) {
			TRACE;

			if (isbAtomStart()) {
				Node *node_bdisj = new BdisjNode();
				parsebAtom(node_bdisj);
				if (match(Tokenizer::T_AND)) {
					parsebConj(node_bdisj);
				}
				node->addChild(node_bdisj);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parsebDisj(Node *node) {
			TRACE;

			if (match(Tokenizer::T_OR)) {
				_tokenizer->nextToken();
				Node *node_bDisj = new BDisjNode();
				parsebdisj(node_bDisj);
				if (match(Tokenizer::T_OR)) {
					parsebDisj(node_bDisj);
				}
				node->addChild(node_bDisj);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_OR);
			}
		}

		void parsebConj(Node *node) {
			TRACE;

			if (match(Tokenizer::T_AND)) {
				_tokenizer->nextToken();
				Node *node_bConj = new BConjNode();
				parsebAtom(node_bConj);
				if (match(Tokenizer::T_AND)) {
					parsebConj(node_bConj);
				}
				node->addChild(node_bConj);
			} else {
				throw PARSER_EXPECTED(Tokenizer::T_OR);
			}

		}

		void parsebAtom(Node *node) {
			TRACE;

			if (isbAtomStart()) {
				Node *node_batom = new BAtomNode();

				if (isAtomStart()) {
					parseCmp(node_batom);
				} else if (match(Tokenizer::T_NOT)) {
					_tokenizer->nextToken();
					Node *node_not = new NotNode();
					parsebAtom(node_not);
					node_batom->addChild(node_not);
				} else if (match(Tokenizer::T_OPENING_SBRACKET)) {
					_tokenizer->nextToken();
					parseBexpression(node_batom);
					if (!match(Tokenizer::T_CLOSING_SBRACKET)) {
						throw PARSER_EXPECTED(Tokenizer::T_CLOSING_SBRACKET);
					}
					_tokenizer->nextToken();
				} else if (match(Tokenizer::T_TRUE)) {
					_tokenizer->nextToken();
					Node *node_true = new TrueNode();
					node_batom->addChild(node_true);
				} else if (match(Tokenizer::T_FALSE)) {
					_tokenizer->nextToken();
					Node *node_false = new FalseNode();
					node_batom->addChild(node_false);
				} else {
					throw PARSER_ILLEGAL;
				}

				node->addChild(node_batom);
			} else {
				throw PARSER_ILLEGAL;
			}
		}


		void parseCmp(Node *node) {
			TRACE;

			if (isAtomStart()) {
				Node *node_cmp_aux = new CmpNode();
				Node *node_cmp = NULL;
				parseExpression(node_cmp_aux);
				if (match(Tokenizer::T_LESS)) {
					node_cmp = new CmpLessNode();
				} else if (match(Tokenizer::T_LESS_OR_EQUAL)) {
					node_cmp = new CmpLessOrEqualNode();
				} else if (match(Tokenizer::T_GREATER)) {
					node_cmp = new CmpGreaterNode();
				} else if (match(Tokenizer::T_GREATER_OR_EQUAL)) {
					node_cmp = new CmpGreaterOrEqualNode();
				} else if (match(Tokenizer::T_EQUAL)) {
					node_cmp = new CmpEqualNode();
				} else if (match(Tokenizer::T_NOT_EQUAL)) {
					node_cmp = new CmpNotEqualNode();
				} else {
					throw PARSER_ILLEGAL;
				}
				_tokenizer->nextToken();

				node_cmp->addChild(node_cmp_aux->get(0));
				// we don't want children to die :) (~Node() deletes children)
				node_cmp_aux->clear();
				delete node_cmp_aux;
				parseExpression(node_cmp);
				node->addChild(node_cmp);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseExpression(Node *node) {
			TRACE;
			if (isAtomStart()) {
				Node *node_expression = new ExpressionNode();
				parseterm(node_expression);
				if (match(Tokenizer::T_PLUS)) {
					parseTerm(node_expression);
				} else if (match(Tokenizer::T_MINUS)) {
					parseTerm(node_expression);
				}
				node->addChild(node_expression);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseterm(Node *node) {
			TRACE;

			if (isAtomStart()) {
				Node *node_term = new termNode();
				parsemult(node_term);
				if (match(Tokenizer::T_MULT)
						|| match(Tokenizer::T_MOD)
						|| match(Tokenizer::T_DIV)) {
					parseMult(node_term);
				}
				node->addChild(node_term);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parsemult(Node *node) {
			TRACE;

			if (isAtomStart()) {
				Node *node_mult = new multNode();
				parseAtom(node_mult);
				if (match(Tokenizer::T_MULT)
						|| match(Tokenizer::T_MOD)
						|| match(Tokenizer::T_DIV)) {
					parseMult(node_mult);
				}
				node->addChild(node_mult);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseTerm(Node *node) {
			TRACE;
			if (match(Tokenizer::T_PLUS)
					|| match(Tokenizer::T_MINUS)) {
				Node *node_Term = NULL;

				if (match(Tokenizer::T_PLUS)) {
					node_Term = new PlusTermNode();
				} else if (match(Tokenizer::T_MINUS)) {
					node_Term = new MinusTermNode();
				} else {
					assert(false);
				}
				_tokenizer->nextToken();

				parseterm(node_Term);
				if (match(Tokenizer::T_PLUS)
						|| match(Tokenizer::T_MINUS)) {
					parseTerm(node_Term);
				}

				node->addChild(node_Term);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseMult(Node *node) {
			TRACE;

			if (match(Tokenizer::T_MULT)
					|| match(Tokenizer::T_MOD)
					|| match(Tokenizer::T_DIV)) {
				Node *node_Mult = NULL;

				if (match(Tokenizer::T_MULT)) {
					node_Mult = new MultMultNode();
				} else if (match(Tokenizer::T_MOD)) {
					node_Mult = new ModMultNode();
				} else if (match(Tokenizer::T_DIV)) {
					node_Mult = new DivMultNode();
				} else {
					assert(false);
				}
				_tokenizer->nextToken();

				parseAtom(node_Mult);
				if (match(Tokenizer::T_MULT)
						|| match(Tokenizer::T_MOD)
						|| match(Tokenizer::T_DIV)){
					parseMult(node_Mult);
				}

				node->addChild(node_Mult);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

		void parseAtom(Node *node) {
			TRACE;
			if (isAtomStart()) {
				Node *node_atom = new AtomNode();
				if (match(Tokenizer::T_ID)) {
					parseId(node_atom);
				} else if (match(Tokenizer::T_INTEGER)) {
					Node *node_integer = new IntegerNode(_tokenizer->getTag());
					_tokenizer->nextToken();
					node_atom->addChild(node_integer);
				}  else if (match(Tokenizer::T_OPENING_RBRACKET)) {
					_tokenizer->nextToken();
					parseExpression(node_atom);
					if (!match(Tokenizer::T_CLOSING_RBRACKET)) {
						throw PARSER_EXPECTED(Tokenizer::T_CLOSING_RBRACKET);
					}
					_tokenizer->nextToken();
				} else if (match(Tokenizer::T_PLUS)) {
					// unary plus -- just ignore
					_tokenizer->nextToken();
					parseAtom(node_atom);
				} else if (match(Tokenizer::T_MINUS)) {
					_tokenizer->nextToken();
					Node *node_negation = new NegationNode();
					parseAtom(node_negation);
					node_atom->addChild(node_negation);
				} else if (match(Tokenizer::T_OPENING_CBRACKET)) {
					parseFuncall(node_atom);
				} else {
					assert(false);
				}
				node->addChild(node_atom);
			} else {
				throw PARSER_ILLEGAL;
			}
		}

};



#endif
