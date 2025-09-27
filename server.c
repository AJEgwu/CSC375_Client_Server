#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>

#define PORT 8080

void sigchld_handler(int signo) {
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void handle_client(int client_sock) {
    char buffer[1024];
    int n;
    while ((n = read(client_sock, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
        if (strcmp(buffer, "shutdown") == 0) {
            printf("Server shutting down...\n");
            close(client_sock);
            exit(0);
        }
        write(client_sock, "Hello from server", 17);
    }
    close(client_sock);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;

    signal(SIGCHLD, sigchld_handler);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock, 5);

    while (1) {
        addr_size = sizeof(client_addr);
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
        if (fork() == 0) {
            handle_client(client_sock);
            exit(0);
        }
    }
    return 0;
}
