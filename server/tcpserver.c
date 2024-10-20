#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

//definizione costanti
#define PORTNUMBER 25123
#define BACKLOG 10
#define BUF_DIM 512

int main()
{
	//definizione delle variabili necessarie
	WSADATA wsaData;
	SOCKET server_s, client_s;
	int lenght;
	struct sockaddr_in server_addr;
	struct sockaddr client_addr;
	struct hostent* localhost;
	char buf[BUF_DIM];

	//inizializzazione della libreria winsock

	if (WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		printf("errore nella WSAStartup\n");
		exit(EXIT_FAILURE);
	}

	//creazione del socket e inizializzazione con bind

	server_s = socket(AF_INET, SOCK_STREAM, 0);
	if (server_s == INVALID_SOCKET)
	{
		printf("errore nella creazione del socket\n");
		exit(EXIT_FAILURE);
	}

	localhost = gethostbyname("");
	if (!localhost)
	{
		printf("errore nella gethostbyname\n");
		exit(EXIT_FAILURE);
	}

	printf("il server si trova nell'indirizzo ip : %s\n", localhost->h_name);

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORTNUMBER);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	//memcpy(&addr.sin_addr, &localhost->h_name, localhost->h_length); (per il server)


	if (bind(server_s, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("errore nella bind\n");
		exit(EXIT_FAILURE);
	}
	listen(server_s, BACKLOG);

	lenght = sizeof(client_addr);
redo:
	client_s = accept(server_s, NULL, NULL);
	if (client_s == INVALID_SOCKET)
	{
		goto redo;
		printf("errore nella accept\n");
		exit(EXIT_FAILURE);
	}

	printf("connesione effettuata\n");
	do {
		recv(client_s, buf, BUF_DIM, 0);
		printf("stringa ricevuta :%s\n", buf);
		send(server_s, "letto", sizeof("letto") + 1, 0);
	} while (strcmp(buf, "quit") != 0);

	send(server_s, "finito", sizeof("letto") + 1, 0);
	closesocket(server_s);
	WSACleanup();
	return 0;
}