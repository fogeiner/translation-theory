#ifndef PARSER_H
#define PARSER_H

#include <cassert>
#include "Logger.h"

class ParserException : public std::exception {
private:
    std::string _msg;
public:

    ParserException(std::string msg) : _msg(msg) {
    }

    ~ParserException() throw () {
    }

    const char *what() throw () {
        return _msg.c_str();
    }
};

class Parser {
private:
	Tokenizer *_tokenizer;
	
public:
	Parser(Tokenizer *tokenizer):
		_tokenizer(tokenizer) {
			DEBUG("Parser::Parser");
			assert(_tokenizer != NULL);
	}
};

#endif
