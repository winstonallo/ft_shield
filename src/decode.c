
#include "shield.h"
#include <stdio.h>

void
decode_strings(ObfuscatedStringTableEntry *strings, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        if (strings[i].decoded) {
            continue;
        }
        for (size_t j = 0; j < strings[i].len - 1; ++j) {
            strings[i].data[j] ^= 0x42;
        }
        strings[i].decoded = true;
    }
}
