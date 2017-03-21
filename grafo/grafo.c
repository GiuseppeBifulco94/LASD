#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "grafo.h"
#include "heap.h"
#include "coda.h"

void distruggi_grafo(struct grafo *graph, struct func funz) {
	void **mappa;
	int i, nnodi;
	if(graph && funz.getMappa) {
		nnodi = graph->nnodi;
		i = nnodi-1;
		mappa = funz.getMappa(graph);
		if(mappa) {
			while(i >= 0) {
				funz.removeVertex(graph, mappa[i], funz);
				i--;
			}
		}
		funz.deallocaAdj(graph, funz);
		if(mappa)
			free(mappa);
		mappa = NULL;
		free(graph);
		graph = NULL;
		return;
	} else if(graph) {
		free(graph);
	}
}
void dealloca_list_adj_complete(struct grafo *graph, struct func funz) {
	dealloca_list_adj(graph->adiacenze, funz);
	if(graph->adiacenze)
		free(graph->adiacenze);
	graph->adiacenze = NULL;
}
void dealloca_matrix_adj(struct grafo *graph, struct func funz) {
	int i;
	if((struct matrice_adj *) graph->adiacenze) {
		for(i=0;i<graph->nnodi;i++) {
			free(((struct matrice_adj *) graph->adiacenze)->matrice[i]);
			funz.libera(((struct matrice_adj *) graph->adiacenze)->mappa[i]);
		}
		free(((struct matrice_adj *) graph->adiacenze)->mappa);
		free(((struct matrice_adj *) graph->adiacenze)->matrice);
	}
	if(graph->adiacenze)
		free(graph->adiacenze);
	graph->adiacenze = NULL;
}
struct grafo *alloca_grafo(struct grafo *graph) {
	graph = malloc(sizeof(struct grafo));
	graph->nnodi = 0;
}

struct lista_adj *alloca_list_adj(struct lista_adj *l) {
	l = malloc(sizeof(struct lista_adj));
	l->adj = NULL;
	l->next = NULL;
	return l;
}
struct lista_pesata *alloca_list_pesata(struct lista_pesata *lp, void *el, float peso) {
	lp = malloc(sizeof(struct lista_pesata));
	lp->peso = peso;
	lp->nodo = el;
	lp->next = NULL;
	return lp;
}
int add_list_vertex(struct grafo *graph, void *el, struct  func funz) {
	struct lista_adj *testa;
	if(is_list_vertex(graph, el, funz) == 0) {
		testa = alloca_list_adj(testa);
		testa->adj = alloca_list_pesata((struct lista_pesata *)testa->adj, el, .0);
		testa->next = (struct lista_adj *) graph->adiacenze;
		graph->adiacenze = testa;
		graph->nnodi = graph->nnodi + 1;
		return 1;
	} else {
		printf("Il vertice esiste gia'.\n");
		return 0;
	}
}
void free_list_pesata(struct lista_pesata *lp, struct func funz) {
	if(!lp) {
		lp = NULL;
		return;
	}
	if(lp->nodo)
		funz.libera(lp->nodo);
	lp->nodo = NULL;
	free(lp);
	lp = NULL;
}
void free_list_adj(struct lista_adj *l, struct func funz) {
	if(!l) {
		l = NULL;
		return;
	}
	if(l->adj)
		free_list_pesata(l->adj, funz);
	l->adj = NULL;
	free(l);
	l = NULL;
}
void remove_list_vertex(struct grafo *graph, void *el, struct func funz) {
	struct lista_adj *big_l, *padre, *tmpb;
	struct lista_pesata *small_l, *tmp, *padre_s;
	big_l = (struct lista_adj *) graph->adiacenze;
	padre = NULL;
	if(big_l == NULL) {
		printf("Errore: la lista di adiacenze e' vuota.\n");
		return;
	}
	while(big_l != NULL) {
		small_l = big_l->adj;
		if(funz.cmp(small_l->nodo, el) == 0) {
			small_l = small_l->next;
			while(small_l != NULL) {
				tmp = small_l->next;
				free_list_pesata(small_l, funz);
				small_l = tmp;
			}
			tmpb = big_l;
			if(padre == NULL) {
				graph->adiacenze = big_l->next;
			} else {
				padre->next = big_l->next;
			}
			free_list_adj(tmpb, funz);
			graph->nnodi = graph->nnodi - 1;
			return;
		} else {
			//rimuovi il vertice da eliminare all'interno delle liste di adiacenza degli altri vertici
			small_l = small_l->next;
			padre_s = NULL;
			while(small_l != NULL) {
				if(funz.cmp(small_l->nodo, el) == 0) {
					tmp = small_l->next;
					free_list_pesata(small_l, funz);
					if(padre_s == NULL) {
						big_l->adj->next = tmp;
					} else {
						padre_s->next = tmp;
					}
					break;
				}
				padre_s = small_l;
				small_l = small_l->next;
			}
		}
		padre = big_l;
		big_l = big_l->next;
	}
	printf("Errore: il nodo da rimuovere non esiste.\n");
}
void init_list_adj(struct grafo *graph, int dim) {
//	graph->adiacenze = alloca_list_adj((struct lista_adj *) graph->adiacenze);
	graph->adiacenze = NULL;	
}
void init_list_grafo(struct grafo *graph, int dim, struct func funz) {
	int i, test;
	void *el;
	init_list_adj(graph, dim);
	i = 0;
	while(i<dim) {
		el = funz.getInput();
		test = add_list_vertex(graph, el, funz);
		if(test == 0) {
			funz.libera(el);
			i--;
		}
		i++;
	}
}

struct matrice_adj *alloca_matrix_adj(struct matrice_adj *m, int dim) {
	int i, j;
	m = malloc(sizeof(struct matrice_adj));
	if(dim < 1) {
		m->matrice = malloc(sizeof(float*));
		m->mappa = malloc(sizeof(void*));
		return m;
	}
	m->matrice = malloc(sizeof(float*) * dim);
	i = 0;
	while(i<dim) {
		j = 0;
		m->matrice[i] = malloc(sizeof(float) * dim);
		while(j<dim) {
			m->matrice[i][j] = .0;
			j++;
		}
		i++;
	}
	m->mappa = malloc(sizeof(void *) * dim);
	i = 0;
	while(i<dim) {
		m->mappa[i] = NULL;
		i++;
	}
	return m;
}

void init_matrix_adj(struct grafo *graph, int dim) {
	graph->adiacenze = alloca_matrix_adj((struct matrice_adj *) graph->adiacenze, dim);
}
void init_matrix_grafo(struct grafo *graph, int dim, struct func funz) {
	int i, test;
	void *el;
	init_matrix_adj(graph, dim);
	graph->nnodi = dim;
	i = 0;
	while(i<dim) {
		el = NULL;
		el = funz.getInput();
		if(i == 0)
			test = 0;
		else
			test = is_matrix_vertex(graph, el, funz);
		if(test == 0)
			((struct matrice_adj *)(graph->adiacenze))->mappa[i] = el;
		else {
			funz.libera(el);
			printf("Il vertice e' gia' presente.\n");
			i--;
		}
		i++;
	}	
}
void stampa_matrix_vertici(struct grafo *graph, struct func funz) {
	int i;
	i = 0;
	while(i<graph->nnodi) {
		printf("[");
		funz.print(((struct matrice_adj *)(graph->adiacenze))->mappa[i]);
		printf("] ");
		i++;
	}
	printf("\n");
}
void stampa_matrix_adj(struct grafo *graph, struct func funz) {
	int i, j;
	i = 0;
	printf("\t");
	while(i<graph->nnodi) {
		funz.print(((struct matrice_adj *)(graph->adiacenze))->mappa[i]);
		printf("\t");
		i++;
	}
	printf("\n");
	i = 0;
	while(i<graph->nnodi) {
		j = 0;
		funz.print(((struct matrice_adj *)(graph->adiacenze))->mappa[i]);
		printf("\t");	
		while(j<graph->nnodi) {
			printf("%.1f\t", ((struct matrice_adj *)(graph->adiacenze))->matrice[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
}

void stampa_list_vertici(struct grafo *graph, struct func funz) {
	int i;
	void **mappa;
	if(graph->nnodi == 0) {
		printf("Il grafo e' vuoto.\n");
		return;
	}
	i = 0;
	mappa = mappa_list_vertex((struct lista_adj *)graph->adiacenze, graph->nnodi);
	while(i<graph->nnodi) {
		printf("[");
		funz.print(mappa[i]);
		printf("] ");
		i++;
	}
	printf("\n");
}

void stampa_list_adj(struct grafo *graph, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	big_l = (struct lista_adj *) graph->adiacenze;
	if(big_l == NULL) {
		printf("La lista di adiacenze e' vuota.\n");
		return;
	}
	while(big_l != NULL) {
		small_l = big_l->adj;
		printf("[");
		funz.print(small_l->nodo);
		printf("] -> ");
		small_l = small_l->next;
		while(small_l != NULL) {
			printf("( ");
			funz.print(small_l->nodo);
			printf(" , peso ");
			printf("%.1f", small_l->peso);
			printf(" ) , ");
			small_l = small_l->next;
		}
		big_l = big_l->next;
		printf("\n");
	}
}

int is_list_vertex(struct grafo *graph, void *el, struct func funz) {
	struct lista_adj *big_l;
	big_l = (struct lista_adj *) graph->adiacenze;
	while(big_l != NULL) {
		if(funz.cmp(big_l->adj->nodo, el) == 0)
			return 1;
		big_l = big_l->next;
	}
	return 0;
}

int add_list_edge(struct grafo *graph, void *dadove, void *adove, float peso, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	struct lista_pesata *nuovatesta;
	if(is_list_vertex(graph, adove, funz) == 0) {
		printf("Errore: il vertice da raggiungere non esiste.\n");
		goto exit0;
	}
	nuovatesta = alloca_list_pesata(nuovatesta, adove, peso);
	big_l = (struct lista_adj *) graph->adiacenze;
	if(big_l == NULL) {
		printf("Errore: la lista di adiacenze e' vuota.\n");
		goto exit0;
	}
	while(big_l != NULL) {
		small_l = big_l->adj;
		if(funz.cmp(small_l->nodo, dadove) == 0) {
			small_l = small_l->next;
			while(small_l != NULL) {
				if(funz.cmp(small_l->nodo, adove) == 0) {
					printf("Errore: l'arco da aggiungere esiste gia'.\n");
					goto exit0;
				}
				small_l = small_l->next;
			}
			nuovatesta->next = big_l->adj->next;
			big_l->adj->next = nuovatesta;
			//solo da dove
			goto exit1;
		}
		big_l = big_l->next;
	}
	printf("Errore: il vertice da cui far partire l'arco non esiste.\n");
	goto exit0;

	exit0:
		if(dadove)
			funz.libera(dadove);
		if(adove)
			funz.libera(adove);
		dadove = NULL;
		adove = NULL;
		return 0;
	exit1:
		if(dadove)
			funz.libera(dadove);
		dadove = NULL;
		return 1;
}
void remove_list_edge(struct grafo *graph, void *dadove, void *adove, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l, *padre, *tmp;
	if(is_list_vertex(graph, adove, funz) == 0) {
		printf("Errore: il vertice da scollegare non esiste.\n");
		return;
	}
	big_l = (struct lista_adj *) graph->adiacenze;
	if(graph->nnodi < 1) {
		printf("Errore: il grafo e' vuoto.\n");
		return;
	}
	if(big_l == NULL) {
		printf("Errore: la lista di adiacenze e' vuota.\n");
		return;
	}
	while(big_l != NULL) {
		small_l = big_l->adj;
		if(funz.cmp(small_l->nodo, dadove) == 0) {
			small_l = small_l->next;
			padre = NULL;
			while(small_l != NULL) {
				if(funz.cmp(small_l->nodo, adove) == 0) {
					tmp = small_l->next;
					free_list_pesata(small_l, funz);
					if(padre == NULL) {
						big_l->adj->next = tmp;
					} else {
						padre->next = tmp;
					}
					return;
				}
				padre = small_l;
				small_l = small_l->next;
			}
			printf("Errore: l'arco non esiste.\n"); //se arrivo qui non ho cancellato nulla quindi l'arco non esiste
			return; 
		}
		big_l = big_l->next;
	}
	printf("Errore: il nodo a cui rimuovere l'arco non esiste.\n");
}

int add_matrix_vertex(struct grafo *graph, void *el, struct  func funz) {
	if(is_matrix_vertex(graph, el, funz) == 0) {
		graph->adiacenze = realloca_matrix_adj((struct matrice_adj *) graph->adiacenze, graph->nnodi);
		((struct matrice_adj *)(graph->adiacenze))->mappa[graph->nnodi] = el;
		graph->nnodi = graph->nnodi + 1;
		return 1;
	} else {
		printf("Il vertice esiste gia'.\n");
		return 0;
	}
}
struct matrice_adj *realloca_matrix_adj(struct matrice_adj *m, int dim) {
	int i;
	m->matrice = realloc(m->matrice, (dim+1) * sizeof(float*));
	m->mappa = realloc(m->mappa, (dim+1) * sizeof(void *));
	m->mappa[dim] = (void *) NULL;
	i = 0;
	while(i<dim) {
		m->matrice[i] = realloc(m->matrice[i], (dim+1) * sizeof(float));
		i++;
	}		
	m->matrice[i] = malloc((dim+1) * sizeof(float)); //non realloc ma malloc fratomo
	i = 0;
	while(i<=dim) {
		m->matrice[dim][i] = .0; //ultima riga
		if(i != dim) {
			m->matrice[i][dim] = .0; //ultima colonna
		}
		i++;
	}
	return m;
}

int is_matrix_vertex(struct grafo *graph, void *el, struct func funz) {
	int i;
	i = 0;
	while(i<graph->nnodi) {
		if(((struct matrice_adj *)(graph->adiacenze))->mappa[i] == NULL) {
			i++;
			continue;
		}
		if(funz.cmp(((struct matrice_adj *)(graph->adiacenze))->mappa[i], el) == 0)
			return 1;
		i++;
	}
	return 0;
}

int add_matrix_edge(struct grafo *graph, void *dadove, void *adove, float peso, struct func funz) {
	int from, to;
	if(is_matrix_vertex(graph, adove, funz) == 0) {
		printf("Errore: il vertice da raggiungere non esiste.\n");
		goto exit0;
	}
	if(is_matrix_vertex(graph, dadove, funz) == 0) {
		printf("Errore: il vertice da cui far partire l'arco non esiste.\n");
		goto exit0;
	}
	from = getnumfromvertex(((struct matrice_adj *)(graph->adiacenze))->mappa, dadove, graph->nnodi, funz);
	to = getnumfromvertex(((struct matrice_adj *)(graph->adiacenze))->mappa, adove, graph->nnodi, funz);
	if(from == -1 || to == -1) {
		printf("Errore: mappatura scorretta.\n");
		goto exit0;
	}
	if(graph->adiacenze != NULL) {
		if(((struct matrice_adj *)(graph->adiacenze))->matrice[from][to] != .0) {
			printf("L'arco esiste gia'.\n");
			goto exit0;
		}
		((struct matrice_adj *)(graph->adiacenze))->matrice[from][to] = peso;
		goto exit1;
	}
	exit0:
		if(dadove)
			funz.libera(dadove);
		if(adove)
			funz.libera(adove);
		dadove = NULL;
		adove = NULL;
		return 0;
	exit1:
		if(dadove)
			funz.libera(dadove);
		if(adove)
			funz.libera(adove);
		dadove = NULL;
		adove = NULL;
		return 1;
}

int getnumfromvertex(void **mappa, void *el, int dim, struct func funz) {
	int i;
	i = 0;
	while(i<dim) {
		if(funz.cmp(mappa[i], el) == 0)
			return i;
		i++;
	}
	return -1;
}
void remove_matrix_vertex(struct grafo *graph, void *el, struct func funz) {
	int num, i, j;
	void *tmp;
	float tmp2;
	if(graph->nnodi < 1) {
		printf("Errore: Il grafo e' vuoto.\n");
		return;
	}
	num = getnumfromvertex(((struct matrice_adj *)(graph->adiacenze))->mappa, el, graph->nnodi, funz);
	if(num == -1) {
		printf("Errore: il vertice da rimuovere non esiste.\n");
		return;
	}
	if(num != graph->nnodi-1) { //se non devo eliminare l'ultimo..
		tmp = ((struct matrice_adj *)(graph->adiacenze))->mappa[num];
		((struct matrice_adj *)(graph->adiacenze))->mappa[num] = ((struct matrice_adj *)(graph->adiacenze))->mappa[graph->nnodi-1];
		((struct matrice_adj *)(graph->adiacenze))->mappa[graph->nnodi-1] = tmp;
		i = 0; //scalo riga e colonna all'ultima riga e colonna
		while(i<graph->nnodi) {
			tmp2 = ((struct matrice_adj *)(graph->adiacenze))->matrice[num][i];
			((struct matrice_adj *)(graph->adiacenze))->matrice[num][i] = ((struct matrice_adj *)(graph->adiacenze))->matrice[graph->nnodi-1][i];
			((struct matrice_adj *)(graph->adiacenze))->matrice[graph->nnodi-1][i] = tmp2;
			i++;
		}
		i = 0;
		while(i<graph->nnodi) {
			tmp2 = ((struct matrice_adj *)(graph->adiacenze))->matrice[i][num];
			((struct matrice_adj *)(graph->adiacenze))->matrice[i][num] = ((struct matrice_adj *)(graph->adiacenze))->matrice[i][graph->nnodi-1];
			((struct matrice_adj *)(graph->adiacenze))->matrice[i][graph->nnodi-1] = tmp2;
			i++;
		}
	}
	tmp = ((struct matrice_adj *)(graph->adiacenze))->mappa[graph->nnodi-1];
	funz.libera(tmp); //libera el da mappa
	((struct matrice_adj *)(graph->adiacenze))->mappa[graph->nnodi-1] = NULL;
	((struct matrice_adj *)(graph->adiacenze))->mappa = realloc(((struct matrice_adj *)(graph->adiacenze))->mappa, (sizeof(void *)*(graph->nnodi-1)));
	i = 0;
	free(((struct matrice_adj *)(graph->adiacenze))->matrice[graph->nnodi-1]);
	graph->nnodi = graph->nnodi-1;
}

void remove_matrix_edge(struct grafo *graph, void *dadove, void *adove, struct func funz) {
	int da, a, i, j;
	if(graph->nnodi < 1) {
		printf("Errore: Il grafo e' vuoto.\n");
		return;
	}
	da = getnumfromvertex(((struct matrice_adj *)(graph->adiacenze))->mappa, dadove, graph->nnodi, funz);
	if(da == -1) {
		printf("Errore: il vertice a cui rimuovere l'arco non esiste.\n");
		return;
	}
	a = getnumfromvertex(((struct matrice_adj *)(graph->adiacenze))->mappa, adove, graph->nnodi, funz);
	if(a == -1) {
		printf("Errore: il vertice da scollegare non esiste.\n");
		return;
	}
	if(((struct matrice_adj *)(graph->adiacenze))->matrice[da][a] == .0) {
		printf("Errore: l'arco non esiste.\n");
		return;
	}
	((struct matrice_adj *)(graph->adiacenze))->matrice[da][a] = .0;
}
void trasposta_matrix(struct grafo *graph, struct func funz) {
	float tmp;
	int i, j;
	if(graph->nnodi < 1) {
		printf("Errore: Il grafo e' vuoto.\n");
		return;
	}
	i = 0;
	while(i<graph->nnodi) {
		j = i;
		while(j<graph->nnodi) {
			tmp = ((struct matrice_adj *)graph->adiacenze)->matrice[i][j];
			((struct matrice_adj *)graph->adiacenze)->matrice[i][j] = ((struct matrice_adj *)graph->adiacenze)->matrice[j][i];
			((struct matrice_adj *)graph->adiacenze)->matrice[j][i] = tmp;
			j++;
		}
		i++;
	}
}

void trasposta_list(struct grafo *graph, struct func funz) {
	struct lista_adj *old_l, *big_l, *lista_trasposta, *lt;
	struct lista_pesata *small_l, *ltp;
	void *nodo;
	if(graph->nnodi < 1) {
		printf("Errore: Il grafo e' vuoto.\n");
		return;
	}
	old_l = (struct lista_adj *) graph->adiacenze;
	big_l = old_l;
	graph->adiacenze = NULL;
	graph->adiacenze = alloca_list_adj((struct lista_adj *) graph->adiacenze);
	graph->adiacenze = NULL;
	graph->nnodi = 0;
	while(big_l != NULL) {
		add_list_vertex(graph, big_l->adj->nodo, funz);
		big_l = big_l->next;
	}
	big_l = old_l;
	while(big_l != NULL) {
		small_l = big_l->adj;
		nodo = small_l->nodo;
		small_l = small_l->next;
		while(small_l != NULL) {
			add_list_edge(graph, small_l->nodo, nodo, small_l->peso, funz);
			small_l = small_l->next;
		}
		big_l = big_l->next;
	}
	//deallocazione vecchia lista
	big_l = old_l;
	dealloca_list_adj(old_l, funz);
}
struct lista_pesata *reverse_list(struct lista_pesata *lp) {
	struct lista_pesata *prec, *tmp;
	prec = NULL;
    while(lp != NULL) {
		tmp = lp;
		lp = lp->next;
		tmp->next = prec;
		prec = tmp;
    }
    return prec;
}

void converti_list_to_matrix(struct grafo *graph, struct func funz) {
	int i;
	struct matrice_adj *nuova_adj;
	struct lista_adj *big_l, *old_l;
	struct lista_pesata *small_l;
	void *nodo;
	nuova_adj = alloca_matrix_adj(nuova_adj, graph->nnodi);
	i = 0;
	big_l = (struct lista_adj *) graph->adiacenze;
	while(big_l != NULL) {
		nuova_adj->mappa[i] = big_l->adj->nodo;
		big_l = big_l->next;
		i++;
	}
	big_l = (struct lista_adj *) graph->adiacenze;
	while(big_l != NULL) {
		small_l = big_l->adj;
		nodo = small_l->nodo;
		small_l = small_l->next;
		while(small_l != NULL) {
			nuova_adj->matrice[getnumfromvertex(nuova_adj->mappa, nodo, graph->nnodi, funz)][getnumfromvertex(nuova_adj->mappa, small_l->nodo, graph->nnodi, funz)] = small_l->peso;
			small_l = small_l->next;
		}
		big_l = big_l->next;
	}
	old_l = (struct lista_adj *) graph->adiacenze;
	dealloca_list_adj(old_l, funz);
	graph->adiacenze = (void *) nuova_adj;	
}

void dealloca_list_adj(struct lista_adj *old_l, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	void *nodo;
	big_l = old_l;
	while(big_l != NULL) {
		small_l = big_l->adj;
		while(small_l != NULL) {
			nodo = small_l;
			small_l = small_l->next;
			free(nodo);
//			free_list_pesata(((struct lista_pesata *)nodo), funz);
		}
		nodo = big_l;
		big_l = big_l->next;
		((struct lista_adj *)nodo)->adj = NULL;
		free_list_adj(((struct lista_adj *)nodo), funz);
	}
}
//DA AGGIUSTARE
void converti_matrix_to_list(struct grafo *graph, struct func funz) {
	void *nodo;
	int i, j, dim;
	struct matrice_adj *old_m;
	void **mappa;
	old_m = (struct matrice_adj *) graph->adiacenze;
	graph->adiacenze = NULL;
	if(!old_m)
		return;
	i = 0;
	dim = graph->nnodi;
	mappa = malloc(sizeof(void*)*dim);
	graph->nnodi = 0;
	while(i<dim) {
		nodo = funz.allocaval(old_m->mappa[i]);
		mappa[i] = nodo;
		add_list_vertex(graph, mappa[i], funz);
		i++;
	}
	i = 0;
	while(i<graph->nnodi) {
		j = 0;
		while(j<graph->nnodi) {
			if(old_m->matrice[i][j] != .0 ) //l'arco esiste
				add_list_edge(graph, funz.allocaval(mappa[i]), funz.allocaval(mappa[j]), old_m->matrice[i][j], funz);
			j++;
		}
		i++;
	}
	for(i=0;i<dim;i++) {
		free(old_m->matrice[i]);
		funz.libera(old_m->mappa[i]);
	}
	free(old_m->matrice);
	free(old_m->mappa);
	free(mappa);
	free(old_m);
//	dealloca_matrix_adj_generic(old_m, graph->nnodi, funz);
}

void dealloca_matrix_adj_generic(struct matrice_adj *m, int dim, struct func funz) {
	int i;
	if(m) {
		for(i=0;i<dim-1;i++) {
			free(m->matrice[i]);
		}
		free(m->mappa);
		free(m->matrice);
		free(m);
	}
	m = NULL;
}


void **mappa_list_vertex(struct lista_adj *l, int dim) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	void **ret;
	int i;
	big_l = (struct lista_adj *) l;
	if(dim == 0) {
		return (void **) NULL;
	}
	ret = malloc(sizeof(void *) * dim);
	i = 0;
	while(big_l != NULL) {
//		ret[i] = malloc(sizeof(void *));
		ret[i] = big_l->adj->nodo;
		big_l = big_l->next;
		i++;
	}
	return ret;
}

void **mappa_list_edge(void *l, void *el, int *numadj, int dim, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	void **ret;
	void *primo_adj;
	int i;
	big_l = (struct lista_adj *) l;
	*numadj = 0;
	if(big_l == NULL) {
		return (void **) NULL;
	}
	while(big_l != NULL) {
		small_l = big_l->adj;
		if(funz.cmp(el,small_l->nodo) == 0) {
			i = 0;
			small_l = small_l->next;
			primo_adj = small_l;
			while(small_l != NULL) {
				i++;
				small_l = small_l->next;
			}
			if(i == 0) { //lista_adj vuota -> così evito n realloc!
				return (void **) NULL;
			} else {
				ret = malloc(sizeof(void *) * i);
				*numadj = i;
				small_l = primo_adj;
				i = 0;
				while(small_l != NULL) {
					ret[i] = small_l->nodo;
					small_l = small_l->next;
					i++;
				}
				return ret;
			}
		}
		big_l = big_l->next;
	}
}

void **mappa_matrix_edge(void *m, void *el, int *numadj, int dim, struct func funz) {
	void **ret;
	int i, j, r;
	*numadj = 0;
	if(dim == 0) {
		return (void **) NULL;
	}
	r = 0;
	i = 0;
	j = getnumfromvertex(((struct matrice_adj*)(m))->mappa, el, dim, funz);
	if(j == -1) {
		return (void **) NULL;
	}
	while(i<dim) {
		if(((struct matrice_adj*)(m))->matrice[j][i] != .0)
			r++;
		i++;
	}
	if(r == 0) {
		return (void **) NULL;
	}
	*numadj = r;
	ret = malloc(sizeof(void *) * r);
	i = 0;
	r = 0;
	while(i<dim) {
		if(((struct matrice_adj*)(m))->matrice[j][i] != .0) {
			ret[r] = ((struct matrice_adj*)(m))->mappa[i];
			r++;
		}
		i++;
	}
	return ret;
}

//DOMANDA BFS PER GRAFO PESATO?
void BFS(struct grafo *graph, void *s, struct func funz, void **dist, int *col, void **pred) { //non riesco ad allocare dist pred col qua dentro
	struct coda *Q;
	void **mappa, **adj_u;
	void *u;
	int i, numadj, vnum, unum;
	float *tmp;
	mappa = funz.getMappa(graph);
	for(i=0;i<graph->nnodi;i++) {
		dist[i] = NULL; //infinito ?
		pred[i] = NULL;
		col[i] = 0; //0->bianco , 1->grigio , 2->nero
	}
	i = getnumfromvertex(mappa, s, graph->nnodi, funz);
	if(i==-1) {
		printf("Errore: il vertice sorgente [");
		funz.print(s);
		printf("] non appartiene al grafo.\n");
		return;
	}
	col[i] = 1;
	pred[i] = NULL;
	tmp = malloc(sizeof(float));
	*tmp = .0;
	dist[i] = tmp;
	Q = NULL;
	Q = accoda_coda(Q, s);
	while(Q != NULL) {
		u = testa_coda(Q);
		//visita u
		unum = getnumfromvertex(mappa, u, graph->nnodi, funz);
		adj_u = funz.getAdj(graph->adiacenze, u, &numadj, graph->nnodi, funz);
		for(i=0;i<numadj;i++) {
			vnum = getnumfromvertex(mappa, adj_u[i], graph->nnodi, funz);
			if(col[vnum] == 0) {
				col[vnum] = 1;
				tmp = malloc(sizeof(float));
				*tmp = (*(float*)dist[unum]) + funz.getPeso(graph, unum, vnum, funz);
				dist[vnum] = tmp;
				pred[vnum] = u;
				Q = accoda_coda(Q, mappa[vnum]);
			}
		}
		if(adj_u)
			free(adj_u);
		col[unum] = 2;
		Q = decoda_coda(Q);
	}
	if(mappa)
		free(mappa);
	mappa = NULL;
}
void stampa_percorso(struct grafo *graph, void *s, void *v, struct func funz, void **pred) {
	if(funz.cmp(v,s) == 0) {
		printf(" [");
		funz.print(s);
		printf("] ");
	} else {
		void **mappa;
		mappa = funz.getMappa(graph);
		if(pred[getnumfromvertex(mappa, v, graph->nnodi, funz)] == NULL) {
			printf("Non esiste percorso tra [");
			funz.print(s);
			printf("] e [");
			funz.print(v);
			printf("]\n");
		} else {
			stampa_percorso(graph, s, pred[getnumfromvertex(mappa, v, graph->nnodi, funz)], funz, pred);
			printf(" [");
			funz.print(v);
			printf("] ");
		}
		if(mappa)
			free(mappa);
	}
}
float getpeso_matrix(struct grafo *graph, int a, int b, struct func funz) {
	return ((struct matrice_adj *)(((struct grafo *)graph)->adiacenze))->matrice[a][b];
}
float getpeso_list(struct grafo *graph, int a, int b, struct func funz) {
	struct lista_adj *big_l;
	struct lista_pesata *small_l;
	void *nodo, *adj, **mappa;
	float peso = .0;
	big_l = (struct lista_adj *) ((struct grafo *)graph)->adiacenze;
	mappa = mappa_list_vertex((struct lista_adj *)((struct grafo *)graph)->adiacenze, ((struct grafo *)graph)->nnodi);
	nodo = mappa[a];
	adj = mappa[b];
	while(big_l != NULL) {
		small_l = big_l->adj;
		if(funz.cmp(nodo,small_l->nodo) == 0) {
			small_l = small_l->next;
			while(small_l != NULL) {
				if(funz.cmp(adj,small_l->nodo) == 0) {
					peso = small_l->peso;
					free(mappa);
					return peso;
				}
				small_l = small_l->next;
			}
		}
		big_l = big_l->next;
	}
	free(mappa);
	return .0;
}
void **getmappa_list(struct grafo *graph) {
	return mappa_list_vertex((struct lista_adj *)((struct grafo *)graph)->adiacenze, ((struct grafo *)graph)->nnodi);
}
void **getmappa_matrix(struct grafo *graph) {
	void **ret;
	int i;
	ret = malloc(sizeof(void *) * graph->nnodi);
	for(i=0;i<graph->nnodi;i++) {
		ret[i] = (((struct matrice_adj *)(((struct grafo *)graph)->adiacenze))->mappa)[i];
	}
	return ret;
}
void DFS(struct grafo *graph, struct func funz, int *col, void **pred) {
	void **mappa;
	int i;
	mappa = funz.getMappa(graph);
	for(i=0;i<graph->nnodi;i++) {
		pred[i] = NULL;
		col[i] = 0; //0->bianco , 1->grigio , 2->nero
	}
	i = 0;
	while(i<graph->nnodi) {
		if(col[i] == 0) {
			DFS_visit(graph, mappa, i, funz, col, pred);
		}
		i++;
	}
	if(mappa)
		free(mappa);
}
void DFS_visit(struct grafo *graph, void **mappa, int i, struct func funz, int *col, void **pred) {
	void **adj;
	void *u;
	int numadj, x, vnum;
	x = 0;
	numadj = 0;
	u = mappa[i];
	adj = funz.getAdj(graph->adiacenze, u, &numadj, graph->nnodi, funz);
	col[i] = 1;
//	printf("[");
//	funz.print(u);
//	printf("] ");
	while(x<numadj) {
		vnum = getnumfromvertex(mappa, adj[x], graph->nnodi, funz);
		if(col[vnum] == 0) {
			pred[vnum] = u;
			DFS_visit(graph, mappa, vnum, funz, col, pred);
		}
		x++;
	}
	if(adj)
		free(adj);
	col[i] = 2;
}

int check_aciclico(struct grafo *graph, struct func funz) {
	int *col;
	void **mappa;
	int i, ret;
	mappa = funz.getMappa(graph);
	col = malloc(sizeof(int) * graph->nnodi);
	for(i=0;i<graph->nnodi;i++) {
		col[i] = 0; //0->bianco , 1->grigio , 2->nero
	}
	i = 0;
	ret = 1;
	while(i<graph->nnodi) {
		if(col[i] == 0) {
			ret = check_aciclicomain(graph, mappa, i, funz, col);
			if(!ret)
				break;
		}
		i++;
	}
	free(col);
	if(mappa)
		free(mappa);
	return ret;
}

int check_aciclicomain(struct grafo *graph, void **mappa, int i, struct func funz, int *col) {
	void **adj;
	void *u;
	int numadj, x, vnum;
	x = 0;
	numadj = 0;
	u = mappa[i];
	adj = funz.getAdj(graph->adiacenze, u, &numadj, graph->nnodi, funz);
	col[i] = 1;
	while(x<numadj) {
		vnum = getnumfromvertex(mappa, adj[x], graph->nnodi, funz);
		if(col[vnum] == 0) {
			if(adj)
				free(adj);
			return check_aciclicomain(graph, mappa, vnum, funz, col);
		} else if(col[vnum] == 1) {
			if(adj)
				free(adj);
			return 0;
		}
		x++;
	}
	if(adj)
		free(adj);
	col[i] = 2;
	return 1;
}


int Bellman_Ford(struct grafo *graph, void *s, struct func funz, void **dist, void **pred) {
	void **mappa;
	int i, j, k, continua;
	float *tmp, peso;
	mappa = funz.getMappa(graph);
	for(i=0;i<graph->nnodi;i++) {
		dist[i] = NULL;
		pred[i] = NULL;
	}
	i = getnumfromvertex(mappa, s, graph->nnodi, funz);
	if(i==-1) {
		printf("Errore: il vertice sorgente [");
		funz.print(s);
		printf("] non appartiene al grafo.\n");
		goto exit00;
	}
	tmp = malloc(sizeof(float));
	*tmp = .0;
	dist[i] = tmp;

	for(i=0;i<(graph->nnodi-1);i++) {
		for(j=0;j<graph->nnodi;j++) {
			for(k=0;k<graph->nnodi;k++) {
				if((peso=funz.getPeso(graph, j, k, funz)) != .0) { //arco j->k
					continua = 0;
					if(dist[k] == NULL) {
						continua = 1;
					} else {
						if(dist[j] == NULL) {
							continua = 1;
						} else if((*(float*)dist[k]) > ((*(float*)dist[j]) + peso)) {
							continua = 1;
						}
					}
					if(continua == 1) {
						if(dist[k])
							free(dist[k]);
						tmp = malloc(sizeof(float));
						if(dist[j])
							*tmp = (*(float*)dist[j]);
						else
							*tmp = .0;
						*tmp += peso;
						dist[k] = tmp;
						pred[k] = mappa[j];
					} 
				}
			}
		}
	}

	for(j=0;j<graph->nnodi;j++) {
		for(k=0;k<graph->nnodi;k++) {
			if((peso=funz.getPeso(graph, j, k, funz)) != .0) { //arco j->k
				if(dist[k] == NULL || dist[j] == NULL)
					continue;
				if((*(float*)dist[k]) > ((*(float*)dist[j]) + peso)) {
					printf("Errore: il grafo contiene cicli negativi.\n");
					goto exit0;
				}
			}
		}
	}
	goto exit1;

	exit0:
		if(mappa)
			free(mappa);
		mappa = NULL;
		return 0;
	exit00:
		if(mappa)
			free(mappa);
		mappa = NULL;
		return -1;
	exit1:
		if(mappa)
			free(mappa);
		mappa = NULL;
		return 1;
}

void Dijkstra(struct grafo *graph, void *s, struct func funz, void **dist, void **pred) {
	void **mappa;
	int r, i, unum, vnum, continua;
	float *tmp, peso;
	struct heap *Q;
	void *u, **adj_u;
	Q = NULL;
	Q = alloca_heap(Q);
	setHeap(Q, funz.tipo_el, 2, 1);
	Q = init_heap(Q, 0);
	mappa = funz.getMappa(graph);
	i = getnumfromvertex(mappa, s, graph->nnodi, funz);
	for(r=0;r<graph->nnodi;r++) {
		dist[r] = NULL;
		pred[r] = NULL;
		if(r == i)
			InsertKey(Q, mappa[r], .0);
		else
			InsertKey(Q, mappa[r], 99999.9);
	}
	if(i==-1) {
		printf("Errore: il vertice sorgente [");
		funz.print(s);
		printf("] non appartiene al grafo.\n");
		goto exit;
	}
	tmp = malloc(sizeof(float));
	*tmp = .0;
	dist[i] = tmp;
	while(Q->heapsize > 0) {
		u = extractHeap(Q);
		unum = getnumfromvertex(mappa, (((struct coppia*)u)->content), graph->nnodi, funz);
		if(((struct coppia*)u)->priority == 99999.9) {
			free(u);
			goto exit;
		}
		adj_u = funz.getAdj(graph->adiacenze, ((struct coppia *)u)->content, &r, graph->nnodi, funz);
		for(i=0;i<r;i++) {
			vnum = getnumfromvertex(mappa, adj_u[i], graph->nnodi, funz);
			peso=funz.getPeso(graph, unum, vnum, funz); //arco u->v
			continua = 0;
			if(dist[unum]) {
				if(dist[vnum] == NULL) {
					continua = 1;
				} else if((*(float*)dist[vnum]) > ((*(float*)dist[unum]) + peso)) {
					continua = 1;
				}
			}
			if(continua == 1) {
				if(dist[vnum])
					free(dist[vnum]);
				tmp = malloc(sizeof(float));
				*tmp = peso;
				if(dist[unum])
					*tmp += (*(float*)dist[unum]);
				dist[vnum] = tmp;
				pred[vnum] = mappa[unum];
				vnum = heapgetnumfromel(Q, mappa[vnum]);
				DecreaseKey(Q, heapgetnumfromel(Q, mappa[vnum]), *tmp);
			}
		}
		if(adj_u)
			free(adj_u);
		if(u)
			free(u);
	}

	exit:
		if(Q)
			distruggi_heap(Q);
		if(mappa)
			free(mappa);
}