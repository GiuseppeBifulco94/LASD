/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*)(it. e ric.) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "generalizza.h"
#include "abr.h"
#include "listaABR.h"
#include "libreria.h"
#include "mainappoggio.h"
#include "stack.h"

int main() {
	char c;
	int num_ABR, current_ABR;
	struct func funz;
	LISTA *corrente;				/* Albero corrente della lista */
	num_ABR = 0;					/* Numero di ABR istanziati */
	current_ABR = 0;				/* Numero di ABR su cui si lavora */
	LISTA *listaABR = NULL;
	printf("\nAutore: MIRKO ALICASTRO N86/1437\n");
	printf("Libreria: ABR generici con (void*)\n");
	printf("Versione: RICORSIVA\n\n");
	srand(time(NULL));
	c='1';
	while(c != '0') {
		listaABR = scegli_albero(listaABR, &current_ABR, &num_ABR);
		corrente = estrai_lista(listaABR, current_ABR);
		setLib(&funz, corrente->tipo);
		mostra_menu(current_ABR, num_ABR);
		switch(c=scelta_utente()) {
			case CASE_TERMINA:
				termina(listaABR, funz);
				return 0;
			break;
			case CASE_STAMPA:
				stampa_albero(corrente->content, funz);
			break;
			case CASE_INSERISCI_ELEMENTO:
				corrente->content = inserisci_elem(corrente->content, funz);
			break;
			case CASE_INSERISCI_CASUALI:
				corrente->content = inserisci_casuali(corrente->content, funz);
			break;
			case CASE_CANCELLA_ELEMENTO:
				corrente->content = cancella_elem(corrente->content, funz);
			break;
			case CASE_CANCELLA_PARI_DISPARI_TRA_A_E_B:
				corrente->content = esegui_es5(corrente->content, funz);
			break;
			case CASE_DISTRUGGI:
				corrente->content = distruggi(corrente->content, funz);
				printf("\nOperazione terminata con successo!\n");
			break;
			case CASE_VERIFICA_ABR_UGUALI:
				verifica_abr_uguali(corrente->content, listaABR, current_ABR, num_ABR, funz);
			break;
			case CASE_DUPLICAZIONE:
				listaABR = duplicazione(listaABR, &num_ABR, corrente->content, funz);
			break;
			case CASE_ARRAY_ORDINATO_DA_ABR:
				array_ordinato(corrente->content, funz);
			break;
			case CASE_PERFETTAMENTE_BILANCIATO:
				corrente->content = bilanciamento(corrente->content, funz);
			break;
			default:
			case CASE_CAMBIA_ALBERO:
			break;
		}
	}
	return 0;
}
