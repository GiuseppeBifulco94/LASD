#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "maze.h"
#include "heap.h"
#include "coda.h"

#include <unistd.h>


void distruggi_labyrinth(struct labyrinth *graph) {
	int i, j;
	if(graph) {
		for(i=0;i<graph->righe;i++) {
			if(graph->maze[i])
				free(graph->maze[i]);
		}
		if(graph->maze)
			free(graph->maze);
		graph->maze = NULL;
		free(graph);
		graph = NULL;
	}
}
struct labyrinth *alloca_labyrinth(struct labyrinth *graph) {
	graph = malloc(sizeof(struct labyrinth));
	graph->righe = 0;
	graph->colonne = 0;
}

void alloca_maze(struct labyrinth *graph, int righe, int colonne) {
	int i;
	graph->maze = (char **) malloc(sizeof(char *)*righe);
	for(i=0;i<righe;i++)
		graph->maze[i] = (char *) malloc(sizeof(char)*colonne);
}

void init_maze_labyrinth(struct labyrinth *graph, int righe, int colonne, char defaultchar) {
	int i;
	int j;
	graph->righe = righe;
	graph->colonne = colonne;
	alloca_maze(graph, righe, colonne);
	if(defaultchar != '\0') { //riempi il labirinto di tutti 'defaultchar' se il parametro passato è diverso da \0
		for(i=0;i<righe;i++) {
			for(j=0;j<colonne;j++) {
				graph->maze[i][j] = defaultchar;
			}
		}
	}
}
void stampa_maze(struct labyrinth *graph) {
	int i, j;
	printf("\nDIMENSIONI: %d,%d\n",graph->righe, graph->colonne);
	printf("  ");
	for(i=0;i<graph->colonne;i++) {
		printf("%d ", i%10);
	}
	printf("\n");
	for(i=0;i<graph->righe;i++) {
		printf("%d ", i%10);
		for(j=0;j<graph->colonne;j++) {
			printf("%c ", graph->maze[i][j]);
		}
		printf("\n");
	}
}

int add_maze_vertex(struct labyrinth *graph, int x, int y, char el) {
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Fuori di coordinate.\n");
		return 0;
	}
	if(graph->maze[x][y] == el) {
		printf("Il vertice esiste gia'.\n");
		return 0;
	} else {
		graph->maze[x][y] = el;
		return 1;
	}
}

int is_maze_vertex(struct labyrinth *graph, int x, int y, char el) {
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Fuori di coordinate.\n");
		return 0;
	}
	if(graph->maze[x][y] == el) {
		return 1;
	} else {
		return 0;
	}
}

int add_maze_edge(struct labyrinth *graph, int x, int y, int dir, char el) {
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Fuori di coordinate.\n");
		return 0;
	}
	if(graph->maze[x][y] != el) {
		printf("Non puoi creare un arco se non hai un origine dello stesso carattere.\n");
		return 0;
	}
	switch(dir) {
		case 0:
			x--;
		break;
		case 1:
			y++;
		break;
		case 2:
			x++;
		break;
		case 3:
			y--;
		break;
	}
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Fuori di coordinate.\n");
		return 0;
	}
	if(graph->maze[x][y] == el) {
		printf("Gia' presente.\n");
		return 0;
	} else {
		graph->maze[x][y] = el;
		return 1;
	}
}


char *get_maze_adj(struct labyrinth *graph, int x, int y) {
	char *ret;
	ret = malloc(sizeof(char)*4);
	/* Ritorna ret[0] ret[1] ret[2] ret[3] , che sono rispettivamente UP RIGHT DOWN LEFT . Valgono \0 se non esiste adj. */
	if(x > 0) {
		ret[0] = graph->maze[x-1][y];
	} else {
		ret[0] = '\0';
	}
	if(y < graph->colonne) {
		ret[1] = graph->maze[x][y+1];
	} else {
		ret[1] = '\0';
	}
	if(x < graph->righe) {
		ret[2] = graph->maze[x+1][y];
	} else {
		ret[2] = '\0';
	}
	if(y > 0) {
		ret[3] = graph->maze[x][y-1];
	} else {
		ret[3] = '\0';
	}
	return ret;
}

void freearray_ofint(void *a) {
	int *tmp;
	if(a) {
		tmp = ((struct coda*)a)->content;
		if(tmp)
			free(tmp);
		free(a);
	}
}

void BFS(struct labyrinth *graph, int *s, int **dist, int **col, int ***pred) {
	struct coda *Q;
	void *u;
	int i, j, k, x, y;
	int dir;
	int *appoggio;
	for(i=0;i<graph->righe; i++) {
		for(j=0;j<graph->colonne;j++) {
			dist[i][j] = INFINITO; //infinito ?
			pred[i][j][0] = -1; //pred x
			pred[i][j][1] = -1; //pred y
			col[i][j] = 0; //0->bianco , 1->grigio , 2->nero
			if(graph->maze[i][j] == '|' || graph->maze[i][j] == '-' || graph->maze[i][j] == '#') { // Non puoi passare per il muro
				col[i][j] = 2;
			}
		}
	}
	if(s[0] < 0 || s[0] >= graph->righe || s[1] < 0 || s[1] >= graph->colonne) {
		printf("Out of coordinate.\n");
		return;
	}
	pred[s[0]][s[1]][0] = s[0];
	pred[s[0]][s[1]][1] = s[1];
	dist[s[0]][s[1]] = 0;
	col[s[0]][s[1]] = 1;
	Q = NULL;
	appoggio = malloc(sizeof(int)*2);
	appoggio[0] = s[0];
	appoggio[1] = s[1];
	Q = accoda_coda(Q, appoggio);
	while(Q != NULL) {
		u = testa_coda(Q);
		//visita u
		x = ((int*)u)[0];
		y = ((int*)u)[1];
		dir = rand() % 4;
		dir--;
		for(k=0;k<4;k++) {
			dir = fix_circular(dir+1, 4);
			switch(dir) {
				default:
				case 0:   i = x-1; j = y;   break;
				case 1:   i = x; j = y+1;   break;
				case 2:   i = x+1; j = y;   break;
				case 3:   i = x; j = y-1;   break;
			}
			if(i < 0 || j < 0 || i > graph->righe-1 || j > graph->colonne-1)
				continue;
			if(col[i][j] == 0) {
				col[i][j] = 1;
				dist[i][j] = (dist[x][y] + 1);
				pred[i][j][0] = x;
				pred[i][j][1] = y;
				appoggio = malloc(sizeof(int)*2);
				appoggio[0] = i; appoggio[1] = j;
				Q = accoda_coda(Q, appoggio);
			}
		}
		col[x][y] = 2;
		Q = decoda_coda(Q, &freearray_ofint);
	}
}


int fix_circular(int i, int max) {
	if(i < 0) {
		while(i<0) {
			i += max;
		}
	} else if(i >= max) {
		i -= max;
	}
	return i;
}

void stampa_percorso(struct labyrinth *graph, int *s, int *v, int ***pred) {
	int appoggio[1];
	if(s[0] == v[0] && s[1] == v[1]) { // uguali
		printf(" [%d][%d] ", s[0], s[1]);
	} else {
		if(pred[v[0]][v[1]][0] == -1) {
			printf("Non esiste percorso tra [%d][%d] e [%d][%d]\n", s[0],s[1], v[0],v[1]);
		} else {
			appoggio[0] = pred[v[0]][v[1]][0];
			appoggio[1] = pred[v[0]][v[1]][1];
			stampa_percorso(graph, s, appoggio, pred);
			printf(" [%d][%d] ", v[0],v[1]);
		}
	}
}
void colora_percorso(struct labyrinth *graph, int *s, int *v, int ***pred, char c) {
	int appoggio[1];
	if(s[0] == v[0] && s[1] == v[1]) { // uguali
		graph->maze[v[0]][v[1]] = c;
	} else {
		if(pred[v[0]][v[1]][0] == -1) {
			printf("Non esiste percorso tra [%d][%d] e [%d][%d]\n", s[0],s[1], v[0],v[1]);
		} else {
			appoggio[0] = pred[v[0]][v[1]][0];
			appoggio[1] = pred[v[0]][v[1]][1];
			colora_percorso(graph, s, appoggio, pred, c);
			graph->maze[v[0]][v[1]] = c;
		}
	}
}


void DFS(struct labyrinth *graph, int **col, int ***pred) {
	int i, j;

	for(i=0;i<graph->righe; i++) {
		for(j=0;j<graph->colonne;j++) {
			pred[i][j][0] = -1; //pred x
			pred[i][j][1] = -1; //pred y
			col[i][j] = 0; //0->bianco , 1->grigio , 2->nero
			if(graph->maze[i][j] == '|') { // Non puoi passare per il muro
				col[i][j] = 2;
			}
		}
	}

	for(i=0;i<graph->righe;i++) {
		for(j=0;j<graph->colonne;j++) {
			if(col[i][j] == 0)
				DFS_visit(graph, i, j, col, pred);
		}
	}
}
void DFS_visit(struct labyrinth *graph, int x, int y, int **col, int ***pred) {
	int k, i, j;
	col[x][y] = 1;
	for(k=0;k<4;k++) {
		switch(k) {
			default:
			case 0:   i = x-1; j = y;   break;
			case 1:   i = x; j = y+1;   break;
			case 2:   i = x+1; j = y;   break;
			case 3:   i = x; j = y-1;   break;
		}
		i = fix_circular(i, graph->righe);
		j = fix_circular(j, graph->colonne);
		if(col[i][j] == 0) {
			pred[i][j][0] = x;
			pred[i][j][1] = y;
			DFS_visit(graph, i, j, col, pred);
		}
	}
	col[x][y] = 2;
}

int check_aciclico(struct labyrinth *graph) {
	int i, j;
	int **col;
	int ret;
	col = malloc(sizeof(int*)*graph->righe);
	for(i=0;i<graph->righe; i++) {
		col[i] = malloc(sizeof(int)*graph->colonne);
		for(j=0;j<graph->colonne;j++) {
			col[i][j] = 0; //0->bianco , 1->grigio , 2->nero
			if(graph->maze[i][j] == '|') { // Non puoi passare per il muro
				col[i][j] = 2;
			}
		}
	}

	ret = 1;
	for(i=0;i<graph->righe;i++) {
		for(j=0;j<graph->colonne;j++) {
			if(col[i][j] == 0)
				ret = check_aciclicomain(graph, i, j, col);
			if(!ret)
				break;
		}
	}
	for(i=0;i<graph->righe;i++)
		free(col[i]);
	free(col);
	return ret;
}

int check_aciclicomain(struct labyrinth *graph, int x, int y, int **col) {

	int k, i, j;

	col[x][y] = 1;
	for(k=0;k<4;k++) {
		switch(k) {
			default:
			case 0:   i = x-1; j = y;   break;
			case 1:   i = x; j = y+1;   break;
			case 2:   i = x+1; j = y;   break;
			case 3:   i = x; j = y-1;   break;
		}
		i = fix_circular(i, graph->righe);
		j = fix_circular(j, graph->colonne);
		if(col[i][j] == 0) {
			if(check_aciclicomain(graph, i, j, col) == 0)
				return 0;
		} else if(col[i][j] == 1) {
			return 0;
		}
	}
	col[x][y] = 2;
	return 1;
}

int cmparray_ofint(void *a, void *b) {
	int a1, a2, b1, b2;
	if(a == NULL || b == NULL) {
		return -1;
	}
	a1 = ((int*)a)[0];
	a2 = ((int*)a)[1];
	b1 = ((int*)b)[0];
	b2 = ((int*)b)[1]; 
	if(a1 == b1) {
		if(a2 == b2) {
			return 0;
		} else {
			if(a2 > b2) {
				return 1;
			} else {
				return -1;
			}
		}
	} else {
		if(a1 > b1)
			return 1;
		else
			return -1;
	}
}
void printx(void *a) {
	if(a)
		printf("[%d,%d]",((int*)a)[0],((int*)a)[1]);
	else
		printf("[NULL]");
}
int stima(int s_x, int s_y, int d_x, int d_y) {
	int a, b;
	a = s_x - d_x;
	b = s_y - d_y;
	if(a < 0)
		a *= -1;
	if(b < 0)
		b *= -1;
	return (a+b);
}
void Astar(struct labyrinth *graph, int s_x, int s_y, int d_x, int d_y, int **dist, int ***pred) {
	int i, j, current_i, current_j, k, dir, cost, dist_tra, flag;
	int **f, *appoggio;
	int found;
	struct heap *Closed, *Open;
	struct coppia *current;
	if(s_x < 0 || s_x >= graph->righe || s_y < 0 || s_y >= graph->colonne) {
		printf("Out of cordinate.\n");
		return;
	}
	if(d_x < 0 || d_x >= graph->righe || d_y < 0 || d_y >= graph->colonne) {
		printf("Out of cordinate.\n");
		return;
	}
	f = malloc(sizeof(int*)*graph->righe);
	for(i=0;i<graph->righe;i++) {
		f[i] = malloc(sizeof(int)*graph->colonne);
		for(j=0;j<graph->colonne;j++) {
			if(i == s_x && j == s_y) {
				dist[s_x][s_y] = 0;
				pred[s_x][s_y][0] = s_x;
				pred[s_x][s_y][1] = s_y;
			} else {
				dist[i][j] = INFINITO;
				pred[i][j][0] = -1;
				pred[i][j][1] = -1;
			}
		}
	}
	f[s_x][s_y] = stima(s_x, s_y, d_x, d_y); /* Stima per la sorgente */

	found = 0; /* Non ancora trovato il path */

	/* Inizializzo i due minHeap */
	Closed = alloca_heap(Closed);
	Open = alloca_heap(Open);
	setHeap(Closed, 4, 1, 1); //Argomenti: {Heap, 4:tipo per scopi interni, metodo implementazione, minheap}
	setHeap(Open, 4, 1, 1); //Argomenti: {Heap, 4:tipo per scopi interni, metodo implementazione, minheap}
	Closed = init_heap(Closed, 0);
	Open = init_heap(Open, 0);
	Closed->cmp = &cmparray_ofint;
	Open->cmp = &cmparray_ofint;
	Closed->print = &printx;
	Open->print = &printx;

	appoggio = malloc(sizeof(int)*2);
	appoggio[0] = s_x;
	appoggio[1] = s_y;
	InsertKey(Open, appoggio, f[s_x][s_y]);

	while(Open->heapsize > 0 && !found) {
		current = ((struct coppia *)extractHeap(Open));
		if(!current)
			break;
		current_i = ((int*) (current->content))[0];
		current_j = ((int*) (current->content))[1];
		if(current_i == d_x && current_j == d_y) {
			found = 1;
			Open->libera(current);
			break;
		}
		appoggio = malloc(sizeof(int)*2);
		appoggio[0] = current_i;
		appoggio[1] = current_j;
		InsertKey(Closed, appoggio, f[current_i][current_j]);


		dir = rand() % 4;
		dir--;
		for(k=0;k<4;k++) {
			i = current_i;
			j = current_j;
			dir = fix_circular(dir+1, 4);
			switch(dir) {
				default:
				case 0:
					i--;
				break;
				case 1:
					j++;
				break;
				case 2:
					i++;
				break;
				case 3:
					j--;
				break;
			}
			if(i < 0 || j < 0 || i > graph->righe-1 || j > graph->colonne-1) {
				continue;
			}

			if(graph->maze[i][j] == '|' || graph->maze[i][j] == '-' || graph->maze[i][j] == '#') { // Non puoi passare per il muro
				continue;
				dist_tra = INFINITO;
			} else {
				dist_tra = 1;
			}
			appoggio = malloc(sizeof(int)*2);
			appoggio[0] = i;
			appoggio[1] = j;
			if(heapgetnumfromel(Closed, appoggio) != -1) { //Ignora l'adiacente se e' stato gia' valutato
				free(appoggio);
				continue;
			}
			free(appoggio);
			cost = dist[current_i][current_j] + dist_tra;
			flag = 0;
			appoggio = malloc(sizeof(int)*2);
			appoggio[0] = i;
			appoggio[1] = j;
			if(heapgetnumfromel(Open, appoggio) == 1) {
				free(appoggio);
				if(cost >= dist[i][j]) {
					continue;
				}
			} else {
				free(appoggio);
				flag = 1;
			}
			pred[i][j][0] = current_i;
			pred[i][j][1] = current_j;
			dist[i][j] = cost;
			f[i][j] = cost + stima(i,j,d_x,d_y);
			if(flag) {
				appoggio = malloc(sizeof(int)*2);
				appoggio[0] = i;
				appoggio[1] = j;
				InsertKey(Open, appoggio, f[i][j]);
			}
		}
		Open->libera(current);
	}
	for(i=0;i<graph->righe;i++) {
		free(f[i]);
	}
	free(f);

	if(Closed)
		distruggi_heap(Closed);

	if(Open)
		distruggi_heap(Open);

}

void Dijkstra(struct labyrinth *graph, int x, int y, int **dist, int ***pred) {
	int i, j, k, u_i, u_j;
	int alt, dist_tra;
	int *appoggio;
	int dir;
	struct heap *Q;
	struct coppia *u;
	// inizializzo il minheap
	Q = NULL;
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Out of cordinate.\n");
		return;
	}
	Q = alloca_heap(Q);
	setHeap(Q, 4, 1, 1); //Argomenti: {Heap, 4:tipo per scopi interni, metodo implementazione, minheap}
	Q = init_heap(Q, 0);
	Q->cmp = &cmparray_ofint;
	Q->print = &printx;
	for(i=0;i<graph->righe;i++) {
		for(j=0;j<graph->colonne;j++) {
			if(i == x && j == y) {
				dist[x][y] = 0;
				pred[x][y][0] = x;
				pred[x][y][1] = y;
			} else {
				dist[i][j] = INFINITO;
				pred[i][j][0] = -1;
				pred[i][j][1] = -1;
			}
			appoggio = malloc(sizeof(int)*2);
			appoggio[0] = i;
			appoggio[1] = j;
			InsertKey(Q, appoggio, dist[i][j]);
		}
	}

	while(Q->heapsize > 0) {
		u = ((struct coppia *)extractHeap(Q));
		if(!u)
			break;
		u_i = ((int*) (u->content))[0];
		u_j = ((int*) (u->content))[1];
		if(dist[u_i][u_j] == INFINITO) {
			Q->libera(u);
			break;
		}
		dir = rand() % 4;
		dir--;
		for(k=0;k<4;k++) {
			i = u_i;
			j = u_j;
			dir = fix_circular(dir+1, 4);
			switch(dir) {
				default:
				case 0:
					i--;
				break;
				case 1:
					j++;
				break;
				case 2:
					i++;
				break;
				case 3:
					j--;
				break;
			}
			if(i < 0 || j < 0 || i > graph->righe-1 || j > graph->colonne-1) {
				continue;
			}

			if(graph->maze[i][j] == '|' || graph->maze[i][j] == '-' || graph->maze[i][j] == '#') { // Non puoi passare per il muro
				dist_tra = INFINITO;
			} else {
				dist_tra = 1;
			}
			alt = dist[u_i][u_j] + dist_tra;
			if(alt < dist[i][j]) {
				dist[i][j] = alt;
				pred[i][j][0] = u_i;
				pred[i][j][1] = u_j;
				appoggio = malloc(sizeof(int)*2);
				appoggio[0] = i;
				appoggio[1] = j;
				DecreaseKey(Q, heapgetnumfromel(Q,appoggio), alt);
				free(appoggio);
			}
		}
		Q->libera(u);
	}
	if(Q)
		distruggi_heap(Q);
}

int Bellman_Ford(struct labyrinth *graph, int x, int y, int **dist, int ***pred) {
	int i, j, u_i, u_j, k;
	int alt;

	// inizializzo il minheap
	if(x < 0 || x >= graph->righe || y < 0 || y >= graph->colonne) {
		printf("Out of cordinate.\n");
		return -1;
	}

	//assegno i valori iniziali di dist e pred alla sorgente
	dist[x][y] = 0; /* oppure il peso di [x][y]? */
	pred[x][y][0] = x;
	pred[x][y][1] = y;

	//assegno tutti i valori provvisori agli altri elementi (pred indefinito e dist infinita)
	for(i=0;i<graph->righe; i++) {
		for(j=0;j<graph->colonne;j++) {
			if(i==x && j==y)
				continue;
			dist[i][j] = INFINITO;
			pred[i][j][0] = -1;
			pred[i][j][1] = -1;
		}
	}

	for(u_i=0;u_i<graph->righe;u_i++) {
		for(u_j=0;u_j<graph->colonne;u_j++) {
			if(u_i == (graph->righe-1) && u_j == (graph->colonne-2)) { //rilassa tutti gli archi (righe*colonne - 1) volte
				u_j++;
				continue;
			}

			//per ogni arco
			for(k=0;k<4;k++) {
				switch(k) {
					default:
					case 0:   i = u_i-1; j = u_j;   break;
					case 1:   i = u_i; j = u_j+1;   break;
					case 2:   i = u_i+1; j = u_j;   break;
					case 3:   i = u_i; j = u_j-1;   break;
				}
				i = fix_circular(i, graph->righe);
				j = fix_circular(j, graph->colonne);

				// RELAX
				alt = dist[u_i][u_j] + graph->getpeso_maze(graph, i, j);
				if(alt < dist[i][j]) {
					dist[i][j] = alt;
					pred[i][j][0] = u_i;
					pred[i][j][1] = u_j;
				}
			}
		}
	}
	// controlla se non ci sono cicli negativi
	for(u_i=0;u_i<graph->righe;u_i++) {
		for(u_j=0;u_j<graph->colonne;u_j++) {
			for(k=0;k<4;k++) {
				switch(k) {
					default:
					case 0:   i = u_i-1; j = u_j;   break;
					case 1:   i = u_i; j = u_j+1;   break;
					case 2:   i = u_i+1; j = u_j;   break;
					case 3:   i = u_i; j = u_j-1;   break;
				}
				i = fix_circular(i, graph->righe);
				j = fix_circular(j, graph->colonne);
				alt = dist[u_i][u_j] + graph->getpeso_maze(graph, i, j);
				//printf("DA [%d][%d] A [%d][%d] era %d ma puo essere %d\n\n\n", u_i, u_j, i, j, dist[i][j], alt);
				if(alt < dist[i][j]) { // vuol dire che posso rilassare ulteriormente (all'infinito) quindi cicli negativi
					return 0;
				}
			}
		}
	}
	//ho controllato che tutte le distanze calcolate precedentemente non possono essere ulteriormente ridotte
	//non ci sono quindi cicli negativi
	//e le distanze sono corrette
	return 1;
}

int load_maze_from_file(struct labyrinth *graph, char *file, char *allowed, int dim, int min_righe, int min_colonne, int massimo_righe, int massimo_colonne) {
	FILE *fd;
	char **old, **new_maze, **last_maze;
	int i, max_righe, max_colonne, j, k, per_riga, err, a, ok;
	char c;
	fd = fopen(file, "r");
	if(!fd) {
		return 0;
	}
	i = 0;
	j = 0;
	max_righe = 10;
	max_colonne = 10;
	per_riga = 0;
	err = 0;
	/* Comincio a creare una matrice di caratteri di [max_righe][max_colonne] */
	new_maze = malloc(sizeof(char*)*max_righe);
	for(k=0;k<max_righe;k++)
		new_maze[k] = malloc(sizeof(char)*max_colonne);
	c = '\0';
	/* Loop.. */
	do {
		c = fgetc(fd); /* Prendo il carattere */
		if(feof(fd)) {
			if(j != per_riga) {
				err = 1;
			}
			break;
		}
		if(i > 0 && j > per_riga) { /* La riga attuale non aveva 'per_riga' colonne */
			err = 1;
			break;
		}
		if(i > massimo_righe || per_riga > massimo_colonne) {
			err = 1;
			break;
		}
		if(j > max_colonne-1) { /* Se devo reallocare le colonne.. */
			max_colonne = max_colonne * 2;
			for(k=0;k<max_righe;k++)
				new_maze[k] = realloc(new_maze[k], sizeof(char)*max_colonne);
		}
		if(i > max_righe-1) { /* Se devo reallocare le righe.. */
			max_righe = max_righe * 2;
			new_maze = realloc(new_maze, sizeof(char*)*max_righe);
			for(k=0;k<max_righe;k++) {
				/* Realloco / Alloco le colonne nuove*/
				if(k < max_righe/2)
					new_maze[k] = realloc(new_maze[k], sizeof(char)*max_colonne);
				else
					new_maze[k] = malloc(sizeof(char)*max_colonne);
			}
		}
		if(c == '\n' || c == '\r') { /* Se e' un 'a capo', allora */
			 /* Se ancora non ho scritto nessuna linea,
			 ripeti il ciclo senza considerare
			 questo 'a capo' iniziale */
			if(j == 0) {
				continue;
			}
			if(i == 0) { /* Ho terminato la prima riga, ogni riga deve avere 'per_riga' colonne */
				per_riga = j;
			}
			if(j < per_riga) { /* La riga attuale non aveva 'per_riga' colonne */
				err = 1;
				break;
			}
			i++;
			j = 0;
		} else { /* Altrimenti se non e' un 'a capo' */
			ok = 0;
			for(a=0;a<dim;a++) {
				if(allowed[a] == c) {
					ok = 1;
					break;
				}
			}
			if(ok) /* Il carattere e' valido */
				new_maze[i][j] = c; /* Inserisco il carattere nella matrice */
			else {
				err = 1;
				break;
			}
			j++; /* Incremento la colonna e continuo */
		}
	} while(!err);
	i++;
	fclose(fd);
	/* Se si e' verificato un errore,
	oppure se il nuovo labirinto e' troppo
	piccolo rispetto al minor numero di righe
	e colonne specificato da argomenti, allora..  */
	if(err || i < min_righe || j < min_colonne) {
		/* Libera ciò che aveva allocato */
		for(k=0;k<max_righe;k++)
			free(new_maze[k]);
		if(new_maze)
			free(new_maze);
		/* Ritorna errore */
		return 0;
	}
	last_maze = malloc(sizeof(char*)*i);
	for(k=0;k<i;k++) {
		last_maze[k] = malloc(sizeof(char)*j);
		for(a=0;a<j;a++)
			last_maze[k][a] = new_maze[k][a];
	}

	for(k=0;k<max_righe;k++)
		free(new_maze[k]);
	free(new_maze);

	old = graph->maze;

	graph->maze = NULL;
	graph->maze = last_maze;

	for(a=0;a<graph->righe;a++)
		free(old[a]);
	free(old);
	graph->righe = i;
	graph->colonne = j;
	return !err;
}