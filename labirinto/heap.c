#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "heap.h"
#include "utility.h"
#include "tree.h"

void distruggi_heap(struct heap *ogg) {
	struct coppia *tmp;
	if(!ogg)
		return;
	while(ogg->heapsize > 0) {
		tmp = extractHeap(ogg);
		ogg->libera(tmp);
	}
	if(ogg->content)
		free(ogg->content);
	ogg->content = NULL;
	if(ogg)
		free(ogg);
}

int heapgetnumfromel(struct heap *ogg, void *el) {
	int i;
	struct coppia *tmp;
	i = 0;
	while(i<ogg->heapsize) {
		tmp = ogg->get_el(ogg, i);
		if(tmp) {
			if(ogg->cmp(tmp->content,el) == 0)
			return i;
		}
		i++;
	}
	return -1;
}

struct heap *alloca_heap(struct heap *ogg) {
	ogg = malloc(sizeof(struct heap));
	return ogg;
}
void InsertKey(struct heap *ogg, void *el, float priorita) {
	struct coppia *elem;
	ogg->heapsize = ogg->heapsize+1;
	elem = malloc(sizeof(struct coppia));
	elem->content = el;
	elem->priority = priorita;
	if(ogg->arrayortree == 1) {
		realloca_heap_array(ogg); //controlla se devo reallocare o meno (solo nel caso dell'array ovviamente..)
		((struct coppia **)(ogg->content))[ogg->heapsize-1] = elem;
	} else {
		ogg->content = instree(((struct tree *)ogg->content), ogg->heapsize-1, elem);
	}
//	IncreaseKey(ogg, ogg->heapsize-1, priorita);
	int indice = ogg->heapsize-1;
	while(indice > 0 && ogg->checkminormax(priorita,ogg->get_el(ogg, (int)((indice+1)/2)-1)->priority) == 1) {
		swap(ogg->get_el(ogg, indice), ogg->get_el(ogg, (int)((indice+1)/2)-1));
		indice = (int)((indice+1)/2)-1;
	}
}
void DeleteHeap(struct heap *ogg, int indice) {
	if(ogg->heapsize < 1) {
//		////printf("Heap underflow.\n");
	} else if(indice >= ogg->heapsize || indice < 0) {
//		////printf("Indice non presente.\n");
	} else {
		int padre;
		struct coppia *el, *padre_el, *last;
		el = ogg->get_el(ogg, indice);
//		////printf("Hai eliminato: ");
		////printelcoda(el, ogg);
		last = ogg->get_el(ogg, ogg->heapsize-1);
		swap(ogg->get_el(ogg, indice), ogg->get_el(ogg, ogg->heapsize-1));
		ogg->heapsize = ogg->heapsize-1;
		padre = (int)((indice+1)/2)-1;
		padre_el = ogg->get_el(ogg, padre);
		if(ogg->arrayortree == 2) {
			struct tree *padre_nodo;
			padre_nodo = get_nodo(ogg, (int)((ogg->heapsize+1)/2)-1);
			if(padre_nodo != NULL) {
				if(padre_nodo->dx != NULL) {
					if(padre_nodo->dx->val == last) {
						padre_nodo->dx = NULL;
					}
				}
				if(padre_nodo->sx != NULL) {
					if(padre_nodo->sx->val == last) {
						padre_nodo->sx = NULL;
					}
				}
			}
		}
		el = NULL;
		if(indice < ( ogg->heapsize / 2)) { //non era una foglia
			if(indice == 0 || (padre_el != NULL && padre_el->priority < el->priority)) { //testa oppure nessuna violazione verso l'alto
				Heapify(ogg, indice);
			} else {
				while(indice > 0 && ogg->get_el(ogg, (int)((indice+1)/2)-1)->priority < ogg->get_el(ogg, indice)->priority) {
					swap(ogg->get_el(ogg, indice), ogg->get_el(ogg, (int)((indice+1)/2)-1));
					indice = (int)((indice+1)/2)-1;
				}
			}
		}
	el = last;
//	////printf("Hai eliminato: ");
//	//printelcoda(el, ogg);
	ogg->libera(el);
	}

}
void IncreaseKey(struct heap *ogg, int indice, float priorita) {
	struct  coppia *in;
	int i;
	in = ogg->get_el(ogg, indice);
	if(in == NULL) {
		////printf("Indice non presente.\n");
	} else if(priorita < in->priority) {
		////printf("Nuova priorita' piu' piccola.\n");
	} else {
		in->priority = priorita;
		//era minore
		if(ogg->minormax == 2) { //MAXHEAP
			while(indice > 0 && ogg->get_el(ogg, (int)((indice+1)/2)-1)->priority < ogg->get_el(ogg, indice)->priority) {
				swap(ogg->get_el(ogg, indice), ogg->get_el(ogg, (int)((indice+1)/2)-1));
				indice = (int)((indice+1)/2)-1;
			}
		} else { //MINHEAP Devi andare giù
			Heapify(ogg, indice);
		}
	}
}
void DecreaseKey(struct heap *ogg, int indice, float priorita) {
	//NEL MINHEAP NON FUNZIONE NOTA BENE
	struct coppia *in;
	in = ogg->get_el(ogg, indice);
	if(in == NULL) {
		//printf("Cercavi %d\n", indice);
		//printf("Indice non presente.\n");
	} else if(priorita > in->priority) {
		//printf("Nuova priorita' piu' grande.\n");
	} else {
		//printf("Diminuisco la priorita' di ");
		//ogg->print(in->content);
		//printf(" da %f a %f\n", in->priority, priorita);
		in->priority = priorita;
		if(ogg->minormax == 2) { //Scendi
			Heapify(ogg, indice);	
		} else { //Sali
			while(indice > 0 && ogg->get_el(ogg, (int)((indice+1)/2)-1)->priority > ogg->get_el(ogg, indice)->priority) {
				swap(ogg->get_el(ogg, indice), ogg->get_el(ogg, (int)((indice+1)/2)-1));
				indice = (int)((indice+1)/2)-1;
			}
		}
		////printf("Operazione terminata con successo!\n");
	}
}
struct coppia *gettop(struct heap *ogg) {
	if(ogg->heapsize < 1) {
		////printf("Heap underflow\n");
		return (struct coppia *) NULL;
	} else {
		return ogg->get_el(ogg, 0);
	}
}
struct coppia *extractHeap(struct heap *ogg) {
	if(ogg->heapsize < 1) {
		////printf("Heap underflow\n");
		return (struct coppia *) NULL;
	} else {
		struct coppia *top, *last, *primo;
		primo = ogg->get_el(ogg, 0);
		last = ogg->get_el(ogg, ogg->heapsize-1);
		top = malloc(sizeof(struct coppia));
		top->content = primo->content;
		top->priority = (int) (primo->priority);
		ogg->get_el(ogg, 0)->content = last->content;
		ogg->get_el(ogg, 0)->priority = last->priority;
		// stacca(last) {
		if(ogg->arrayortree == 2) {
			struct tree *padre;
			int zind;
			int pad;
			zind = (ogg->heapsize);
			pad = zind/2;
			pad--;
			padre = get_nodo(ogg, pad);
			if(padre != NULL) {
				if(padre->dx != NULL) {
					if(padre->dx->val == last) {
						free(padre->dx);
						padre->dx = NULL;
					}
				}
				if(padre->sx != NULL) {
					if(padre->sx->val == last) {
						free(padre->sx);
						padre->sx = NULL;
					}
				}
			}
		}
		// }
		free(last);
		ogg->heapsize = ogg->heapsize - 1;
		Heapify(ogg, 0);
		return top;
	}
}
void stampa_heap(struct heap *ogg) {
	int i;
	if(ogg->heapsize < 1) {
		//printf("NULL");
	} else {
		if(ogg->arrayortree == 1) {
			i = 0;
			while(i<ogg->heapsize) {
				//printf("[%d] ", i);
				//printelcoda(((struct coppia **)(ogg->content))[i], ogg);
				i++;
			}
		} else {
			//preorder((struct tree *)ogg->content, ogg);
		}
	}
}
void Heapify(struct heap *ogg, int i) {
	int l, r, m;
	struct coppia *left, *right, *current, *max;
	l = (2*i)+1;
	r = l+1;
	m = i;
	left = ogg->get_el(ogg, l);
	right = ogg->get_el(ogg, r);
	current = ogg->get_el(ogg, i);
	if(left != NULL && l < ogg->heapsize && ogg->checkminormax(left->priority,current->priority)==1) {
		m = l;
	} else {
		m = i;
	}
	max = ogg->get_el(ogg, m);
	if(right != NULL && r < ogg->heapsize && ogg->checkminormax(right->priority,max->priority)==1) {
		m = r;
	}
	if(m != i) {
		swap(current, ogg->get_el(ogg, m));
		/* 1 2 3 4 5 -> 5 4 3 1 2 */
		Heapify(ogg, m);
	}
}
void swap(struct coppia *a, struct coppia *b) {
	int tmp1;
	void *tmp2;
	tmp1 = a->priority;
	a->priority = b->priority;
	b->priority = tmp1;
	tmp2 = a->content;
	a->content = b->content;
	b->content = tmp2;
}
void build_heap(struct heap *ogg) {
	int i;
	i = ogg->heapsize / 2;
	while(i>=0) {
		Heapify(ogg, i);
		i--;
	}
}
struct coppia *get_el_array(struct heap *ogg, int indice) {
	if(indice >= 0 && indice < ogg->heapsize) {
		return ((struct coppia **)(ogg->content))[indice];
	} else {
		return (struct coppia *) NULL;
	}
}
struct coppia *get_el_tree(struct heap *ogg, int indice) {
	int i, j, heapsize, mask, primo1;
	struct tree *cur;
	i=0;
	j=0;
	primo1 = 0;
	heapsize = indice+1;
	if(heapsize == 1)
		return (((struct tree *)ogg->content) != NULL) ? ((struct tree *)ogg->content)->val : (struct coppia *) NULL;
	else if(heapsize < 1)
		return (struct coppia *) NULL;
	mask = pow(2, (int)log2(heapsize));
	cur = ((struct tree *)ogg->content);
	while(j<log2((double)(mask))+1 && cur != NULL){
		i=(mask>>j)&(heapsize);
		if(i>0){
			if(primo1 == 0) {
				primo1 = 1;
			} else {
//				////printf("1");
				cur = cur->dx;
			}
		}else {
//			////printf("0");
			cur = cur->sx;
		}
		j++;
	}
	if(cur != NULL)
		return (struct coppia *) cur->val;
	else
		return (struct coppia *) NULL;
}
struct tree *get_nodo(struct heap *ogg, int indice) {
	int i, j, heapsize, mask, primo1;
	struct tree *cur;
	i=0;
	j=0;
	primo1 = 0;
	heapsize = indice+1;
	if(heapsize == 1)
		return (((struct tree *)ogg->content) != NULL) ? ((struct tree *)ogg->content) : (struct tree *) NULL;
	else if(heapsize < 1)
		return (struct tree *) NULL;
	mask = pow(2, (int)log2(heapsize));
	cur = ((struct tree *)ogg->content);
	while(j<log2((double)(mask))+1 && cur != NULL){
		i=(mask>>j)&(heapsize);
		if(i>0){
			if(primo1 == 0) {
				primo1 = 1;
			} else {
//				////printf("1");
				cur = cur->dx;
			}
		}else {
//			////printf("0");
			cur = cur->sx;
		}
		j++;
	}
	if(cur != NULL)
		return (struct tree *) cur;
	else
		return (struct tree *) NULL;
}

struct heap *init_heap(struct heap *ogg, int dimensione) {
	if(ogg->arrayortree == 1) {
		return init_heap_array(ogg, dimensione);
	} else {
		return init_heap_tree(ogg, dimensione);
	}
}
struct heap *init_heap_array(struct heap *ogg, int dimensione) {
	int i;
	struct coppia **arr;
	arr = malloc(sizeof(struct coppia**) * (dimensione*2));
	ogg->heapsize = dimensione;
	ogg->real_allocated = dimensione*2;
	i = 0;
	while(i<dimensione) {
		arr[i] = (struct coppia *) malloc(sizeof(struct coppia));
		arr[i]->content = ogg->getInput();
		arr[i]->priority = getnum(-99999999,99999999,"\nInserisci la priorita': ");
		i++;
	}
	ogg->content = arr;
	return ogg;
}
struct heap *realloca_heap_array(struct heap *ogg) {
	struct coppia **arr;
	if((ogg->heapsize+1) >= ogg->real_allocated) {
		ogg->real_allocated = (ogg->heapsize+1)*2;
		arr = ((struct coppia **)(ogg->content));
		arr = realloc(arr, sizeof(struct coppia*) * (ogg->real_allocated));
		ogg->content = arr;
	}
	return ogg;
}
struct heap *init_heap_tree(struct heap *ogg, int dimensione) {
	int i;
	struct coppia *el;
	struct tree *al;
	al = NULL;
	ogg->content = al;
	ogg->heapsize = dimensione;
	i = 0;
	while(i<dimensione) {
		el = malloc(sizeof(struct coppia));
		el->content = ogg->getInput();
		el->priority = getnum(-99999999,99999999,"\nInserisci la priorita': ");
		ogg->content = instree(((struct tree *)ogg->content), i, el);
		i++;
	}
	preorder(((struct tree *)ogg->content), ogg);
	return ogg;
}

void printelcoda(struct coppia *el, struct heap *ogg) {
	if(el) {
		//printf("{ ");
		//ogg->print(el->content);
		//printf(" , priorita' %f }\n", el->priority);
	} else {
		//printf("{ NULL }\n");
	}
}
void freecoppia(struct coppia *tmp) {
	if(!tmp)
		return;
	if(tmp->content) {
		free(tmp->content);
	}
	if(tmp)
		free(tmp);
}
void setHeap(struct heap *funz, int type, int arrayortree, int minormax) {
	funz->tipo = type;
	funz->arrayortree = arrayortree;
	funz->minormax = minormax;
	switch(minormax) {
		default:
			case 1:
				funz->checkminormax = &minore;
		break;
		case 2:
				funz->checkminormax = &maggiore;
		break;
	}
	switch(arrayortree) {
		default:
			case 1: //ARRAY
				funz->get_el = &get_el_array;
		break;
		case 2: //ALBERO
			funz->get_el = &get_el_tree;
		break;
	}
	funz->libera = &freecoppia;
	switch(type) {
		default:
			case 0: //STRING
				funz->print = &stampastringa;
				funz->getInput = &getinputstringa;
				funz->cmp = &cmpstr;
		break;
		case 1: //INT
			funz->print = &stampaintero;
			funz->getInput = &getinputintero;
			funz->cmp = &cmpint;
		break;
		case 2: //DOUBLE
			funz->print = &stampadouble;
			funz->getInput = &getinputdouble;
			funz->cmp = &cmpdouble;
		break;
		case 3: //STRUCT
			funz->print = &stampapersona;
			funz->getInput = &getinputpersona;
			funz->cmp = &cmppers;
			break;
		case 4: //SOLO PER SCOPI INTERNI
			funz->print = NULL;
			funz->getInput = NULL;
			funz->cmp = NULL;
			funz->libera = freecoppia;
		break;
	}
}
