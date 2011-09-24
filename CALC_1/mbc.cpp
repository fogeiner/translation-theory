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

int main(int argc, char** argv) {
    Logger::setLevel(Logger::ERROR);
    ifstream input("data");
    if (!input.good()) {
        ERROR(fmt("Failed to open input file: %s", strerror(errno)));
        return 1;
    }
    try {
        Tokenizer tokenizer(&input);

        tokenizer.
                addKeyword("print").
                addKeyword("exit").
                addKeyword("return");

        for (Tokenizer::ValueType valueType = tokenizer.nextToken();
                valueType != Tokenizer::T_EOF;
                valueType = tokenizer.nextToken()) {
            cout << tokenizer.getStringValue()
                    << "\t" << tokenizer.getTokenTag(valueType) << '\t'
                    << tokenizer.lineNumber() << '\t'
                    << tokenizer.linePosition() << endl;
        }
    } catch (exception ex) {
        ERROR(ex.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

