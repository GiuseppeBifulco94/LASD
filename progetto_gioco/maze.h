#ifndef INFINITO
#define INFINITO 999
#endif
//((1.0)/(0.0))

struct labyrinth {
	char **maze;
	int righe;
	int colonne;
	int (*getpeso_maze)(struct labyrinth *, int, int);
};

struct labyrinth *alloca_labyrinth(struct labyrinth *graph);
void alloca_maze(struct labyrinth *graph, int righe, int colonne);
void init_maze_labyrinth(struct labyrinth *graph, int righe, int colonne, char defaultchar);
void stampa_maze(struct labyrinth *graph);
int add_maze_vertex(struct labyrinth *graph, int x, int y, char el);
int is_maze_vertex(struct labyrinth *graph, int x, int y, char el);
int add_maze_edge(struct labyrinth *graph, int x, int y, int dir, char el);
char *get_maze_adj(struct labyrinth *graph, int x, int y);
int getpeso_maze(struct labyrinth *graph, int x, int y);

void BFS(struct labyrinth *graph, int *s, int **dist, int **col, int ***pred);
void stampa_percorso(struct labyrinth *graph, int *s, int *v, int ***pred);
int fix_circular(int i, int max);


void DFS(struct labyrinth *graph, int **col, int ***pred);
void DFS_visit(struct labyrinth *graph, int x, int y, int **col, int ***pred);

void distruggi_labyrinth(struct labyrinth *graph);

int check_aciclico(struct labyrinth *graph); /* MA A CHE SERVE? IL GRAFO NON E' ORIENTATO QUINDI NON HA SENSO... */
int check_aciclicomain(struct labyrinth *graph, int x, int y, int **col);

void Dijkstra(struct labyrinth *graph, int x, int y, int **dist, int ***pred);
int Bellman_Ford(struct labyrinth *graph, int x, int y, int **dist, int ***pred);
int stima(int s_x, int s_y, int d_x, int d_y);
void Astar(struct labyrinth *graph, int s_x, int s_y, int d_x, int d_y, int **dist, int ***pred);

int cmparray_ofint(void *a, void *b); //per dijkstra
void freearray_ofint(void *a);

void printx(void *a);

int load_maze_from_file(struct labyrinth *graph, char *file, char *allowed, int dim, int min_righe, int min_colonne, int massimo_righe, int massimo_colonne);