#include "shield.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// This is just a placeholder for proper binary generation.
static int
generate_binary() {
    const char *src_path = "server", *target_path = "/bin/ft_shield";
    int src_fd = open(src_path, O_RDONLY);
    if (src_fd == -1) {
        __log(stderr, "(open) Error opening %s: %s\n", src_path, strerror(errno));
        return -1;
    }
    int target_fd = open(target_path, 0755);
    if (target_fd == -1) {
        __log(stderr, "(open) Error opening %s: %s\n", target_path, strerror(errno));
        close(src_fd);
        return -1;
    }

    ssize_t bytes_read;
    uint8_t buf[BLOCK_SIZE];
    while ((bytes_read = read(src_fd, buf, sizeof(buf))) != 0) {
        if (bytes_read == -1) {
            __log(stderr, "(read) Error reading from %s (fd %d): %s", src_path, src_fd, strerror(errno));
            close(src_fd);
            close(target_fd);
            return -1;
        }
        if (write(target_fd, buf, bytes_read) != bytes_read) {
            __log(stderr, "(write) Error writing to %s (fd %d): %s", target_path, target_fd, strerror(errno));
            close(src_fd);
            close(target_fd);
            return -1;
        }
    }

    close(src_fd);
    close(target_fd);
    return 0;
}

int
configure_systemd() {
    int fd = open(SYSTEMD_CONFIG_PATH, O_TRUNC | O_CREAT | O_WRONLY);
    if (fd == -1) {
        __log(stderr, "(open) Error opening %s: %s\n", SYSTEMD_CONFIG_PATH, strerror(errno));
        return -1;
    }

    if (write(fd, SYSTEMD_CONFIG, sizeof(SYSTEMD_CONFIG)) != sizeof(SYSTEMD_CONFIG)) {
        if (errno != 0) {
            __log(stderr, "(write) Error writing to %s: %s\n", SYSTEMD_CONFIG_PATH, strerror(errno));
        } else {
            __log(stderr, "(write) Error writing %s\n", SYSTEMD_CONFIG_PATH);
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
    const char *reload_cmd = "systemctl daemon-reload";
    if (system(reload_cmd) == -1) {
        __log(stderr, "(system:%s) Error reloading systemctl: %s\n", reload_cmd, strerror(errno));
        return -1;
    }

    const char *start_cmd = "systemctl start ft_shield";
    if (system(start_cmd) == -1) {
        __log(stderr, "(system:%s) Error starting ft_shield service: %s\n", start_cmd, strerror(errno));
        return -1;
    }
    return 0;
}

int
main() {
    if (getuid() != 0) {
        fprintf(stderr, "Error: ft_shield must be ran as root\n");
        return 1;
    }

    __log(stdout, "DEBUG=1\n");
#if DEBUG == 1
    if (generate_binary() == -1) {
        return 1;
    }
    if (configure_systemd() == -1) {
        return 1;
    }
    if (start_service() == -1) {
        return 1;
    }
#else
    if (generate_binary() != -1 && configure_systemd() != -1 && start_service() != -1) {
    }
#endif

    printf("abied-ch\n");
}
