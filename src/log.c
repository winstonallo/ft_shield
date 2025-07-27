#include <stdarg.h>
#include <stdio.h>

size_t __log(__attribute__((unused))FILE* restrict __stream, __attribute__((unused))const char* restrict __format, ...) {
    int res = 0;
    #if DEBUG == 1
    va_list args;
    va_start(args, __format);
    res = vfprintf(__stream, __format, args);
    va_end(args);
    #endif
    return res;
}
