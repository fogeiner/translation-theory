#ifndef PARSER_H
#define PARSER_H

#include <cassert>
#include "Tokenizer.h"
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

#define PARSER_EXPECTED(expected) ParserException(fmt("Failed on %d:%d: expected %s", _tokenizer->getLineNumber(), _tokenizer->getLinePosition(), _tokenizer->getTokenDescription(expected).c_str()))
#define PARSER_ILLEGAL(error) ParserException(fmt("Failed on %d:%d: illegal token %s", _tokenizer->getLineNumber(), _tokenizer->getLinePosition(), _tokenizer->getTokenDescription(error).c_str()))

class Parser {
private:
	Tokenizer *_tokenizer;
	
	bool match(Tokenizer::ValueType type) {
		if (_tokenizer->getToken() == type) {
			DEBUG(fmt("Matched %s", _tokenizer->getTag().c_str()));
			return true;
		}
		return false;
	}
		
public:
	Parser(Tokenizer *tokenizer):
		_tokenizer(tokenizer) {
			TRACE;
			_tokenizer->nextToken();
			assert(_tokenizer != NULL);
			
			parseProgram();
			
			if (_tokenizer->getToken() != Tokenizer::T_EOF) {
				throw PARSER_EXPECTED(Tokenizer::T_EOF);
			}
	}
	
	void parseProgram() {
		TRACE;
		parseFuncdefs();
	}
	
	void parseFuncdefs() {
		TRACE;
		if (match(Tokenizer::T_DEF)) {
			parseFuncdef();
			parseFuncdefs();
		}
		// eps is ok
	}
	
	void parseFuncdef() {
		TRACE;
		if (match(Tokenizer::T_DEF)) {
			_tokenizer->nextToken();
		
			parseType();
			parseId();
			parseFunargs();
			if (!match(Tokenizer::T_COLON)) {
				throw PARSER_EXPECTED(Tokenizer::T_COLON);
			}
			_tokenizer->nextToken();
		
			// STATEMENTS!
			parseStatements();
		
			if (!match(Tokenizer::T_ENDDEF)) {
				throw PARSER_EXPECTED(Tokenizer::T_ENDDEF);
			}
			_tokenizer->nextToken();
		} else {
			PARSER_EXPECTED(Tokenizer::T_DEF);
		}
	}
	
	void parseType() {
		TRACE;
		if (match(Tokenizer::T_TYPE_INT)) {
			_tokenizer->nextToken();
		} else if (match(Tokenizer::T_TYPE_FLOAT)) {
			_tokenizer->nextToken();
		} else {
			throw PARSER_ILLEGAL(_tokenizer->getToken());
		}
	}
	
	void parseId() {
		TRACE;
		if (match(Tokenizer::T_ID)) {
			_tokenizer->nextToken();
		} else {
			throw PARSER_EXPECTED(Tokenizer::T_ID);
		}
	}
	
	void parseFunargs() {
		TRACE;
		
		if (match(Tokenizer::T_TYPE_INT) 
			|| match(Tokenizer::T_TYPE_FLOAT)) {		
			parseType();
			parseId();
			parseFunargsrest();
		}
		// eps
	}
	
	void parseFunargsrest() {
		TRACE;
		
		if (match(Tokenizer::T_COMMA)) {
			_tokenizer->nextToken();
			parseId();
			parseFunargsrest();
		}
		// eps
	}
	
	void parseStatements() {
		TRACE;
		
		if (
			match(Tokenizer::T_WHILE)
			|| match(Tokenizer::T_IF)
			|| match(Tokenizer::T_ID)
			|| match(Tokenizer::T_INTEGER)
			|| match(Tokenizer::T_FLOAT)
//			|| match(Tokenizer::T_OPENING_RBRACKET)
//			|| match(Tokenizer::T_PLUS)
//			|| match(Tokenizer::T_MINUS)
			|| match(Tokenizer::T_TYPE_INT)
			|| match(Tokenizer::T_TYPE_FLOAT)
//			|| match(Tokenizer::T_RETURN)
//			|| match(Tokenizer::T_PRINT)
//			|| match(Tokenizer::T_READ)
			) {
				parseStatement();
				parseStatements();
			}
			// eps
	}
	
	void parseStatement() {
		if (match(Tokenizer::T_WHILE)) {
			parseWhi
		} else 
	}
};

#endif
