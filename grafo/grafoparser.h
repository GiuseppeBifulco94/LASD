#include "func.h"

void read_grafo(void *graph, char *percorso, int impl_grafo, struct func *funz, int *error);
void write_grafo(void *graph, char *percorso, struct func funz);

int crea_il_grafo(void *graph, FILE *file, int *tipo_el, int *nnodi);
char *leggi_compreso(FILE *file, int da, char a, int *length, int *off);
int match(FILE *file, char s, int *off);
int match_reverse(FILE *file, char s);
int num_da_parentesi(FILE *file, int *ok);
void costruisci_vertici(void *graph, FILE *file, struct func funz, int *err);
void costruisci_archi(void *graph, FILE *file, struct func funz, int *err);
