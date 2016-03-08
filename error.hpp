#ifndef ERROR_H
#define ERROR_H

//#include <cstdlib>
#include <cstdarg>
#include <iostream>
#include <cstring>
#include <cstdio>

#define ERR_BUFFER_SIZE (256)


#define WARN warn
#define ERROR error




inline void warn(const char *file, int line, const char * msg, ...)
{
    char out[ERR_BUFFER_SIZE];
    va_list args;

    va_start(args, msg);
    vsprintf(out, msg, args);
    va_end(args);

    std::cerr << "[WARNING]  <" << file << ":l" << line << ">  " <<
         out << std::endl;
}


inline void error(const char *file, int line, const char * msg, ...)
{
    char out[ERR_BUFFER_SIZE];
    va_list args;

    va_start(args, msg);
    vsprintf(out, msg, args);
    va_end(args);

    std::cerr << "[ERROR]  <" << file << ":l" << line << ">  " <<
         out << std::endl;
}

#endif
