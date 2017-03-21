#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "generalizza.h"

int length_int(int i) {
	int n;
	n = 1;
	i = abs(i);
	while (i > 9) {
		n++;
		i /= 10;
	}
	return n;
}
int minore(int a, int b) {
	return (a < b ? 1 : 0);
}
int maggiore(int a, int b) {
	return (a > b ? 1 : 0);
}

int getnum(int start, int end, char *text) {
	char *dove, c, *errore;
	int err, length1, length2, length, numero, i;
	length1 = length_int(start);
	length2 = length_int(end);
	length = length1 > length2 ? length1 : length2;
	err = 0;
	while(err == 0) {
		dove = getalfa(1, length, text);
		i = 0;
		err = 1;
		while(i<strlen(dove)) {
			if(!isdigit(dove[i])) {
				err = 0;
				break;
			}
			i++;
		}
		numero = atoi(dove);
		free(dove);
	}
	return numero;
}

char *getalfa(int start, int end, char *text) {
	char *dove, c, *stringa;
	int i, max;
	max = (end-start+1);
	dove = (char *) malloc(sizeof(char)*(max+1));
	for(i=0;i<(max+1);i++)
		dove[i] = '\0';
	i = 0;
	while( (i == 0) || (i < start || i > end)) {
		write(1, text, strlen(text)*sizeof(char));
		dove[0] = '\0';
		i = 0;
		while ((read(0, &c, sizeof(c)) == sizeof(char)) && (c != '\n')) {
			if((i+1) > max) {
				max = max*2;
				dove = (char *) realloc(dove, (sizeof(char)*max)+1);
			}
			dove[i] = (char)c;
			i++;
		}
		dove[i] = '\0';
	}
	stringa = (char *) malloc(sizeof(char) * (i+1));
	strcpy(stringa, dove);
	free(dove);
	return stringa;
}

void CopiaDato(void *dest,int dpos, void *src,int spos, int dim) {
	void *dst_addr = dest+(dpos*dim);
	void *src_addr = src+(spos*dim);
	memcpy(dst_addr,src_addr,dim);
}

void dealloca_oggetto(void *s) {
	if(s != NULL)
		free(s);
}
void dealloca_persona(void *s) {
	if(s != NULL) {
		free(((struct persona*)s)->nome);
		free(((struct persona*)s)->cognome);
		free(s);
	}
}
double getdouble(double start, double end, char *text) {
	char *dove, c, *errore;
	int i, max, err;
	double numero;
	errore = NULL;
	numero = .0;
	max = (end-start+1);
	dove = (char *) calloc(max+1,sizeof(char));
	i = 0;
	errore = NULL;
	err = 1;
	while( (i == 0) || (numero < start || numero > end) || err == 1) {
		write(1, text, strlen(text)*sizeof(char));
		dove[0] = '\0';
		i = 0;
		max = 5;
		while ((read(0, &c, sizeof(c)) > 0) && (c != '\n')) {
			if((i+1) > max) {
				max = max*2;
				dove = (char *) realloc(dove, (sizeof(char)*max)+1);
			}
			dove[i] = c;
			i++;
		}
		dove[i] = '\0';
		numero = (double) strtod(dove, &errore);
		if(errore != NULL) {
			if(strlen(errore)>0) {
				err = 1;
			} else {
				err = 0;
			}
		} else {
			err = 0;
		}
		errore = NULL;
	}
	free(dove);
	return numero;
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
	pers->nome = getalfa(1,999,"\nInserisci un nome: ");
	pers->cognome = getalfa(1,999,"\nInserisci un cognome: ");
	return (void *)pers;
}
void* getinputstringa() {
	char *stringa;
	stringa = getalfa(1,999,"\nInserisci una stringa: ");
	return (void *)stringa;
}
void* getinputintero() {
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
	if(a == NULL || b == NULL) {
		return -1;
	}
	return strcmp((char *)a, (char *)b);
}
int cmpint(void *a, void *b) {
	int na, nb;
	if(a == NULL || b == NULL) {
		return -1;
	}
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
	if(a == NULL || b == NULL) {
		return -1;
	}
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
	if(a == NULL || b == NULL) {
		return -1;
	}
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