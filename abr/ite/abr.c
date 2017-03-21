/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "abr.h"
#include "libreria.h"
#include "generalizza.h"
#include "stack.h"

void dealloca_persona(void *s) {
	if(s != NULL) {
		free(((struct persona*)s)->nome);
		free(((struct persona*)s)->cognome);
		free(s);
	}
}
void dealloca_nodo(ABR *nodo) {
	free(nodo);
}
ABR *alloca_nodo(ABR *nodo) {
	nodo = (ABR *) malloc(sizeof(ABR));
	return nodo;
}
void *alloca_stringa(void *nodo, void *elem) {
	nodo = (void *) malloc(sizeof(char) * (strlen((char *)elem)+1));
	CopiaDato(nodo, 0, elem, 0, sizeof(char)*(strlen((char *)elem)+1));
	return nodo;
}
void *alloca_intero(void *nodo, void *elem) {
	nodo = (void *)malloc(sizeof(int));
	CopiaDato(nodo, 0, elem, 0, sizeof(int));
	return nodo;
}
void *alloca_double(void *nodo, void *elem) {
	nodo = (void *) malloc(sizeof(double));
	CopiaDato(nodo, 0, elem, 0, sizeof(double));
	return nodo;
}
void *alloca_persona(void *nodo, void *elem) {
	nodo = (void *) malloc(sizeof(struct persona));
	((struct persona *)nodo)->nome = alloca_stringa(((struct persona *)nodo)->nome, ((struct persona *)elem)->nome);
	((struct persona *)nodo)->cognome = alloca_stringa(((struct persona *)nodo)->cognome, ((struct persona *)elem)->cognome);
	return nodo;
}
void stampa_array(void **array, int length, FUNC funz) {
	int i;
	for(i=0;i<length;i++) {
		printf("Array[%d] = ", i);
		funz.print(array[i]);
		printf("\n");
	}
}
int nnodi_iterativo(ABR *testa) {
	ABR *cur;
	int res;
	struct stack *st;
	cur = testa;
	st = NULL;
	res = 0;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			res = res + 1;
			if(cur->dx != NULL)
				st = pushst(st, cur->dx);
			cur = cur->sx;
		} else {
			cur = topst(st);
			st = popst(st);
		}
	}
	return res;
}
void riempi_array_da_ABR_iterativo(ABR *testa, void **array, FUNC funz) {
	struct stack *st;
	ABR *cur;
	int indice;
	cur = testa;
	st = NULL;
	indice = 0;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			st = pushst(st, cur);
			cur = cur->sx;
		} else {
			cur = topst(st);
			array[indice] = funz.alloca_valore(array[indice], cur->val);
			st = popst(st);
			cur = cur->dx;
			indice++;
		}
	}
}

ABR *bilancia_da_arraymain_iterativo(void **array, int num_nodi, FUNC funz) {
	struct stack *st, *st1, *st2;
	ABR *cur, *testa;
	int i, j, m;
	i = 0;
	j = num_nodi-1;
	m = (i+j)/2;
	testa = alloca_nodo(testa);
	cur = testa;
	st = NULL;
	st1 = NULL;
	st2 = NULL;
	st1 = pushst(st1, getvoidint(i));
	st2 = pushst(st2, getvoidint(j));
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			i = *(int*)topst(st1);
			j = *(int*)topst(st2);
			m = ( i + j ) / 2;
			cur->val = funz.alloca_valore(cur->val, array[m]);
			free(topst(st1));
			free(topst(st2));
			st1 = popst(st1);
			st2 = popst(st2);
			if( (j-m-1) >= 0) {
				cur->dx = alloca_nodo(cur->dx);
				st = pushst(st, cur->dx);
				st1 = pushst(st1, getvoidint(m+1));
				st2 = pushst(st2, getvoidint(j));
			}
			if( (m-i-1) >= 0) {
				cur->sx = alloca_nodo(cur->sx);
				st1 = pushst(st1, getvoidint(i));
				st2 = pushst(st2, getvoidint(m-1));
			}
			cur = cur->sx;
		} else {
			cur = topst(st);
			st = popst(st);
		}
	}
	return testa;
}

ABR *bilancia_iterativo(ABR *testa, FUNC funz) {
	ABR *tmp;
	void **array;
	int i, num_nodi;
	num_nodi = nnodi_iterativo(testa);
	array = (void **) malloc(num_nodi * sizeof(void *));
	riempi_array_da_ABR_iterativo(testa, array, funz);
	tmp = bilancia_da_arraymain_iterativo(array, num_nodi, funz);
	dealloca_array(array, num_nodi, funz);
	return tmp;
}

bool ABR_uguali_iterativo(ABR *a, ABR *b, FUNC funz) {
	ABR *cur1;
	ABR *cur2;
	struct stack *st1;
	struct stack *st2;
	st1 = NULL;
	st2 = NULL;
	bool ret = true;
	cur1 = a;
	cur2 = b;
	while(cur1 != NULL || st1 != NULL) {
		if(cur1 != NULL) {
			if(cur2 == NULL) {
				break;
			}
			if(funz.cmp(cur1->val, cur2->val) != 0) {
				break;
			}
			if(cur1->dx != NULL) {
				if(cur2->dx == NULL) {
					break;
				}
				st1 = pushst(st1, cur1->dx);
				st2 = pushst(st2, cur2->dx);
			}
			cur1 = cur1->sx;
			cur2 = cur2->sx;
		} else {
			if(cur2 != NULL) {
				break;
			}
			cur1 = topst(st1);
			cur2 = topst(st2);
			st1 = popst(st1);
			st2 = popst(st2);
		}
	}
	if(st1 != NULL || st2 != NULL || cur2 != NULL) {
		st1 = svuotastackst(st1);
		st2 = svuotastackst(st2);
		ret = false;
	}
	return ret;
}

ABR *duplica_iterativo(ABR *testa, FUNC funz) {
	ABR *testa2, *cur1, *cur2;
	struct stack *st1, *st2;
	st1 = NULL;
	st2 = NULL;
	testa2 = NULL;
	testa2 = alloca_nodo(testa2);
	if(testa == NULL) {
		return testa2;
	}
	testa2->val = funz.alloca_valore(testa2->val, testa->val);
	cur1 = testa;
	cur2 = testa2;
	st1 = NULL;
	if(testa->sx != NULL) {
		cur2->sx = alloca_nodo(cur2->sx);
		cur2->sx->val = funz.alloca_valore(cur2->sx->val, cur1->sx->val);
	}
	if(testa->dx != NULL) {
		cur2->dx = alloca_nodo(cur2->dx);
		cur2->dx->val = funz.alloca_valore(cur2->dx->val, cur1->dx->val);
		st1 = pushst(st1, testa->dx);
		st2 = pushst(st2, testa2->dx);
	}
	cur2 = cur2->sx;
	cur1 = cur1->sx;
	while(cur1 != NULL || st1 != NULL) {
		if(cur1 != NULL) {
			if(cur1->sx != NULL) {
				cur2->sx = alloca_nodo(cur2->sx);
				cur2->sx->val = funz.alloca_valore(cur2->sx->val, cur1->sx->val);
			}
			if(cur1->dx != NULL) {
				cur2->dx = alloca_nodo(cur2->dx);
				cur2->dx->val = funz.alloca_valore(cur2->dx->val, cur1->dx->val);
				st1 = pushst(st1, cur1->dx);
				st2 = pushst(st2, cur2->dx);
			}
			cur1 = cur1->sx;
			cur2 = cur2->sx;
		} else {
			cur1 = topst(st1);
			st1 = popst(st1);
			cur2 = topst(st2);
			st2 = popst(st2);
		}
	}
	return (ABR *) testa2;
}

ABR *inserisci_iterativo(ABR *testa, void *valore, FUNC funz) {
	ABR *cur, *padre, *nuovonodo;
	cur = testa;
	padre = NULL;
	while(cur != NULL) {
		padre = cur;
		if(funz.cmp(cur->val, valore) < 0) {
			cur = cur->dx;
		} else if(funz.cmp(cur->val, valore) > 0) {
			cur = cur->sx;
		} else {
			return testa;
		}
	}
	nuovonodo = alloca_nodo(nuovonodo);
	nuovonodo->val = funz.alloca_valore(nuovonodo->val, valore);
	nuovonodo->sx = NULL;
	nuovonodo->dx = NULL;
	if(padre != NULL) {
		if(funz.cmp(padre->val, valore) < 0) {
			padre->dx = nuovonodo;
		} else {
			padre->sx = nuovonodo;
		}
		return testa;
	} else {
		return nuovonodo;
	}
}

ABR *insrand_iterativo(ABR *testa, int n, FUNC funz) {
	int i;
	void *elem;
	elem = NULL;
	for(i=0;i<n;i++) {
		elem = funz.oggetto_random();
		testa = inserisci_iterativo(testa, elem, funz);
		if(elem != NULL) {
			funz.libera(elem);
		}
	}
	return testa;
}

ABR *staccamin_iterativo(ABR *nodo, ABR *padre) {
	ABR *cur, *p;
	p = padre;
	cur = nodo;
	while(cur->sx != NULL) {
		p = cur;
		cur = cur->sx;
	}
	if(cur == p->sx) {
		p->sx = cur->dx;
	} else {
		p->dx = cur->dx;
	}
	return cur;
}

ABR *cancella_iterativo(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		ABR *nodo = testa;
		if(testa->sx == NULL) {
			testa = testa->dx;
		} else if(testa->dx == NULL) {
			testa = testa->sx;
		} else {
			nodo = staccamin_iterativo(testa->dx, testa);
			funz.libera(testa->val);
			testa->val = funz.alloca_valore(testa, nodo->val);
		}
		if(nodo != NULL) {
			funz.libera(nodo->val);
			dealloca_nodo(nodo);
		}
	}
	return testa;
}

ABR *es5_iterativo(ABR *testa, int flag, void *a, void *b, FUNC funz) {
	/* cancella tutti i nodi dell'albero con chiave pari(flag=0) o dispari(flag!=0) e chiave compresa tra a e b */
	struct stack *st;
	ABR *cur, *last;
	ABR *tmp;
	st = NULL;
	last = NULL;
	cur = testa;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			if(funz.cmp(cur->val, a) < 0) {
				cur = cur->dx;
			} else if(funz.cmp(cur->val,b) > 0) {
				cur = cur->sx;
			} else {
				st = pushst(st, cur);
				cur = cur->sx;
			}
		} else {
			cur = topst(st);
			if(cur->dx != NULL && cur->dx != last) {
				cur = cur->dx;
			} else {
				last = cur;
				st = popst(st);
				if( funz.cmp(cur->val, a) >= 0 && funz.cmp(cur->val, b) <= 0  && ((funz.condizionees5(cur->val) == 0 && flag == 0) || (funz.condizionees5(cur->val) != 0 && flag != 0))) {
					tmp = topst(st);
					if(tmp) {
						if(tmp->sx == cur) {
							tmp->sx = cancella_iterativo(cur, funz);
						} else {
							tmp->dx = cancella_iterativo(cur, funz);
						}
					} else {
						testa = cancella_iterativo(testa, funz);
					}
				}
				cur = NULL;
			}
		}
	}
	return testa;
}


ABR *distruggi_iterativo(ABR *testa, FUNC funz) {
	struct stack *st;
	ABR *cur, *last;
	cur = testa;
	last = NULL;
	st = NULL;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			st = pushst(st, cur);
			cur = cur->sx;
		} else {
			cur = topst(st);
			if(cur->dx != NULL && cur->dx != last) {
				cur = cur->dx;
			} else {
				last = cur;
				funz.libera(cur->val);
				dealloca_nodo(cur);
				st = popst(st);
				cur = NULL;
			}
		}
	}
	return (ABR *) NULL;
}

void preorder_iterativo(ABR *testa, FUNC funz) {
	struct stack *st;
	ABR *cur;
	cur = testa;
	st = NULL;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			funz.print(cur->val);
			printf("\n");
			if(cur->dx != NULL)
				st = pushst(st, cur->dx);
			cur = cur->sx;
		} else {
			cur = topst(st);
			st = popst(st);
		}
	}
}

void inorder_iterativo(ABR *testa, FUNC funz) {
	struct stack *st;
	ABR *cur;
	cur = testa;
	st = NULL;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			st = pushst(st, cur);
			cur = cur->sx;
		} else {
			cur = topst(st);
			funz.print(cur->val);
			printf("\n");
			st = popst(st);
			cur = cur->dx;
		}
	}
}

void postorder_iterativo(ABR *testa, FUNC funz) {
	struct stack *st;
	ABR *cur, *last;
	cur = testa;
	st = NULL;
	last = NULL;
	while(cur != NULL || st != NULL) {
		if(cur != NULL) {
			st = pushst(st, cur);
			cur = cur->sx;
		} else {
			cur = topst(st);
			if(cur->dx != NULL && cur->dx != last) {
				cur = cur->dx;
			} else {
				funz.print(cur->val);
				printf("\n");
				last = cur;
				st = popst(st);
				cur = NULL;
			}
		}
	}
}

void stampaABR_iterativo(ABR *testa, int mode, FUNC funz) {
		if(mode == 0) { /* visita in pre-order */
			preorder_iterativo(testa, funz);
		} else if(mode == 1) { /* visita in order */
			inorder_iterativo(testa, funz);
		} else { /* visita in post-order */
			postorder_iterativo(testa, funz);
		}
}

ABR *cancella_valore_iterativo(ABR *testa, void *elem, FUNC funz) {
	ABR *cur, *padre;
	cur = testa;
	padre = NULL;
	while(cur != NULL) {
		if(funz.cmp(cur->val, elem) < 0) {
			padre = cur;
			cur = cur->dx;
		} else if(funz.cmp(cur->val, elem) > 0) {
			padre = cur;
			cur = cur->sx;
		} else {
			break;
		}
	}
	if(cur != NULL) { //L'elemento da cancellare esiste ed è contenuto in cur
		if(padre != NULL) {
			if(padre->sx == cur) {
				padre->sx = cancella_iterativo(padre->sx, funz);
			} else {
				padre->dx = cancella_iterativo(padre->dx, funz);
			}
		} else { //L'elemento da cancellare è la testa dell'ABR
			testa = cancella_iterativo(testa, funz);
		}
	}
	return testa;
}

void *random_struct(void) {
	char a[9], b[9];
	int length, lengthb, j, opz;
	struct persona *tmp;
	tmp = malloc(sizeof(struct persona));
	length = random_num(4, 12);
	lengthb = random_num(4, 12);
	tmp->nome = malloc((length+1)*sizeof(char));
	tmp->cognome = malloc((lengthb+1)*sizeof(char));
	for(j=0;j<length;j++) {
		opz = random_num(1,99);
		if(opz <= 50) {
			a[j] = random_num(65,90);
		} else {
			a[j] = random_num(97,122);
		}
	}
	a[j] = '\0';
	strcpy(tmp->nome, a);
	for(j=0;j<lengthb;j++) {
		opz = random_num(1,99);
		if(opz <= 50) {
			b[j] = random_num(65,90);
		} else {
			b[j] = random_num(97,122);
		}
	}
	b[j] = '\0';
	strcpy(tmp->cognome, b);
	return (void *)tmp;
}
void dealloca_array(void **array, int length, FUNC funz) {
	int i;
	void *tmp;
	for(i=0;i<length;i++) {
		if(array[i] != NULL) {
			tmp = array[i];
			funz.libera(tmp);
		}
	}
	if(array != NULL) {
		free(array);
	}
}
void setLib(FUNC *funz, int type) {
	funz->tipo = type;
	switch(type) {
		default:
			case 0: //STRING
				funz->print = &stampastringa;
				funz->getInput = &getinputstringa;
				funz->alloca_valore = &alloca_stringa;
				funz->libera = &dealloca_oggetto;
				funz->cmp = &cmpstr;
				funz->condizionees5 = &stringaparidispari;
				funz->oggetto_random = &random_string;
		break;
		case 1: //INT
			funz->print = &stampaintero;
			funz->getInput = &getinputint;
			funz->alloca_valore = &alloca_intero;
			funz->libera = &dealloca_oggetto;
			funz->cmp = &cmpint;
			funz->condizionees5 = &intparidispari;
			funz->oggetto_random = &random_int;
		break;
		case 2: //DOUBLE
			funz->print = &stampadouble;
			funz->getInput = &getinputdouble;
			funz->alloca_valore = &alloca_double;
			funz->libera = &dealloca_oggetto;
			funz->cmp = &cmpdouble;
			funz->condizionees5 = &doubleparidispari;
			funz->oggetto_random = &random_double;
		break;
		case 3: //STRUCT
			funz->print = &stampapersona;
			funz->getInput = &getinputpersona;
			funz->alloca_valore = &alloca_persona;
			funz->libera = &dealloca_persona;
			funz->cmp = &cmppers;
			funz->condizionees5 = &personaparidispari;
			funz->oggetto_random = &random_struct;
			break;
	}
}
