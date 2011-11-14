#ifndef ARITHMETICSPARSER_H
#define	ARITHMETICSPARSER_H

#include "Tokenizer.h"
#include "Logger.h"
#include "SyntaxTree.h"

#include <exception>
#include <string>
#include <cmath>
#include <utility>

#define ARITHMETICS_PARSER_DEBUG DEBUG(fmt("%s %s %d:%d",\
            _token.c_str(),\
            _tokenizer.getTokenTag(_type).c_str(),\
            _tokenizer.lineNumber(),\
            _tokenizer.linePosition()\
            ))

using std::string;
using std::exception;
using std::pair;

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

/** 
 * term1 -> term2 T1 | term2
 * term2 -> term3 T2 | term3
 * term3 -> termN T3 | termN
 * termN -> +termN | -termN | number | (term1)
 * T1 -> +- term2 T1 | +- term2 
 * T2 -> /*% term3 T2 | /*% term3
 * T3 -> ^ T3 termN | ^ termN
 * number -> real | integer 
 */
class ArithmeticsParser {
	typedef pair<double, Node*> ParseData;
private:
    Tokenizer &_tokenizer;

    Tokenizer::ValueType _type;
    string _token;
    int _lineNumber;
    int _linePosition;

	double _value;
	Node *_root;
	
    ParseData parseTerm1() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        Node *node = NodeFactory::create(N_TERM1);
        
        ParseData term2 = parseTerm2();
        result = term2.first;
        node->addChild(term2.second);
        
        parseT1(&result, node);
        return ParseData(result, node);
    }

    ParseData parseTerm2() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
		Node *node = NodeFactory::create(N_TERM2);
        
        ParseData term3 = parseTerm3();
		result = term3.first;
        node->addChild(term3.second);

        try {
            parseT2(&result, node);
        } catch (ParseException &ex) {
        }

        return ParseData(result, node);
    }

    ParseData parseTerm3() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
		Node *node = NodeFactory::create(N_TERM3);
        
        ParseData termn = parseTermN();
		result = termn.first;
        node->addChild(termn.second);

        try {
            parseT3(&result, node);
        } catch (ParseException &ex) {
        }

        return ParseData(result, node);
    }

    void parseT1(double *resultValue, Node *resultNode) {
        ARITHMETICS_PARSER_DEBUG;
        ParseData term;
        Node *node = NodeFactory::create(N_T1);
        Tokenizer::ValueType type = _type;

        if (type == Tokenizer::T_PLUS
                || type == Tokenizer::T_MINUS) {
			if (type == Tokenizer::T_PLUS) {
				node->addChild(NodeFactory::create(N_PLUS));
			} else {
				node->addChild(NodeFactory::create(N_MINUS));
			}
            nextToken();

            term = parseTerm2();
            
            if (type == Tokenizer::T_PLUS) {
                *resultValue += term.first;
            } else {
                *resultValue -= term.first;
            }
            
            node->addChild(term.second);
            resultNode->addChild(node);
            
            try {
                parseT1(resultValue, node);
            } catch (ParseException &ex) {
            }
        }

    }

    void parseT2(double *resultValue, Node *resultNode) {
        ARITHMETICS_PARSER_DEBUG;
        ParseData term;
        Node *node = NodeFactory::create(N_T2);
        Tokenizer::ValueType type = _type;

        if (type == Tokenizer::T_MULT
                || type == Tokenizer::T_DIV
                || type == Tokenizer::T_MOD) {
			if (type == Tokenizer::T_MULT) {
				node->addChild(NodeFactory::create(N_MULTIPLICATION));
			} else if (type == Tokenizer::T_DIV) {
				node->addChild(NodeFactory::create(N_DIVISION));
			} else {
				node->addChild(NodeFactory::create(N_MOD));
			}
			
            nextToken();

            term = parseTerm3();
            
            if (type == Tokenizer::T_MULT) {
                *resultValue *= term.first;
            } else if (type == Tokenizer::T_DIV){
                *resultValue /= term.first;
            } else {
				*resultValue = mod(*resultValue, term.first);
			}
            
            node->addChild(term.second);
            resultNode->addChild(node);
            
            try {
                parseT2(resultValue, node);
            } catch (ParseException &ex) {
            }
        }

    }
    
    void parseT3(double *resultValue, Node *resultNode) {
        ARITHMETICS_PARSER_DEBUG;
        ParseData term;
		Node *node = NodeFactory::create(N_T3);
		
        Tokenizer::ValueType type = _type;
        if (type == Tokenizer::T_POWER) {
			node->addChild(NodeFactory::create(N_POWER));
			
            nextToken();
            term = parseTermN();
            node->addChild(term.second);
            resultNode->addChild(node);

            try {
                parseT3(resultValue, node);
            } catch (ParseException &ex) {
            }

            *resultValue = pow(*resultValue, term.first);
        }
    }

    ParseData parseTermN() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        Node *node = NodeFactory::create(N_TERMN);
        if (_type == Tokenizer::T_OPENING_RBRACKET) {
			node->addChild(NodeFactory::create(N_OPENING_RBRACKET));
            
            nextToken();
            
            ParseData term1 = parseTerm1();
            result = term1.first;
            node->addChild(term1.second);
            
            if (_type != Tokenizer::T_CLOSING_RBRACKET) {
				delete node;
				
                throw ParseException(
                        LOG_MSG(
                        fmt("Expected ')' but got '%s' on %d:%d",
                        _token.c_str(), _lineNumber, _linePosition).c_str()
                        ));
            }
            node->addChild(NodeFactory::create(N_CLOSING_RBRACKET));
            nextToken();
        } else if (_type == Tokenizer::T_PLUS) {
			node->addChild(NodeFactory::create(N_PLUS));
            
            nextToken();
            
            ParseData termn = parseTermN();
            result = +termn.first;
            node->addChild(termn.second);
        } else if (_type == Tokenizer::T_MINUS) {
   			node->addChild(NodeFactory::create(N_MINUS));
            
            nextToken();
            
            ParseData termn = parseTermN();
            result = -termn.first;
            node->addChild(termn.second);
        } else {
            ParseData number = parseNumber();
            result = number.first;
            node->addChild(number.second);
        }
        return ParseData(result, node);
    }

    ParseData parseNumber() {
        ARITHMETICS_PARSER_DEBUG;
        double result;
        Node *node;
        if (_type == Tokenizer::T_REAL) {
            result = atof(_token.c_str());
            node = NodeFactory::create(N_REAL, _token);
        } else if (_type == Tokenizer::T_INTEGER) {
            result = atoi(_token.c_str());
            node = NodeFactory::create(N_INTEGER, _token);
        } else {
            throw ParseException(
                    LOG_MSG(fmt("Expected number but got '%s' on %d:%d",
                    _token.c_str(), _lineNumber, _linePosition).c_str()
                    ));
        }
        nextToken();
        return ParseData(result, node);
    }

    double mod(double x, double y) {
        if (y < 0) {
            return mod(-x, -y);
        } else {
            double result = x - int(x / y) * y;
            if (result < 0) {
                result += y;
            }
            return result;
        }
    }

    void nextToken() {
        ARITHMETICS_PARSER_DEBUG;
        _tokenizer.nextToken();
        _type = _tokenizer.getTokenType();
        _token = _tokenizer.getToken();
        _lineNumber = _tokenizer.lineNumber();
        _linePosition = _tokenizer.linePosition();
    }
public:

    ArithmeticsParser(Tokenizer &tokenizer) :
    _tokenizer(tokenizer),
    _value(0.0),
    _root(NULL) {
        ARITHMETICS_PARSER_DEBUG;
        
		NodeFactory::defaultInit();
		
		nextToken();
        ParseData result = parseTerm1();
        _value = result.first;
        _root = result.second;
        
        if (_type != Tokenizer::T_EOF) {
            throw ParseException(
                    LOG_MSG(fmt("Unparsed token '%s' found on %d:%d",
                    _token.c_str(), _tokenizer.lineNumber(),
                    _tokenizer.linePosition()).c_str()
					));
        }
    
    }
    
    ~ArithmeticsParser() {
        delete _root;
    }

    double value() const {
        return _value;
    }
    
    Node *syntaxTree() const {
		return _root;
	}

};

// abstraction is parsing the expression; rules are all 
// the same but the operations are different
#endif	/* ARITHMETICSPARSER_H */

