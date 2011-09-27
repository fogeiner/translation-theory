#include <assert.h>
#include <cctype>
#include <vector>
#include "Tokenizer.h"
#include "create_map.h"
#include "utils.h"

#define DEBUG_LOG_OFF
#include "Logger.h"


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

LocatableStream::LocatableStream(std::istream &stream) :
_stream(stream),
_canUnget(false),
_lineNumber(1),
_linePosition(1) {

}

int LocatableStream::get() {
    int symbol = _stream.get();
    DEBUG(fmt("get() %d (%c)", symbol, symbol));

    if (symbol == '\n') {
        _lineNumber++;
        _lastPosition = _linePosition;
        _linePosition = 0;
    } else if (!_stream.eof()) {
        _linePosition++;
    }

    _lastSymbol = symbol;
    _canUnget = true;
    DEBUG(fmt("(%d,%d)", _lineNumber, _linePosition));

    return symbol;
}

void LocatableStream::unget() {
    DEBUG(fmt("unget() %d (%c)", _lastSymbol, _lastSymbol));
    if (!_canUnget) {
        CRITICAL("Can't unget with no get before");
    }
    if (_lastSymbol == '\n') {
        _lineNumber--;
        _linePosition = _lastPosition;
    } else if (!_stream.eof()) {
        _linePosition--;
    }
    _canUnget = false;
    DEBUG(fmt("(%d,%d)", _lineNumber, _linePosition));
    _stream.unget();
}

bool LocatableStream::eof() const {
    return _stream.eof();
}

int LocatableStream::getLineNumber() const {
    if (_lastSymbol == '\n' && _canUnget) {
        return _lineNumber - 1;
    }
    return _lineNumber;
}

int LocatableStream::getLinePosition() const {
    if (_lastSymbol == '\n' && _canUnget)
        return _lastPosition;
    return _linePosition;
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
(T_EQUALS, "Equals")
(T_SEMICOLON, "Semicolon")
(T_OPENING_RBRACKET, "Opening round bracket")
(T_CLOSING_RBRACKET, "Closing round bracket")
(T_OPENING_CBRACKET, "Opening curly bracket")
(T_CLOSING_CBRACKET, "Closing curly bracket")
(T_EOF, "End Of File")
(T_KEYWORD, "Keyword");

Tokenizer::Tokenizer(istream &stream) :
_type(Tokenizer::T_UNDEFINED) {
    _stream = new LocatableStream(stream);
}

Tokenizer::~Tokenizer() {
    delete _stream;
}

string Tokenizer::getToken() const {
    return this->_token;
}

int Tokenizer::lineNumber() const {
    return _stream->getLineNumber();
}

int Tokenizer::linePosition() const {

    return _stream->getLinePosition() - _token.length();
}

Tokenizer &Tokenizer::addKeyword(const string keyword) {
    _keywords.push_back(keyword);
    return *this;
}

void Tokenizer::nextToken() {
    string token;
    while (true) {
        int symbol = _stream->get();
        // operations, EOF, / -> // | /*
        if (_stream->eof()) {
            token = "";
            _type = T_EOF;
        } else if (isspace(symbol)) {
            while (isspace(symbol) && !_stream->eof()) {
                symbol = _stream->get();
            }
            _stream->unget();
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
            token = symbol;
            _type = T_EQUALS;
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
        } else if (symbol == '#') {
            // line comment
            while ((symbol != '\n') && !_stream->eof()) {
                symbol = _stream->get();
            }
            _stream->unget();
            continue;
        } else if (symbol == '/') {
            int next_symbol = _stream->get();

            if (next_symbol == '/') {
                // line comment
                while ((symbol != '\n') && !_stream->eof()) {
                    symbol = _stream->get();
                }
                _stream->unget();
                continue;
            } else if (next_symbol == '*') {
                symbol = _stream->get();
                next_symbol = _stream->get();
                while ((symbol != '*') || (next_symbol != '/') && !_stream->eof()) {
                    symbol = next_symbol;
                    next_symbol = _stream->get();
                }
                continue;
            } else {
                _stream->unget();
                token = symbol;
                _type = T_DIV;
            }
        } else if (isdigit(symbol) || symbol == '.') {
            int penalty = (symbol == '.') ? 1 : 0;
            token += symbol;
            symbol = _stream->get();
            while ((isdigit(symbol) || symbol == '.') && !_stream->eof()) {
                token += symbol;
                if (symbol == '.') {
                    penalty += 1;
                    if (penalty > 1) {
                        break;
                    }
                }
                symbol = _stream->get();
            }
            // if things like 100abc are forbidden,
            // here should be some check that symbol is
            // a delimiter, an operation or a comment
            _stream->unget();
            if (penalty == 0) {
                _type = T_INTEGER;
            } else if (penalty == 1) {
                _type = T_REAL;
            } else {
                _type = T_UNDEFINED;
            }
        } else if (isalpha(symbol) || symbol == '_') {
            token += symbol;
            symbol = _stream->get();
            while ((isalnum(symbol) || symbol == '_') && !_stream->eof()) {
                token += symbol;
                symbol = _stream->get();
            }
            _stream->unget();

            _type = T_WORD;
            for (vector<string>::iterator iter = _keywords.begin();
                    iter != _keywords.end();
                    ++iter) {
                if ((*iter) == token) {
                    _type = T_KEYWORD;
                    break;
                }
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
