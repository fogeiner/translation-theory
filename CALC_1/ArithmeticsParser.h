#ifndef ARITHMETICSPARSER_H
#define	ARITHMETICSPARSER_H

#include "Tokenizer.h"
#include "Logger.h"

#include <exception>
#include <string>
#include <cmath>

#define ARITHMETICS_PARSER_DEBUG DEBUG(fmt("%s %s %d:%d",\
            _token.c_str(),\
            _tokenizer.getTokenTag(_type).c_str(),\
            _tokenizer.lineNumber(),\
            _tokenizer.linePosition()\
            ))


using std::string;
using std::exception;

class ParseException : public exception {
private:
    string _msg;
public:

    ParseException(string str) {
        _msg = str;
    }

    ~ParseException() throw () {
    }

    const char *what() throw () {
        return _msg.c_str();
    }
};

/**
 * term0 -> term1 T1 | term1
 * term1 -> term2 T2 | term2
 * term2 -> termN T3 | termN
 * termN -> +termN | -termN | number | (expr)
 * T1 -> +- term1 T1 | +- term1 
 * T2 -> /* termN T2 | /* termN
 * T3 -> % termN T3 | % termN
 * 
 * number → real | integer 
 */
class ArithmeticsParser {
private:
    Tokenizer &_tokenizer;
    Tokenizer::ValueType _type;
    string _token;

    double parseTerm0() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        result = parseTerm1();
        parseT1(&result);
        return result;
    }

    void parseT1(double *result) {
        ARITHMETICS_PARSER_DEBUG;
        double term;
        Tokenizer::ValueType type = _type;
        if (type == Tokenizer::T_PLUS
                || type == Tokenizer::T_MINUS) {

            nextToken();
            term = parseTerm1();

            if (type == Tokenizer::T_PLUS) {
                *result += term;
            } else {
                *result -= term;
            }

            try {
                parseT1(result);
            } catch (ParseException &ex) {

            }
        }

    }

    void parseT2(double *result) {
        ARITHMETICS_PARSER_DEBUG;
        double term;

        Tokenizer::ValueType type = _type;
        if (type == Tokenizer::T_MULT
                || type == Tokenizer::T_DIV) {

            nextToken();
            term = parseTermN();
            if (type == Tokenizer::T_MULT) {
                *result *= term;
            } else {
                *result /= term;
            }

            try {
                parseT2(result);
            } catch (ParseException &ex) {
            }
        }
    }

    void parseT3(double *result) {
        ARITHMETICS_PARSER_DEBUG;
        double term;

        Tokenizer::ValueType type = _type;
        if (type == Tokenizer::T_MOD) {
            nextToken();
            term = parseTermN();
            *result = mod(*result, term);
            try {
                parseT3(result);
            } catch (ParseException &ex) {
            }
        }
    }

    double parseTerm1() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        result = parseTerm2();
        try {
            parseT2(&result);
        } catch (ParseException &ex) {
        }
        return result;
    }

    double parseTerm2() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        result = parseTermN();
        try {
            parseT3(&result);
        } catch (ParseException &ex) {
        }
        return result;
    }

    double parseTermN() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        if (_type == Tokenizer::T_OPENING_RBRACKET) {
            nextToken();
            result = parseTerm0();
            if (_type != Tokenizer::T_CLOSING_RBRACKET) {
                throw ParseException(
                        LOG_MSG(
                        fmt("Expected ')' but got '%s' on %d:%d",
                        _token.c_str(), _tokenizer.lineNumber(),
                        _tokenizer.linePosition()).c_str()
                        ));
            }
            nextToken();
        } else if (_type == Tokenizer::T_PLUS) {
            nextToken();
            result = parseTermN();
        } else if (_type == Tokenizer::T_MINUS) {
            nextToken();
            result = -parseTermN();
        } else {
            result = parseNumber();
        }
        return result;
    }

    double parseNumber() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        if (_type == Tokenizer::T_REAL) {
            result = atof(_token.c_str());
            nextToken();
        } else if (_type == Tokenizer::T_INTEGER) {
            result = atoi(_token.c_str());
            nextToken();
        } else {
            throw ParseException(
                    LOG_MSG(fmt("Expected number but got '%s' on %d:%d",
                    _token.c_str(), _tokenizer.lineNumber(),
                    _tokenizer.linePosition()).c_str()
                    ));
        }
        return result;
    }

    double mod(double x, double y) {
        if (y < 0) {
            return mod(-x, -y);
        } else {
            double result = x - int(x / y) * y;
            if (result < 0) {
                result += y;
            }
            return result;
        }
    }

    void nextToken() {
        ARITHMETICS_PARSER_DEBUG;
        _tokenizer.nextToken();
        _type = _tokenizer.getTokenType();
        _token = _tokenizer.getToken();
    }
public:

    ArithmeticsParser(Tokenizer &tokenizer) :
    _tokenizer(tokenizer) {
        ARITHMETICS_PARSER_DEBUG;
    }

    double parse() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        nextToken();
        result = parseTerm0();
        if (_type != Tokenizer::T_EOF) {
            throw ParseException(
                    LOG_MSG(fmt("Unparsed token '%s' on %d:%d",
                    _token.c_str(), _tokenizer.lineNumber(),
                    _tokenizer.linePosition()).c_str()
                    ));
        }
        return result;
    }

};

#endif	/* ARITHMETICSPARSER_H */

