#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SZ 4096

int main() {
    int sock;
    struct sockaddr_in server_addr;
    char sendbuf[BUF_SZ];
    char recvbuf[BUF_SZ];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return 1; }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return 1;
    }

    printf("Connected to server on 127.0.0.1:%d\n", PORT);
    printf("Type messages to send. Type 'quit' to exit.\n");

    while (1) {
        if (!fgets(sendbuf, sizeof(sendbuf), stdin)) break;
        size_t len = strlen(sendbuf);
        if (len == 0) continue;
        if (sendbuf[len-1] == '\n') sendbuf[len-1] = '\0';
        if (strcmp(sendbuf, "quit") == 0) break;

        ssize_t sent = send(sock, sendbuf, strlen(sendbuf), 0);
        if (sent < 0) { perror("send"); break; }

        ssize_t n = read(sock, recvbuf, sizeof(recvbuf) - 1);
        if (n < 0) { perror("read"); break; }
        if (n == 0) {
            printf("Server closed connection\n");
            break;
        }
        recvbuf[n] = '\0';
        printf("Server response: %s\n", recvbuf);
    }

    close(sock);
    return 0;
}
