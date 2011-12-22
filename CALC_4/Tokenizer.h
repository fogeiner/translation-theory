#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <exception>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include "LocatableStream.h"

class TokenizerException : public std::exception {
private:
    const std::string _msg;
public:

    TokenizerException(std::string msg);
    ~TokenizerException() throw ();
    const char *what() const throw ();
};

class Tokenizer {
public:

    enum ValueType {
        T_UNDEFINED, // ?
        T_ID, // abc
        T_INTEGER, // 1
        //T_FLOAT, // 1.0
        T_PLUS, // +
        T_MINUS, // -
        T_MULT, // *
        T_DIV, // /
        //T_POWER, // ^
        T_MOD, // %
        T_ASSIGNMENT, // =
        T_SEMICOLON, // ;
        T_OPENING_RBRACKET, // (
        T_CLOSING_RBRACKET, // )
        T_OPENING_CBRACKET, // {
        T_CLOSING_CBRACKET, // }
        T_EOF, // getc == EOF
        T_TYPE_INT, // int
        //T_TYPE_FLOAT, // float
        T_PRINT, // print
        T_READ, // read
		T_FOR, // for
        T_WHILE, // while
        T_IF, // if
        T_ELSE, // else
        T_LESS, // <
        T_LESS_OR_EQUAL, // <=
        T_GREATER, // >
        T_GREATER_OR_EQUAL, // >=
        //T_SHIFT_LEFT, // <<
        //T_SHIFT_RIGHT, // >>
        T_EQUAL, // ==
        T_NOT_EQUAL, // !=
        T_OPENING_SBRACKET, // [
        T_CLOSING_SBRACKET, // ]
        T_AND, // and
        T_OR, // or
        T_NOT, // not OR !
        T_FALSE, // false
        T_TRUE, // true
        T_RETURN, // return
        //T_DOT, // .
        T_DEF,
        T_COLON,
        T_ENDDEF,
        T_COMMA,
        T_DO,
        T_DONE,
        T_THEN,
        T_FI,
    };

    static std::map<ValueType, std::string> _valueTypeTags;

private:
    LocatableStream &_stream;
    Tokenizer::ValueType _type;
    std::string _tag;

    bool _peeking;
    std::string _saved_tag;
    Tokenizer::ValueType _saved_type;
    int _saved_line_number;
    int _saved_line_position;
public:

    Tokenizer(LocatableStream &stream);
    virtual ~Tokenizer();
    Tokenizer::ValueType peekToken();
    Tokenizer::ValueType nextToken();
    Tokenizer::ValueType getToken();
    std::string getTag() const;
    std::string peekTag();

    int getLineNumber() const;
    int getLinePosition() const;

    int peekLineNumber();
    int peekLinePosition();

    std::string getTokenDescription(ValueType valueType) const;
};



#endif  /* TOKENIZER_H */

