/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include "abr.h"
#include "listaABR.h"
#include "libreria.h"

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
int char_is_number(char *s, int length, int withdot) {
	int i, isok, off;
	i = 0;
	off = 0;
	if(!s)
		return 0;
	while(off<length && (s[off] == ' ' || s[off] == '\t' || s[off] == '\n')) {
		off++;
	}
	i = off;
	while(i<length) {
		isok = 0;
		if(i==off && s[i] == '-') {
			isok = 1;
		} else if(isdigit(s[i])) {
			isok = 1;
		} else if(s[i] == '.' && withdot == 1 && i != off && (i+1)<length) {
			withdot = 0;
			isok = 1;
		}
		if(isok==0)
			return 0;
		i++;
	}
	return 1;
}

int is_double(char *s) {
	if(s && *s) {
		if(strlen(s) > 0)
			return char_is_number(s, strlen(s), 1);
	}
	return 0;
}
void CopiaDato(void *dest,int dpos, void *src,int spos, int dim) {
	void *dst_addr = dest+(dpos*dim);
	void *src_addr = src+(spos*dim);
	memcpy(dst_addr,src_addr,dim);
}
int random_num(int a, int b) {
	return (a+(rand()%(b-a+1)));
}

void dealloca_oggetto(void *s) {
	if(s != NULL)
		free(s);
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
		if(numero < start || numero > end)
			err = 0;
		free(dove);
	}
	return numero;
}

double getdouble(double start, double end, char *text) {
	char *dove, c, *errore;
	int err, length1, length2, length;
	double numero;
	length1 = length_int(start);
	length2 = length_int(end);
	length = length1 > length2 ? length1 : length2;
	err = 0;
	while(err == 0) {
		dove = getalfa(1, length, text);
		err = 1;
		if(!is_double(dove))
			err = 0;
		numero = atof(dove); //atof ritorna un double non un float
		if(numero < start || numero > end)
			err = 0;
		free(dove);
	}
	return numero;
}
void *random_int(void) {
	int *in;
	in = malloc(sizeof(int));
	*in = random_num(1, 999);
	return in;
}
void *random_double(void) {
	double *db;
	db = malloc(sizeof(double));
	*db = (double)((double)random_num(1,999) / (double)random_num(1,999));
	return db;
}
void *random_string(void) {
	void *stringa;
	int j, opz, length;
	char string[9];
	length = random_num(1,8);
	stringa = malloc((length+1)*sizeof(char));
	for(j=0;j<length;j++) {
		opz = random_num(1,99);
		if(opz <= 33) {
			string[j] = random_num(65,90);
		} else if(opz <= 66) {
			string[j] = random_num(97,122);
		} else {
			string[j] = random_num(48,57);
		}
	}
	string[j] = '\0';
	CopiaDato(stringa, 0, string, 0, sizeof(char) * (strlen(string)+1));
	return stringa;
}