#include <stdio.h>
#include <stdlib.h>
#include "utility.h"
#include "maze.h"
#include "mainappoggio.h"

void colora_con_astar(struct labyrinth *mygraph) {
	int dadove[1], adove[1];
	int **dist;
	int ***pred;
	int i, j;
	int x, y;
	dadove[0] = -1;
	adove[0] = -1;
	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(mygraph->maze[i][j] == 'D') {
				adove[0] = i;
				adove[1] = j;
			} else if(mygraph->maze[i][j] == 'X') {
				dadove[0] = i;
				dadove[1] = j;
			}
		}
	}
	if(dadove[0] == -1 || adove[0] == -1) {
		printf("Non e' stato definito l'ingresso (X) oppure l'uscita (D).\n");
		return;
	}
	dist = malloc(sizeof(int*)*mygraph->righe);
	pred = malloc(sizeof(int**)*mygraph->righe);
	for(i=0;i<mygraph->righe;i++) {
		dist[i] = malloc(sizeof(int)*mygraph->colonne);
		pred[i] = malloc(sizeof(int*)*mygraph->colonne);
		for(j=0;j<mygraph->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2);
	}
	Astar(mygraph, dadove[0], dadove[1], adove[0], adove[1], dist, pred);
	x = adove[0];
	y = adove[1];
	colora_percorso(mygraph, dadove, adove, pred, '*');
	mygraph->maze[adove[0]][adove[1]] = 'D';
	mygraph->maze[dadove[0]][dadove[1]] = 'X';
	stampa_maze(mygraph);
	colora_percorso(mygraph, dadove, adove, pred, ' ');
	mygraph->maze[adove[0]][adove[1]] = 'D';
	mygraph->maze[dadove[0]][dadove[1]] = 'X';
	for(i=0;i<mygraph->righe;i++) {
		free(dist[i]);
		for(j=0;j<mygraph->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(pred);
}
void colora_con_dijkstra(struct labyrinth *mygraph) {
	int i, j;
	int x, y;
	int a, b;
	int *uscita;
	int s[1];
	int **dist, **col;
	int ***pred;
	printf("Dijkstra:\n");
	s[0] = -1;
	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(mygraph->maze[i][j] == 'X') {
				s[0] = i;
				s[1] = j;
				a = i;
				b = j;
			}
		}
	}
	if(s[0] == -1) {
		printf("Non esiste l'ingresso nel labirinto!\n");
		return;
	}
	dist = malloc(sizeof(int*)*mygraph->righe);
	pred = malloc(sizeof(int**)*mygraph->righe);
	for(i=0;i<mygraph->righe;i++) {
		dist[i] = malloc(sizeof(int)*mygraph->colonne);
		pred[i] = malloc(sizeof(int*)*mygraph->colonne);
		for(j=0;j<mygraph->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}

	Dijkstra(mygraph, s[0], s[1], dist, pred);

	uscita = malloc(sizeof(int)*2);
	uscita[0] = -1;
	uscita[1] = -1;
	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(mygraph->maze[i][j] == 'D') {
				uscita[0] = i;
				uscita[1] = j;
			}
		}
	}
	if(uscita[0] == -1) {
		printf("Non esiste l'uscita nel labirinto\n");
		stampa_maze(mygraph);
	} else {
		x = uscita[0];
		y = uscita[1];
		colora_percorso(mygraph, s, uscita, pred, '*');
		mygraph->maze[x][y] = 'D';
		mygraph->maze[a][b] = 'X';
		stampa_maze(mygraph);
		colora_percorso(mygraph, s, uscita, pred, ' ');
		mygraph->maze[x][y] = 'D';
		mygraph->maze[a][b] = 'X';
	}

	for(i=0;i<mygraph->righe;i++) {
		free(dist[i]);
		for(j=0;j<mygraph->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(pred);
	free(uscita);

}
void stampa_la_dfs(struct labyrinth *mygraph) {
	int **col;
	int ***pred;
	int i, j;
	col = malloc(sizeof(int*)*mygraph->righe);
	pred = malloc(sizeof(int**)*mygraph->righe); 
	for(i=0;i<mygraph->righe;i++) {
		col[i] = malloc(sizeof(int)*mygraph->colonne);
		pred[i] = malloc(sizeof(int*)*mygraph->colonne);
		for(j=0;j<mygraph->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}
	printf("DFS:\n");
		
	DFS(mygraph, col, pred);

	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(col[i][j] == 0) {
				printf("ERRORE\n");
			}
		}
	}

	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			printf("col[%d][%d] = %d\n", i, j, col[i][j]);
			printf("pred[%d][%d] = [%d][%d]\n", i, j, pred[i][j][0], pred[i][j][1]);
			printf("\n");
		}
	}

	for(i=0;i<mygraph->righe;i++) {
		free(col[i]);
		for(j=0;j<mygraph->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(col);
	free(pred);

}
void stampa_percorso_minimo_bfs(struct labyrinth *mygraph) {
	int dadove[1], adove[1];
	int **dist, **col;
	int ***pred;
	int i, j;
	dist = malloc(sizeof(int*)*mygraph->righe);
	col = malloc(sizeof(int*)*mygraph->righe);
	pred = malloc(sizeof(int**)*mygraph->righe); 
	for(i=0;i<mygraph->righe;i++) {
		dist[i] = malloc(sizeof(int)*mygraph->colonne);
		col[i] = malloc(sizeof(int)*mygraph->colonne);
		pred[i] = malloc(sizeof(int*)*mygraph->colonne);
		for(j=0;j<mygraph->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}
	printf("\nInserisci le coordinate della sorgente: \n");
	dadove[0] = getnum(0,50,"X: ");
	dadove[1] = getnum(0,50,"Y: ");
	printf("\nInserisci le coordinate della destinazione: \n");
	adove[0] = getnum(0,50,"X: ");
	adove[1] = getnum(0,50,"Y: ");
	BFS(mygraph, dadove, dist, col, pred);

	stampa_percorso(mygraph, dadove, adove, pred);
	printf("\n");

	for(i=0;i<mygraph->righe;i++) {
		free(dist[i]);
		free(col[i]);
		for(j=0;j<mygraph->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(col);
	free(pred);
}
void colora_con_bfs(struct labyrinth *mygraph) {
	int i, j;
	int x, y;
	int a, b;
	int *uscita;
	int s[1];
	int **dist, **col;
	int ***pred;
	printf("BFS:\n");
	s[0] = -1;
	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(mygraph->maze[i][j] == 'X') {
				s[0] = i;
				s[1] = j;
				a = i;
				b = j;
			}
		}
	}
	if(s[0] == -1) {
		printf("Non esiste l'ingresso nel labirinto!\n");
		return;
	}
	dist = malloc(sizeof(int*)*mygraph->righe);
	col = malloc(sizeof(int*)*mygraph->righe);
	pred = malloc(sizeof(int**)*mygraph->righe);
	for(i=0;i<mygraph->righe;i++) {
		dist[i] = malloc(sizeof(int)*mygraph->colonne);
		col[i] = malloc(sizeof(int)*mygraph->colonne);
		pred[i] = malloc(sizeof(int*)*mygraph->colonne);
		for(j=0;j<mygraph->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}

	BFS(mygraph, s, dist, col, pred);

	uscita = malloc(sizeof(int)*2);
	uscita[0] = -1;
	uscita[1] = -1;
	for(i=0;i<mygraph->righe;i++) {
		for(j=0;j<mygraph->colonne;j++) {
			if(mygraph->maze[i][j] == 'D') {
				uscita[0] = i;
				uscita[1] = j;
			}
		}
	}
	if(uscita[0] == -1) {
		printf("Non esiste l'uscita nel labirinto\n");
		stampa_maze(mygraph);
	} else {
		x = uscita[0];
		y = uscita[1];
		colora_percorso(mygraph, s, uscita, pred, '*');
		mygraph->maze[x][y] = 'D';
		mygraph->maze[a][b] = 'X';
		stampa_maze(mygraph);
		colora_percorso(mygraph, s, uscita, pred, ' ');
		mygraph->maze[x][y] = 'D';
		mygraph->maze[a][b] = 'X';
	}
	for(i=0;i<mygraph->righe;i++) {
		free(dist[i]);
		free(col[i]);
		for(j=0;j<mygraph->colonne;j++)
			free(pred[i][j]);
			free(pred[i]);
		}
	free(dist);
	free(col);
	free(pred);
	free(uscita);
}
void carica_da_file(struct labyrinth *mygraph) {
	int i, j, ret;
	char *stringa;
	char allwd[5] = "0-DX";
	stringa = getalfa(1,999,"\nInserisci il percorso del file: ");
	printf("Ci e' riuscito? %s\n", (ret = load_maze_from_file(mygraph, stringa, allwd, 5, 1, 1,100,100)) == 1 ? "Si" : "No");
	free(stringa);
	if(ret) {
		for(i=0;i<mygraph->righe;i++) {
			for(j=0;j<mygraph->colonne;j++) {
				if(mygraph->maze[i][j] == '0')
					mygraph->maze[i][j] = ' ';
			}
		}
	}
	stampa_maze(mygraph);
}
void rimuovi_arco(struct labyrinth *mygraph) {
	int x, y, dir;
	int res;
	printf("Inserisci le coordinate:\nX:\n");
	x = getnum(0,50,"X:\n");
	y = getnum(0,50,"Y:\n");
	printf("Scegli la direzione:\n0) NORD\n1) EST\n2) SUD\n3) OVEST\n");
	dir = getnum(0,3,"Direzione: ");
	res = add_maze_edge(mygraph, x, y, dir, '-');
	if(res == 1)
		printf("Operazione effettuata con successo!\n");
	else
		printf("Errore!\n");
}
void rimuovi_vertice(struct labyrinth *mygraph) {
	int x, y;
	int res;
	printf("Inserisci le coordinate:\nX:\n");
	x = getnum(0,50,"X:\n");
	y = getnum(0,50,"Y:\n");
	res = add_maze_vertex(mygraph, x, y, '|'); /* in linea di principio aggiungi un muro */
	if(res == 1)
		printf("Operazione effettuata con successo!\n");
	else
		printf("Errore!\n");
}
void aggiungi_arco(struct labyrinth *mygraph) {
	int x, y, dir;
	int res;
	printf("Inserisci le coordinate:\nX:\n");
	x = getnum(0,50,"X:\n");
	y = getnum(0,50,"Y:\n");
	printf("Scegli la direzione:\n0) NORD\n1) EST\n2) SUD\n3) OVEST\n");
	dir = getnum(0,3,"Direzione: ");
	res = add_maze_edge(mygraph, x, y, dir, ' ');
	if(res == 1)
		printf("Operazione effettuata con successo!\n");
	else
		printf("Errore!\n");
}
void aggiungi_vertice(struct labyrinth *mygraph) {
	int x, y;
	int res;
	printf("Inserisci le coordinate:\nX:\n");
	x = getnum(0,50,"X:\n");
	y = getnum(0,50,"Y:\n");
	res = add_maze_vertex(mygraph, x, y, ' '); /* in linea di principio aggiungi un corridoio */
	if(res == 1)
		printf("Operazione effettuata con successo!\n");
	else
		printf("Errore!\n");
}
void stampa_tutti_adiacenti(struct labyrinth *mygraph) {
	int x, y;
	char *array;
	printf("\nInserisci le coordinate:\n");
	x = getnum(0,50,"X:\n");
	y = getnum(0,50,"Y:\n");
	array = get_maze_adj(mygraph, x, y);
	if(array[0] == '\0' && array[1] == '\0' && array[2] == '\0' && array[3] == '\0') {
		printf("\nNessun adiacente.\n");
		return;
	}
	printf("\nAdiacente nord: ");
	if(array[0] == '\0')
		printf("[Non esiste]");
	else
		printf("%c", array[0]);
	printf("\nAdiacente est: ");
	if(array[1] == '\0')
		printf("[Non esiste]");
	else
		printf("%c", array[1]);
	printf("\nAdiacente sud: ");
	if(array[2] == '\0')
		printf("[Non esiste]");
	else
		printf("%c", array[2]);
	printf("\nAdiacente ovest: ");
	if(array[3] == '\0')
		printf("[Non esiste]");
	else
		printf("%c", array[3]);
	printf("\n");
	if(array)
		free(array);
}

int getpeso_maze(struct labyrinth *graph, int x, int y) {
	int ret;
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Out of cordinate.\n");
		return -1;
	} else {
		switch(graph->maze[x][y]) {
			default:
				ret = INFINITO;
			break;
			case '|':
				ret = INFINITO;
			break;
			case ' ':
				ret = 1;
			break;
		}
		return ret;
	}
}

void print_menu() {
	printf("0) Esci\n");
	printf("1) Stampa il labirinto\n");
	printf("2) Stampa tutti gli adiacenti di un vertice\n");
	printf("3) Aggiungi un vertice (ins. corridoio)\n");
	printf("4) Aggiungi un arco\n");
	printf("5) Rimuovi un vertice (ins. muro)\n");
	printf("6) Rimuovi un arco\n");
	printf("7) Carica il labirinto da file\n");
	printf("8) Esegui stampa percorso minimo di uscita a partire da un nodo (con BFS)\n");
	printf("9) Stampa il percorso tra due nodi con BFS\n");
	printf("10) Esegui la DFS\n");
	printf("11) Controlla se il labyrinth e' aciclico\n");
	printf("12) Esegui stampa percorso minimo di uscita a partire da un nodo (con Dijkstra)\n");
	printf("13) Esegui stampa percorso minimo di uscita a partire da un nodo (con A*)\n");
}