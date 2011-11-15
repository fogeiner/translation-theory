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

    LocatableStream *ls = NULL;

    try {
        if (!strcmp(argv[1], "-")) {
            ls = new LocatableStream(0);
        } else {
            ls = new LocatableStream(argv[1]);
        }
        
        Tokenizer tokenizer(*ls);
        tokenizer
                .addKeyword("print")
                .addKeyword("return");

        ArithmeticsParser parser(tokenizer);
#ifdef  PRINT_TOKENS
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

    } catch (BufferedStreamException &ex) {
        ERROR(ex.what());
    } catch (ParseException &ex) {
        ERROR(ex.what());
    } catch (exception &ex) {
        ERROR(ex.what());
    }
    
    delete ls;
    
    return EXIT_SUCCESS;
}
