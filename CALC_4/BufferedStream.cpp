#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include "BufferedStream.h"
#include "Logger.h"
using std::string;
using std::exception;


BufferedStream::BufferedStream(std::string filename) :
	_spos(0),
	_epos(0),
	_unget(false),
	_eof(false) {
		_fd = open(filename.c_str(), O_LARGEFILE | O_NOCTTY);
		if (_fd == -1) {
			throw BufferedStreamException(LOG_MSG(strerror(errno)));
		}
		_data = new char[BUF_SIZE];
		if (_data == NULL) {
			throw BufferedStreamException(LOG_MSG(strerror(errno)));
		}
	}

BufferedStream::BufferedStream(int fd) :
	_fd(fd),
	_spos(0),
	_epos(0),
	_unget(false),
	_eof(false) {
		_data = new char[BUF_SIZE];
		if (_data == NULL) {
			throw BufferedStreamException(LOG_MSG(strerror(errno)));
		}
	}

BufferedStream::~BufferedStream() {
	close(_fd);
	delete[] _data;
}

int BufferedStream::get() {
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
		_eof = true;
		return 0;
	} else {
		_unget = true;
		_epos = bytesRead;
		_spos = 0;
		return _data[_spos++];
	}
}

void BufferedStream::unget() {
	if (_unget == true) {
		_unget = false;
		_spos--;
	} else {
		throw BufferedStreamUnderflowException(
				LOG_MSG("BufferedStream unget underflow"));
	}
}

int BufferedStream::peek() {
	int symbol = get();
	if (symbol != 0)
		unget();
	return symbol;
}

bool BufferedStream::eof() {
	return _eof;
}

