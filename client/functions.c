#define _CRT_SECURE_NO_WARNINGS

#include <stddef.h>
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include "functions.h"

struct casella {
	unsigned char status; // presenza di acqua o nave (1 presenza di una parte di nave, 0 altrimenti)
	unsigned char hit;// nave colpita o meno (1 per colpita, 0 altrimenti)
};

struct griglia {
	struct casella disp[10][10]; //rappresentazione della griglia come matrice di caselle
};

char lettere[] = "ABCDEFGHIJ";

int lettoint(char letter)
{
	switch (letter)
	{
	case 'A':
		return 0;
	case 'B':
		return 1;
	case 'C':
		return 2;
	case 'D':
		return 3;
	case 'E':
		return 4;
	case 'F':
		return 5;
	case 'G':
		return 6;
	case 'H':
		return 7;
	case 'I':
		return 8;
	case 'J':
		return 9;
	default:
		return 0;
	}
}

void printgriglia(struct griglia *a)
{
	int i, j;
	int lenght = sizeof(char) * sizeof(" 1 2 3 4 5 6 7 8 9 10") + 1;
	char *riga = malloc(lenght * 2);
	char carattere;
	puts(" 1 2 3 4 5 6 7 8 9 10");
	//strcat_s()
	for (i = 0; i < 10; i++)
	{
		memset((void*)riga, 0, lenght * 2);
		puts(" ---------------------");
		riga = lettere[i];
		for (j = 0; j < 10; i++)
		{
			strcat(riga, "|");
			if ((a->disp)[i][j].hit)
			{
				if ((a->disp)[i][j].status)
				{
					carattere = 'X';
				}
				else {
					carattere = 'O';
				}
			}
			else {
				carattere = " ";
			}
			strcat(riga, carattere);
		}
		strcat(riga, "|");
		puts(riga);
	}
	puts(" ---------------------");
	return;
}

int main()
{
	struct griglia prova;
	printgriglia(&prova);
	return;
}