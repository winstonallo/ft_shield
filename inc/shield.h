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

size_t __log(FILE *restrict __stream, const char *restrict __format, ...);

int remote_shell(char **env);

void decode_strings(ObfuscatedStringTableEntry *strings, size_t len);

#endif
