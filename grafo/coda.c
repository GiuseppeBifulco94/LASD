#include <stdlib.h>
#include "coda.h"
struct coda *accoda_coda(struct coda *st, void *el) {
	struct coda *first, *nuovo;
	nuovo = malloc(sizeof(struct coda));
	nuovo->content = el;
	nuovo->next = NULL;
	if(st == NULL) {
		st = nuovo;
	} else {
		first = st;
		while(first->next != NULL) {
			first = first->next;
		}
		first->next = nuovo;
	}
	return st;
}
struct coda *decoda_coda(struct coda *st) {
	if(st) {
		struct coda *tmp;
		tmp = st;
		st = st->next;
		free(tmp);
	}
	return st;
}
void *testa_coda(struct coda *st) {
	if(st) {
		return st->content;
	} else {
		return NULL;
	}
}

struct coda *svuota_coda(struct coda *st) {
	while(testa_coda(st) != NULL) {
		st = decoda_coda(st);
	}
	return st;
}