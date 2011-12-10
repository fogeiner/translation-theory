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
#define PARSER_ILLEGAL ParserException(fmt("Failed on %d:%d: illegal token %s", _tokenizer->getLineNumber(), _tokenizer->getLinePosition(), _tokenizer->getTokenDescription(_tokenizer->getToken()).c_str()))



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
			assert(_tokenizer != NULL);
			
			_tokenizer->nextToken();
			
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
			throw PARSER_ILLEGAL;
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
		
		if (false 
//			|| match(Tokenizer::T_WHILE)
//			|| match(Tokenizer::T_IF)
			|| match(Tokenizer::T_ID)
//			|| match(Tokenizer::T_INTEGER)
//			|| match(Tokenizer::T_FLOAT)
//			|| match(Tokenizer::T_OPENING_RBRACKET)
//			|| match(Tokenizer::T_PLUS)
//			|| match(Tokenizer::T_MINUS)
//			|| match(Tokenizer::T_TYPE_INT)
//			|| match(Tokenizer::T_TYPE_FLOAT)
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
		TRACE;
		//if (match(Tokenizer::T_WHILE)) {
		//	parseWhile();
		//} else if (match(Tokenizer::T_IF)) {
		//	parseIf();
		//} else 
		if (match(Tokenizer::T_ID)) {
			// this can be: {expression, funcall} or assignment
			// if peek assignment -> parseAssigment
			// else parseExpression
			if (_tokenizer->peekToken == Tokenizer::T_ASSIGNMENT) {
				parseAssignment();
			}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseAssignment() {
		TRACE;
		if (match(Tokenizer::T_ID)) {
			parseId();
			if (!match(Tokenizer::T_ASSIGNMENT)) {
				throw PARSER_EXPECTED(Tokenizer::T_ASSIGNMENT);
			}
			parseExpression();
		} else {
			throw PARSER_EXPECTED(Tokenizer::T_ID);
		}
	}
	
	bool isAtomStart() {
		return match(Tokenizer::T_ID)
			|| match(Tokenizer::T_INT)
			|| match(Tokenizer::T_FLOAT)
			|| match(Tokenizer::T_OPENING_RBRACKET)
			|| match(Tokenizer::T_MINUS)
			|| match(Tokenizer::T_PLUS);
	}
	
	void parseExpression() {
		TRACE;
		if (isAtomStart()) {
				parseterm();
				if (match(Tokenizer::T_PLUS)
					|| match(Tokenizer::T_MINUS)) {
					parseTerm();
				}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseterm() {
		TRACE;
		if (isAtomStart()) {
			parsemult();
			if (match(Tokenizer::T_MULT)
				|| match(Tokenizer::T_DIV)
				|| match(Tokenizer::T_MOD)) {
					parseMult();
			}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseTerm() {
		TRACE;
		if (match(Tokenizer::T_PLUS)) {
			_tokenizer->nextToken();
			parseterm();
			
			if (match(Tokenizer::T_PLUS)
				|| match(Tokenizer::T_MINUS)) {	
				parseTerm();
			}
		} else if (match(Tokenizer::T_MINUS)) {
			_tokenizer->nextToken();
			parseterm();
			
			if (match(Tokenizer::T_PLUS)
				|| match(Tokenizer::T_MINUS)) {	
				parseTerm();
			}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parsemult() {
		TRACE;
		if (isAtomStart()) {
			parseAtom();
			
			if (match(Tokenizer::T_MULT)
				|| match(Tokenizer::T_DIV)
				|| match(Tokenizer::T_MOD)) {
				parseMult();
			}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseMult() {
		TRACE;
		if (match(Tokenizer::T_MULT)) {
			_tokenizer->nextToken();
			parseAtom();
			if (match(Tokenizer::T_MULT)
				|| match(Tokenizer::T_DIV) 
				|| match(Tokenizer::T_MOD)) {
				parseMult();
			}
		} else if (match(Tokenizer::T_DIV)) {
			_tokenizer->nextToken();
			parseAtom();
			if (match(Tokenizer::T_MULT)
				|| match(Tokenizer::T_DIV) 
				|| match(Tokenizer::T_MOD)) {
				parseMult();
			}
		} else if (match(Tokenizer::T_MOD)) {
			_tokenizer->nextToken();
			parseAtom();
			if (match(Tokenizer::T_MULT)
				|| match(Tokenizer::T_DIV) 
				|| match(Tokenizer::T_MOD)) {
				parseMult();
			}
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseAtom() {
		TRACE;
		if (match(Tokenizer::T_ID)) {
		} else if (match(Tokenizer::T_INT)) {
		} else if (match(Tokenizer::T_FLOAT)) {
		} else if (match(Tokenizer::T_OPENING_RBRACKET)) {
		} else if (match(Tokenizer::T_PLUS)) {
		} else if (match(Tokenizer::T_MINUS)) {
		} else {
			throw PARSER_ILLEGAL;
		}
	}
	
	void parseWhile() { // TODO:
		TRACE;
		if (match(Tokenizer::T_WHILE)) {
			_tokenizer->nextToken();
			// parseBexpression
			// match do
			// parseStatements
			// match done
		} else {
			throw PARSER_EXPECTED(Tokenizer::T_WHILE);
		}
	}
	
	void parseIf() { // TODO:
		TRACE;
		if (match(Tokenizer::T_IF)) {
			_tokenizer->nextToken();
			// parseBexpression
			// match then
			// parseStatements
			// if T_FI -> return
			// else if T_ELSE -> parseStatements, match T_FI
			// else throw ILLEGAL
		} else {
			throw PARSER_EXPECTED(Tokenizer::T_IF);
		}
	}
};

#endif
