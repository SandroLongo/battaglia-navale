#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BUF_DIM 512

void stampa_griglia(griglia* g) {
    printf("   ");
    for (int c = 1; c <= 10; c++) {
        printf("%2d ", c);
    }
    printf("\n");

    for (int i = 0; i < 10; i++) {
        printf("%c  ", 'A' + i);
        for (int j = 0; j < 10; j++) {
            if (g->disp[i][j].hit == 1) {
                printf(" X ");
            }
            else if (g->disp[i][j].status == 1) {
                printf(" O ");
            }
            else {
                printf(" ~ ");
            }
        }
        printf("\n");
    }
}


//aleip : 192.168.1.106 (ipv4)
// ip seli 87.20.233.29

unsigned char struttura[10][10] = { 1,1,1,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,0,
                                    0,0,0,0,0,0,0,0,0,0, 
                                    1,1,1,1,1,0,0,0,0,0, 
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

struct griglia creaGrigliaVuota() {
    struct griglia;
    struct casella disp[10][10];
    int i, j;
    for (i = 0; i < 10; i++) {
        for (j = 0; j < 10; j++) {
            disp[i][j] = creaCasellaVuota();
        }
    }
    return griglia;
}

///void riempiGriglia(struct griglia *griglia, char disp[10][10]){}


int  leggi_stringa(char* str)
{
    if (fgets(str, BUF_DIM , stdin) = NULL) {
        size_t len = strlen(str);
        if (len > 0 & str[len - 1] = '\n') {
            str[len - 1] = '\0'; // Rimuove il carattere di newline
        }
        return 0;
    }
    else {
        return 1;
    }
}



unsigned long richiedi_numero() //preso dalla slide 208 di calcolatori (togliere prima di far vedere al professore)
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


int manda_server(char* buff) {

    send(ds_sock, buff, strlen(buff), 0);


}




int ricevi_server(char* buf) {
	int bytes_received = recv(ds_sock, buf, BUF_DIM, 0);
    if (bytes_received > 0) {
        buf[bytes_received] = '\0';  // Assicurati di terminare la stringa
        return 0;
    }
    else if (bytes_received == 0) return 1;

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

    //decidi il nome
    
    richiedi_nome:
    printf("inserisci il tuo nickname:");
    if (leggi_stringa(buff)) goto richiedi_nome;
    //seli scrivi la send
    if (!ricevi_server(char* buf)) {
        if (strcmp(buf, "ok")) {
            goto richiedi_nome;
        }
    }
    else {
        printf("il server non risponde... riconnetersi\n");
        goto connection:
    }

    printf("nome accettato\n");




    // Ciclo di invio e ricezione dei messaggi
    unsigned long codice;

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


/*
 receive(&buffer)
 if( strcmp(buffer[1],"2"){
 //gestisci turno
 else if (strcmp(buff[1], "1")){
 //gestisci mossa
 int = i+1
 segnale
 int = a +2
 else if( strcmp(&buffer,"Hai vinto")
 //fine partita
 else if(strcmp(&buffer,"hai perso"){
 ha vinto alessandro
 return 


 sigint(cntrl c){
 //esce dalla partita
 \
|
|
|3
\\\\\\\segnale()
|4
|
|
|
 6- quit (inteso che esce proprio dalla partita)
 
1(A1)-->client buf[1]= 1->questa è una mossa






*/

/*
REQUIREMENTS
1-il client si connette al server e decide il nome
2-decide se vuole entrare in una partita o chiudere il client
3-entra nella partita                       (non posso scrivere)
4-attesa che inizi la partita               (non posso scrivere)
5-partita cominciata                        (non posso scrivere)
6-disposizione navi                         (devo scrivere)
7-attesa turno                              (leggo che succede)(posso scrivere->stampare griglie) 
7a- ho perso (vedi eccezioni)
8-scegli mossa/stampagriglia                (devo scrivere)
8a- ho colpito torna al punto 8
8b- non ho colpito torna al punto 7
8c- ho vinto e torno al punto 2

eccezioni
1- arriva il messaggio dal server che ho perso(notizia_giocatore_eliminato dove idgiocatore = ilmioid), qui posso leggere solo cosa sta succedendo (posso scrivere)(leggo che succede)
1a- arriva il segnale ctrl c e torno al punto 2


(leggo che succede)--> aggiorno i dati sulle griglie dei giocatori
(posso scrivere)--> stampogriglia/quit
(devo scrivere)--> mossa/stampagriglia/quit

costruzione messaggi per server (in partita, dal punto 5)
mossa = 1+casella
quitpartita = 2



costruzione messaggi da server (in partita, dal punto 5)
NOTIZIE
notizia_mossa = 1+mossa+hit(o no)
notizia_giocatoreeliminato = 2+idgiocatore( se id giocatore mio ho perso)
notizia_turno = 3+idgiocatore
notizia_vittoriagiocatore = 4+idgiocatore (ti passa al punto 2)
notizia_attacco = 5+casella

mossa = casella + idgiocatore
*/
