#include "Logger.h"

using std::string;
using std::ostream;
using std::cerr;
using std::endl;

Logger Logger::_logger;
std::ostream * Logger::_stream = &cerr;
Logger::ReportingLevel Logger::_level = Logger::ERROR;

void Logger::log(string msg, string level, string name, int line) {
    (*_stream) << level << " "
            << name << "[" << line << "] "
            << msg << endl;
}

void Logger::setStream(ostream *stream) {
    Logger::_stream = stream;
}

void Logger::setLevel(Logger::ReportingLevel level) {
    _level = level;
}

void Logger::debug(string msg, string name, int line) {
    if (_level <= Logger::DEBUG)
        Logger::log(msg, "DEBUG", name, line);
}

void Logger::warn(string msg, string name, int line) {
    if (_level <= Logger::WARN)
        Logger::log(msg, "WARN", name, line);
}

void Logger::error(string msg, string name, int line) {
    if (_level <= Logger::ERROR)
        Logger::log(msg, "ERROR", name, line);
}


