#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <cstdio>
#include <string>
#include <stdarg.h>
#include <cstdlib>
using std::string;

string fmt(const char *fmt, ...) {
    char *c_str;

    va_list ap;
    va_start(ap, fmt);
    vasprintf(&c_str, fmt, ap);
    va_end(ap);

    string str(c_str);
    delete[] c_str;
    return str;
}
