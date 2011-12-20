#ifndef PARSER_H
#define PARSER_H

#include <cassert>
#include <string>
#include <vector>

#include "Tokenizer.h"
#include "Logger.h"

class Node;
std::string buildXMLTree(Node *root, int level = 0);

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
    virtual std::string _getDefaultXMLTag() const {
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

class DeclarationNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "declaration";
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
private:
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

class ExpressionNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "expression";
    }
};

class termNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "term";
    }
};

class multNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "mult";
    }
};

class NegationNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "negation";
    }
};

class PlusTermNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "plusterm";
    }
};

class MinusTermNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "minusterm";
    }
};

class MultMultNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "multmult";
    }
};

class ModMultNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "modmult";
    }
};

class DivMultNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "divmult";
    }
};

class AtomNode: public Node {
private:
    virtual std::string _getDefaultXMLTag() const {
        return "atom";
    }
};

class IntegerNode: public Node {
public:
    IntegerNode(std::string integer): Node(integer) {}
private:
    virtual std::string _getDefaultXMLTag() const {
        return "integer";
    }
};

class FloatNode: public Node {
public:
    FloatNode(std::string flt): Node(flt) {}
private:
    virtual std::string _getDefaultXMLTag() const {
        return "float";
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
        return buildXMLTree(_root);
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
        } else if (match(Tokenizer::T_TYPE_FLOAT)) {
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
            _tokenizer->nextToken();
            Node *node_declaration = new DeclarationNode();
            Node *node_type = new TypeNode();
            node_type->addChild(new TypeIntNode());
            node_declaration->addChild(node_type);
            parseId(node_declaration);
            node->addChild(node_declaration);
        } else if (match(Tokenizer::T_TYPE_FLOAT)) {
            _tokenizer->nextToken();
            Node *node_declaration = new DeclarationNode();
            Node *node_type = new TypeNode();
            node_type->addChild(new TypeFloatNode());
            node_declaration->addChild(node_type);
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
            || match(Tokenizer::T_FLOAT)
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

            node_cmp->addChild((*node_cmp_aux)[0]);
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
            } else if (match(Tokenizer::T_FLOAT)) {
                Node *node_float = new FloatNode(_tokenizer->getTag());
                _tokenizer->nextToken();
                node_atom->addChild(node_float);
            } else if (match(Tokenizer::T_OPENING_RBRACKET)) {
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
