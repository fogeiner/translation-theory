#include <cstdlib>
#include <iostream>
#include <fstream>
#include <exception>
#include <cassert>
#include <cerrno>
#include <cstring>
#include "utils.h"
#include "Logger.h"
#include "Tokenizer.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::string;
using std::exception;

double parseExpr(Tokenizer &tokenizer);
double parseT(Tokenizer &tokenizer);
double parseTerm(Tokenizer &tokenizer);
double parseF(Tokenizer &tokenizer);
double parseFactor(Tokenizer &tokenizer);

double parseExpr(Tokenizer &tokenizer) {
    DEBUG("parseExpr");

    double result = 0;
    double term = parseTerm(tokenizer);
    result += term;

    DEBUG(fmt("term = %f", term));

    Tokenizer::ValueType type = tokenizer.getTokenType();

    if (type == Tokenizer::T_PLUS
            || type == Tokenizer::T_MINUS) {
        double T = parseT(tokenizer);
        DEBUG(fmt("T = %f", T));
        result += T;
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }

    return result;
}

double parseT(Tokenizer &tokenizer) {
    DEBUG("parseT");

    double result = 0;

    Tokenizer::ValueType type = tokenizer.getTokenType();
    tokenizer.nextToken();
    double term = parseTerm(tokenizer);
    if (type == Tokenizer::T_PLUS) {
        result += term;
    } else if (type == Tokenizer::T_MINUS) {
        result -= term;
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }
    DEBUG(fmt("term = %f", term));

    type = tokenizer.getTokenType();
    if (type == Tokenizer::T_PLUS
            || type == Tokenizer::T_MINUS) {
        double T = parseT(tokenizer);
        DEBUG(fmt("T = %f", T));
        result += T;
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }

    return result;
}

double parseTerm(Tokenizer &tokenizer) {
    DEBUG("parseTerm");
    double result = 0;
    double factor = parseFactor(tokenizer);
    result += factor;

    DEBUG(fmt("factor = %f", factor));

    Tokenizer::ValueType type = tokenizer.getTokenType();

    if (type == Tokenizer::T_MULT
            || type == Tokenizer::T_DIV) {
        double F = parseF(tokenizer);
        DEBUG(fmt("F = %f", F));
        result += F;
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }

    return result;
}

double parseF(Tokenizer &tokenizer) {
    DEBUG("parseF");

    double result = 1.0;

    Tokenizer::ValueType type = tokenizer.getTokenType();
    tokenizer.nextToken();
    double factor = parseTerm(tokenizer);
    if (type == Tokenizer::T_MULT) {
        result *= factor;
    } else if (type == Tokenizer::T_DIV) {
        result /= factor;
    } else {
        ERROR(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
        exit(1);
    }
    DEBUG(fmt("factor = %f", factor));

    type = tokenizer.getTokenType();
    if (type == Tokenizer::T_MULT
            || type == Tokenizer::T_DIV) {
        double F = parseF(tokenizer);
        DEBUG(fmt("F = %f", F));
        result *= F;
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }

    return result;
}

double parseFactor(Tokenizer &tokenizer) {
    DEBUG("parseFactor");
    Tokenizer::ValueType type = tokenizer.getTokenType();
    double result;

    if (type == Tokenizer::T_INTEGER) {
        result = atol(tokenizer.getToken().c_str());
    } else if (type == Tokenizer::T_REAL) {
        result = atof(tokenizer.getToken().c_str());
    } else if (type == Tokenizer::T_OPENING_RBRACKET) {
        tokenizer.nextToken();
        result = parseExpr(tokenizer);
    } else {
        WARN(fmt("Unexpected token %s (%s) on %d:%d",
                tokenizer.getToken().c_str(), tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(), tokenizer.linePosition()));
    }

    tokenizer.nextToken();

    return result;
}

int main(int argc, char** argv) {
    Logger::setLevel(Logger::DEBUG);
    ifstream input("data");
    if (!input.good()) {
        CRITICAL(fmt("Failed to open input file: %s", strerror(errno)));
    }
    try {
        Tokenizer tokenizer(&input);

        tokenizer.
                addKeyword("print").
                addKeyword("exit").
                addKeyword("return");

        tokenizer.nextToken();
        cout << "Value is " << parseExpr(tokenizer);

       /* 
        for (tokenizer.nextToken();
                tokenizer.getTokenType() != Tokenizer::T_EOF;
                tokenizer.nextToken()) {
            cout << tokenizer.getToken()
                    << "\t" << tokenizer.getTokenTag(tokenizer.getTokenType()) << '\t'
                    << tokenizer.lineNumber() << '\t'
                    << tokenizer.linePosition() << endl;
        }*/
    } catch (exception ex) {
        CRITICAL(ex.what());
    }

    return EXIT_SUCCESS;
}

