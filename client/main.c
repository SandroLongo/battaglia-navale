#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BUF_DIM 512

typedef struct casella {
    unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
    unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

typedef struct griglia {
    struct casella disp[10][10]; //rappresentazione della griglia come matrice di caselle
    char playerid[21];
    int count;
};

int converti_input(const char* input, int* riga, int* colonna) { 
    if (input[0] < 'A' || input[0] > 'J' ||
        (input[1] < '1' || (input[1] > '9' && !(input[1] == '1' && input[2] == '0'))) || input[2] != '\0')) {
            printf("Formato di input non valido\n");
            return 0;
    }

    *riga = (int)(input[0] - 'A');
    *colonna = (input[1] == '1' && input[2] == '0') ? 9 : atoi(&input[1]) - 1;

    return 1;
}  

int applica_attacco(struct griglia *g, int riga, int colonna) {
    // Controlla se la casella è già stata colpita
    if (g->disp[riga][colonna].hit == 1) {
        printf("Casella già colpita! Riprova.\n");
        return 1;
    }
    else {

        if (g->disp[riga][colonna].status == 1) { // C'è una nave
            g->disp[riga][colonna].hit = 1; // Segna la casella come colpita
            g->count += 1;
            return 2;
        }
        else { // Non c'è una nave
            g->disp[riga][colonna].hit = 1; // Anche se non è colpita, la casella è ora colpita
            return 3;
        }
    }
}


void stampa_griglia(struct griglia* g) {
    printf("%s\n", g->playerid);
    printf("   ");
    for (int c = 1; c <= 10; c++) {
        printf("%2d ", c);
    }
    printf("\n");

    for (int i = 0; i < 10; i++) {
        printf("%c  ", 'A' + i);
        for (int j = 0; j < 10; j++) {
            if (g->disp[i][j].hit == 1) {
                if (g->disp[i][j].status == 1) {  // hit = 1, status = 1 (una nave e colpita)
                    printf(" X ");
                }
                else if {                         // hit = 1, status = 0 (acqua colpita)
                    printf(" Ø ")
                }  
            }
            else if (g->disp[i][j].status == 1) { //hit = 0, status = 1
                printf(" = "); //ce la nostra nave
            }
            else {
                printf(" ~ ");   //hit = 0, status = 0
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


void initializeGrid(struct griglia* g, char *playerid) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            g->disp[i][j].status = 0; // No ship
            g->disp[i][j].hit = 0;    // Not hit
        }
    }
    g->playerid = playerid;
    g->count = 0;
}


int inserisci_nave(struct griglia* griglia, unsigned dim, int riga, int colonna, char dir) {  //dir 1 = N, 2 = S, 3 = W, 4 = E
    int n, s, e, w;
    int new_riga = riga;
    int new_colonna = colonna;
    if (dir == 'n') {
        n = 1;
    }
    else if (dir == 's') {
        s = 1;
    }
    else if (dir == 'w') {
        w = 1;
    }
    else {
        e = 1;
    }
     //controlla se posso

    for (int i= 0; i <dim; i++){
        if (griglia->disp[new_riga][new_colonna].status == 1) {
            return 1;
        }
		new_riga = new_riga + s - n;
		new_colonna = new_colonna + e - w;
    }
	//piazza 
	new_riga = riga;
	new_colonna = colonna;
	for (int i = 0; i < dim; i++) {
        griglia->disp[new_riga][new_colonna].status = 1;
		new_riga = new_riga + s - n;
		new_colonna = new_colonna + e - w;
	}
    return 0;

}



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


int manda_server(int sock,char* buff) {

   int bytes_sent = send(ds_sock, buff, strlen(buff), 0);
   if (bytes_sent == -1) {
       printf("errore durante l'invio dei dati al server\n");
       return -1;
   }
   return 0;
}




int ricevi_server(int sock,char* buf) {
	int bytes_received = recv(sock, buf, BUF_DIM, 0);
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
    char buff_send[BUF_DIM]; //(buff)
    char buff_receive[BUF_DIM]; //(buf)
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
    if (leggi_stringa(buff_send)) goto richiedi_nome;

    if (manda_server(ds_sock, buff_send) == -1) {
        printf("Errore nell'invio del nome al server. Riprova.\n");
        goto richiedi_nome;
    }


    if (!ricevi_server(ds_sock, buff_receive)) {
        if (strcmp(buff, "ok")) {
            goto richiedi_di_giocare;
        }
    }
    else {
        printf("il server non risponde... riconnettersi\n");
        goto connection:
    }

    printf("nome accettato\n");
    char* my_id = buff_send;

richiedi_di_giocare:
    unsigned long scelta;
    do {
        printf("Scegli un'azione:\n1 - Entra in partita\n2 - Chiudi client\nScelta: ");
        scelta = richiedi_numero();

        if (scelta != 1 && scelta != 2) {
            printf("Scelta non valida. Riprova.\n");
        }
    } while (scelta != 1 && scelta != 2);

    // Gestione delle scelte
    if (scelta == 1) {
        printf("Entrando in partita...\n");
        // Invio al server richiesta per entrare in partita

        strcpy(buff_send, "entra_in_partita");
        unsigned int tentativi = 0;
        unsigned int max_tentativi = 3;
        unsigned int controllo;
        do {
            controllo = manda_server(ds_sock, buff_send);
            if (controllo == -1) {
                tentativi++;
                sleep(1);
                printf("tentativo  nr %d di mandare il msg", tentativi);
            }
            else if (controllo == 0) {
                break;
            }



        } while (tentativi < max_tentativi);

        if (tentativi == max_tentativi) {
            printf("impossibile mandare il msg...riconnessione con il server.\n")
                //chiudi_client(ds_sock, EXIT_FAILURE);
                goto connect; //altrimenti possiamo chiudere la connessione con il server (e magari poi tentare di riconnetterci)
        }



    }
    else if (scelta == 2) {
        printf("Chiusura del client...\n");
        chiudi_client(ds_sock, EXIT_SUCCESS);

    }
    /*
    se sono arrivato fino a qui significa che non voglio uscire dal gioco e ho mandato correttamente il msg
    Qui attenderò la risposta del server, per vedere se mi farà entrare o devo rimanere in attesa.
    */

    int controllo;
cerco_di_entrare:   //da mettere controlli
    sleep(1);
    controllo = ricevi_server(ds_sock, buff_receive);

    /*
    il server può mandare che la lobby è gia piena, oppure che sono entrato in lobby e aspettiamo altri giocatori,
    */


    if (strcmp(buff_receive, "entralobby") != 0 || controllo != 0) goto cerco_di_entrare;

    //siamo in partita
    printf("in attesa di altri giocatori...\n");
    controllo = ricevi_server(ds_sock, buff_receive);
    if (strcmp(buff_receive, "disposizione navi") != 0 || controllo != 0) {
        printf("qualcosa è andato storto\n");
        goto cerco_di_entrare;
    }
    //arrivano i nomi dei giocatori
    int num_giocatori;
    receive(ds_sock, num_giocatori);
    char** giocatori = malloc(sizeof(char*) * num_giocatori);
    int k;
    for (k = 0;k < num_giocatori;k++) {
        receive(ds_sock, &buff_receive);
        strcpy(giocatori + k, buff_receive);
        printf("%s ", buff_receive);
    }


    printf("Disponi le tue navi!\n");
    unsigned lista[7] = { 2,2,2,3,3,3,4 };
    char** lista2[7] = { "corvetta", "corvetta", "corvetta", "sottomarino", "sottomarino", "sottomarino", "corazzata" };
    struct griglia my_griglia;
    initializeGrid(&my_griglia, "la mia griglia");
    printf("per disporre le navi inserire la casella di partenza (lettera-numero) e la direzione verso cui la nave va disposta(N(su)E(destra)W(sinistra)S(giù))\n");
    printf("esempio: A1 S");
    int riga, colonna;
    for (int i = 0; i < 7; i++) {
        printf("inserisci %s (%u caselle)\n", lista2[i], lista[i]);
    metti_casella:
        printf("inserisci casella di partenza: ");
        leggi_stringa(buff_receive);   //da mettere controllo
        if (converti_input(&buff_receive, &riga, &colonna) == 1) {
            printf("Scrivere una casella valida.\n");
            goto metti_casella;
        }
        printf("inserisci direzione : ");
    metti_dir:
        leggi_stringa(buff_receive);
        int length = strlen(buff_receive);
        if (buff_receive[0] != 'n' || buff_receive[0] != 's' || buff_receive[0] != 'w' || buff_receive[0] != 'e' || length > 1) {
            printf("Scrivere una direzione valida.\n");
            goto metti_dir;
        }


        if (inserisci_nave(&my_griglia, lista[i], int riga, int colonna, buff_receive[0]) != 0) {
            printf("L'inserimento della nave non è andato a buon fine,perfavore riprovare.\n");
            goto metti_casella;
        }

    }
    send(ds_sock, "completata");
    receive(ds_sock, buff_receive);

    if (strcmp(buff_receive, "iniziopartita")) {
        printf("qualcosa è andato storto\n");
        goto connection;
    }

    //partita inziata ufficialmente
    struct griglia* lista_griglie = malloc(sizeof(struct griglia) * num_giocatori);
    for (int i = 0; i < num_giocatori; i++) {
        initializeGrid(lista_griglie + i, giocatori[i]);
    }
    //analisi di ricezione notizie
    /*
    costruzione messaggi da server (in partita, dal punto 5)
    NOTIZIE
    notizia_mossa = 1+casella+hit(o no)+idgiocatore
    notizia_giocatoreeliminato = 2+idgiocatore( se id giocatore mio ho perso)
    notizia_turno = 3+idgiocatore
    notizia_vittoriagiocatore = 4+idgiocatore (ti passa al punto 2)
    notizia_attacco = 5+casella
    mossa = casella + idgiocatore*/

    int a;
    char* casella;
    int colonna;
    int riga;
    int hit;
    char* idgiocatore;
    char* idgiocatore_corrente;
    char* casella;
    //int turno; //indice gicoatore che ha turno  da fare (sapere chi ha attaccato chi)
ricezione_notizie:
    ricevi_server(ds_sock, buff_receive);
    a = atoi(buff_receive[0]);
    switch (a) {
    case 1:
        ricevi_server(ds_sock, buff_receive); //ci arrvva la casella
        converti_input(buff_receive, &riga, &colonna);
        strcpy(casella, buff_receive) // salviamo la casella dove si vuole attaccare
            ricevi_server(ds_sock, buff_receive); //ci arriva la hit
        hit = atoi(buff_receive);
        ricevi_server(ds_sock, buff_receive); //ci arriva id giocatore
        strcpy(idgiocatore, buff_receive);
        for (int i = 0; i < num_giocaotori; i++) {
            if (strcmp(lista_griglie[i].idgiocatore, idgiocatore) == 0) {
                lista_griglie[i].disp.hit = 1;
                if (hit) {
                    lista_griglie[i].disp.status = 1;
                }
                break;
            }
        }
        printf("%s ha attaccato %s in %s e %sha colpito\n", idgiocatore_corrente, idgiocatore, casella, (hit == 1) ? "non " : "");
        goto ricezione_notizie;
    case 2:
        ricevi_server(ds_sock, buff_receive);
        printf("%s è stato eliminato, bye bye  !\n", buff_receive);
        if (strcmp(buff_receive, my_id) == 0) {
            printf("sei  tu.. :/ \n");
        
        }
        int i;
        for (i = 0; i < num_giocatore; i++) {
            if (strcmp(giocatori[i], buff_receive) == 0) {
                break;
            }
        }
		memmove(lista_griglie + i, lista_griglie + i + 1, (num_giocatori - i - 1) * sizeof(struct griglia));
		memmove(giocatori + i, giocatori + i + 1, (num_giocatori - i - 1) * sizeof(char*));
		num_giocatori -= 1;
        goto ricezione_notizie;

    case 3:
        ricevi_server(ds_sock, buff_receive);
        if (strcmp(buff_receive, my_id) == 0) {
            goto gestione_turno;
        }
        strcpy(idgiocatore_corrente, buff_receive);
        printf("é il turno di %s\n", idgiocatore_corrente);
        goto ricezione_notizie;

    case 4: // la partita è finita
        ricevi_server(ds_sock, buff_receive);
        if (strcmp(buff_receive, my_id) != 0) {
            printf("Il giocatore %s ha vinto!\n");
        }
        else {
            printf("Hai vinto!\n");
        }
		free(liste_griglie);
		free(giocatori);
        goto richiedi_di_giocare;


    case 5:
        int risultato;
        ricevi_server(ds_sock, buff_receive);
        converti_input(buff_receive, &riga, &colonna);
        risultato = applica_attacco(&my_griglia, riga, colonna);
            if (risultato == 2) {
                if (my_griglia.count == 19) { //19 nr totale navi
                    printf("hai perso")!
                        manda_server(ds_sock, "perso");
                }
            }
        printf("Sei stato colpito!\n");

        manda_server(ds_sock, "hit");

            else {
                manda_server(ds_sock, "miss");
                printf("sei stato mancato!\n");
            }

        goto ricezione_notizie;


    default:
        goto ricezione_notizie;
    }



gestione_turno:
    char* giocatore;
    printf("E'il tuo turno!\n");
scelta_azione:
    printf("Scegli cosa vuoi fare.\n");
    printf("1-prepara mossa\n2-stampa griglie\n"); //da mettere anche l'opzione quit
    unsigned long int numero = richiedi_numero(); //da mettere controlli
    if (numero == 1) {
        printf("scegli il giocatore da attaccare:");
        leggi_stringa(buff_send);
        int j;
        for (j = 0; j < num_giocatore; j++) {
            if (strcmp(giocatori[j], buff_send) == 0) {
                goto scelta_attacco;
            
             }
            
        }
        printf("non esiste un giocatore con quel nome\n");
        goto scelta_azione:
    scelta_attacco:
        manda_server(ds_sock, buff_send);
        printf("scegli una casella valida per l'attacco")
            strcpy(giocatore, buff_send);
        leggi_stringa(buff_send);
        if (input[0] < 'A' || input[0] > 'J' || (input[1] < '1' || (input[1] > '9' && !(input[1] == '1' && input[2] == '0'))) || input[2] != '\0')) {
            printf("l'input non è valido\n")
                goto scelta_attacco;
            }
            strcpy(mossa, buff_send);
            converti_input(mossa, &riga, &colonna);
            int i;
            for (i = 0; i < num_giocatore; i++) {
                if (strcmp(lista_griglie[i].idgiocatore, buff_send) == 0) {

                    if (lista_griglie[i].disp[riga][colonna].hit == 1) {
                        printf("la casella è gia stata attaccata\n");
                        goto scelta_attacco;
                    }
                    break;
                }
            }
            
            //la mossa è valida per l'attacco
            manda_server(ds_sock, buff_send);   
            //1 allora è colpito
            //2 allora non é calpito
            //3 giocatore eliminato
            //altro non è colpito
            ricevi_server(ds_sock, buff_receive);
            if (strcmp(buffreceive, "1") == 0) {
                lista_griglie[i].disp[riga][colonna].hit = 1;
                lista_griglie[i].disp[riga][colonna].status = 1;
                printf("Hai colpito una nave!.\n");
                goto scelta_azione;
            }
            else if (strcmp(buff_receive, "2")) {
                lista_griglie[i].disp[riga][colonna].hit = 1;
                lista_griglie[i].disp[riga][colonna].status = 0;
                puts("Il colpo è andato a vuoto");
                goto ricezione_notizie;
                
            }
            else if (strcmp(buff_receive, "3")) { //lista_griglie giocatori e num_giocatori vengono aggiornati
                
				memmove(lista_griglie+i, lista_griglie+i+1, (num_giocatori-i-1)*sizeof(struct griglia));
				memmove(giocatori + i, giocatori + i + 1, (num_giocatori - i - 1) * sizeof(char *));
                num_giocatori -= 1;
                printf("hai affondato l'ultima nave di %s!!! Il giocatore è stato quindi eliminato", giocatore);
                
                goto scelta_azione;
            }
            else if (strcmp(buff_receive, "4")) {
                printf("hai vinto!!!\n");
                free(liste_griglie);
                free(giocatori);

                goto chiedi_di_giocare:
            }
            else {
                goto ricezione_notizie;
            }
            


    }
 
}
       
      
      
      
      
      
      //attesa di altri giocatori
 
    /*
    il server ci dai il permesso "entralobby"  $ 
    il client aspetta $
    il server ci dice che la partita e iniziata "disposizionenavi" $
    il server ci dice in quanti siamo e i nomi dei giocatori avversari  $
    1- comunica il numero di giocatori
    2- manda uno per uno il nome dei giocatori
    il cliente sceglie la disposizione $
    il client comunica al server che ha completato la disposizione ----> manda "completata"  $
    il server comunica ufficialmente l'inizio della partita "iniziopartita" $
    COMINCIA PARTITA
    1-attesa (analizzare le NOTIZIE) $
    2-turno (scegli mossa o altro) $
    ricevi informazioni dal server
    
    */
    
    
   

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
notizia_mossa = 1+mossa+hit(o no)+idgiocatore 
notizia_giocatoreeliminato = 2+idgiocatore( se id giocatore mio ho perso)
notizia_turno = 3+idgiocatore
notizia_vittoriagiocatore = 4+idgiocatore (ti passa al punto 2)
notizia_attacco = 5+casella
*notizia_entralobby =6
mossa = casella + idgiocatore



*/
