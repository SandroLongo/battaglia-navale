#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BUF_DIM 512

//aleip : 192.168.1.106
// ip seli 87.20.233.29
/*
typedef struct casella {
    unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
    unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

typedef struct griglia {
    casella[10][10] disp; //rappresentazione della griglia come matrice di caselle
};
*/

int main() {
    WSADATA wsaData;
    SOCKET ds_sock;
    struct sockaddr_in server_addr;
    char buff[BUF_DIM];
    char buf[BUF_DIM];

    // Inizializzazione di Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("Errore nella WSAStartup\n");
        return 1;
    }

    // Creazione del socket
    ds_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ds_sock == INVALID_SOCKET) {
        printf("Errore nella creazione del socket\n");
        WSACleanup();
        return 1;
    }

    // Impostazione dell'indirizzo del server
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORTNUMBER);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  // Modifica l'indirizzo se necessario

    // Connessione al server
    if (connect(ds_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore nella connect\n");
        closesocket(ds_sock);
        WSACleanup();
        return 1;
    }

    printf("Connessione effettuata al server\n");

    // Ciclo di invio e ricezione dei messaggi
    do {
        printf("Inserisci un messaggio (o 'quit' per terminare): ");
        fgets(buff, BUF_DIM, stdin);
        buff[strcspn(buff, "\n")] = '\0';  // Rimuovi il newline

        // Invia il messaggio al server
        send(ds_sock, buff, strlen(buff), 0);

        //  risposta dal server
        int bytes_received = recv(ds_sock, buf, BUF_DIM, 0);
        if (bytes_received > 0) {
            buf[bytes_received] = '\0';  // Assicurati di terminare la stringa
            printf("Risposta dal server: %s\n", buf);
        }
    } while (strcmp(buff, "quit") != 0);

    // Chiusura del socket
    closesocket(ds_sock);
    WSACleanup();

    return 0;
}
