#ifndef LOCATABLESTREAM_H
#define	LOCATABLESTREAM_H
#include "BufferedStream.h"

class LocatableStream : public BufferedStream {
private:
    int _lineNumber;
    int _linePosition;
    int _savedLineNumber;
    int _savedLinePosition;
    int _savedSavedLineNumber;
    int _savedSavedLinePosition;

    void saveState() {
        _savedSavedLineNumber = _savedLineNumber;
        _savedSavedLinePosition = _savedLinePosition;

        _savedLineNumber = _lineNumber;
        _savedLinePosition = _linePosition;
    }

    void restoreState() {
        _lineNumber = _savedLineNumber;
        _linePosition = _savedLinePosition;
        
        _savedLineNumber = _savedSavedLineNumber;
        _savedLinePosition = _savedSavedLinePosition;
    }
public:

    LocatableStream(std::string filename);
    LocatableStream(int fd);
    int get();
    void unget();
    int getLineNumber() const;
    int getLinePosition() const;
};

#endif	/* LOCATABLESTREAM_H */

