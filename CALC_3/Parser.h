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
            _tokenizer->nextToken();

            parseType(node);
            parseId(node);
            parseFunargs(node);

            if (!match(Tokenizer::T_COLON)) {
                throw PARSER_EXPECTED(Tokenizer::T_COLON);
            }
            _tokenizer->nextToken();

            parseStatements(node);

            if (!match(Tokenizer::T_ENDDEF)) {
                throw PARSER_EXPECTED(Tokenizer::T_ENDDEF);
            }
            _tokenizer->nextToken();
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
            parseType(node);
            parseId(node);
            parseFunargsrest(node);
        }
        // eps
    }

    void parseFunargsrest(Node *node) {
        TRACE;

        if (match(Tokenizer::T_COMMA)) {
            _tokenizer->nextToken();
            parseId(node);
            parseFunargsrest(node);
        }
        // eps
    }

    void parseStatements(Node *node) {
        TRACE;

        if (false
//          || match(Tokenizer::T_WHILE)
//          || match(Tokenizer::T_IF)
            || match(Tokenizer::T_ID)
//          || match(Tokenizer::T_INTEGER)
//          || match(Tokenizer::T_FLOAT)
//          || match(Tokenizer::T_OPENING_RBRACKET)
//          || match(Tokenizer::T_PLUS)
//          || match(Tokenizer::T_MINUS)
//          || match(Tokenizer::T_TYPE_INT)
//          || match(Tokenizer::T_TYPE_FLOAT)
//          || match(Tokenizer::T_RETURN)
//          || match(Tokenizer::T_PRINT)
//          || match(Tokenizer::T_READ)
            ) {
                parseStatement(node);
                parseStatements(node);
            }
            // eps
    }

    void parseStatement(Node *node) {
        TRACE;
        //if (match(Tokenizer::T_WHILE)) {
        //  parseWhile();
        //} else if (match(Tokenizer::T_IF)) {
        //  parseIf();
        //} else
        if (match(Tokenizer::T_ID)) {
            // this can be: {expression, funcall} or assignment
            // if peek assignment -> parseAssigment
            // else parseExpression
            if (_tokenizer->peekToken() == Tokenizer::T_ASSIGNMENT) {
                parseAssignment(node);
            }
        } else {
            throw PARSER_ILLEGAL;
        }
    }

    void parseAssignment(Node *node) {
        TRACE;
        if (match(Tokenizer::T_ID)) {
            parseId();
            if (!match(Tokenizer::T_ASSIGNMENT)) {
                throw PARSER_EXPECTED(Tokenizer::T_ASSIGNMENT);
            }
            parseExpression(node);
        } else {
            throw PARSER_EXPECTED(Tokenizer::T_ID);
        }
    }

    bool isAtomStart(Node *node) {
        return match(Tokenizer::T_ID)
            || match(Tokenizer::T_INTEGER)
            || match(Tokenizer::T_FLOAT)
            || match(Tokenizer::T_OPENING_RBRACKET)
            || match(Tokenizer::T_MINUS)
            || match(Tokenizer::T_PLUS);
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

    void parseWhile(Node *node) { // TODO:
        TRACE;
        if (match(Tokenizer::T_WHILE)) {
            _tokenizer->nextToken();
            // parseBexpression
            // match do
            // parseStatements
            // match done
        } else {
            throw PARSER_EXPECTED(Tokenizer::T_WHILE);
        }
    }

    void parseIf(Node *node) { // TODO:
        TRACE;
        if (match(Tokenizer::T_IF)) {
            _tokenizer->nextToken();
            // parseBexpression
            // match then
            // parseStatements
            // if T_FI -> return
            // else if T_ELSE -> parseStatements, match T_FI
            // else throw ILLEGAL
        } else {
            throw PARSER_EXPECTED(Tokenizer::T_IF);
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
