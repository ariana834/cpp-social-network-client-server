#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};
    char message[1024];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    serv_addr.sin_family = AF_INET;

if (inet_pton(AF_INET,  "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("Invalid address");
    exit(1);
}


    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server.\n");

    while (1) {
        fgets(message, sizeof(message), stdin);
        message[strcspn(message, "\n")] = 0;
        if (strcmp(message, "exit") == 0)
            break;
        send(sock, message, strlen(message), 0);
        memset(buffer, 0, sizeof(buffer));
        int bytes = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) {
            printf("Server disconnected.\n");
            break;
        }
        printf("Server replied: %s\n", buffer);
    }
    close(sock);
    return 0;
}