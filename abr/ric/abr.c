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
//	CopiaDato(nodo, 0, elem, 0, sizeof(struct persona));
//	CopiaDato( ((struct persona *)nodo)->nome, 0, ((struct persona *)elem)->nome, 0, sizeof(char)*strlen((char*)((struct persona *)elem)->nome));
//	CopiaDato( ((struct persona *)nodo)->cognome, 0, ((struct persona *)elem)->cognome, 0, sizeof(char)*strlen((char*)((struct persona *)elem)->nome));
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
int riempi_array_da_ABRmain(ABR *testa, void ** array, int length, FUNC funz) {
	if(testa != NULL) {
		length = riempi_array_da_ABRmain(testa->sx, array, length, funz);
		array[length] = funz.alloca_valore(array[length], testa->val);
		return riempi_array_da_ABRmain(testa->dx, array, (length + 1), funz);
	}
	return length;
}

int nnodi(ABR *testa) {
    if (testa != NULL)
        return nnodi(testa->sx) + nnodi(testa->dx) + 1;
    else
        return 0;
}
void riempi_array_da_ABR(ABR *testa, void **array, FUNC funz) {
	int length;
	length = 0;
	length = riempi_array_da_ABRmain(testa, array, length, funz);
}

ABR *bilancia_da_arraymain(void **array, int inizio, int fine, FUNC funz) {
	int middle;
	ABR *nodo = alloca_nodo(nodo);
	if(inizio > fine || inizio < 0 || fine < 0) {
		free(nodo);
		nodo = NULL;
		return nodo;
	} else {
		middle = (fine+inizio)/2;
		nodo->val = funz.alloca_valore(nodo->val, array[middle]);
		nodo->sx = bilancia_da_arraymain(array, inizio, middle-1, funz);
		nodo->dx = bilancia_da_arraymain(array, middle+1, fine, funz);
		return nodo;
	}
}

ABR *bilancia(ABR *testa, FUNC funz) {
	ABR *tmp;
	void **array;
	int i, num_nodi;
	num_nodi = nnodi(testa);
	array = (void **) malloc(num_nodi * sizeof(void *));
	riempi_array_da_ABR(testa, array, funz);
	tmp = bilancia_da_arraymain(array, 0, num_nodi-1, funz);
	dealloca_array(array, num_nodi, funz);
	return tmp;
}

bool ABR_uguali(ABR *a, ABR *b, FUNC funz) {
	bool sin, des;
	sin = des = (a == NULL && b == NULL) ? true : false;
	if(a != NULL && b != NULL) {
		if(funz.cmp(a->val,b->val) == 0) {
			sin = ABR_uguali(a->sx, b->sx, funz);
			des = ABR_uguali(a->dx, b->dx, funz);
		}
	}
	return (sin && des);
}

ABR *duplica(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		ABR *nodo = alloca_nodo(nodo);
		nodo->val =  funz.alloca_valore(nodo->val, testa->val);
		nodo->sx = duplica(testa->sx, funz);
		nodo->dx = duplica(testa->dx, funz);
		return nodo;
	}
	return testa;
}

ABR *inserisci(ABR *testa, void *valore, FUNC funz) {
	int confronto;
	if(testa == NULL) {
		ABR *nodo = alloca_nodo(nodo);
		nodo->val = funz.alloca_valore(nodo->val, valore);
		nodo->sx = NULL;
		nodo->dx = NULL;
		return nodo;
	} else {
		confronto = funz.cmp(testa->val, valore);
		if(confronto < 0) {
			testa->dx = inserisci(testa->dx, valore, funz);
		} else if(confronto == 0) {
			/* stringa già presente */
		} else {
			testa->sx = inserisci(testa->sx, valore, funz);
		}
		return testa;
	}
}

ABR *insrand(ABR *testa, int n, FUNC funz) {
	void *elem;
	if(n == 0)
		return testa;
	elem = funz.oggetto_random();
	testa = inserisci(testa, elem, funz);
	funz.libera(elem);
	return insrand(testa, n-1, funz);
}

ABR *staccamin(ABR *nodo, ABR *padre) {
	if(nodo != NULL) {
		if(nodo->sx != NULL) {
			return staccamin(nodo->sx, nodo);
		} else {
			if(nodo == padre->sx) {
				padre->sx = nodo->dx;
			} else {
				padre->dx = nodo->dx;
			}
		}
	}
	return nodo;
}

ABR *cancella(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		ABR *nodo = testa;
		if(testa->sx == NULL) {
			testa = testa->dx;
		} else if(testa->dx == NULL) {
			testa = testa->sx;
		} else {
			nodo = staccamin(testa->dx, testa);
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

ABR *es5(ABR *testa, int flag, void *a, void *b, FUNC funz) {
	/* cancella tutti i nodi dell'albero con chiave pari(flag=0) o dispari(flag!=0) e chiave compresa tra a e b */
	if(testa != NULL) {
		if(funz.cmp(testa->val, a) <= 0) {
			testa->dx = es5(testa->dx, flag, a, b, funz);
		} else if(funz.cmp(testa->val, b) >= 0) {
				testa->sx = es5(testa->sx, flag, a, b, funz);
		} else {
			testa->sx = es5(testa->sx, flag, a, b, funz);
			testa->dx = es5(testa->dx, flag, a, b, funz);
				if( (funz.condizionees5(testa->val) == 0 && flag == 0) || (funz.condizionees5(testa->val) != 0 && flag != 0))
					testa = cancella(testa, funz);
		}
	}
	return testa;
}

ABR *distruggi(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		ABR *nodo = testa;
		testa->sx = distruggi(testa->sx, funz);
		testa->dx = distruggi(testa->dx, funz);
		funz.libera(testa->val);
		dealloca_nodo(testa);
	}
	return (ABR *) NULL;
}

void preorder(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		funz.print(testa->val);
		printf("\n");
		preorder(testa->sx, funz);
		preorder(testa->dx, funz);
	}
}

void inorder(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		inorder(testa->sx, funz);
		funz.print(testa->val);
		printf("\n");
		inorder(testa->dx, funz);
	}
}

void postorder(ABR *testa, FUNC funz) {
	if(testa != NULL) {
		postorder(testa->sx, funz);
		postorder(testa->dx, funz);
		funz.print(testa->val);
		printf("\n");
	}
}

void stampaABR(ABR *testa, int mode, FUNC funz) {
		if(mode == 0) { /* visita in pre-order */
			preorder(testa, funz);
		} else if(mode == 1) { /* visita in order */
			inorder(testa, funz);
		} else { /* visita in post-order */
			postorder(testa, funz);
		}
}

ABR *cancella_valore(ABR *testa, void *elem, FUNC funz) {
	int confronto;
	if(testa != NULL) {
		confronto = funz.cmp(testa->val, elem);
		if(confronto > 0) {
			testa->sx = cancella_valore(testa->sx, elem, funz);
		} else if(confronto < 0) {
			testa->dx = cancella_valore(testa->dx, elem, funz);
		} else {
			testa = cancella(testa, funz);
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
