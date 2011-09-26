#ifndef ARITHMETICSPARSER_H
#define	ARITHMETICSPARSER_H

#include "Tokenizer.h"
#include "Logger.h"

#include <exception>
#include <string>

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
 * expr	→      term1 T1 | term1
 * T1 		→ ±   term1 T1 | ± term1
 * T2 		→ /*  term2 T2 | /* term2
 * T3 		→ %  term3 T3 | % term3
 * T4 		→ ^   T4 atom | ^ atom
 * 
 * term1 	→ term2 T2 | term2
 * term2 	→ term3 T3 | term3
 * term3 	→ term4 T4 | term4
 * 
 * atom 	→ + atom | - atom | (expr) | number
 * 
 * number → real | integer 
 */

class ArithmeticsParser {
private:
    Tokenizer &_tokenizer;

    double parseExpr() {
        double result = 0;
        if (!parseTerm1(&result)) {
            throw ParseException("__LINE__");
        }
        parseT1(&result);
        return result;
    }

    bool parseT1(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();

        double term = 0.0;
        if (type == Tokenizer::T_PLUS
                || type == Tokenizer::T_MINUS) {

            if (!parseTerm1(term))
                throw ParseException("__LINE__");

            if (type == Tokenizer::T_PLUS)
                *result += term;
            else
                *result -= term;

            return parseT1(result);

        } else {
            throw ParseException("__LINE__");
        }
    }

    bool parseT2(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();

        double term = 1.0;
        if (type == Tokenizer::T_MULT
                || type == Tokenizer::T_DIV) {

            if (!parseTerm2(term))
                throw ParseException("__LINE__");

            if (type == Tokenizer::T_MULT)
                *result *= term;
            else
                *result /= term;

            return parseT2(result);

        } else {
            throw ParseException("__LINE__");
        }
    }

    bool parseT3(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();

        double term = 1.0;
        if (type == Tokenizer::T_MOD) {

            if (!parseTerm3(term))
                throw ParseException("__LINE__");

            *result %= term;

            return parseT3(result);
        } else {
            throw ParseException("__LINE__");
        }
    }

    bool parseT4(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();

        double term = 1.0;
        if (type == Tokenizer::T_MOD) {

            if (!parseTerm3(term))
                throw ParseException("__LINE__");

            *result %= term;

            return parseT3(result);
        } else {
            throw ParseException("__LINE__");
        }    }

    bool parseTerm1(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();
        string token = _tokenizer.getToken();
    }

    bool parseTerm2(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();
        string token = _tokenizer.getToken();
    }

    bool parseTerm3(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();
        string token = _tokenizer.getToken();
    }

    void parseAtom(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();
        string token = _tokenizer.getToken();
        if (type == Tokenizer::T_OPENING_RBRACKET) {
            _tokenizer.nextToken();
            *result = parseExpr();
            if (type != Tokenizer::T_OPENING_RBRACKET) {
                throw ParseException("__LINE__");
            } else {
                _tokenizer.nextToken();
            }
        } else if (type == Tokenizer::T_PLUS) {
            _tokenizer.nextToken();
            *result = parseAtom();
        } else if (type == Tokenizer::T_MINUS) {
            _tokenizer.nextToken();
            *result = -parseAtom();
        } else {
            _tokenizer.nextToken();
            parseNumber(result);
        }
    }

    void parseNumber(double *result) {
        Tokenizer::ValueType type = _tokenizer.getTokenType();
        string token = _tokenizer.getToken();
        if (type == Tokenizer::T_REAL) {
            *result = atof(token.c_str());
        } else if (type == Tokenizer::T_INTEGER) {
            *result = atoi(token.c_str());
        } else {
            throw ParseException("__LINE__");
        }
        _tokenizer.nextToken();
    }

public:

    ArithmeticsParser(Tokenizer &tokenizer) :
    tokenizer(tokenizer) {
    }

    double parse() {
        _tokenizer.nextToken();
        return parseExpr();
    }

};

#endif	/* ARITHMETICSPARSER_H */

