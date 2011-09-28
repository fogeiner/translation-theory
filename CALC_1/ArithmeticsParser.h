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
 * expr -> term1 T1 | term1
 * term1 -> atom T2 | atom
 * atom -> var | number | (expr)
 * T1 -> +- term1 T1 | +- term1 
 * T2 -> /* atom T2 | /* atom 
 * number → real | integer 
 */
class ArithmeticsParser {
private:
    Tokenizer &_tokenizer;
    Tokenizer::ValueType _type;
    string _token;

    double parseExpr() {
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
            term = parseAtom();
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

    double parseTerm1() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        result = parseAtom();
        try {
            parseT2(&result);
        } catch (ParseException &ex) {
        }
        return result;
    }

    double parseAtom() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        if (_type == Tokenizer::T_OPENING_RBRACKET) {
            nextToken();
            result = parseExpr();
            if (_type != Tokenizer::T_CLOSING_RBRACKET) {
                throw ParseException(
                        LOG_MSG(
                        fmt("Expected ')' but got '%s' on %d:%d",
                        _token.c_str(), _tokenizer.lineNumber(),
                        _tokenizer.linePosition()).c_str()
                        ));
            }
            nextToken();
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
            return -mod(-x, -y);
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
        result = parseExpr();
        return result;
    }

};

#endif	/* ARITHMETICSPARSER_H */

