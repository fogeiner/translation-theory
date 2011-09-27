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

#include "ArithmeticsParser.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istream;
using std::string;
using std::exception;

/*
double parseFactor(Tokenizer &tokenizer) {
    DEBUG(fmt("%s %s %d:%d",
            _token.c_str(),
            _tokenizer.getTokenTag(_type).c_str(),
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
 */

int main(int argc, char** argv) {
    Logger::setLevel(Logger::DEBUG);

    istream *input;

    if (argc <= 1) {
        cout << "Usage: " << argv[0] << " [file|-]" << endl;
        exit(1);
    } else if (!strcmp(argv[1], "-")) {
        input = &cin;
    } else {
        input = new ifstream(argv[1]);
        if (!input->good()) {
            CRITICAL(fmt("Failed to open input file: %s", strerror(errno)));
        }
    }

    try {
        Tokenizer tokenizer(*input);

        tokenizer.addKeyword("print");

        /*
        for (tokenizer.nextToken();
                tokenizer.getTokenType() != Tokenizer::T_EOF;
                tokenizer.nextToken()) {
            cout << tokenizer.getToken()
                    << "\t" << tokenizer.getTokenTag(tokenizer.getTokenType()) << '\t'
                    << tokenizer.lineNumber() << '\t'
                    << tokenizer.linePosition() << endl;
        }*/

        ArithmeticsParser parser(tokenizer);
        cout << "Value is " << parser.parse() << endl;
        
    } catch (exception ex) {
        CRITICAL(ex.what());
    }

    if (input != &cin) {
        delete input;
    }

    return EXIT_SUCCESS;
}

