#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "shield.h"


int configure_systemd() {
    int fd = open(SYSTEMD_CONFIG_PATH, O_TRUNC | O_CREAT | O_WRONLY);
    if (fd == -1) {
        __log(stderr, "Could not open %s: %s\n", SYSTEMD_CONFIG_PATH, strerror(errno));
        return -1;
    }

    if (write(fd, SYSTEMD_CONFIG, sizeof(SYSTEMD_CONFIG)) != sizeof(SYSTEMD_CONFIG)) {
        if (errno != 0) {
            __log(stderr, "Error writing to %s: %s\n", SYSTEMD_CONFIG_PATH, strerror(errno));
        } else {
            __log(stderr, "Unknow error opening %s\n", SYSTEMD_CONFIG_PATH);
        }
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int main() {
    __log(stdout, "DEBUG=1\n");
    configure_systemd();
    printf("abied-ch\n");
}
