#ifndef SHIELD_H
#define SHIELD_H

#include <stdio.h>

#define SYSTEMD_CONFIG_PATH "/etc/systemd/system/ft_shield.service"

#define SYSTEMD_CONFIG "[Unit]" \
"Description=None of your business G" \
"After=network.target" \
"StartLimitIntervalSec=0" \
 \
"[Service]" \
"Type=simple" \
"Restart=always" \
"RestartSec=1" \
"User=root" \
"ExecStart=%s" \
\
"[Install]" \
"WantedBy=multi-user.target"

size_t __log(FILE* restrict __stream, const char* restrict __format, ...);


#endif
