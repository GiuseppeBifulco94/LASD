/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "generalizza.h"
#include "libreria.h"

int stringaparidispari(void *obj) {
	return (strlen((char *) obj)%2);
}
int intparidispari(void *obj) {
	return ((*(int *)obj)%2);
}
int doubleparidispari(void *obj) {
	return (((int)(*(double*)obj))%2);
}
int personaparidispari(void *obj) {
	return (strlen((*(struct persona *)obj).nome)%2 && strlen((*(struct persona *)obj).cognome)%2);
}
void stampaintero(void *obj) {
	if(obj)
		printf("%d", *(int *)obj);
	else
		printf("NULL");
}
void stampadouble(void *obj) {
	if(obj)
		printf("%lf", *(double*)obj);
	else
		printf("NULL");
}
void stampapersona(void *obj) {
	if(obj)
		printf("{ Nome: %s , Cognome: %s }", (*(struct persona *)obj).nome, (*(struct persona *)obj).cognome);
	else
		printf("{ NULL }");
}
void stampastringa(void *obj) {
	if(obj)
		printf("%s", (char *)obj);
	else
		printf("NULL");
}
void* getinputpersona() {
	struct persona *pers;
	pers = malloc(sizeof(struct persona));
	pers->nome = getalfa(0,99999,"\nInserisci un nome: ");
	pers->cognome = getalfa(0,99999,"\nInserisci un cognome: ");
	return (void *)pers;
}
void* getinputstringa() {
	char *stringa;
	stringa = getalfa(0,99999,"\nInserisci una stringa: ");
	return (void *)stringa;
}
void* getinputint() {
	int *intero;
	intero = malloc(sizeof(int));
	*intero = getnum(0,999999999,"\nInserisci un numero intero: ");
	return (void *)intero;
}
void* getinputdouble() {
	double *decimale;
	decimale = malloc(sizeof(double));
	*decimale = getdouble(.0,999999999.9,"\nInserisci un numero decimale: ");
	return (void *)decimale;
}
int cmpstr(void *a, void *b) {
	return strcmp((char *)a, (char *)b);
}
int cmpint(void *a, void *b) {
	int na, nb;
	na = *(int *)a;
	nb = *(int *)b;
	if(na == nb) {
		return 0;
	} else if(na > nb) {
		return 1;
	} else {
		return -1;
	}
}
int cmpdouble(void *a, void *b) {
	double na, nb;
	na = *(double *)a;
	nb = *(double *)b;
	if(na == nb) {
		return 0;
	} else if(na > nb) {
		return 1;
	} else {
		return -1;
	}
}
int cmppers(void *a, void *b) {
	int uno;
	char *na, *nb, *ca, *cb;
	na = (*(struct persona *)a).nome;
	nb = (*(struct persona *)b).nome;
	ca = (*(struct persona *)a).cognome;
	cb = (*(struct persona *)b).cognome;
	uno = strcmp(ca, cb);
	if(uno == 0) {
		return strcmp(na, nb);
	} else {
		return uno;
	}
}
void *getvoidint(int a) {
	int *b;
	b = malloc(sizeof(int));
	*b = a;
	return (void *)b;
}