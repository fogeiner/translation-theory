#include "LocatableStream.h"
#define DEBUG_LOG_OFF
#include "Logger.h"

using std::string;

LocatableStream::LocatableStream(string filename) :
BufferedStream(filename),
_lineNumber(1),
_linePosition(1) {

}

LocatableStream::LocatableStream(int fd) :
BufferedStream(fd),
_lineNumber(1),
_linePosition(1) {

}

int LocatableStream::get() {
    int symbol = BufferedStream::get();
    DEBUG(fmt("get() %d (%c)", symbol, symbol));

    if (symbol == '\n') {
        saveState();
        _lineNumber++;
        _linePosition = 1;
    } else if (symbol == 0) {
        // if EOF do nothing
    } else {
        saveState();
        _linePosition++;
    }

    DEBUG(fmt("(%d,%d)", _lineNumber, _linePosition));

    return symbol;
}

void LocatableStream::unget() {
    BufferedStream::unget();
    restoreState();
}

int LocatableStream::getLineNumber() const {
    return _savedLineNumber;
}

int LocatableStream::getLinePosition() const {
    return _savedLinePosition;
}

