#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#include "tree.h"

struct tree *instree(struct tree *testa, int indice, struct coppia *el) {
	if(indice == 0) {
		testa = alloca_nodo(testa);
		testa->val = el;
		testa->sx = NULL;
		testa->dx = NULL;
		return testa;
	} else {
		struct tree *cur;
		int i, j, heapsize, mask, primo1;
		cur = testa;
		i=0;
		j=0;
		primo1 = 0;
		heapsize = indice+1;
		mask = pow(2, (int)log2(heapsize));
		while(j<log2((double)(mask))+1){
		i=(mask>>j)&(heapsize);
		if(i>0){
			if(primo1 == 0) {
				primo1 = 1;
			} else {
//				printf("1");
				if(cur->dx == NULL) {
					cur->dx = instree(cur, 0, el);
					break;
				} else {
					cur = cur->dx;
				}
			}
		}else {
//				printf("0");
				if(cur->sx == NULL) {
					cur->sx = instree(cur, 0, el);
					break;
				} else {
					cur = cur->sx;
				}
			}
		j++;
		}
	}
	printf("\n");
	return testa;
}

struct tree *alloca_nodo(struct tree *nodo) {
	nodo = (struct tree *) malloc(sizeof(struct tree));
	return nodo;
}

void preorder(struct tree *testa, struct heap *funz) {
	if(testa != NULL) {
		printelcoda(testa->val, funz);
		preorder(testa->sx, funz);
		preorder(testa->dx, funz);
	}
}
