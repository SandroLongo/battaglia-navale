#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

#define PORTNUMBER 25123
#define BACKLOG 10
#define BUF_DIM 512
#define NUM_MUTEX 10


//dynamic vector
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

struct array;

extern void* init_vector(size_t nmemb, size_t size);//inizializza il vettore
extern bool insert_at(struct array* arr, size_t pos, void* el);//inserisce elemento in uno slot a piacere
extern bool push_back(struct array* arr, void* el);//aggiunge elemento alla fine
extern bool read_from(struct array* arr, size_t pos, void* buffer);//leggere valore
extern bool extract_from(struct array* arr, size_t pos, void* buffer);//elimina un elemento
extern void free_vector(struct array* arr);//free di tutto

struct array {
	size_t count; // currently used slots
	size_t slots; // total slots 
	size_t size; // size of a slot
	void* arr; // the actual array
};

static inline bool resize(struct array* arr, size_t new_size)
{
	assert(arr->count < new_size);
	arr->arr = realloc(arr->arr, new_size * arr->size);
	if (!arr->arr)
		return false;
	arr->slots = new_size;
	return true;
}

void* init_vector(size_t nmemb, size_t size)
{
	struct array* ret;
	void* p;

	// Check for overflow
	if (size && nmemb > (size_t)-1 / size) {
		errno = ENOMEM;
		return 0;
	}

	p = malloc(size * nmemb);
	if (!p)
		return p;
	memset(p, 0, size);

	ret = malloc(sizeof(struct array));
	if (!ret) {
		free(p);
		return ret;
	}

	ret->count = 0;
	ret->slots = nmemb;
	ret->size = size;
	ret->arr = p;

	return ret;
}

void free_vector(struct array* arr)
{
	free(arr->arr);
	free(arr);
}

bool push_back(struct array* arr, void* el)
{
	return insert_at(arr, arr->count, el);
}

bool insert_at(struct array* arr, size_t pos, void* el)
{
	char* from, * to;
	size_t size;

	if (pos > arr->count)
		return false;

	if (arr->count + 1 > arr->slots)
		if (!resize(arr, arr->slots * 2))
			return false;

	from = (char*)arr->arr + pos * arr->size;
	if (pos + 1 < arr->slots) {
		to = from + arr->size;
		size = (arr->count - pos) * arr->size;
		memmove(to, from, size);
	}

	memcpy(from, el, arr->size);

	arr->count++;
	assert(arr->count <= arr->slots);
	return true;
}

bool read_from(struct array* arr, size_t pos, void* buffer)
{
	if (pos < arr->count) {
		memcpy(buffer, (char*)arr->arr + pos * arr->size, arr->size);
		return true;
	}

	return false;
}

bool extract_from(struct array* arr, size_t pos, void* buffer) 
{
	char* from, * to;
	size_t size;

	if (!read_from(arr, pos, buffer))
		return false;

	if (pos + 1 < arr->slots) {
		from = (char*)arr->arr + (pos + 1) * arr->size;
		to = from - arr->size;
		size = (arr->count - pos) * arr->size;
		memmove(to, from, size);
	}

	arr->count--;
	if (arr->count < arr->slots / 2)
		resize(arr, arr->slots / 2);
	return true;
}

typedef	struct {
	SOCKET server_s;
} Parametri_client;

typedef struct {
	int codice;
	char casella[5];
	int hit;
	char idgiocatore[20];
	int num_giocatori;
	char** giocatori;

} Sharednotizie;

typedef struct {
	int codice;
	char buff[BUF_DIM];

} Sharedrichieste;

//VARIABILI GLOBALI
struct array* nomi; //ARRAY DI NOMI(mutex)
bool iniziata;//bool partita iniziata(o no) 
// (mutex)
HANDLE mutex[NUM_MUTEX];
Sharednotizie* notizie;//area di mem per scambio messaggi(notizie) (mutex)
Sharedrichieste* richieste; //area di mem per scambio messaggi(richieste dei thread) (mutex)
HANDLE hnotizie;
HANDLE hrichieste;
HANDLE comunicazione_pronta, comunicazione_letta, partita_iniziata;
HANDLE richiesta_pronta, richiesta_scrivi, mutex_controllo;
SOCKET server_s; //il descrittore socket socket
//funzioni dei thread


//funzione del thread_client

int manda_client(SOCKET sock, char* buff) {

	size_t bytes_sent =   send(sock, buff, strlen(buff), 0);
	if (bytes_sent == -1) {
		printf("errore durante l'invio dei dati al server\n");
		return -1;
	}
	return 0;
}
int ricevi_client(SOCKET sock, char* buf) {
	int bytes_received = recv(sock, buf, BUF_DIM, 0);
	if (bytes_received > 0) {
		buf[bytes_received] = '\0';  
		return 0;
	}
	else if (bytes_received == 0) return 1;

}

void chiudi_thread_client(SOCKET client_s,HANDLE * a)
 
{
	closesocket(client_s);
	if (a != NULL)
	{
		//da fare
	}
	
	ExitThread(0);
} 

void funzione_client(LPVOID lpparam) 
{
	int ret, tentativi, a, i, j;
	Parametri_client* param = (Parametri_client*)lpparam;
	SOCKET client_s = param->server_s; // Ottieni il socket del client
	char buff_receive[BUF_DIM], buff_send[BUF_DIM];
	char numero[10];
	//DWORD mutexRet;
	char my_id[20];
	
	//ricezione nome e controllo
richiedi_nome:
	ret = ricevi_client(client_s, buff_receive);
	if (ret != 0) {
		printf("il client ha chiuso la connessione\n");
		chiudi_thread_client(client_s, NULL);
		}
	

	if (WaitForSingleObject(richiesta_scrivi, INFINITE) == WAIT_FAILED) {
		ExitThread(0);
	}
	printf("preso richiesta_scrivi\n");
	richieste->codice = 1;
	strcpy(richieste->buff, buff_receive);
	ReleaseSemaphore(richiesta_pronta, 1, NULL);
	printf("rilasciato richiesta_pronta\n");
	if (WaitForSingleObject(mutex_controllo, INFINITE) == WAIT_FAILED) {
		ExitThread(0);
	}
	if (richieste->codice) {
		manda_client(client_s, "nonok");
		ReleaseSemaphore(richiesta_scrivi, 1, NULL);
		printf("rilasciato richiesta_scrivi\n");
		goto richiedi_nome;
	}
	ReleaseSemaphore(richiesta_scrivi, 1, NULL);
	printf("rilasciato mutex richiesta_scrivi");
	manda_client(client_s, "ok");
	strcpy(my_id, buff_receive);
	printf("id thread client: %s\n", my_id);
entra_partita:
	ret = ricevi_client(client_s, buff_receive);
	if (ret != 0) {
		printf("il client ha chiuso la connessione\n");
		chiudi_thread_client(client_s, NULL);
	}
	if (strcmp(buff_receive, "entra_in_partita") != 0) {
		goto entra_partita;
	}
	tentativi = 0;
	printf("riga 266\n");
	do {
		if (WaitForSingleObject(richiesta_scrivi, INFINITE) == WAIT_FAILED) {
			
			ExitThread(0);

		}
		
		richieste->codice = 2;
		strcpy(richieste->buff, my_id);
		printf("richieste->buff(277): %s\n", richieste->buff);
		ReleaseSemaphore(richiesta_pronta, 1, NULL);
		printf("rilasciato mutex richiesta_pronta(277)\n");
		if (WaitForSingleObject(mutex_controllo, INFINITE) == WAIT_FAILED) {
			ExitThread(0);
		}
		if (richieste->codice == 0) {
			manda_client(client_s, "entralobby");
			ReleaseSemaphore(richiesta_scrivi, 1, NULL);
			printf("rilasciato mutex richiesta_scrivi(284)\n");
			goto gestione_partita;
		
		}
		manda_client(client_s, "nonok");
		ReleaseSemaphore(richiesta_scrivi, 1, NULL);
		printf("rilasciato mutex richiesta_scrivi(289)\n");
		tentativi++;
		
	} while (tentativi < 5);
	chiudi_thread_client(client_s, NULL);
gestione_partita:
//attesa che inizi e apertura mutex
	
	HANDLE notizia_attacco = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, richieste->buff);
	strcpy(buff_receive, my_id);//da creare  in thread_partita
	strcat(buff_receive, "1");
	HANDLE notizia_letto = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, NULL, buff_receive);//da creare in thread_partita
	ReleaseSemaphore(richiesta_scrivi, 1, NULL);
	manda_client(client_s, "entralobby");
	HANDLE notizia_pronta = OpenSemaphoreA(SEMAPHORE_ALL_ACCESS, FALSE, my_id);;
	if (WaitForSingleObject(notizia_pronta, INFINITE) == WAIT_FAILED) { //aspetta che la partita sia iniziata, attendo che il thread_partita abbia preso i giocatori necessari/è passatp il tempo limite di attesa
		ExitThread(0);
	}


	//partita iniziata
	manda_client(client_s, "disposizione navi");
	

	
	//mando  num giocatori
	_itoa(7, numero, 10);
	manda_client(client_s, numero);
	//mando id giocatori
	for (i = 0; i < notizie->num_giocatori;i++) {
		manda_client(client_s, notizie->giocatori[i]);
	}
	 
	ret = ricevi_client(client_s, buff_receive);
	if (ret != 0) {
		printf("il client ha chiuso la connessione\n");
		chiudi_thread_client(client_s, NULL);
	}
	if (strcmp(buff_receive, "completata") != 0) {
		chiudi_thread_client(client_s, NULL);
	}
	ReleaseSemaphore(notizia_letto, 1, NULL);
	WaitForSingleObject(notizia_pronta, INFINITE);
	manda_client(client_s, "iniziopartita");


gestione_notizie:
	/*   MESSAGGI CLIENT PARTITA-CLIENT SERVER

   costruzione messaggi da server (in partita, dal punto 5)
NOTIZIE
notizia_mossa(globale) = 1+casella+hit(o no)+idgiocatore
notizia_giocatoreeliminato(globale) = 2+idgiocatore( se id giocatore mio ho perso)
notizia_turno(globale) = 3+idgiocatore
notizia_vittoriagiocatore(globale) = 4+idgiocatore (ti passa al punto 2)
notizia_attacco(privato) = 5+casella
mossa = casella + idgiocatore
*/
	WaitForSingleObject(notizia_pronta, INFINITE);
	a = notizie->codice;
	switch (a) {
	case 1://devo mandare msg   1 + casella + hit(o no) + idgiocatore
		_itoa(notizie->codice, numero, 10);
		manda_client(client_s, numero); //mandiamo il codice
		manda_client(client_s, notizie->casella);
		_itoa(notizie->hit, numero, 10);
		manda_client(client_s, numero);//mandiamo la hit
		manda_client(client_s, notizie->idgiocatore);
		ReleaseSemaphore(notizia_letto, 1, NULL);
	case 2:
		_itoa(notizie->codice, numero, 10);
		manda_client(client_s, numero);
		manda_client(client_s, notizie->idgiocatore);
		ReleaseSemaphore(notizia_letto, 1, NULL);
	case 3:
		_itoa(notizie->codice, numero, 10);
		manda_client(client_s, numero);
		manda_client(client_s, notizie->idgiocatore);
		ReleaseSemaphore(notizia_letto, 1, NULL);
		if (strcmp(notizie->idgiocatore, my_id) == 0) goto gestione_turno;
	case 4:
		_itoa(notizie->codice, numero, 10);
		manda_client(client_s, numero);
		manda_client(client_s, notizie->idgiocatore);
		goto entra_partita;
	case 5: //dopo ci arriva hit miss o perso
		
		_itoa(notizie->codice, numero, 10);
		manda_client(client_s, numero);
		manda_client(client_s, notizie->casella);
		ret = ricevi_client(client_s, buff_receive);
		if (ret != 0) {
			printf("il client ha chiuso la connessione\n");
			chiudi_thread_client(client_s, NULL);
		}
		/*
		se arriva hit -> codice = 1
		se arriva miss -> codice = 2
		se arriva perso -> codice = 3
		*/
		if (strcmp("hit", buff_receive)) {
			notizie->codice = 1;
		}
		else if (strcmp("miss", buff_receive)) {
			notizie->codice = 2;
		}
		else if (strcmp("perso", buff_receive)) {
			notizie->codice = 3;
		}
		ReleaseSemaphore(notizia_letto, 1, NULL);

	default:
		printf("errore comunicazione.\n");
		ExitThread(0);
	}
	//la disposizione è finita, partita iniziata ufficialmente
 //da mettere controllo
gestione_turno: //thread partita aspetta che inseriamo la mossa
	ricevi_client(client_s, buff_receive);
	strcpy(notizie->idgiocatore, buff_receive);
	ricevi_client(client_s, buff_receive);
	strcpy(notizie->casella, buff_receive);
	notizie->codice = 5;
	ReleaseSemaphore(notizia_attacco, 1, NULL);
	WaitForSingleObject(notizia_pronta, INFINITE); //partita scrive il codice che ci interessa
	a = notizie->codice;
	switch (a) {

	case 1:
		manda_client(client_s, "1");
		notizie->codice = 1;
		notizie->hit = 1;
		ReleaseSemaphore(notizia_attacco, 1, NULL);//abbiamo scritto una notizia
		ReleaseSemaphore(notizia_letto, 1, NULL);// aspetto che tutti leggano il mio msg
		goto gestione_turno;
	case 2:
		manda_client(client_s, "2");
		notizie->codice = 1;
		notizie->hit = 0;
		ReleaseSemaphore(notizia_attacco, 1, NULL);
		ReleaseSemaphore(notizia_letto, 1, NULL);
		goto gestione_notizie;
	case 3:
		manda_client(client_s, "3");
		notizie->codice = 2;
		ReleaseSemaphore(notizia_attacco, 1, NULL);
		ReleaseSemaphore(notizia_letto, 1, NULL);
		goto gestione_turno;
	case 4:
		manda_client(client_s, "4");
		notizie->codice = 4;
		strcpy(notizie->idgiocatore, my_id);
		ReleaseSemaphore(notizia_attacco, 1, NULL);
		ReleaseSemaphore(notizia_letto, 1, NULL);
		goto entra_partita;
	}

	



}



void accettazione() //funzione del thread_accettazione
{
	SOCKET client_s;
	struct sockaddr_in client_addr;
	int length;
	HANDLE thread_client;
	//int ret;

	Parametri_client* param = (Parametri_client*)malloc(sizeof(Parametri_client));
	while (1) {
		length = sizeof(client_addr);
		client_s = accept(server_s, (struct sockaddr*)&client_addr, &length);
		if (client_s == INVALID_SOCKET) {
			printf("errore nella accept\n");
			continue;
		}
		param->server_s = client_s;
		thread_client = CreateThread(
			NULL,      // Attributi predefiniti
			0,         // Dimensione dello stack predefinita
			funzione_client, // Funzione del thread
			(LPVOID)param,      // Parametri per la funzione
			0,         // Esegui il thread immediatamente
			NULL       // ID del thread
		);

		printf("connessione effettuata con un client\n");
	}



}


#define MAX_GIOCATORI 7




void partita() //funzione del thread_partita
{
	
	struct array* nomi_partita, * mutex_pronta, * mutex_letta,* nomi_in_vita;
	HANDLE mutex_attacco;
	HANDLE handle; //salva momenatenamente il mutex
	char nome[60];
	
	
	avvio_partita:

	/* INIZIALIZZAZIONE VARIABILI */
	
	mutex_attacco = CreateSemaphoreA(NULL, 1, 1, NULL);
	if (!mutex_attacco) {
		printf("errore nella creazione del mutexpartita\n");
		exit(EXIT_FAILURE);
	}

	nomi_partita = init_vector(2, sizeof(char *));
	if (nomi_partita == NULL) {
		printf("errore nella creazione del vettore dinamico\n");
		exit(EXIT_FAILURE);
	}

	mutex_pronta = init_vector(2, sizeof(HANDLE)); //notizia pronta
	if (mutex_pronta == NULL) {
		printf("errore nella creazione del vettore dinamico\n");
		exit(EXIT_FAILURE);
	}

	mutex_letta = init_vector(2, sizeof(HANDLE));
	if (nomi_partita == NULL) {
		printf("errore nella creazione del vettore dinamico\n");
		exit(EXIT_FAILURE);
	}

	nomi_in_vita = init_vector(2, sizeof(HANDLE));
	if (nomi_in_vita == NULL) {
		printf("errore nella creazione del vettore dinamico\n");
		exit(EXIT_FAILURE);
	}

	//ingresso giocatori in paritita
	/*da creare per client
	salvare il nome,
	creare un mutex con il suo nome (notizia_pronta)
	creare un mutex con il suo nome+'1' (notizia_letta)
	mettere nel buffer "notiziaAttacco"
	*/
	printf("inizializzazione del thread partita completata\n");
	do {
		//ReleaseMutex(partita_iniziata);
		WaitForSingleObject(comunicazione_pronta, INFINITE); // da mettere controlli
		
		push_back(nomi_partita, &(richieste->buff));
		push_back(nomi_in_vita, &(richieste->buff));
		printf("richieste->buff(553): %s\n", richieste->buff);
		handle = CreateSemaphoreA(NULL, 0, 1, richieste->buff);
		printf("richieste->buff(555): %s\n", richieste->buff);
		push_back(mutex_pronta, &handle);
		strcpy(nome, richieste->buff);
		printf("richieste->buff(558): %s\n", richieste->buff);
		printf("nome(559): %s\n", nome);
		strcat(nome, "1");
		handle = CreateSemaphoreA(NULL, 0, 1, nome);      //notizia +1
		push_back(mutex_letta,&handle);
		ReleaseSemaphore(comunicazione_letta, 1, NULL);
		//WaitForSingleObject(partita_iniziata, INFINITE);
		//printf("%s accettato nella partita\n", richieste->buff);
	} while (nomi->count < MAX_GIOCATORI);

   	iniziata = true;
	
	for (int j = 0; j < nomi->count;j++) {
		read_from(mutex_pronta, j, &handle);
		ReleaseSemaphore(handle, 1, NULL);
		
	}
	//aspettiamo la disposizione delle navi
	for (int j = 0; j < nomi->count;j++) {
		read_from(mutex_letta, j, &handle);
		WaitForSingleObject(mutex_letta,INFINITE);
	}

	for (int j = 0; j < nomi->count;j++) {
		read_from(mutex_pronta, j, &handle);
		ReleaseSemaphore(handle, 1, NULL);

	}
	int turno = 0;//gestione del turno buffer circolare 
	int attuale, i; 
	//gestiamo il turno	
	gestione_turno_partita:
	//scriviamo in notizie
	notizie->codice = 3;
	read_from(nomi_in_vita, turno, nome);
	strcpy(nome, notizie->idgiocatore);
	
	
	for (int j = 0; j < nomi->count;j++) {//vedere condizioni for
		read_from(mutex_pronta, j, &handle);
		ReleaseSemaphore(handle, 1, NULL);
	}
	for (int j = 0; j < nomi->count;j++) {
		read_from(mutex_letta, j, &handle);
		WaitForSingleObject(mutex_letta, INFINITE);
	}
	
	
	//salviamo l'id attuale
	for (i = 0; i < nomi->count; i++) {
		read_from(nomi_partita, i, nome);
		if (strcmp(notizie->idgiocatore, nome) == 0) break;
	}
	attuale = i;
    gestione_turno_singolo: //gestione turno del singolo giocatore
	
	WaitForSingleObject(mutex_attacco, INFINITE);
	for (i = 0; i < nomi->count; i++) {
		read_from(nomi_partita, i, nome);
		if (strcmp(notizie->idgiocatore, nome) == 0) break;
		
	}
	read_from(mutex_pronta, i, &handle);
	ReleaseSemaphore(handle, 1, NULL);
	read_from(mutex_letta, i, &handle);
	WaitForSingleObject(handle, INFINITE);//aspetto mutex letta
	
	//leggiamo 1 2 oppure 3
	if (notizie->codice == 3) { // il client manda 3 se era quella la sua ultima nave
		//parte dove sistemiamo i giocatori in vita
		for (i = 0; i < nomi->count; i++) {
			read_from(nomi_in_vita ,  i, nome);
			if (strcmp(notizie->idgiocatore, nome) == 0) break;

		}
		extract_from(nomi_in_vita, i, nome);
		//con l'if verifichiamo se l'utente è l'unico rimasto
		if (nomi_in_vita->count == 1) {
			//dobbiamo resettare il thread partita per l'inzio di una nuova partita free_vector(struct array* arr) CloseHandle
			read_from(mutex_pronta, attuale, &handle);
			ReleaseSemaphore(handle, 1, NULL);
			WaitForSingleObject(mutex_attacco, INFINITE);
			notizie->codice = 4;
			//strcpy(id_giocatore)
			for (int j = 0; j < nomi->count;j++) {
				read_from(mutex_pronta, j, &handle);
				ReleaseSemaphore(handle, 1, NULL);
			}
			for (int j = 0; j < nomi->count;j++) {
				read_from(mutex_letta, j, &handle);
				WaitForSingleObject(mutex_letta, INFINITE);
			}
			//chiudiamo tutto
			for (int j = 0; j < nomi->count;j++) {
				read_from(mutex_letta, j, &handle);
				CloseHandle(handle);
				read_from(mutex_pronta, j, &handle);
				CloseHandle(handle);
			}
			free_vector(nomi_in_vita);
			free_vector(nomi_partita);
			free_vector(mutex_pronta);
			free_vector(mutex_letta);

			goto avvio_partita;
		}
		
		//parte costante
		read_from(mutex_pronta, attuale, &handle);
		ReleaseSemaphore(handle, 1, NULL);
		WaitForSingleObject(mutex_attacco, INFINITE);
		notizie->codice = 2;
		//strcpy(id_giocatore)
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_pronta, j, &handle);
			ReleaseSemaphore(handle, 1, NULL);
		}
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_letta, j, &handle);
			WaitForSingleObject(mutex_letta, INFINITE);
		}
	
		
		goto gestione_turno_singolo;
		
	
	}
	else if (notizie->codice == 2) { //il client non ha colpito niente
		
		
		
		read_from(mutex_pronta, attuale, handle);
		ReleaseSemaphore(handle, 1, NULL);
		WaitForSingleObject(mutex_attacco, INFINITE);
		notizie->codice = 1;
		//strcpy(id_giocatore)
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_pronta, j, &handle);
			ReleaseSemaphore(handle, 1, NULL);
		}
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_letta, j, &handle);
			WaitForSingleObject(mutex_letta, INFINITE);
		}

		turno = (turno + 1) % nomi_in_vita->count;
		goto gestione_turno_singolo;
	}
	else if (notizie->codice == 1) {

	
		read_from(mutex_pronta, attuale, &handle);
		ReleaseSemaphore(handle, 1, NULL);
		WaitForSingleObject(mutex_attacco, INFINITE);
		notizie->codice = 1;
		//strcpy(id_giocatore)
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_pronta, j, &handle);
			ReleaseSemaphore(handle, 1, NULL);
		}
		for (int j = 0; j < nomi->count;j++) {
			read_from(mutex_letta, j, &handle);
			WaitForSingleObject(mutex_letta, INFINITE);
		}

		turno = (turno + 1) % nomi_in_vita->count;
		goto gestione_turno_partita;
	
	
	
	}











}



/*thread-controllo_gestione (main):
   1-prepara tutto
   2-lancia thread accettazione e thread partita
   3- gestisce le richieste dei thread(ex: voglio che cancelli il mio nome)*/


int main() { //thread-controllo_gestione
    WSADATA wsaData;
    SOCKET client_s;
    int length;
    struct sockaddr_in server_addr, client_addr;
    char buff[BUF_DIM];
	DWORD ret_mutex;

	//PREPARAZIONE DEL NECESSARIO

	//preparazione scocket

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

	bool iniziata = false;

	hnotizie = CreateFileMapping(
		INVALID_HANDLE_VALUE, // Usa la memoria di paging del sistema
		NULL,                 // Sicurezza predefinita
		PAGE_READWRITE,      // Accesso in lettura/scrittura
		0,                    // Dimensione massima dell'oggetto (ignora)
		sizeof(Sharednotizie),   // Dimensione dell'oggetto
		"MySharedMemory1"      // Nome dell'oggetto di memoria condivisa
	);

	if (hnotizie == NULL) {
		printf("CreateFileMapping failed (%d).\n", GetLastError());
		return 1;
	}

	notizie = (Sharednotizie*)MapViewOfFile(
		hnotizie,            // Handle dell'oggetto di memoria condivisa
		FILE_MAP_ALL_ACCESS, // Accesso in lettura/scrittura
		0,                   // Offset alto
		0,                   // Offset basso
		0                    // Dimensione
	);

	if (notizie == NULL) {
		printf("MapViewOfFile failed (%d).\n", GetLastError());
		CloseHandle(hnotizie);
		return 1;
	}

	/*
		*********************** SHARED MEM PER IL CONTROLLO*************************
		*/
	
	    hrichieste = CreateFileMappingA(
		INVALID_HANDLE_VALUE, // Usa la memoria di paging del sistema
		NULL,                 // Sicurezza predefinita
		PAGE_READWRITE,      // Accesso in lettura/scrittura
		0,                    // Dimensione massima dell'oggetto (ignora)
		sizeof(Sharedrichieste),   // Dimensione dell'oggetto
		"MySharedMemory2"      // Nome dell'oggetto di memoria condivisa
	);

	if (hrichieste == NULL) {
		printf("CreateFileMapping failed (%d).\n", GetLastError());
		return 1;
	}

	richieste = (Sharedrichieste*)MapViewOfFile(
		hrichieste,            // Handle dell'oggetto di memoria condivisa
		FILE_MAP_ALL_ACCESS, // Accesso in lettura/scrittura
		0,                   // Offset alto
		0,                   // Offset basso
		0                    // Dimensione
	);

	if (richieste == NULL) {
		printf("MapViewOfFile failed (%d).\n", GetLastError());
		CloseHandle(hrichieste);
		return 1;
	}

	/*
		*********************** SEMAFORI PER SINCRONIZZAZIONE *************************
		CreateSemaphore(NULL, 1, 1, buffer); CreateSemaphore(NULL, 1, 1, NULL);
		
		*/
	richiesta_pronta = CreateSemaphoreA(NULL, 0, 1, NULL);
	if (richiesta_pronta == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}
	
	richiesta_scrivi = CreateSemaphoreA(NULL, 1, 1, NULL);
	if (richiesta_scrivi == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}

	comunicazione_letta = CreateSemaphoreA(NULL, 0, 1, NULL);
	if (comunicazione_letta == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}

	comunicazione_pronta = CreateSemaphoreA(NULL, 0, 1, NULL);
	if (comunicazione_letta == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}

	partita_iniziata = CreateSemaphoreA(NULL, 1, 1, NULL);
	if (partita_iniziata == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}

	mutex_controllo = CreateSemaphoreA(NULL, 0, 1, NULL);
	if (mutex_controllo == NULL) {
		DWORD error = GetLastError();
		printf("Errore nella creazione del semaforo: %lu\n", error);
		switch (error) {
		case ERROR_INVALID_PARAMETER:
			printf("Parametro non valido (controlla lInitialCount e lMaximumCount).\n");
			break;
		case ERROR_ALREADY_EXISTS:
			printf("Un semaforo con lo stesso nome esiste già.\n");
			break;
		default:
			printf("Errore non gestito: %lu\n", error);
		}
		exit(EXIT_FAILURE);
	}
	/*
		*********************** array dinamico  per nomi *************************
	

		*/

	nomi = init_vector(10, sizeof(char*));
	
	
	//creazione del thread-accettazione

	
	HANDLE thread_accettazione = CreateThread(
		NULL,      // Attributi predefiniti
		0,         // Dimensione dello stack predefinita
		accettazione, // Funzione del thread
		NULL,      // Parametri per la funzione
		0,         // Esegui il thread immediatamente
		NULL       // ID del thread
	);

	HANDLE thread_partita = CreateThread(
		NULL,      // Attributi predefiniti
		0,         // Dimensione dello stack predefinita
		partita, // Funzione del thread
		NULL,      // Parametri per la funzione
		0,         // Esegui il thread immediatamente
		NULL       // ID del thread
	);

	//tutto pronto ci mettiamo a rispondere alle richieste di infomazioni


	



	//Creare i thread: Crea i thread che utilizzeranno la memoria condivisa.
	/*
	HANDLE hThread = CreateThread(
		NULL,      // Attributi predefiniti
		0,         // Dimensione dello stack predefinita
		ThreadFunction, // Funzione del thread
		NULL,      // Parametri per la funzione
		0,         // Esegui il thread immediatamente
		NULL       // ID del thread
	);
	*/
	// Attendi che il thread termini
	
	
	/*
		1+idgiocatore = richiesta nome
		2+idgiocatore = richiesta entrare in partita

*/
	/* *********** thread controllo in azione 
	
	extern bool push_back(struct array* arr, void* el);//aggiunge elemento alla fine
extern bool read_from(struct array* arr, size_t pos, void* buffer);//leggere valore
struct array {
	size_t count; // currently used slots
	size_t slots; // total slots
	size_t size; // size of a slot
	void* arr; // the actual array
	
	*/
	int a = 0;
	printf("thread di controllo operativo\n");
	while (1) {
		ret_mutex = WaitForSingleObject(richiesta_pronta, INFINITE);
		if (ret_mutex == WAIT_ABANDONED) {
			printf("WAIT_ABANDONED\n");
		}
		else if (ret_mutex == WAIT_OBJECT_0) {
			printf("WAIT_OBJECT_0\n");
		}
		else if (ret_mutex == WAIT_TIMEOUT) {
			printf("WAIT_TIMEOUT\n");
		}
		else if (ret_mutex == WAIT_FAILED) {
			printf("WAIT_FAILED\n");
		}
		 
		printf("presa in considerazione richiesta, preso richiesta pronta\n");
		a = richieste->codice;
		
		switch (a) {
		case 1: //inserisci nuovo nome
			printf("inserendo nuovo nome->");
			for (int i = 0;i < nomi->count;i++) {
				read_from(nomi, i, buff);
				if(strcmp(buff,richieste->buff)== 0){
					richieste->codice = 1;
					ReleaseSemaphore(mutex_controllo, 1, NULL);
					printf("mutex_controllo rilasciato\n");
					printf("nome rifiutato\n");
					continue;
				
				}
			
			
			}

			push_back(nomi, richieste->buff);
			richieste->codice = 0;
			printf("nome accettato: %s\n", richieste->buff);
			ReleaseSemaphore(mutex_controllo,1,NULL);
			printf("mutex_controllo rilasciato");
			continue;
		case 2: //entra in partita, se disponibile
			WaitForSingleObject(partita_iniziata, INFINITE);
			if (!iniziata) {
				
				ReleaseSemaphore(comunicazione_pronta, 1, NULL);
				WaitForSingleObject(comunicazione_letta, INFINITE);//aspetto thread partita
				richieste->codice = 0;
				ReleaseSemaphore(mutex_controllo, 1, NULL);
			}
			else {
				richieste->codice = 1;
				ReleaseSemaphore(mutex_controllo,1,NULL);
			}
			continue;
		case 3: //togliere il nome
			for (int i = 0;i < nomi->count;i++) {
				read_from(nomi, i, buff);
				if (strcmp(buff, richieste->buff) == 0) {
					extract_from(nomi,  i, buff);
					ReleaseSemaphore(mutex_controllo, 1, NULL);
					continue;
				
				}
				

			}
			ReleaseSemaphore(mutex_controllo, 1, NULL);
			continue;
		
		}

	
	
	
	
		//a = 0;
	
	
	
	
	
	
	}
	
	
	
	
	
	
	
	
	
	
	//mutex = CreateMutex(NULL, FALSE, NULL);






}




/*
   attori e ruoli
   thread client: 
   1-gestisce i messaggi client-server (sia in ricezione che in invio)
   2-creare le notizie che scrivera su un area di memoria dove tutti le prenderanno
   3-fa entrare il client in partita (comunicando al thread-partita il suo id)
   4- 

   thread-controllo_gestione (main):
   1-prepara tutto
   2-lancia thread accettazione e thread partita
   3- gestisce le richieste dei thread(ex: voglio che cancelli il mio nome) 

   thread-accettazione:
   1-accettare nuove connessioni
   2-creare thread-client

   thread-partita:
   1-gestiste i turni
   2-avvia la partita
   3-dice ai thread-client che cè una notizia(diffonde notizie ai thread client)

   RISORSE (VARIABILI GLOBALI)

   ARRAY DI NOMI (mutex)
   bool partita iniziata (o no) (mutex)
   area di mem per scambio messaggi (notizie) (mutex)
   il descrittore socket socket






   MESSAGGI CLIENT PARTITA-CLIENT SERVER
   
   costruzione messaggi da server (in partita, dal punto 5)
NOTIZIE
notizia_mossa(globale) = 1+casella+hit(o no)+idgiocatore
notizia_giocatoreeliminato(globale) = 2+idgiocatore( se id giocatore mio ho perso)
notizia_turno(globale) = 3+idgiocatore
notizia_vittoriagiocatore(globale) = 4+idgiocatore (ti passa al punto 2)
notizia_attacco(privato) = 5+casella+idgiocatore
mossa = casella + idgiocatore

MESSAGGI AL THREAD CONTROLLO

1+idgiocatore = richiesta nome
2+idgiocatore = richiesta entrare in partita

   
   
   
   
   



*/