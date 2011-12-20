#ifndef BUFFEREDSTREAM_H
#define	BUFFEREDSTREAM_H

#include <exception>
#include <string>

class BufferedStreamException : public std::exception {
private:
    std::string _msg;
public:

    BufferedStreamException(std::string msg) : _msg(msg) {
    }

    ~BufferedStreamException() throw () {
    }

    const char *what() throw () {
        return _msg.c_str();
    }
};

class BufferedStreamUnderflowException : public BufferedStreamException {
public:

    BufferedStreamUnderflowException(std::string msg) : BufferedStreamException(msg) {
    }

    ~BufferedStreamUnderflowException() throw () {
    }

};

class BufferedStream {
private:
    // file descriptor
    int _fd;
    // buffer
    char *_data;
    // start position of data in buffer
    int _spos;
    // end position of data in buffer
    int _epos;
    // true if can unget
    bool _unget;
    bool _eof;
public:
    const static int BUF_SIZE = 4096;

    BufferedStream(std::string filename);
    BufferedStream(int fd);
    ~BufferedStream();

    virtual int get();
    virtual void unget();
    virtual int peek();
    virtual bool eof();
};

#endif	/* BUFFEREDSTREAM_H */

