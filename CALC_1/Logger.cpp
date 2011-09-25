#include "Logger.h"

using std::string;
using std::ostream;
using std::cerr;
using std::endl;

Logger Logger::_logger;
std::ostream * Logger::_stream = &cerr;
Logger::ReportingLevel Logger::_level = Logger::ERROR;

void Logger::log(string msg, string level, string file, string function, int line) {
    (*_stream) << level << " "
            << file << ':'
            << function << ':' << line << " "
            << msg << endl;
}

void Logger::setStream(ostream *stream) {
    Logger::_stream = stream;
}

void Logger::setLevel(Logger::ReportingLevel level) {
    _level = level;
}

void Logger::debug(string msg, string file, string function, int line) {
    if (_level <= Logger::DEBUG)
        Logger::log(msg, "DEBUG", file, function, line);
}

void Logger::warn(string msg, string file, string function, int line) {
    if (_level <= Logger::WARN)
        Logger::log(msg, "WARN", file, function, line);
}

void Logger::error(string msg, string file, string function, int line) {
    if (_level <= Logger::ERROR)
        Logger::log(msg, "ERROR", file, function, line);
}


