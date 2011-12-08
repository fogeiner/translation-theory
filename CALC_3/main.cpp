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

        for (tokenizer.nextToken();
                tokenizer.getToken() != Tokenizer::T_EOF;
                tokenizer.nextToken()) {
            cout << tokenizer.getTag()
                    << "\t" << tokenizer.getTokenDescription(tokenizer.getToken()) << '\t'
                    << tokenizer.getLineNumber() << '\t'
                    << tokenizer.getLinePosition() << endl;
        }
    } catch (BufferedStreamException &ex) {
        ERROR(ex.what());
    } catch (exception &ex) {
        ERROR(ex.what());
    }
    
    delete ls;
    
    return EXIT_SUCCESS;
}
