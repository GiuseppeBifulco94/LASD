#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "generalizza.h"

int menu_scelta(struct heap *funz) {
	int scelta;
	printf("\n0) Termina il programma\n");
	printf("1) Stampa la coda\n");
	printf("2) Inserisci un elemento in coda\n");
	printf("3) Elimina l'elemento i-esimo dalla coda\n");
	printf("4) Incrementa la priorità di un elemento\n");
	printf("5) Decrementa la priorità di un elemento\n");
	printf("6) Estrai il ");
	if(funz->minormax == 1)
		printf("minimo\n");
	else
		printf("massimo\n");
	printf("7) Stampa il ");
	if(funz->minormax == 1)
		printf("minimo");
	else
		printf("massimo");
	printf("\n");
	scelta = getnum(0,7,"\nInserisci il numero dell'operazione da effettuare: ");
	return scelta;
}

int info_heap(struct heap *ogg) {
	int minormax, arrayortree, tipo, dimensione;
	printf("Vuoi realizzare un\n1. MIN HEAP\n2. MAX HEAP\n");
	minormax = getnum(1,2,"\nInserisci il numero dell'operazione da effettuare: ");
	printf("Con che struttura vuoi implementare l'HEAP?\n1. ARRAY\n2. ALBERO\n");
	arrayortree = getnum(1,2,"\nInserisci il numero dell'operazione da effettuare: ");
	printf("Scegli il tipo degli elementi dell'HEAP\n");
	printf("0. Stringhe\n");
	printf("1. Interi\n");
	printf("2. Double\n");
	printf("3. Struct (Persona)\n");
	tipo = getnum(0,3,"\nInserisci il numero dell'operazione da effettuare: ");
	dimensione = getnum(0,9999,"\nQuanti elementi vuoi inserire? ");
	setHeap(ogg, tipo, arrayortree, minormax);
	return dimensione;
}