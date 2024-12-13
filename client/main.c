#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <synchapi.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BUF_DIM 512 


struct casella { 
    unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
    unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

struct griglia {
    struct casella disp[10][10]; //rappresentazione della griglia come matrice di caselle
    char playerid[21];
    int count;
};

	 
int converti_input(const char* input, int* riga, int* colonna) {
	if (strlen(input) < 2 || strlen(input) > 3) {
		printf("Formato di input non valido\n");
		return 0;
	}

	 
	if (input[0] < 'A' || input[0] > 'J') {
		printf("Riga non valida (deve essere tra 'A' e 'J')\n");
		return 0;
	}

	 
	if (input[1] < '1' || input[1] > '9') {
		printf("Colonna non valida\n");
		return 0;
	}

	 
	if (input[1] == '1' && input[2] == '0' && input[3] != '\0') {
		printf("Formato di input non valido\n");
		return 0;
	}

	 
	*riga = input[0] - 'A';   
	*colonna = (input[1] == '1' && input[2] == '0') ? 9 : (input[1] - '1'); 
	return 1; // Input valido
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
                else {                         // hit = 1, status = 0 (acqua colpita)
					printf(" O ");
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


 

void initializeGrid(struct griglia* g, char *playerid) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            g->disp[i][j].status = 0; 
            g->disp[i][j].hit = 0;   
        }
    }
    strcpy(g->playerid , playerid);
    g->count = 0;
}


int inserisci_nave(struct griglia* griglia, unsigned dim, int riga, int colonna, char dir) {  //dir 1 = N, 2 = S, 3 = W, 4 = E
    int n, s, e, w;
    int new_riga = riga;
    int new_colonna = colonna;
    n = 0;
    s = 0;
    e = 0;
    w = 0;
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
        if (new_colonna < 0 || new_colonna > 9 || new_riga < 0 || new_riga > 9) return 1;
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
    if (fgets(str, BUF_DIM , stdin) != NULL) {
        size_t len = strlen(str);
        if (len > 0 & str[len - 1] == '\n') {
            str[len - 1] = '\0'; // Rimuove il carattere di newline
        }
        return 0;
    }
    else {
        return 1;
    }
}



unsigned long richiedi_numero() 
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


void chiudi_client(SOCKET ds_sock, int status)  
{ 
    if ((ds_sock != NULL) & (ds_sock != INVALID_SOCKET)) {   
        closesocket(ds_sock);
    }
    WSACleanup();
    printf("gioco chiuso con successo\n");
    exit(status);
}

SOCKET socket_connection()
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
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");  

    // Connessione al server
    if (connect(ds_sock, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Errore nella connect\n");
        closesocket(ds_sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    return ds_sock;
}


int manda_server(int sock,char* buff) {

   int bytes_sent = send(sock, buff, strlen(buff), 0);
   if (bytes_sent == -1) {
       printf("errore durante l'invio dei dati al server\n");
       return -1;
   }
   return 0;
}




int ricevi_server(int sock,char* buf) {
	int bytes_received = recv(sock, buf, BUF_DIM, 0);
    if (bytes_received > 0) {
        buf[bytes_received] = '\0';   
        return 0;
    }
    else if (bytes_received <= 0) return 1;

}


int main() {
    // Inizializzazione di Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
        printf("Errore nella WSAStartup\n");
        exit(EXIT_FAILURE);
    }
    char buff_send[BUF_DIM];  
    char buff_receive[BUF_DIM];  
    //lancio della connessione
    SOCKET ds_sock;
connection:
    ds_sock = socket_connection();
    if (ds_sock == INVALID_SOCKET)
    {
    richiesta_nuovo_tentativo:
        printf("la connessione con il server è fallita, riprovare?[Y/N]:");
        fgets(buff_receive, BUF_DIM, stdin);
        buff_receive[strcspn(buff_receive, "\n")] = '\0';   
        if (strcmp(buff_receive, "Y") == 0)
        {
            printf("riprovando la connessione...\n");
            goto connection;
        }
        else if (strcmp(buff_receive, "N") == 0)
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


    if (ricevi_server(ds_sock, buff_receive) == 0) {
        if (strcmp(buff_receive, "ok")) {
            printf("buff_receive: %s\n", buff_receive);
            printf("il none scelto è gia esistente o è stato rifiutato. Riprovare...\n");
            goto richiedi_nome;
        }
    }
    else {
        printf("il server non risponde... riconnettersi\n");
        goto connection;
    }

    printf("nome accettato\n");
    char my_id[40];
    strcpy(my_id, buff_send);

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
                Sleep(1);
                printf("tentativo  nr %d di mandare il msg", tentativi);
            }
            else if (controllo == 0) {
                break;
            }



        } while (tentativi < max_tentativi);

        if (tentativi == max_tentativi) {
            printf("impossibile mandare il msg...riconnessione con il server.\n");
             
            goto connection;  
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
cerco_di_entrare:  
    Sleep(1);
    controllo = ricevi_server(ds_sock, buff_receive);

    /*
    il server può mandare che la lobby è gia piena, oppure che sono entrato in lobby e aspettiamo altri giocatori,
    */

    if (strcmp(buff_receive, "entralobby") != 0) {
        goto cerco_di_entrare;
    }
    else if (controllo != 0) {
        exit(EXIT_FAILURE);
    }
    manda_server(ds_sock, "ok");
    //siamo in partita
    printf("in attesa di altri giocatori...\n");
    controllo = ricevi_server(ds_sock, buff_receive);
    if (strcmp(buff_receive, "disposizione navi") != 0 || controllo != 0) {
        printf("qualcosa è andato storto\n");
        
        exit(EXIT_FAILURE);
    }
    manda_server(ds_sock, "ok");
    //arrivano i nomi dei giocatori
    int num_giocatori = 0;
    ricevi_server(ds_sock, buff_receive);
    //riceviamo num giocatori
    num_giocatori = atoi(buff_receive);
    char** giocatori = malloc(sizeof(char*) * num_giocatori);
    int k;
    manda_server(ds_sock, "ok");
   
    //riceviamo nomi giocatori
    for (k = 0;k < num_giocatori;k++) {
        ricevi_server(ds_sock, &buff_receive);
        strcpy(giocatori + k, buff_receive);
        printf("%s\n", giocatori+k);
        manda_server(ds_sock, "ok");
    }

    
    printf("Disponi le tue navi!\n");
    unsigned lista[7] = { 2,2,2,3,3,3,4 };
    char** lista2[7] = { "corvetta", "corvetta", "corvetta", "sottomarino", "sottomarino", "sottomarino", "corazzata" };
    struct griglia my_griglia;
    initializeGrid(&my_griglia, "la mia griglia");
    printf("per disporre le navi inserire la casella di partenza (lettera-numero) e la direzione verso cui la nave va disposta(N(su)E(destra)W(sinistra)S(giu'))\n");
    printf("esempio: A1 S\n");
	stampa_griglia(&my_griglia);
    int riga, colonna;
    for (int i = 0; i < 7; i++) {
        printf("inserisci %s (%u caselle)\n", lista2[i], lista[i]);
    metti_casella:
        printf("inserisci casella di partenza: ");
        leggi_stringa(buff_receive);   
        if (converti_input(&buff_receive, &riga, &colonna) != 1) {
            printf("Scrivere una casella valida.\n");
            goto metti_casella;
        }
        printf("inserisci direzione : ");
    metti_dir:
        leggi_stringa(buff_receive);
        int length = strlen(buff_receive);
        if (buff_receive[0] != 'n' && buff_receive[0] != 's' && buff_receive[0] != 'w' && buff_receive[0] != 'e' && length > 1) {
            printf("Scrivere una direzione valida.\n");
            goto metti_dir;
        }


        if (inserisci_nave(&my_griglia, lista[i], riga, colonna, buff_receive[0]) != 0) {
            printf("L'inserimento della nave non è andato a buon fine,perfavore riprovare.\n");
            goto metti_casella;
        }
        stampa_griglia(&my_griglia);
    }

    strcpy(buff_send, "completata");
    manda_server(ds_sock, buff_send);
    printf("attendendo il posizionamento delle navi da parte degli altri giocatori...\n");
    ricevi_server(ds_sock, buff_receive);
    manda_server(ds_sock, "ok");
    if (strcmp(buff_receive, "iniziopartita") !=0) {
        printf("qualcosa è andato storto\n");
        goto connection;
    }
    printf("la partita e' iniziata ufficialmente\n");
    //partita inziata ufficialmente
    struct griglia* lista_griglie = malloc(sizeof(struct griglia) * num_giocatori);
    for (int i = 0; i < num_giocatori; i++) {
        initializeGrid(lista_griglie + i, giocatori + i);
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

    int hit;
    char idgiocatore[40];
    char idgiocatore_corrente[40];
    char casella[20] ;
    //int turno; //indice gicoatore che ha turno  da fare (sapere chi ha attaccato chi)
ricezione_notizie:
    ricevi_server(ds_sock, buff_receive);
    manda_server(ds_sock, "ok");
    a = atoi(buff_receive);
    switch (a) {
    case 1:
        ricevi_server(ds_sock, buff_receive); //ci arriva la casella
		manda_server(ds_sock, "ok");
        converti_input(buff_receive, &riga, &colonna);
        strcpy(casella, buff_receive); // salviamo la casella dove si vuole attaccare
        ricevi_server(ds_sock, buff_receive); //ci arriva la hit
		manda_server(ds_sock, "ok");
        hit = atoi(buff_receive);
        ricevi_server(ds_sock, buff_receive); //ci arriva id giocatore
        manda_server(ds_sock, "ok");
        strcpy(idgiocatore, buff_receive);
        for (int i = 0; i < num_giocatori; i++) {
            if (strcmp(lista_griglie[i].playerid, idgiocatore) == 0) {
                lista_griglie[i].disp[riga][colonna].hit = 1;
                if (hit) {
                    lista_griglie[i].disp[riga][colonna].status = 1;
                }
                break;
            }
        }
        printf("%s ha attaccato %s in %s e %sha colpito\n", idgiocatore_corrente, idgiocatore, casella, (hit == 1) ? " " : "non ");
        goto ricezione_notizie;
    case 2:
        ricevi_server(ds_sock, buff_receive);
		manda_server(ds_sock, "ok");
        if (strcmp(buff_receive, my_id) == 0) {
            printf("sei stato eliminato :/ \n");

        }
        else {
            printf("%s e' stato eliminato, bye bye !\n", buff_receive);
        }
        int i;
        for (i = 0; i < num_giocatori; i++) {
            if (strcmp(giocatori+i, buff_receive) == 0) {
                break;
            }
        }
        memmove(lista_griglie + i, lista_griglie + i + 1, (num_giocatori - i - 1) * sizeof(struct griglia));
        memmove(giocatori + i, giocatori + i + 1, (num_giocatori - i - 1) * sizeof(char*));
        num_giocatori -= 1;
        goto ricezione_notizie;

    case 3:
        ricevi_server(ds_sock, buff_receive);
		manda_server(ds_sock, "ok");
        if (strcmp(buff_receive, my_id) == 0) {
            goto gestione_turno;
        }
        strcpy(idgiocatore_corrente, buff_receive);
      
        printf("e' il turno di %s\n", idgiocatore_corrente);
        goto ricezione_notizie;

    case 4: // la partita è finita
        ricevi_server(ds_sock, buff_receive);
		manda_server(ds_sock, "ok");
        if (strcmp(buff_receive, my_id) != 0) {
            printf("Il giocatore %s ha vinto!\n", buff_receive);
        }
        else {
            printf("Hai vinto!\n");
        }
        free(lista_griglie);
        free(giocatori);
        goto richiedi_di_giocare;


    case 5:
        int risultato;
        ricevi_server(ds_sock, buff_receive);
        converti_input(buff_receive, &riga, &colonna);
        risultato = applica_attacco(&my_griglia, riga, colonna);
        if (risultato == 2) {
            if (my_griglia.count == 19) { //19 nr totale navi
                printf("hai perso!\n");
                manda_server(ds_sock, "perso");
                goto ricezione_notizie;
            }

            printf("Sei stato colpito!\n");

            manda_server(ds_sock, "hit");

        }
        else {
            manda_server(ds_sock, "miss");
            printf("Sei stato mancato!\n");
        }

        goto ricezione_notizie;


    default:
        goto ricezione_notizie;
    }



gestione_turno:
    char giocatore[40];
    printf("E'il tuo turno!\n");
scelta_azione:
    printf("Scegli cosa vuoi fare.\n");
    printf("1-prepara mossa\n2-stampa griglie\n"); 
    unsigned long int numero = richiedi_numero(); 
    if (numero == 1) {
        printf("scegli il giocatore da attaccare:");
        leggi_stringa(buff_send);
        int j;
        for (j = 0; j < num_giocatori; j++) {
            if (strcmp(giocatori+j, buff_send) == 0) {
                strcpy(giocatore, buff_send);
                goto scelta_attacco;

            }

        }
        printf("non esiste un giocatore con quel nome\n");
        goto scelta_azione;
        scelta_attacco:
        printf("scegli una casella valida per l'attacco:");
        leggi_stringa(buff_send);
        if (buff_send[0] < 'A' || buff_send[0] > 'J' ||
            buff_send[1] < '1' || (buff_send[1] > '9' && !(buff_send[1] == '1' && buff_send[2] == '0') || buff_send[2] != '\0')) {
            printf("l'input non è valido\n");
            goto scelta_attacco;
        }
        //prima di casella c'era mossa    
        strcpy(casella, buff_send);
        converti_input(casella, &riga, &colonna);
        int i;
        for (i = 0; i < num_giocatori; i++) {
            if (strcmp(lista_griglie[i].playerid, giocatore) == 0) {
                if (lista_griglie[i].disp[riga][colonna].hit == 1) {
                    printf("la casella è gia stata attaccata\n");
                    goto scelta_attacco;
                }
                break;
            }
        }
        manda_server(ds_sock, giocatore);//mandiamo il nome da attaccare 
        controllo = ricevi_server(ds_sock, buff_receive);//aspettiamo ok
        if (controllo == 1) {
            printf("il server non risponde\n");
            goto connection;
        }
        //la mossa è valida per l'attacco
        manda_server(ds_sock, casella);
        //1 allora è colpito
        //2 allora non é calpito
        //3 giocatore eliminato
        //altro non è colpito
        ricevi_server(ds_sock, buff_receive);
        if (strcmp(buff_receive, "1") == 0) {
            lista_griglie[i].disp[riga][colonna].hit = 1;
            lista_griglie[i].disp[riga][colonna].status = 1;
            printf("Hai colpito una nave!.\n");
            goto scelta_azione;
        }
        else if (strcmp(buff_receive, "2") == 0) {
            printf("i:%d riga:%d colonna:%d\n ", i, riga, colonna);
            lista_griglie[i].disp[riga][colonna].hit = 1;
            lista_griglie[i].disp[riga][colonna].status = 0;
            puts("Il colpo è andato a vuoto");
            goto ricezione_notizie;

        }
        else if (strcmp(buff_receive, "3") == 0) { //lista_griglie giocatori e num_giocatori vengono aggiornati

            memmove(lista_griglie + i, lista_griglie + i + 1, (num_giocatori - i - 1) * sizeof(struct griglia));
            memmove(giocatori + i, giocatori + i + 1, (num_giocatori - i - 1) * sizeof(char*));
            num_giocatori -= 1;
            printf("hai affondato l'ultima nave di %s!!! Il giocatore è stato quindi eliminato\n", giocatore);

            goto scelta_azione;
        }
        else if (strcmp(buff_receive, "4") == 0) {
            printf("hai vinto!!!\n");
            free(lista_griglie);
            free(giocatori);

            goto richiedi_di_giocare;
        }
        else {
            goto ricezione_notizie;
        }



    }
    else if (numero == 2) {
        for (int i = 0; i < num_giocatori; i++) {   
            stampa_griglia(lista_griglie + i);
        }
        stampa_griglia(&my_griglia);
        goto scelta_azione;
        
    }
    

       

    exit(EXIT_FAILURE);
}


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




