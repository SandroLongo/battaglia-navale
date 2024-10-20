#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

//definizione costanti
#define PORTNUMBER 25123
#define BUF_DIM 512

const char* host = "192.168.1.1"; //"LAPTOP-IJ698CFS"; //https://www.whatismyip.com/ 

//aleip : 192.168.1.1
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
	//creazione connessione (include  accesso univoco utente  )


	WSADATA wsaData;
	SOCKET ds_sock;
	int lenght, res;
	struct sockaddr_in client;
	struct hostent* hp;
	char buff[BUF_DIM];
	char buf[BUF_DIM];
	unsigned long server_addr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("errore nella WSAStartup\n");
		exit(EXIT_FAILURE);
	}
	ds_sock = socket(AF_INET, SOCK_STREAM, 0);
	client.sin_family = AF_INET;
	client.sin_port = htons(PORTNUMBER);  //da modificare o (da rivedere) tra 10 anni
	//client.sin_addr.s_addr = INADDR_ANY;
	//hp = gethostbyname("");
	server_addr = inet_addr(host);
	printf("il server si trova nell'indirizzo ip : %ul\n", server_addr);
	//memcpy(hp->h_addr, &client.sin_addr, hp->h_length);
	memcpy((void*)server_addr, &client.sin_addr.s_addr, sizeof(server_addr));

	printf("inizio della connessione\n");
	fflush(stdout);

	res = connect(ds_sock, (SOCKADDR*)&client, sizeof(client));
	if (res == -1) {
		printf("Errore nella connect \n");
		fflush(stdout);
		exit(1);
	}

	printf("connesione effettuata\n");
	printf("scrivere messaggi\n");
	do {
		scanf_s("%s", buff);
		send(ds_sock, buff, BUF_DIM, 0);
		recv(ds_sock, buf, BUF_DIM, 0);
		printf("%s\n", buf);
	} while (strcmp(buf, "quit") != 0);

	closesocket(ds_sock);






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