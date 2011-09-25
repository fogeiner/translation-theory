#include <cstdlib>
#include <iostream>
#include <fstream>
#include <istream>
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
using std::istream;
using std::string;
using std::exception;

double parseExpr(Tokenizer &tokenizer);
double parseT(Tokenizer &tokenizer, double partial);
double parseTerm(Tokenizer &tokenizer);
double parseF(Tokenizer &tokenizer, double partial);
double parseFactor(Tokenizer &tokenizer);

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

double parseExpr(Tokenizer &tokenizer) {
    DEBUG(fmt("%s %s %d:%d",
            tokenizer.getToken().c_str(),
            tokenizer.getTokenTag(tokenizer.getTokenType()).c_str(),
            tokenizer.lineNumber(),
            tokenizer.linePosition()
            ));

    double result = 0;
    double term;

    try {
        term = parseTerm(tokenizer);
        result += term;
    } catch (ParseException &ex) {
        CRITICAL(ex.what());
    }

    DEBUG(fmt("term = %f", term));

    try {
        result = parseT(tokenizer, term);
        DEBUG(fmt("T = %f", term));
    } catch (ParseException &ex) {
        DEBUG(fmt("Failed to parse T: %s", ex.what()));
    }

    return result;
}

double parseTerm(Tokenizer &tokenizer) {
    DEBUG(fmt("%s %s %d:%d",
            tokenizer.getToken().c_str(),
            tokenizer.getTokenTag(tokenizer.getTokenType()).c_str(),
            tokenizer.lineNumber(),
            tokenizer.linePosition()
            ));

    double result = 0;
    double factor;

    try {
        factor = parseFactor(tokenizer);
        result += factor;
    } catch (ParseException &ex) {
        CRITICAL(ex.what());
    }

    DEBUG(fmt("factor = %f", factor));

    try {
        result = parseF(tokenizer, result);
    } catch (ParseException &ex) {
        DEBUG("Failed to parse F");
    }

    DEBUG(fmt("factor = %f", factor));

    return result;
}

double parseT(Tokenizer &tokenizer, double partial) {
    DEBUG(fmt("%s %s %d:%d",
            tokenizer.getToken().c_str(),
            tokenizer.getTokenTag(tokenizer.getTokenType()).c_str(),
            tokenizer.lineNumber(),
            tokenizer.linePosition()
            ));

    double result;
    double term;

    Tokenizer::ValueType type = tokenizer.getTokenType();

    if (type != Tokenizer::T_PLUS
            && type != Tokenizer::T_MINUS) {
        throw ParseException(
                LOG_MSG(fmt("Expected + or - but got %s (%s) on %d:%d",
                tokenizer.getToken().c_str(),
                tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(),
                tokenizer.linePosition()).c_str()
                ));
    }

    tokenizer.nextToken();
    term = parseTerm(tokenizer);

    DEBUG(fmt("term = %f", term));

    if (type == Tokenizer::T_PLUS) {
        result = partial + term;
    } else if (type == Tokenizer::T_MINUS) {
        result = partial - term;
    }

    try {
        result = parseT(tokenizer, result);
        DEBUG(fmt("T = %f", term));
    } catch (ParseException &ex) {
        DEBUG("Failed to parse T");
    }


    return result;
}

double parseF(Tokenizer &tokenizer, double partial) {
    DEBUG(fmt("%s %s %d:%d",
            tokenizer.getToken().c_str(),
            tokenizer.getTokenTag(tokenizer.getTokenType()).c_str(),
            tokenizer.lineNumber(),
            tokenizer.linePosition()
            ));

    double result;
    double factor;

    Tokenizer::ValueType type = tokenizer.getTokenType();
    if (type != Tokenizer::T_MULT
            && type != Tokenizer::T_DIV) {
        throw ParseException(
                LOG_MSG(fmt("Expected * or / but got %s (%s) on %d:%d",
                tokenizer.getToken().c_str(),
                tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(),
                tokenizer.linePosition()).c_str()
                ));
    }

    tokenizer.nextToken();
    factor = parseFactor(tokenizer);
    DEBUG(fmt("factor = %f", factor));
    if (type == Tokenizer::T_MULT) {
        result = partial * factor;
    } else if (type == Tokenizer::T_DIV) {
        result = partial / factor;
    }

    try {
        result = parseF(tokenizer, result);
        DEBUG(fmt("F = %f", factor));
    } catch (ParseException &ex) {
        DEBUG("Failed to parse F");
    }

    return result;
}

double parseFactor(Tokenizer &tokenizer) {
    DEBUG(fmt("%s %s %d:%d",
            tokenizer.getToken().c_str(),
            tokenizer.getTokenTag(tokenizer.getTokenType()).c_str(),
            tokenizer.lineNumber(),
            tokenizer.linePosition()
            ));
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
        throw ParseException(
                LOG_MSG(fmt("Expected integer, real or ( but got %s (%s) on %d:%d",
                tokenizer.getToken().c_str(),
                tokenizer.getTokenTag(type).c_str(),
                tokenizer.lineNumber(),
                tokenizer.linePosition()).c_str()
                ));
    }

    tokenizer.nextToken();

    return result;
}

int main(int argc, char** argv) {
    istream *input;

    if (argc <= 1) {
        cout << "Usage: " << argv[0] << " [file|-]" << endl;
        exit(1);
    } else if (!strcmp(argv[1], "-")) {
        input = &cin;
    } else {
        ifstream fileStream(argv[1]);
        if (!fileStream.good()) {
            CRITICAL(fmt("Failed to open input file: %s", strerror(errno)));
        }
        input = &fileStream;
    }


    Logger::setLevel(Logger::ERROR);

    try {
        Tokenizer tokenizer(input);

        tokenizer.
                addKeyword("print").
                addKeyword("exit").
                addKeyword("return");

        tokenizer.nextToken();
        cout << "Value is " << parseExpr(tokenizer) << endl;

        for (tokenizer.nextToken();
                tokenizer.getTokenType() != Tokenizer::T_EOF;
                tokenizer.nextToken()) {
            cout << tokenizer.getToken()
                    << "\t" << tokenizer.getTokenTag(tokenizer.getTokenType()) << '\t'
                    << tokenizer.lineNumber() << '\t'
                    << tokenizer.linePosition() << endl;
        }

    } catch (exception ex) {
        CRITICAL(ex.what());
    }

    return EXIT_SUCCESS;
}

