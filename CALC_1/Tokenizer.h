#ifndef TOKENIZER_H
#define	TOKENIZER_H

#include <exception>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <map>

class TokenizerException : public std::exception {
private:
    const std::string _msg;
public:

    TokenizerException(std::string msg);
    ~TokenizerException() throw ();
    const char *what() const throw ();
};

class LocatableStream {
private:
    std::istream &_stream;
    bool _canUnget;
    int _lineNumber;
    int _linePosition;
    int _lastPosition;
    int _lastSymbol;
public:

    LocatableStream(std::istream &stream);

    int get();
    void unget();
    bool eof() const ;
    int getLineNumber() const;
    int getLinePosition() const;
};

class Tokenizer {
public:

    enum ValueType {
        T_UNDEFINED, // ?
        T_WORD, // abc
        T_INTEGER, // 1
        T_REAL, // 1.0
        T_PLUS, // +
        T_MINUS, // -
        T_MULT, // *
        T_DIV, // /
        T_POWER, // ^
        T_MOD, // %
        T_EQUALS, // =
        T_SEMICOLON, // ;
        T_OPENING_RBRACKET, // (
        T_CLOSING_RBRACKET, // )
        T_OPENING_CBRACKET, // {
        T_CLOSING_CBRACKET, // }
        T_EOF, // getc == EOF
        T_KEYWORD // print, return etc 
    };

    static std::map<ValueType, std::string> _valueTypeTags;

private:
    LocatableStream _stream;
    Tokenizer::ValueType _type;
    std::vector<std::string> _keywords;
    std::string _token;
public:

    Tokenizer(std::istream &stream);
    virtual ~Tokenizer();
    Tokenizer &addKeyword(const std::string keyword);
    void nextToken();
    Tokenizer::ValueType getTokenType();
    std::string getToken() const;

    int lineNumber() const;
    int linePosition() const;

    const std::string getTokenTag(ValueType valueType) const;
};



#endif	/* TOKENIZER_H */

