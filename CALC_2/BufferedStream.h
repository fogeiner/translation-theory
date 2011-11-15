#ifndef BUFFEREDSTREAM_H
#define	BUFFEREDSTREAM_H

#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>
#include <cstring>

#include <string>
#include <exception>

#include <cstdio>

#include "Logger.h"

using std::string;
using std::exception;

class BufferedStreamException : public exception {
private:
    string _msg;
public:

    BufferedStreamException(string msg) : _msg(msg) {
    }

    ~BufferedStreamException() throw () {
    }

    const char *what() throw () {
        return _msg.c_str();
    }
};

class BufferedStreamUnderflowException : public BufferedStreamException {
public:

    BufferedStreamUnderflowException(string msg) : BufferedStreamException(msg) {
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
public:
    const static int BUF_SIZE = 4096;

    BufferedStream(std::string filename) :
    _spos(0),
    _epos(0),
    _unget(false) {
        _fd = open(filename.c_str(), O_LARGEFILE | O_NOCTTY);
        if (_fd == -1) {
            throw BufferedStreamException(strerror(errno));
        }
        _data = new char[BUF_SIZE];
        if (_data == NULL) {
            throw BufferedStreamException(LOG_MSG(strerror(errno)));
        }
    }

    BufferedStream(int fd, int ungetSize = 8) :
    _fd(fd),
    _spos(0),
    _epos(0),
    _unget(false) {
        _data = new char[BUF_SIZE];
        if (_data == NULL) {
            throw BufferedStreamException(LOG_MSG(strerror(errno)));
        }
    }

    ~BufferedStream() {
        close(_fd);
        delete[] _data;
    }

    virtual int get() {
        // Still have something in the buffer?
        if (_spos != _epos) {
            _unget = true;
            return _data[_spos++];
        }
        // Reading from fd to the buffer
        int bytesRead = read(_fd, _data, BUF_SIZE);
        if (bytesRead == -1) { // error
            _unget = false;
            throw BufferedStreamException(LOG_MSG(strerror(errno)));
        } else if (bytesRead == 0) { // EOF
            _unget = true;
            return 0;
        } else {
            _unget = true;
            _epos = bytesRead;
            _spos = 0;
            return _data[_spos++];
        }
    }

    virtual void unget() {
        if (_unget == true) {
            _unget = false;
            _spos--;
        } else {
            throw BufferedStreamUnderflowException(
                    LOG_MSG("BufferedStream unget underflow"));
        }
    }

    virtual int peek() {
        int symbol = get();
        if (symbol != 0)
            unget();
        return symbol;
    }

    virtual bool eof() {
        return peek() == 0;
    }
};

/*
int main3(int argc, char *argv[]) {
    BufferedStream *bs = NULL;
    try {

        bs = new BufferedStream("/etc/passwd");
        while (!bs->eof()) {
            int s;
            s = bs->get();
            printf("%c", s);
        }
        bs->unget();
        printf("%d", bs->get());

    } catch (BufferedStreamException &ex) {
        printf("%s", ex.what());

    }
    delete bs;

}
 */
#endif	/* BUFFEREDSTREAM_H */

