#ifndef SHIELD_H
#define SHIELD_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define ACTIVATION_KEY_LEN 64

typedef struct ObfuscatedStringTableEntry {
    uint8_t *data;
    size_t len;
    bool decoded;
} ObfuscatedStringTableEntry;

int remote_shell(char **env);

void decode_strings(ObfuscatedStringTableEntry *strings, size_t len);

#include <stdarg.h>

#if DEBUG == 1
#define __log(__stream, __format, ...)                                                                                                                         \
    do {                                                                                                                                                       \
        fprintf(__stream, __format, ##__VA_ARGS__);                                                                                                            \
    } while (0)
#else
#define __log(__stream, __format, ...)                                                                                                                         \
    do {                                                                                                                                                       \
        (void)(__stream);                                                                                                                                      \
    } while (0)
#endif

#endif
