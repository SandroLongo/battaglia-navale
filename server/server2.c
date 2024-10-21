#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BACKLOG 10
#define BUF_DIM 512

int main() {
    WSADATA wsaData;
    SOCKET server_s, client_s;
    int length;
    struct sockaddr_in server_addr, client_addr;
    char buf[BUF_DIM];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("errore nella WSAStartup\n");
        exit(EXIT_FAILURE);
    }

    server_s = socket(AF_INET, SOCK_STREAM, 0);
    if (server_s == INVALID_SOCKET) {
        printf("errore nella creazione del socket\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORTNUMBER);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_s, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("errore nella bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(server_s, BACKLOG) == SOCKET_ERROR) {
        printf("errore nella listen\n");
        exit(EXIT_FAILURE);
    }

    printf("Server in ascolto su porta %d\n", PORTNUMBER);

    while (1) {
        length = sizeof(client_addr);
        client_s = accept(server_s, (struct sockaddr*)&client_addr, &length);
        if (client_s == INVALID_SOCKET) {
            printf("errore nella accept\n");
            continue;
        }

        printf("connessione effettuata con un client\n");

        do {
            int bytes_received = recv(client_s, buf, BUF_DIM, 0);
            if (bytes_received > 0) {
                buf[bytes_received] = '\0';  // Assicurati di terminare la stringa
                printf("stringa ricevuta: %s\n", buf);
                send(client_s, "letto", sizeof("letto"), 0);
            }
            else {
                printf("errore nella ricezione o client disconnesso\n");
                break;  // Uscire dal ciclo se il client si disconnette
            }
        } while (strcmp(buf, "quit") != 0);

        closesocket(client_s);
        printf("client disconnesso\n");
    }

    closesocket(server_s);
    WSACleanup();
    return 0;
}
