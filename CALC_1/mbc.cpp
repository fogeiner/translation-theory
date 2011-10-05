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
#include "SyntaxTree.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istream;
using std::string;
using std::exception;

int main(int argc, char** argv) {
    Logger::setLevel(Logger::ERROR);

    if (argc <= 1) {
        CRITICAL(fmt("Usage: %s [file|-]", argv[0]));
    }

    istream *input = !strcmp(argv[1], "-") ? &cin : new ifstream(argv[1]);

    if (!input->good()) {
        CRITICAL(fmt("Failed to open input: %s", strerror(errno)));
    }

    try {
        Tokenizer tokenizer(*input);
        tokenizer
                .addKeyword("print")
                .addKeyword("return");

        ArithmeticsParser parser(tokenizer);

#ifdef PRINT_TOKENS
        for (tokenizer.nextToken();
                tokenizer.getTokenType() != Tokenizer::T_EOF;
                tokenizer.nextToken()) {
            cout << tokenizer.getToken()
                    << "\t" << tokenizer.getTokenTag(tokenizer.getTokenType()) << '\t'
                    << tokenizer.lineNumber() << '\t'
                    << tokenizer.linePosition() << endl;
        }
#else
        double expressionValue = parser.value();
        cout << "Value is " << expressionValue << endl;
		Node *root = parser.syntaxTree();
		cout << XMLTree(root) << endl;
        
#endif

    } catch (ParseException &ex) {
        ERROR(ex.what());
    } catch (exception &ex) {
        ERROR(ex.what());
    }

    if (input != &cin) {
        delete input;
    }

    return EXIT_SUCCESS;
}
