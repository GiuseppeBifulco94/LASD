#include "func.h"

struct grafo {
	unsigned int nnodi;
	void *adiacenze;
};

struct lista_adj {
	struct lista_pesata *adj;
	struct lista_adj *next;
};

struct lista_pesata {
	void *nodo;
	float peso;
	struct lista_pesata *next;
};

struct matrice_adj {
	float **matrice;
	void **mappa;
};

struct grafo *alloca_grafo(struct grafo *graph);
void BFS(struct grafo *graph, void *s, struct func funz, void **dist, int *col, void **pred);
void stampa_percorso(struct grafo *graph, void *s, void *v, struct func funz, void **pred);
int getnumfromvertex(void **mappa, void *el, int dim, struct func funz);
void DFS(struct grafo *graph, struct func funz, int *col, void **pred);
void DFS_visit(struct grafo *graph, void **mappa, int i, struct func funz, int *col, void **pred);
void distruggi_grafo(struct grafo *graph, struct func funz);

void Dijkstra(struct grafo *graph, void *s, struct func funz, void **dist, void **pred);
int Bellman_Ford(struct grafo *graph, void *s, struct func funz, void **dist, void **pred);

void init_list_adj(struct grafo *graph, int dim);
struct lista_adj *alloca_list_adj(struct lista_adj *l);
struct lista_pesata *alloca_list_pesata(struct lista_pesata *lp, void *el, float peso);
struct lista_pesata *reverse_list(struct lista_pesata *lp);
void init_list_grafo(struct grafo *graph, int dim, struct func funz);
int add_list_vertex(struct grafo *graph, void *el, struct func funz);
void remove_list_vertex(struct grafo *graph, void *el, struct func funz);
int add_list_edge(struct grafo *graph, void *dadove, void *adove, float peso, struct func funz);
void remove_list_edge(struct grafo *graph, void *dadove, void *adove, struct func funz);
void stampa_list_vertici(struct grafo *graph, struct func funz);
void stampa_list_adj(struct grafo *graph, struct func funz);
int is_list_vertex(struct grafo *graph, void *el, struct func funz);
void free_list_pesata(struct lista_pesata *lp, struct func funz);
void free_list_adj(struct lista_adj *l, struct func funz);
void trasposta_list(struct grafo *graph, struct func funz);
void converti_list_to_matrix(struct grafo *graph, struct func funz);
void dealloca_list_adj(struct lista_adj *old_l, struct func funz);
void **mappa_list_vertex(struct lista_adj *l, int dim);
void **mappa_list_edge(void *l, void *el, int *numadj, int dim, struct func funz);
void **getmappa_list(struct grafo *graph);
float getpeso_list(struct grafo *graph, int a, int b, struct func funz);
void dealloca_list_adj_complete(struct grafo *graph, struct func funz);

void init_matrix_adj(struct grafo *graph, int dim);
struct matrice_adj *alloca_matrix_adj(struct matrice_adj *m, int dim);
struct matrice_adj *realloca_matrix_adj(struct matrice_adj *m, int dim);
void init_matrix_grafo(struct grafo *graph, int dim, struct func funz);
void stampa_matrix_adj(struct grafo *graph, struct func funz);
void stampa_matrix_vertici(struct grafo *graph, struct func funz);
int add_matrix_vertex(struct grafo *graph, void *el, struct func funz);
int is_matrix_vertex(struct grafo *graph, void *el, struct func funz);
int add_matrix_edge(struct grafo *graph, void *dadove, void *adove, float peso, struct func funz);
void remove_matrix_vertex(struct grafo *graph, void *el, struct func funz);
void remove_matrix_edge(struct grafo *graph, void *dadove, void *adove, struct func funz);
void trasposta_matrix(struct grafo *graph, struct func funz);
void converti_matrix_to_list(struct grafo *graph, struct func funz);
void **mappa_matrix_edge(void *m, void *el, int *numadj, int dim, struct func funz);
void **getmappa_matrix(struct grafo *graph);
float getpeso_matrix(struct grafo *graph, int a, int b, struct func funz);
void dealloca_matrix_adj(struct grafo *graph, struct func funz);
void dealloca_matrix_adj_generic(struct matrice_adj *m, int dim, struct func funz);


int check_aciclico(struct grafo *graph, struct func funz);
int check_aciclicomain(struct grafo *graph, void **mappa, int i, struct func funz, int *col);
