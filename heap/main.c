/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA CODE A PRIORITA' */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include "heap.h"
#include "generalizza.h"
#include "mainappoggio.h"

int main() {
	int scelta, dim, menu;
	struct heap *ogg;
	printf("\nAutore: MIRKO ALICASTRO N86/1437\n");
	printf("Libreria: code a priorita'\n\n");
	scelta = 1;
	ogg = alloca_heap(ogg);
	dim = info_heap(ogg);
	ogg = init_heap(ogg, dim);
	build_heap(ogg);
	while(scelta != 0) {
		scelta = menu_scelta(ogg);
		printf("HEAPSIZE: %d\n", ogg->heapsize);
		switch(scelta) {
			case 0:
				distruggi_heap(ogg);
				return 0;
			break;
			case 1:
				stampa_heap(ogg);
			break;
			case 2:
				{
					void *el;
					int pr;
					el = ogg->getInput();
					pr = getnum(-99999999,99999999, "\nInserisci la priorita': ");
					InsertKey(ogg, el, pr);
				}
			break;
			case 3:
				{
					int i;
					i = getnum(0, 99999, "\nInserisci l'indice della chiave da eliminare: ");
					DeleteHeap(ogg, i);
				}
			break;
			case 4:
				{
					int i, prio;
					i = getnum(0, 99999, "\nInserisci l'indice della chiave da incrementare: ");
					prio = getnum(-99999999,99999999, "\nInserisci la nuova priorita': ");
					IncreaseKey(ogg, i, prio);
				}
			break;
			case 5:
				{
					int i, prio;
					i = getnum(0, 99999, "\nInserisci l'indice della chiave da decrementare: ");
					prio = getnum(-99999999,99999999, "\nInserisci la nuova priorita': ");
					DecreaseKey(ogg, i, prio);
				}
			break;
			case 6:
				{
					struct coppia *topel;
					topel = extractHeap(ogg);
					printf("TOP -> ");
					printelcoda(topel, ogg);
					if(topel) {
						ogg->libera(topel);
					}
				}
			break;
			case 7:
				{
					struct coppia *topel;
					topel = gettop(ogg);
					printf("TOP -> ");
					printelcoda(topel, ogg);
				}
			break;
		}
	}
	return 0;
}