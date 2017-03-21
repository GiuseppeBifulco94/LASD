#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "maze.h"
#include "utility.h"
#include "mainappoggio.h"

int main() {
	struct labyrinth *mygraph;
	int righe, colonne;
	int c;
	mygraph = alloca_labyrinth(mygraph);
	righe = 15; colonne = 15;
	init_maze_labyrinth(mygraph, righe, colonne, '|');
	mygraph->getpeso_maze = &getpeso_maze; 
	c = 1;
	srand(time(NULL));
	while(c != 0) {
		print_menu();
		c = getnum(0, 13, "\nInserisci il numero dell'operazione da effettuare: ");
		switch(c) {
			case 0:
				distruggi_labyrinth(mygraph);
				printf("Programma terminato con successo!\n");
				return 0;
			break;
			case 1:
				stampa_maze(mygraph);
			break;
			case 2:
				stampa_tutti_adiacenti(mygraph);
			break;
			case 3:
				aggiungi_vertice(mygraph);
			break;
			case 4:
				aggiungi_arco(mygraph);
			break;
			case 5:
				rimuovi_vertice(mygraph);
			break;
			case 6:
				rimuovi_arco(mygraph);
			break;
			case 7:
				carica_da_file(mygraph);
			break;
			case 8:
				colora_con_bfs(mygraph);
			break;
			case 9:
				stampa_percorso_minimo_bfs(mygraph);
			break;
			case 10:
				stampa_la_dfs(mygraph);
			break;
			case 11:
			{
				int aciclico;
				aciclico = check_aciclico(mygraph);
				if(aciclico == 1)
					printf("Il labyrinth e' aciclico.\n");
				else
					printf("Il labyrinth non e' aciclico.\n");
			}
			break;
			case 12:
				colora_con_dijkstra(mygraph);
			break;
			case 13:
				colora_con_astar(mygraph);
			break;
		}
	}
	return 0;
}
