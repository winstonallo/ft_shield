#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(int ac, char**av, char**env) {
    (void)ac;
    (void)av;
    int server_fd, client_fd;
    struct sockaddr_in address;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(4242);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        fprintf(stderr, "Error binding to fd %d: %s", server_fd, strerror(errno));
        return 1;
    }
    if (listen(server_fd, 1) == -1) {
        fprintf(stderr, "Error listening on server fd fd %d: %s", server_fd, strerror(errno));
        return 1;
    }
    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        fprintf(stderr, "Error accepting client: %s", strerror(errno));
        return 1;
    }
    if (dup2(client_fd, 0) == -1) {
        fprintf(stderr, "Error duplicating stdin to client fd %d: %s", client_fd, strerror(errno));
        return 1;
    }
    if (dup2(client_fd, 1) == -1) {
        fprintf(stderr, "Error duplicating stdout to client fd %d: %s", client_fd, strerror(errno));
        return 1;
    }
    if (dup2(client_fd, 2) == -1) {
        fprintf(stderr, "Error duplicating stderr to client fd %d: %s", client_fd, strerror(errno));
        return 1;
    }

    char *args[2] = {"/bin/bash", NULL};
    execve("/bin/bash", args, env);

    return 0;
}
