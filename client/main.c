#include <windows.h>;
#include <winsock.h>;
#include <winsock.h>;

#define MAX_DIM 1024;

char* host = "LAPTOP-IJ698CFS"; //https://www.whatismyip.com/ 

//aleip : 31.191.30.102
// ip seli 87.20.233.29
typedef struct casella {
	unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
	unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

typedef struct griglia {
	casella[10][10] disp; //rappresentazione della griglia come matrice di caselle
};

int main() {
	//creazione connessione (include  accesso univoco utente  )
	int WSAStartup(MAKEWORD(2, 2), wsaData); //da mettere controllo


	WSADATA wsaData;
	SOCKET ds_sock;
	int lenght; res;
	struct sockaddr_in client;
	struct hostent* hp;
	char buff[MAX_DIM]

		ds_sock = socket(AF_INET, SOCK_STREAM, 0);
	client.sin_family = AF_INET;
	client.sin_port = htons(25000);  //da modificare o (da rivedere) tra 10 anni
	hp = gethostbyname(*host);
	bcopy(hp->h_addr, &client.sin_addr, hp->h_length);
	res = connect(ds_sock, &client, sizeof(client));
	if (res == -1) {
		printf("Errore nella connect \n");
		exit(1);
	}






	/******
	crezione/ricerca lobby

	1-selezionare ricerca o creazione
	se creazione
	2)inserimento nome

	***********/

	/**********
	GESTIONE PARTITA

	1-disposizione navi
	2-gestione turni
	3-inserimenti e valutazione mossa
	4-terminazione partita

	*************/

	WSACleanup(); // alla fine



}