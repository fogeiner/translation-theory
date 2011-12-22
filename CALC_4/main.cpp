#include <cstdlib>
#include <iostream>
#include <fstream>
#include <istream>
#include <exception>
#include <cassert>
#include <cerrno>
#include <cstring>

#include "Logger.h"
#include "Tokenizer.h"
#include "Parser.h"

using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::istream;
using std::string;
using std::exception;

int main(int argc, char** argv) {
    Logger::setLevel(Logger::SILENT);

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

        Tokenizer *tokenizer = new Tokenizer(*ls);
        Parser *parser = new Parser(tokenizer);
//#define TREE_BUILD_TEST
#ifdef TREE_BUILD_TEST
        cout << parser->getXMLTree() << endl;
#endif
        std::string code = parser->generate();
        cout << code << endl;

//#define TOKENIZER_TEST
#ifdef TOKENIZER_TEST
        for (tokenizer->nextToken();
                tokenizer->getToken() != Tokenizer::T_EOF;
                tokenizer->nextToken()) {
            cout << tokenizer->getTag()
                    << "\t" << tokenizer->getTokenDescription(tokenizer->getToken()) << '\t'
                    << tokenizer->getLineNumber() << '\t'
                    << tokenizer->getLinePosition() << endl;
        }
#endif


        delete parser;
        delete tokenizer;
    } catch (BufferedStreamException &ex) {
        delete ls;
        CRITICAL(ex.what());
    } catch (ParserException &ex) {
        delete ls;
        CRITICAL(ex.what());
    } catch (exception &ex) {
        delete ls;
        CRITICAL(ex.what());
    }

    delete ls;

    return EXIT_SUCCESS;
}
