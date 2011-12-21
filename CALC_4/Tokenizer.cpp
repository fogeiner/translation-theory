#include <assert.h>
#include <cctype>
#include <vector>
#include <math.h>
#include "Tokenizer.h"
#include "create_map.h"

#include "Logger.h"
#include "LocatableStream.h"

using std::istream;
using std::string;
using std::exception;
using std::map;
using std::vector;

TokenizerException::TokenizerException(string msg) :
_msg(msg) {
}

TokenizerException::~TokenizerException() throw () {
}

const char *TokenizerException::what() const throw () {
    return _msg.c_str();
}
map<Tokenizer::ValueType, string> Tokenizer::_valueTypeTags =
        create_map<Tokenizer::ValueType, string>
        (T_UNDEFINED, "Undefined symbol")
(T_ID, "ID")
(T_INTEGER, "Integer")
//(T_FLOAT, "Float")
(T_PLUS, "Plus")
(T_MINUS, "Minus")
(T_MULT, "Multiplication")
(T_DIV, "Division")
//(T_POWER, "Power")
(T_MOD, "Mod")
(T_ASSIGNMENT, "Assignment")
(T_SEMICOLON, "Semicolon")
(T_OPENING_RBRACKET, "Opening round bracket")
(T_CLOSING_RBRACKET, "Closing round bracket")
(T_OPENING_CBRACKET, "Opening curly bracket")
(T_CLOSING_CBRACKET, "Closing curly bracket")
(T_EOF, "End Of File")
(T_TYPE_INT, "Integer type")
//(T_TYPE_FLOAT, "Float type")
(T_READ, "Read operation")
(T_PRINT, "Print operation")
(T_WHILE, "While")
(T_IF, "If")
(T_ELSE, "Else")
(T_LESS, "Less")
(T_LESS_OR_EQUAL, "Less or equal")
(T_GREATER, "Greater")
(T_GREATER_OR_EQUAL, "Greater or equal")
//(T_SHIFT_LEFT, "Shift left")
//(T_SHIFT_RIGHT, "Shift right")
(T_EQUAL, "Equal")
(T_NOT_EQUAL, "Not equal")
(T_OPENING_SBRACKET, "Opening square bracket")
(T_CLOSING_SBRACKET, "Closing square bracket")
(T_AND, "And")
(T_OR, "Or")
(T_NOT, "Not")
(T_FALSE, "False")
(T_TRUE, "True")
(T_RETURN, "Return")

(T_DEF, "Define function")
(T_COLON, "Colon")
(T_ENDDEF, "Define function end")
(T_COMMA, "Comma")
(T_DO, "Do")
(T_DONE, "Done")
(T_THEN, "Then")
//(T_DOT, "Dot")
(T_FI, "Fi");


Tokenizer::Tokenizer(LocatableStream &stream) :
_type(Tokenizer::T_UNDEFINED),
_stream(stream),
_peeking(false) {
}

Tokenizer::~Tokenizer() {
}

string Tokenizer::getTag() const {
    if (_peeking) {
        return _saved_tag;
    } else {
        return this->_tag;
    }
}

std::string Tokenizer::peekTag() {
    peekToken();
    return _tag;
}

int Tokenizer::getLineNumber() const {
    if (_peeking) {
        return _saved_line_number;
    } else {
        return _stream.getLineNumber();
    }
}

int Tokenizer::getLinePosition() const {
    if (_peeking) {
        return _saved_line_position;
    } else {
        return _stream.getLinePosition() - _tag.length() + 1;
    }
}

int Tokenizer::peekLineNumber() {
    peekToken();
    return _stream.getLineNumber();
}

int Tokenizer::peekLinePosition() {
    peekToken();
    return _stream.getLinePosition() - _tag.length() + 1;
}

Tokenizer::ValueType Tokenizer::nextToken() {

    if (_peeking) {
        _peeking = false;
        return _type;
    }

    string token;
    while (true) {

        int symbol = _stream.get();

        // operations, EOF, / -> // | /*
        if (_stream.eof()) {
            token = "";
            _type = T_EOF;
        } else if (isspace(symbol)) {
            while (isspace(symbol) && !_stream.eof()) {
                symbol = _stream.get();
            }
            _stream.unget();
            continue;
        } else if (symbol == '+') {
            token = symbol;
            _type = T_PLUS;
        } else if (symbol == '-') {
            token = symbol;
            _type = T_MINUS;
        } else if (symbol == '*') {
            token = symbol;
            _type = T_MULT;
        } else if (symbol == ':') {
            token = symbol;
            _type = T_COLON;
        } else if (symbol == '%') {
            token = symbol;
            _type = T_MOD;
        } else if (symbol == '=') {
            token = symbol;
            int next_symbol = _stream.get();
            if (next_symbol == '=') {
                token += next_symbol;
                _type = T_EQUAL;
            } else {
                _type = T_ASSIGNMENT;
                _stream.unget();
            }
        } else if (symbol == ';') {
            token = symbol;
            _type = T_SEMICOLON;
        } else if (symbol == '(') {
            token = symbol;
            _type = T_OPENING_RBRACKET;
        } else if (symbol == ')') {
            token = symbol;
            _type = T_CLOSING_RBRACKET;
        } else if (symbol == '[') {
            token = symbol;
            _type = T_OPENING_SBRACKET;
        } else if (symbol == ']') {
            token = symbol;
            _type = T_CLOSING_SBRACKET;
        } else if (symbol == '{') {
            token = symbol;
            _type = T_OPENING_CBRACKET;
        } else if (symbol == '}') {
            token = symbol;
            _type = T_CLOSING_CBRACKET;
        } else if (symbol == ',') {
            token = symbol;
            _type = T_COMMA;
        } else if (symbol == '<') {
            token = symbol;
            int next_symbol = _stream.get();
            if (next_symbol == '=') {
                token += next_symbol;
                _type = T_LESS_OR_EQUAL;
            } else {
                _type = T_LESS;
                _stream.unget();
            }
        } else if (symbol == '>') {
            token = symbol;
            int next_symbol = _stream.get();
            if (next_symbol == '=') {
                token += next_symbol;
                _type = T_GREATER_OR_EQUAL;
            } else {
                _type = T_GREATER;
                _stream.unget();
            }
        } else if (symbol == '!') {
            token = symbol;
            int next_symbol = _stream.get();
            if (next_symbol == '=') {
                token += next_symbol;
                _type = T_NOT_EQUAL;
            } else {
                _type = T_NOT;
                _stream.unget();
            }
        } else if (symbol == '#') {
            // line comment
            while ((symbol != '\n') && !_stream.eof()) {
                symbol = _stream.get();
            }
            _stream.unget();
            continue;
        } else if (symbol == '/') {
            int next_symbol = _stream.get();

            if (next_symbol == '/') {
                // line comment
                while ((symbol != '\n') && !_stream.eof()) {
                    symbol = _stream.get();
                }
                _stream.unget();
                continue;
            } else if (next_symbol == '*') {
                symbol = _stream.get();
                next_symbol = _stream.get();
                while ((symbol != '*') || (next_symbol != '/') && !_stream.eof()) {
                    symbol = next_symbol;
                    next_symbol = _stream.get();
                }
                continue;
            } else {
                _stream.unget();
                token = symbol;
                _type = T_DIV;
            }
        } else if (isdigit(symbol)) {
            token = symbol;
            symbol = _stream.get();
            while (isdigit(symbol) && !_stream.eof()) {
                token += symbol;
                symbol = _stream.get();
            }

            // check to forbid things like 100abc should be here

            _type = T_INTEGER;
            _stream.unget();
        } else if (isalpha(symbol) || symbol == '_') {
            token += symbol;
            symbol = _stream.get();
            while ((isalnum(symbol) || symbol == '_') && !_stream.eof()) {
                token += symbol;
                symbol = _stream.get();
            }
            _stream.unget();

            if (token == "int") {
                _type = T_TYPE_INT;
            } else if (token == "read") {
                _type = T_READ;
            } else if (token == "print") {
                _type = T_PRINT;
            } else if (token == "while") {
                _type = T_WHILE;
            } else if (token == "if") {
                _type = T_IF;
            } else if (token == "else") {
                _type = T_ELSE;
            } else if (token == "and") {
                _type = T_AND;
            } else if (token == "or") {
                _type = T_OR;
            } else if (token == "not") {
                _type = T_NOT;
            } else if (token == "false") {
                _type = T_FALSE;
            } else if (token == "true") {
                _type = T_TRUE;
            } else if (token == "return") {
                _type = T_RETURN;
            } else if (token == "def") {
                _type = T_DEF;
            } else if (token == "enddef") {
                _type = T_ENDDEF;
            } else if (token == "do") {
                _type = T_DO;
            } else if (token == "done") {
                _type = T_DONE;
            } else if (token == "then") {
                _type = T_THEN;
            } else if (token == "fi") {
                _type = T_FI;
            } else {
                _type = T_ID;
            }
        } else {
            token += symbol;
            _type = T_UNDEFINED;
        }

        _tag = token;

DEBUG(fmt("Read %s %s", _tag.c_str(), getTokenDescription(_type).c_str()));

        return _type;
    }
}

Tokenizer::ValueType Tokenizer::peekToken() {
    if (_peeking == false) {
        _saved_line_number = this->getLineNumber();
        _saved_line_position = this->getLinePosition();
        _saved_tag = this->getTag();
        _saved_type = this->nextToken();
        _peeking = true;
    }
    return _saved_type;
}

Tokenizer::ValueType Tokenizer::getToken() {
    if (_peeking) {
        return _saved_type;
    } else {
        return _type;
    }
}

std::string Tokenizer::getTokenDescription(ValueType valueType) const {
    return _valueTypeTags[valueType];
}
