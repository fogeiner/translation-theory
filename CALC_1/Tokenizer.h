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
    std::istream *_stream;
    bool canUnget;
    int lineNumber;
    int linePosition;
    int lastPosition;
    int lastSymbol;
public:

    LocatableStream(std::istream *stream);

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
        T_NUMBER, // 1.0
        T_PLUS, // +
        T_MINUS, // -
        T_MULT, // *
        T_DIV, // /
        T_POWER, // ^
        T_MOD, // %
        T_EQUALS, // =
        T_EOL, // \n
        T_SEMICOLON, // ;
        T_OPENING_RBRACKET, // (
        T_CLOSING_RBRACKET, // )
        T_OPENING_CBRACKET, // {
        T_CLOSING_CBRACKET, // }
        T_EOF, // getc = 0
        T_KEYWORD // print, return etc 
    };

    static std::map<ValueType, std::string> _valueTypeTags;

private:
    LocatableStream _stream;
    Tokenizer::ValueType _type;
    std::vector<std::string> _keywords;
    std::string _token;
public:

    Tokenizer(std::istream *stream);

    Tokenizer &addKeyword(const std::string keyword);
    Tokenizer::ValueType nextToken();
    double getNumericValue() const;
    std::string getStringValue() const;

    int lineNumber() const;
    int linePosition() const;

    const std::string getTokenTag(ValueType valueTy) const;
};



#endif	/* TOKENIZER_H */

