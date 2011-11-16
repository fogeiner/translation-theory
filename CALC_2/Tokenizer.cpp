#include <assert.h>
#include <cctype>
#include <vector>
#include <math.h>
#include "Tokenizer.h"
#include "create_map.h"

#define DEBUG_LOG_OFF
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
(T_WORD, "Word")
(T_INTEGER, "Integer")
(T_REAL, "Real")
(T_PLUS, "Plus")
(T_MINUS, "Minus")
(T_MULT, "Multiplication")
(T_DIV, "Division")
(T_POWER, "Power")
(T_MOD, "Mod")
(T_ASSIGNMENT, "Assignment")
(T_SEMICOLON, "Semicolon")
(T_OPENING_RBRACKET, "Opening round bracket")
(T_CLOSING_RBRACKET, "Closing round bracket")
(T_OPENING_CBRACKET, "Opening curly bracket")
(T_CLOSING_CBRACKET, "Closing curly bracket")
(T_EOF, "End Of File")
(T_TYPE_INT, "Integer type")
(T_TYPE_REAL, "Real type")
(T_READ, "Read operation")
(T_WRITE, "Write operation")
(T_WHILE, "While")
(T_IF, "If")
(T_ELSE, "Else")
(T_LESS, "Less")
(T_LESS_OR_EQUAL, "Less or equal")
(T_GREATER, "Greater")
(T_GREATER_OR_EQUAL, "Greater or equal")
(T_SHIFT_LEFT, "Shift left")
(T_SHIFT_RIGHT, "Shift right")
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
(T_DOT, "Dot");

Tokenizer::Tokenizer(LocatableStream &stream) :
_type(Tokenizer::T_UNDEFINED),
_stream(stream) {
}

Tokenizer::~Tokenizer() {
}

string Tokenizer::getToken() const {

    return this->_token;
}

int Tokenizer::lineNumber() const {

    return _stream.getLineNumber();
}

int Tokenizer::linePosition() const {

    return _stream.getLinePosition() - _token.length() + 1;
}

void Tokenizer::nextToken() {
    string token;
    while (true) {
        int symbol;
        if (!_stream.eof()) {
            symbol = _stream.get();
        }
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
        } else if (symbol == '^') {
            token = symbol;
            _type = T_POWER;
        } else if (symbol == '%') {
            token = symbol;
            _type = T_MOD;
        } else if (symbol == '=') {
            // TODO:
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
        } else if (symbol == '{') {
            token = symbol;
            _type = T_OPENING_CBRACKET;
        } else if (symbol == '}') {
            token = symbol;
            _type = T_CLOSING_CBRACKET;
        } else if (symbol == '<') {
            token = symbol;
            int next_symbol = _stream.get();
            if (next_symbol == '=') {
                token += next_symbol;
                _type = T_LESS_OR_EQUAL;
            } else if (next_symbol == '<') {
                token += next_symbol;
                _type = T_SHIFT_LEFT;
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
            } else if (next_symbol == '>') {
                token += next_symbol;
                _type = T_SHIFT_RIGHT;
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

        } else if (symbol == '[') {
            token = symbol;
            _type = T_OPENING_SBRACKET;
        } else if (symbol == ']') {
            token = symbol;
            _type = T_CLOSING_SBRACKET;
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
                // bugfix 2 EOFs is needed to terminal application
                if (_stream.eof()) {
                    continue;
                }
                next_symbol = _stream.get();
                while (((symbol != '*') || (next_symbol != '/')) && !_stream.eof()) {
                    symbol = next_symbol;
                    next_symbol = _stream.get();
                }
                continue;
            } else {
                _stream.unget();
                token = symbol;
                _type = T_DIV;
            }
        } else if (isdigit(symbol) || symbol == '.') {
            token = symbol;
            int dots = (symbol == '.') ? 1 : 0;
            symbol = _stream.get();
            while ((isdigit(symbol) || symbol == '.') && !_stream.eof()) {
                token += symbol;
                if (symbol == '.') {
                    dots++;
                }
                symbol = _stream.get();
            }

            if (dots == 0) {
                _type = T_INTEGER;
            } else if (dots == 1) {
                if (token.length() != 1) {
                    _type = T_REAL;
                } else {
                    _type = T_DOT;
                }
            } else {
                _type = T_UNDEFINED;
            }


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
            } else if (token == "float") {
                _type = T_TYPE_REAL;
            } else if (token == "read") {
                _type = T_READ;
            } else if (token == "write") {
                _type = T_WRITE;
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
            } else {
                _type = T_WORD;
            }
        } else {
            token += symbol;
            _type = T_UNDEFINED;
        }

        _token = token;

        return;
    }
}

Tokenizer::ValueType Tokenizer::getTokenType() {

    return _type;
}

const std::string Tokenizer::getTokenTag(ValueType valueType) const {
    return _valueTypeTags[valueType];
}
