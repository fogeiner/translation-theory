#ifndef PARSER_H
#define PARSER_H

#include <cassert>
#include <string>
#include <vector>

#include "Tokenizer.h"
#include "Logger.h"

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

    Node *operator[](int index) const {
        return _children[index];
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
};

class ProgramNode: public Node {
private:
        virtual std::string _getDefaultXMLTag() const {
            return "program";
        }
};

class FuncdefNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "funcdef";
    }
public:
    FuncdefNode(std::string id): Node(id) {}
};

class TypeIntNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "type_int";
    }
};

class TypeFloatNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "type_float";
    }
};

class IdNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "id";
    }
public:
    IdNode(std::string id):
    Node(id) {}
};

class TypeNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const  {
        return "type";
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
};

class ReturnNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "return";
    }
};

class ReadNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "read";
    }
};

class PrintNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "print";
    }
};

class AssignmentNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "assignment";
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
    Parser(Tokenizer *tokenizer):
        _tokenizer(tokenizer) {
            TRACE;
            assert(_tokenizer != NULL);

            _tokenizer->nextToken();
            parseProgram(&_root);

            if (_tokenizer->getToken() != Tokenizer::T_EOF) {
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
            Node *node_funcdef = new FuncdefNode(_tokenizer->getTag());
            _tokenizer->nextToken();

            Node *node_type = new TypeNode();
            parseType(node_type);
            node_funcdef->addChild(node_type);

            parseId(node_funcdef);

            Node *node_funcargs = new FuncargsNode();
            parseFunargs(node_funcargs);
            node_funcdef->addChild(node_funcargs);

            if (match(Tokenizer::T_ENDDEF)) {
                // with no body; just declaration
                _tokenizer->nextToken();
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
            node->addChild(new TypeIntNode());
            _tokenizer->nextToken();
        } else if (match(Tokenizer::T_TYPE_FLOAT)) {
            node->addChild(new TypeFloatNode());
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

        if (match(Tokenizer::T_TYPE_INT)
            || match(Tokenizer::T_TYPE_FLOAT)) {
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
        } else if (match(Tokenizer::T_IF)) {
            parseIf(node);
        } else if (match(Tokenizer::T_ID)) {
            parseAssignment(node);
        } else if (match(Tokenizer::T_TYPE_INT)) {
            parseDeclaration(node);
        } else if (match(Tokenizer::T_TYPE_FLOAT)) {
            parseDeclaration(node);
        } else if (match(Tokenizer::T_RETURN)) {
            parseReturn(node);
        } else if (match(Tokenizer::T_PRINT)) {
            parsePrint(node);
        } else if (match(Tokenizer::T_READ)) {
            parseRead(node);
        }
        // eps
    }

    void parseDeclaration(Node *node) {
        TRACE;
        if (match(Tokenizer::T_TYPE_INT)) {
            _tokenizer->nextToken();

            Node *node_type = new TypeNode();
            node_type->addChild(new TypeIntNode());
            parseId(node_type);
            node->addChild(node_type);
        } else if (match(Tokenizer::T_TYPE_FLOAT)) {
            _tokenizer->nextToken();

            Node *node_type = new TypeNode();
            node_type->addChild(new TypeFloatNode());
            parseId(node_type);
            node->addChild(node_type);
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
            Node *node_read = new PrintNode();
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
            parseStatements(node_while);
            // match done
            if (!match(Tokenizer::T_DONE)) {
                throw PARSER_EXPECTED(Tokenizer::T_DONE);
            }

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

            // parseStatements
            parseStatements(node_if);

            if (match(Tokenizer::T_ELSE)) {
                // else if T_ELSE -> parseStatements, match T_FI
                _tokenizer->nextToken();
                parseStatements(node_if);

                if (!match(Tokenizer::T_FI)) {
                    throw PARSER_EXPECTED(Tokenizer::T_FI);
                }
            } else if (match(Tokenizer::T_FI)) {
                // if T_FI -> return
                _tokenizer->nextToken();
            } else {
                // else throw ILLEGAL
                throw PARSER_ILLEGAL;
            }

            node->addChild(node_if);
        } else {
            throw PARSER_EXPECTED(Tokenizer::T_IF);
        }
    }

	bool isBatomStart() {
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
			|| match(Tokenizer::T_FLOAT)
			|| match(Tokenizer::T_OPENING_RBRACKET)
			|| match(Tokenizer::T_PLUS)
			|| match(Tokenizer::T_MINUS)
			|| match(Tokenizer::T_OPENING_CBRACKET);
			
		return result;
	}

	void parseFuncall(Node *node) {
		TRACE;
	}
	
	void parseFuncargs(Node *node) {
		TRACE;
	}
	
	void parseFuncargsrest(Node *node) {
		TRACE;
	}

	void parseBexpression(Node *node) {
		TRACE;
	}
	
	void parsebconj(Node *node) {
		TRACE;
	}
	
	void parseBconj(Node *node) {
		TRACE;
	}
	
	void parsebdisj(Node *node) {
		TRACE;
	}
	
	void parseBdisj(Node *node) {
		TRACE;
	}
	
	void parseBatom(Node *node) {
		TRACE;
	}

    void parseExpression(Node *node) {
        TRACE;
        if (isAtomStart()) {
                parseterm();
                if (match(Tokenizer::T_PLUS)
                    || match(Tokenizer::T_MINUS)) {
                    parseTerm(node);
                }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parseterm(Node *node) {
        TRACE;
        if (isAtomStart()) {
            parsemult();
            if (match(Tokenizer::T_MULT)
                || match(Tokenizer::T_DIV)
                || match(Tokenizer::T_MOD)) {
                    parseMult(node);
            }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parseTerm(Node *node) {
        TRACE;
        if (match(Tokenizer::T_PLUS)) {
            _tokenizer->nextToken();
            parseterm(node);

            if (match(Tokenizer::T_PLUS)
                || match(Tokenizer::T_MINUS)) {
                parseTerm(node);
            }
        } else if (match(Tokenizer::T_MINUS)) {
            _tokenizer->nextToken();
            parseterm(node);

            if (match(Tokenizer::T_PLUS)
                || match(Tokenizer::T_MINUS)) {
                parseTerm(node);
            }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parsemult(Node *node) {
        TRACE;
        if (isAtomStart()) {
            parseAtom(node);

            if (match(Tokenizer::T_MULT)
                || match(Tokenizer::T_DIV)
                || match(Tokenizer::T_MOD)) {
                parseMult(node);
            }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parseMult() {
        TRACE;
        if (match(Tokenizer::T_MULT)) {
            _tokenizer->nextToken();
            parseAtom();
            if (match(Tokenizer::T_MULT)
                || match(Tokenizer::T_DIV)
                || match(Tokenizer::T_MOD)) {
                parseMult(node);
            }
        } else if (match(Tokenizer::T_DIV)) {
            _tokenizer->nextToken();
            parseAtom(node);
            if (match(Tokenizer::T_MULT)
                || match(Tokenizer::T_DIV)
                || match(Tokenizer::T_MOD)) {
                parseMult(node);
            }
        } else if (match(Tokenizer::T_MOD)) {
            _tokenizer->nextToken();
            parseAtom();
            if (match(Tokenizer::T_MULT)
                || match(Tokenizer::T_DIV)
                || match(Tokenizer::T_MOD)) {
                parseMult(node);
            }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parseAtom(Node *node) {
        TRACE;
        if (match(Tokenizer::T_ID)) {
        } else if (match(Tokenizer::T_INTEGER)) {
        } else if (match(Tokenizer::T_FLOAT)) {
        } else if (match(Tokenizer::T_OPENING_RBRACKET)) {
        } else if (match(Tokenizer::T_PLUS)) {
        } else if (match(Tokenizer::T_MINUS)) {
        } else {
            throw PARSER_ILLEGAL;
        }
    }

};

#include <cstdio>
int main() {
    ProgramNode pn;
    ::printf("%s\n%s\n", pn.XMLStart().c_str(), pn.XMLEnd().c_str());
    return 0;
}

#endif
