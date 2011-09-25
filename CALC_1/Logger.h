#ifndef LOGGER_H
#define	LOGGER_H

#include <string>
#include <ostream>
#include <iostream>

#ifdef DEBUG_LOG_OFF
#define DEBUG(msg)
#else
#define DEBUG(msg) Logger::debug(msg, __FILE__, __LINE__)
#endif

#define WARN(msg)  Logger::warn (msg, __FILE__, __LINE__)
#define ERROR(msg) Logger::error(msg, __FILE__, __LINE__)
#define CRITICAL(msg) do {Logger::error(msg, __FILE__, __LINE__); exit(1);} while (0)

class Logger {
public:
    // no forward declaration of the enums

    enum ReportingLevel {
        DEBUG,
        WARN,
        ERROR,
        SILENT
    };
private:
    static Logger _logger;

    static std::ostream *_stream;
    static Logger::ReportingLevel _level;

    static void log(std::string msg, std::string level, std::string name, int line);
public:

    static void setStream(std::ostream *stream);
    static void setLevel(Logger::ReportingLevel level);

    static void debug(std::string msg, std::string name, int line);
    static void warn(std::string msg, std::string name, int line);
    static void error(std::string msg, std::string name, int line);
};

#endif

