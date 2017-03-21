/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "listaABR.h"
#include "abr.h"
#include "libreria.h"
#include "mainappoggio.h"
#include "generalizza.h"

/* Tutte le funzioni seguenti sono quelle che utilizza il main (quindi la gestione dei vari switch) */
void stampa_albero(ABR *albero, FUNC funz) {
	int op;
	printf("In che modo vuoi visitare l'albero:\n");
	printf("1. PRE-ORDER:\n");
	printf("2. IN-ORDER:\n");
	printf("3. POST-ORDER:\n");
	printf("\n");
	op = getnum(1,3,"\nInserisci il numero dell'operazione da effettuare: ");
	stampaABR(albero, op-1, funz);
	printf("\nOperazione terminata con successo!\n");
}
ABR *bilanciamento(ABR *albero, FUNC funz) {
	printf("\nOperazione terminata con successo!\n");
	return bilancia(albero, funz);
}

LISTA *duplicazione(LISTA *lista, int *num_ABR, ABR *albero, FUNC funz) {
	lista = aggiungi_lista(lista, *num_ABR, funz.tipo);
	estrai_lista(lista, *num_ABR)->content = duplica(albero, funz);
	printf("L'ABR e' stato duplicato nell'albero numero %d\n", *num_ABR);
	*num_ABR = *num_ABR + 1;
	printf("\nOperazione terminata con successo!\n");
	return lista;
}

void verifica_abr_uguali(ABR *albero, LISTA *lista, int current_ABR, int num_ABR, FUNC funz) {
	int op;
	printf("\nNumero abr massimo: %d\n", num_ABR-1);
	op = getnum(0,num_ABR-1,"\nInserisci il numero dell'ABR con cui confrontarlo: ");
	printf("Gli ABR numero %d e %d ", current_ABR, op);
	if(estrai_lista(lista, op)->tipo != funz.tipo) {
		printf("non sono uguali ma di tipo diverso!\n");
	} else {
		if(ABR_uguali(albero,estrai_lista(lista, op)->content, funz) == false) {
			printf("non ");
		}
		printf("sono uguali!\n");
	}	
	printf("\nOperazione terminata con successo!\n");
}
void array_ordinato(ABR *albero, FUNC funz) {
	void **array;
	int i, num_nodi;
	num_nodi = nnodi(albero);
	array = (void **) malloc(num_nodi * sizeof(void *));
	riempi_array_da_ABR(albero, array, funz);
	stampa_array(array, num_nodi, funz);
	dealloca_array(array, num_nodi, funz);
	printf("\nOperazione terminata con successo!\n");
}
ABR *esegui_es5(ABR *albero, FUNC funz) {
	void *a, *b;
	int op;
	printf("Vuoi cancellare i pari o i dispari:\n");
	printf("1. PARI:\n");
	printf("2. DISPARI:\n");
	op = getnum(1,2,"\nInserisci l'operazione da effettuare: ");
	op--;
	printf("\nElemento A\n");
	a = funz.getInput();
	printf("\nElemento B\n");
	b = funz.getInput();
	albero = es5(albero, op, a, b, funz);
	if(funz.libera != NULL) {
		funz.libera(a);
	}
	if(funz.libera != NULL) {
		funz.libera(b);
	}
	printf("\nOperazione terminata con successo!\n");
	return albero;
}
ABR *cancella_elem(ABR *albero, FUNC funz) {
	void *elem = funz.getInput();
	albero = cancella_valore(albero, elem, funz);
	if(funz.libera != NULL) {
		funz.libera(elem);
	}
	printf("\nOperazione terminata con successo!\n");
	return albero;
}
ABR *inserisci_casuali(ABR *albero, FUNC funz) {
	int op;
	op = getnum(0,9999999,"\nInserisci il numero di elementi casuali da generare: ");
	albero = insrand(albero, op, funz);
	printf("\nOperazione terminata con successo!\n");
	return albero;
}
ABR *inserisci_elem(ABR *albero, FUNC funz) {
	void *elem = funz.getInput();
	albero = inserisci(albero, elem, funz);
	if(funz.libera != NULL) {
		funz.libera(elem);
	}
	printf("\nOperazione terminata con successo!\n");
	return albero;
}
void termina(LISTA *lista, FUNC funz) {
	LISTA *tmp;
	while(lista != NULL) {
			lista->content = distruggi(lista->content, funz);
			tmp = lista;
			lista = lista->next;
			free(tmp);
	}
	printf("Programma terminato.\n");
}
int scelta_utente() {
	int op;
	op = getnum(0,11,"\nInserisci il numero dell'operazione da effettuare: ");
	printf("\n");
	return op;
}
void mostra_menu(int current_ABR, int num_ABR) {
		printf("Stai lavorando con l'ABR numero %d/%d\n\n", current_ABR, num_ABR-1);
		printf("0. Termina il programma\n");
		printf("1. Stampa l'albero\n");
		printf("2. Inserisci un elemento\n");
		printf("3. Inserisci un numero definito di elementi casuali\n");
		printf("4. Cancella dall'albero un elemento\n");
		printf("5. Cancella tutti gli elementi con valori di lunghezza pari/dispari e compresi tra due elementi\n");
		printf("6. Distruggi l'albero\n");
		printf("7. Verifica se l'ABR e' uguale ad un altro ABR\n");
		printf("8. Duplica l'ABR\n");
		printf("9. Crea e stampa un array ordinato con tutti gli elementi dell'ABR\n");
		printf("10. Trasforma l'ABR in un albero perfettamente bilanciato\n");
		printf("11. Cambia albero con cui lavorare\n");
}
LISTA * scegli_albero(LISTA *lista, int *current_ABR, int *num_ABR) {
	int tipo;
	printf("\n");
	if(*num_ABR != 0)
		printf("Inserisci il numero dell'ABR su cui lavorare (MAX %d), oppure\n", *num_ABR-1);
	printf("Inserisci %d per creare un nuovo ABR: ", *num_ABR);
	printf("\n");
	*current_ABR = getnum(0,*num_ABR,"\nInserisci il numero dell'operazione da effettuare: ");
	if(*current_ABR == *num_ABR) {
		printf("Inserisci il TIPO di ABR da utilizzare:\n");
		printf("0. Stringhe\n");
		printf("1. Interi\n");
		printf("2. Double\n");
		printf("3. Struct (Persona)\n");
		tipo = getnum(0,3,"\nInserisci il numero dell'operazione da effettuare: ");
		lista = aggiungi_lista(lista, *current_ABR, tipo);
		*num_ABR = *num_ABR + 1;
	}
	return lista;
}
