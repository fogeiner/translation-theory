#include <assert.h>
#include <cctype>
#include <vector>
#include "Tokenizer.h"
#include "create_map.h"
#include "Logger.h"
#include "utils.h"

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

LocatableStream::LocatableStream(std::istream *stream) :
_stream(stream),
canUnget(false),
lineNumber(0),
linePosition(0) {
}

int LocatableStream::get() {
    int symbol = _stream->get();
    DEBUG(fmt("get() %d (%c)", symbol, symbol));

    if (symbol == '\n') {
        lineNumber++;
        lastPosition = linePosition;
        linePosition = 0;
    } else if (!_stream->eof()) {
        linePosition++;
    }

    lastSymbol = symbol;
    canUnget = true;
    DEBUG(fmt("(%d,%d)", lineNumber, linePosition));

    return symbol;
}

void LocatableStream::unget() {
    DEBUG(fmt("unget() %d (%c)", lastSymbol, lastSymbol));
    if (!canUnget) {
        ERROR("Can't unget with no get before");
        exit(1);
    }
    if (lastSymbol == '\n') {
        lineNumber--;
        linePosition = lastPosition;
    } else if (!_stream->eof()) {
        linePosition--;
    }
    canUnget = false;
    DEBUG(fmt("(%d,%d)", lineNumber, linePosition));
    _stream->unget();
}

bool LocatableStream::eof() const {
    return _stream->eof();
}

int LocatableStream::getLineNumber() const {
    if (lastSymbol == '\n') {
        return lineNumber - 1;
    }
    return lineNumber;
}

int LocatableStream::getLinePosition() const {
    if (lastSymbol == '\n')
        return lastPosition;
    return linePosition;
}

map<Tokenizer::ValueType, string> Tokenizer::_valueTypeTags =
        create_map<Tokenizer::ValueType, string>
        (T_UNDEFINED, "Undefined symbol")
(T_WORD, "Word")
(T_NUMBER, "Number")
(T_PLUS, "Plus")
(T_MINUS, "Minus")
(T_MULT, "Multiplication")
(T_DIV, "Division")
(T_POWER, "Power")
(T_MOD, "Mod")
(T_EQUALS, "Equals")
(T_EOL, "End Of Line")
(T_SEMICOLON, "Semicolon")
(T_OPENING_RBRACKET, "Opening round bracket")
(T_CLOSING_RBRACKET, "Closing round bracket")
(T_OPENING_CBRACKET, "Opening curly bracket")
(T_CLOSING_CBRACKET, "Closing curly bracket")
(T_EOF, "End Of File")
(T_KEYWORD, "Keyword");

Tokenizer::Tokenizer(istream *stream) :
_stream(stream),
_type(Tokenizer::T_UNDEFINED) {
}

double Tokenizer::getNumericValue() const {
    return atof(this->_token.c_str());
}

string Tokenizer::getStringValue() const {
    return this->_token;
}

int Tokenizer::lineNumber() const {
    return _stream.getLineNumber() + 1;
}

int Tokenizer::linePosition() const {

    return _stream.getLinePosition() - _token.length() + 1;
}

Tokenizer &Tokenizer::addKeyword(const string keyword) {
    _keywords.push_back(keyword);
    return *this;
}

Tokenizer::ValueType Tokenizer::nextToken() {

    string token;

    enum states {
        INIT,
        NUMBER,
        WORD,
        LINE_COMMENT,
        MULTILINE_COMMENT
    };
    states state = INIT;

    bool dotOn = false;

    DEBUG("State INIT");
    while (true) {
        int symbol;
        symbol = _stream.get();

        if (_stream.eof()) {
            if (state == NUMBER) {
                _token = token;
                return Tokenizer::T_NUMBER;
            } else if (state == WORD) {
                _token = token;
                // check if keyword
                for (vector<string>::iterator iter = _keywords.begin();
                        iter != _keywords.end();
                        ++iter) {
                    if (_token == (*iter)) {
                        return T_KEYWORD;
                    }
                }
                return Tokenizer::T_WORD;
            } else {
                return Tokenizer::T_EOF;
            }
        }

        DEBUG(fmt("Read symbol %d (%c)", symbol, symbol));

        if (state == INIT) {
            // skipping whitespace
            if (isblank(symbol)) {
                DEBUG("Skipping space or tab");
                continue;
            }

            int next_symbol = _stream.get();
            _stream.unget();

            DEBUG(fmt("Next symbol is %d (%c)", next_symbol, next_symbol));

            if (isalpha(symbol)) {
                token += symbol;
                DEBUG("State WORD");
                state = WORD;
            } else if (isdigit(symbol) || symbol == '.') {
                dotOn = (symbol == '.');
                token += symbol;
                DEBUG("State NUMBER");
                state = NUMBER;
            } else if (symbol == '#' || (symbol == '/' && next_symbol == '/')) {
                _stream.get();
                state = LINE_COMMENT;
                DEBUG("State LINE_COMMENT");
            } else if (symbol == '/' && next_symbol == '*') {
                _stream.get();
                state = MULTILINE_COMMENT;
                DEBUG("State MULTILINE_COMMENT");
            } else if (symbol == '+') {
                _token = symbol;
                return T_PLUS;
            } else if (symbol == '-') {
                _token = symbol;
                return T_MINUS;
            } else if (symbol == '*') {
                _token = symbol;
                return T_MULT;
            } else if (symbol == '/') {
                _token = symbol;
                return T_DIV;
            } else if (symbol == '^') {
                _token = symbol;
                return T_POWER;
            } else if (symbol == '%') {
                _token = symbol;
                return T_MOD;
            } else if (symbol == '=') {
                _token = symbol;
                return T_EQUALS;
            } else if (symbol == '\n') {
                _token = symbol;
                return T_EOL;
            } else if (symbol == ';') {
                _token = symbol;
                return T_SEMICOLON;
            } else if (symbol == '(') {
                _token = symbol;
                return T_OPENING_RBRACKET;
            } else if (symbol == ')') {
                _token = symbol;
                return T_CLOSING_RBRACKET;
            } else if (symbol == '{') {
                _token = symbol;
                return T_OPENING_CBRACKET;
            } else if (symbol == '}') {
                _token = symbol;
                return T_CLOSING_CBRACKET;
            } else {
                ERROR(fmt("Don't know how to handle %d (%c) on %d:%d",
                        symbol, symbol, _stream.getLineNumber(), _stream.getLinePosition()));
                exit(1);
            }

        } else if (state == NUMBER) {
            if (isdigit(symbol)) {
                token += symbol;
            } else if (symbol == '.') {
                if (dotOn != true) {
                    dotOn = true;
                    token += symbol;
                } else {
                    // error
                    token += symbol;
                    _token = token;
                    return T_UNDEFINED;
                }
            } else {
                _stream.unget();
                _token = token;
                return T_NUMBER;
            }
        } else if (state == WORD) {
            if (isalnum(symbol)) {
                token += symbol;
            } else {
                _stream.unget();
                _token = token;
                // check if keyword
                for (vector<string>::iterator iter = _keywords.begin();
                        iter != _keywords.end();
                        ++iter) {
                    if (_token == (*iter)) {
                        return T_KEYWORD;
                    }
                }
                return T_WORD;
            }
        } else if (state == LINE_COMMENT) {
            if (symbol == '\n') {
                state = INIT;
            }
        } else if (state == MULTILINE_COMMENT) {
            int next_symbol = _stream.get();
            _stream.unget();
            if ((symbol == '*') && (next_symbol == '/')) {
                _stream.get();
                state = INIT;
            }
        } else {
            assert(false);
        }

    }
}

const std::string Tokenizer::getTokenTag(ValueType valueType) const {
    return _valueTypeTags[valueType];
}
