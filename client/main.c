#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BUF_DIM 512

//aleip : 192.168.1.106 (ipv4)
// ip seli 87.20.233.29

char struttura[10][10] = { 1,1,1,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0,
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0, 
                           0,0,0,0,0,0,0,0,0,0 };

typedef struct casella {
    unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
    unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

typedef struct griglia {
    struct casella disp[10][10]; //rappresentazione della griglia come matrice di caselle
};

struct casella creaCasellaVuota() {
    struct casella new;
    new.hit = 0;
    new.status = 0;
    return new;
}

struct griglia* creaGrigliaVuota() {
    struct griglia;
    struct casella disp[10][10];
    int i, j;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            disp[i][j] = creaCasellaVuota();
        }
    }
}

unsigned richiedi_numero() //preso dalla slide 208 di calcolatori
{
    unsigned long a;
    char buf[BUF_DIM];
    int success;

    do {
        printf("inserisci numero:");
        if (!fgets(buf, BUF_DIM, stdin)) {
            printf("input reading failed\n");
            return 0;
        }
        char* endptr;

        errno = 0;
        a = strtoul(buf, &endptr, 10);
        if (errno == ERANGE) {
            printf("il numero è troppo grande\n");
            return 0;
        }
        else if (endptr == buf) {
            success = 0;
        }
        else if (*endptr && *endptr != '\n') {
            success = 0;
        }
        else {
            success = 1;
        }
    } while (!success);

    return a;
}


void chiudi_client(SOCKET ds_sock, int status) //funzione che chiude il client facendo tutte le cose necessarie senza che le dobbiamo ripetere ogni volta
{ 
    if ((ds_sock != NULL) & (ds_sock != INVALID_SOCKET)) 
    {                              // da pensare se il socket va salvato in una variabile globale per evitare di passarlo come parametro (ma penso che vada bene cosi)
        closesocket(ds_sock);
    }
    WSACleanup();
    printf("gioco chiuso con successo\n");
    exit(status);
}

SOCKET *socket_connection()
{
    SOCKET ds_sock;
    struct sockaddr_in server_addr;
    char buff[BUF_DIM];
    char buf[BUF_DIM];


    // Creazione del socket
    ds_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ds_sock == INVALID_SOCKET) {
        printf("Errore nella creazione del socket\n");
        WSACleanup();
        return INVALID_SOCKET;
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
        return INVALID_SOCKET;
    }

    return &ds_sock;
}

int main() {
    // Inizializzazione di Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("Errore nella WSAStartup\n");
        exit(EXIT_FAILURE);
    }
    char buff[BUF_DIM];
    char buf[BUF_DIM];
    //lancio della connessione
    SOCKET ds_sock;
    connection:
    ds_sock = *socket_connection();
    if (ds_sock == INVALID_SOCKET)
    {
        richiesta_nuovo_tentativo:
        printf("la connessione con il server è fallita, riprovare?[Y/N]:");
        fgets(buff, BUF_DIM, stdin);
        buff[strcspn(buff, "\n")] = '\0';  // Rimuovi il newline
        if (strcmp(buff, "Y") == 0)
        {
            printf("riprovando la connessione...\n");
            goto connection;
        }
        else if (strcmp(buff, "N") == 0)
        {
            printf("chiusura del gioco...\n");
            chiudi_client(ds_sock, EXIT_SUCCESS);
        }
        else 
        {
            printf("risposta non valida ripetere...\n");
            goto richiesta_nuovo_tentativo;
        }

    }
    printf("Connessione effettuata al server\n");

    // Ciclo di invio e ricezione dei messaggi
    unsigned codice;

    while (1)
    {
        //richiesta azione
        richiesta_azione:
        printf("azioni possibili:\n1-azione1\n2-azione2\n3-azione3\n4-azione4\n");
        codice = richiedi_numero();
        switch (codice) {
        case 1:
            printf("azione 1\n"); //sostituire con l'azione voluta
            goto richiesta_azione;
        case 2:
            printf("azione 2\n");
            goto richiesta_azione;
        case 3:
            printf("azione 3\n");
            goto richiesta_azione;
        case 4:
            printf("azione 4\n");
            goto richiesta_azione;
        default:
            printf("immettere un codice azione valido\n");
            goto richiesta_azione;
        }


    }

    /* parte da copiare per la struttura della ricezione dei messaggi
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
    */

    // Chiusura del socket
    chiudi_client(ds_sock, EXIT_SUCCESS);
    return 0;
}
