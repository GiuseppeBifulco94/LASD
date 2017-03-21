#include <stdio.h>
#include <stdlib.h>
#include "grafo.h"
#include "utility.h"
#include "func.h"
#include "grafoparser.h"

//arco esistente con PESO = NULL => 1 ?
//col pred dist .. iniziallizati nel main per forza, ma perché?
//posso utilizzare 
int main() {
	struct grafo *mygraph;
	struct func funz;
	int impl_grafo, tipo_el, dim;
	int c;
	mygraph = alloca_grafo(mygraph);
	printf("Come vuoi implementare il grafo?\n1. Lista di adiacenze\n2. Matrice di adiacenze\n");
	impl_grafo = getnum(1,2,"\nInserisci il numero dell'operazione da effettuare: ");
	printf("1) Crea un nuovo grafo\n");
	printf("2) Leggi il grafo da file\n");
	c = getnum(1,2,"\nInserisci il numero dell'operazione da effettuare: ");
	if(c == 2) {
		char *nomefile;
		int err;
		err = 1;
		while(err == 1) {
			nomefile = getalfa(1,100,"\nInserisci il percorso del file da cui leggere il grafo: ");
			read_grafo(mygraph, nomefile, impl_grafo, &funz, &err);
			free(nomefile);
		}
//		setlib(&funz, impl_grafo, funz.tipo_el);
	} else {
		printf("Scegli il tipo degli elementi del grafo\n");
		printf("0. Stringhe\n");
		printf("1. Interi\n");
		printf("2. Double\n");
		printf("3. Struct (Persona)\n");
		tipo_el = getnum(0,3,"\nInserisci il numero dell'operazione da effettuare: ");
		dim = getnum(0,9999,"\nQuanti elementi vuoi inserire? ");
		setlib(&funz, impl_grafo, tipo_el);
		funz.init(mygraph, dim, funz);
	}
	c = 1;
	while(c != 0) {
		printf("0) Esci\n");
		printf("1) Stampa la lista di Adj\n");
		printf("2) Stampa tutti i vertici\n");
		printf("3) Stampa tutti gli adiacenti di un vertice\n");
		printf("4) Aggiungi un vertice\n");
		printf("5) Aggiungi un arco\n");
		printf("6) Rimuovi un vertice\n");
		printf("7) Rimuovi un arco\n");
		printf("8) Trasponi il grafo\n");
		printf("9) Converti la rappresentazione del grafo (da %s a %s)\n", funz.impl_grafo == 1 ? (char *) "liste" : "matrice", funz.impl_grafo != 1 ? (char *) "liste" : "matrice");
		printf("10) Esegui la BFS a partire da un nodo\n");
		printf("11) Stampa il percorso tra due nodi con BFS\n");
		printf("12) Esegui la DFS\n");
		printf("13) Controlla se il grafo e' aciclico\n");
		printf("14) Scrivi il grafo su file\n");
		printf("15) Esegui Dijkstra\n");
		printf("16) Esegui Bellman Ford\n");
		printf("17) Stampa il num di vertici\n");
		c = getnum(0, 17, "\nInserisci il numero dell'operazione da effettuare: ");
		switch(c) {
			case 0:
				distruggi_grafo(mygraph, funz);
				printf("Programma terminato con successo!\n");
				return 0;
			break;
			case 1:
				funz.printAllAdj(mygraph, funz);
			break;
			case 2:
				funz.printAllVertex(mygraph, funz);
			break;
			case 3:
				{
					void *nodo;
					void **array;
					int numadj, i;
					printf("\nInserisci il nodo di cui stampare gli adiacenti:\n");
					nodo = funz.getInput();
					array = funz.getAdj(mygraph->adiacenze, nodo, &numadj, mygraph->nnodi, funz);
					if(numadj == 0) {
						printf("\nNessun adiacente.\n");
					} else {
						i = 0;
						printf("\n%d adiacenti:\n", numadj);
						while(i<numadj) {
							printf("[");
							funz.print(array[i]);
							printf("] ");
							i++;
						}
						printf("\n");
						if(numadj > 0)
							free(array);
						free(nodo);
					}
				}
			break;
			case 4:
				{
					void *nuovonodo;
					printf("\nInserisci il nodo da aggiungere\n");
					nuovonodo = funz.getInput();
					funz.addVertex(mygraph, nuovonodo, funz);
				}
			break;
			case 5:
			{
				int conosenza;
				void *dadove, *adove;
				float peso;
				printf("\nInserisci il nodo a cui aggiungere l'arco: \n");
				dadove = funz.getInput();
				printf("\nInserisci il nodo da raggiungere: \n");
				adove = funz.getInput();
				printf("\nChe tipo di arco vuoi realizzare: \n");
				printf("1) Con peso\n2) Senza peso\n");
				conosenza = getnum(1,2,"\nInserisci il numero dell'operazione da effettuare: ");
				if(conosenza == 1) {
					peso = getdouble(-99999,99999,"\nInserisci il peso dell'arco: ");
				} else {
					peso = 1.; //OPPURE ZERO
				}
				funz.addEdge(mygraph, dadove, adove, peso, funz);
			}
			break;
			case 6:
			{
				void *nodo;
				printf("\nInserisci il nodo da rimuovere\n");
				nodo = funz.getInput();
				funz.removeVertex(mygraph, nodo, funz);
				free(nodo);
			}
			break;
			case 7:
			{
				void *dadove, *adove;
				printf("\nInserisci il nodo a cui rimuovere l'arco: \n");
				dadove = funz.getInput();
				printf("\nInserisci il nodo da scollegare: \n");
				adove = funz.getInput();
				funz.removeEdge(mygraph, dadove, adove, funz);
				free(dadove);
				free(adove);
			}
			case 8:
				funz.trasposta(mygraph, funz);			
			break;
			case 9:
			{
				funz.convertiRappresentazione(mygraph, funz);
				switch_func(&funz);
				printf("Conversione effettuata con successo.\n");
			}
			break;
			case 10:
				{
					void **mappa;
					void **pred, **dist, *sorgente;
					int *col, i;
					printf("BFS:\n");
					printf("\nInserisci la sorgente: \n");
					sorgente = funz.getInput();
					dist = malloc(sizeof(void *) * mygraph->nnodi);
					col = malloc(sizeof(int*) * mygraph->nnodi);
					pred = malloc(sizeof(void *) * mygraph->nnodi);
					BFS(mygraph, sorgente, funz, dist, col, pred);
					mappa = funz.getMappa(mygraph);
					for(i=0;i<mygraph->nnodi;i++) {
						printf("col[");
						funz.print(mappa[i]);
						printf("] = %d\n", col[i]);
						printf("dist[");
						funz.print(mappa[i]);
						printf("] = ");
						if(dist[i])
							printf("%.1f", *(float*)dist[i]);
						else
							printf("NULL");
						printf("\n");
						printf("pred[");
						funz.print(mappa[i]);
						printf("] = [");
						funz.print(pred[i]);
						printf("]\n");
						printf("\n");
					}
					for(i=0;i<mygraph->nnodi;i++) {
						if(dist[i])
							free(dist[i]);
					}
					free(dist);
					free(col);
					free(pred);
					free(sorgente);
					free(mappa);
				}
			break;
			case 11:
			{
				void *dadove, *adove;
				void **pred, **dist;
				int *col, i;
				printf("\nInserisci il nodo sorgente: \n");
				dadove = funz.getInput();
				printf("\nInserisci il nodo destinazione: \n");
				adove = funz.getInput();
				dist = malloc(sizeof(void *) * mygraph->nnodi);
				col = malloc(sizeof(int) * mygraph->nnodi);
				pred = malloc(sizeof(void *) * mygraph->nnodi);
				BFS(mygraph, dadove, funz, dist, col, pred);
				stampa_percorso(mygraph, dadove, adove, funz, pred);
				printf("\n");
				for(i=0;i<mygraph->nnodi;i++) {
					if(dist[i])
						free(dist[i]);
				}
				free(dist);
				free(dadove);
				free(adove);
				free(pred);
				free(col);
			}
			break;
			case 12:
			{
					void **mappa;
					void **pred;
					int *col, i;
					printf("DFS:\n");
					col = malloc(sizeof(int) * mygraph->nnodi);
					pred = malloc(sizeof(void *) * mygraph->nnodi);
					mappa = funz.getMappa(mygraph);
					DFS(mygraph, funz, col, pred);
					for(i=0;i<mygraph->nnodi;i++) {
						printf("col[");
						funz.print(mappa[i]);
						printf("] = %d\n", col[i]);
						printf("pred[");
						funz.print(mappa[i]);
						printf("] = [");
						funz.print(pred[i]);
						printf("]\n");
						printf("\n");
					}
					free(pred);
					free(col);
					free(mappa);
			}
			break;
			case 13:
			{
				int aciclico;
				aciclico = check_aciclico(mygraph, funz);
				if(aciclico == 1)
					printf("Il grafo e' aciclico.\n");
				else
					printf("Il grafo non e' aciclico.\n");
			}
			break;
			case 14:
			{
				char *nomefile;
				nomefile = getalfa(1,100,"\nInserisci il percorso del file dove salvare il grafo: ");
				write_grafo(mygraph, nomefile, funz);
				free(nomefile);
			}
			break;
			case 15:
				{
					void **mappa;
					void **pred, **dist, *sorgente;
					int i;
					printf("Dijkstra:\n");
					printf("\nInserisci la sorgente: \n");
					sorgente = funz.getInput();
					dist = malloc(sizeof(void *) * mygraph->nnodi);
					pred = malloc(sizeof(void *) * mygraph->nnodi);
					Dijkstra(mygraph, sorgente, funz, dist, pred);
					mappa = funz.getMappa(mygraph);
					for(i=0;i<mygraph->nnodi;i++) {
						printf("dist[");
						funz.print(mappa[i]);
						printf("] = ");
						if(dist[i])
							printf("%.1f", *(float*)dist[i]);
						else
							printf("NULL");
						printf("\n");
						printf("pred[");
						funz.print(mappa[i]);
						printf("] = [");
						funz.print(pred[i]);
						printf("]\n");
						printf("\n");
					}
					for(i=0;i<mygraph->nnodi;i++) {
						if(dist[i])
							free(dist[i]);
					}
					free(dist);
					free(pred);
					free(sorgente);
					free(mappa);
				}
			break;
			case 16:
			{
				void *dadove;
				void **pred, **dist, **mappa;
				int i, bf;
				printf("\nInserisci il nodo sorgente: \n");
				dadove = funz.getInput();
				dist = malloc(sizeof(void *) * mygraph->nnodi);
				pred = malloc(sizeof(void *) * mygraph->nnodi);
				mappa = funz.getMappa(mygraph);
				bf = Bellman_Ford(mygraph, dadove, funz, dist, pred);
				printf("RISULTATO %d\n\n\n", bf);
				if(bf == 0) {
					printf("Errore: esistono cicli negativi all'interno del grafo, non e' pertanto possibile utilizzare l'algoritmo scelto.\n");
				} else if(bf == 1) {
					for(i=0;i<mygraph->nnodi;i++) {
						printf("pred[");
						funz.print(mappa[i]);
						printf("] = [");
						funz.print(pred[i]);
						printf("]\n");
						printf("dist[");
						funz.print(mappa[i]);
						printf("] = ");
						if(dist[i])
							printf("%.1f", *(float*)dist[i]);
						else
							printf("NULL");
						printf("\n");
					}
				}
				for(i=0;i<mygraph->nnodi;i++) {
					if(dist[i])
						free(dist[i]);
				}
				free(dist);
				free(dadove);
				free(pred);
				free(mappa);
			}
			break;
			case 17:
				printf("\nNumero di vertici: %d\n", mygraph->nnodi);
			break;
		}
	}
	return 0;
}
