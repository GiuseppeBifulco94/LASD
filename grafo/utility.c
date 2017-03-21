#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include "utility.h"
struct persona {
	char *nome;
	char *cognome;
};

void CopiaDato(void *dest,int dpos, void *src,int spos, int dim) {
	void *dst_addr = dest+(dpos*dim);
	void *src_addr = src+(spos*dim);
	memcpy(dst_addr,src_addr,dim);
}

void* allocaintero(void *obj) {
	if(obj) {
		void *obj2;
		obj2 = malloc(sizeof(int));
		CopiaDato(obj2,0,obj,0,sizeof(int));
		return obj2;
	} else {
		return NULL;
	}
}
void* allocadouble(void *obj) {
	if(obj) {
		void *obj2;
		obj2 = malloc(sizeof(double));
		CopiaDato(obj2,0,obj,0,sizeof(double));
		return obj2;
	} else {
		return NULL;
	}
}
void* allocafloat(void *obj) {
	if(obj) {
		void *obj2;
		obj2 = malloc(sizeof(float));
		CopiaDato(obj2,0,obj,0,sizeof(float));
		return obj2;
	} else {
		return NULL;
	}
}
void* allocapersona(void *obj) {
	if(obj) {
		void *obj2;
		obj2 = malloc(sizeof(struct persona));
		((struct persona *)obj2)->nome = malloc(sizeof(char)*(strlen((char*)((struct persona *)obj)->nome)+1));
		((struct persona *)obj2)->cognome = malloc(sizeof(char)*(strlen((char*)((struct persona *)obj)->cognome)+1));
		CopiaDato(((struct persona *)obj2)->nome,0, ((struct persona *)obj)->nome, 0, sizeof(char)*(strlen((char*)((struct persona *)obj)->nome)+1));
		CopiaDato(((struct persona *)obj2)->cognome,0, ((struct persona *)obj)->cognome, 0, sizeof(char)*(strlen((char*)((struct persona *)obj)->cognome)+1));
		return obj2;
	} else {
		return NULL;
	}
}
void* allocastringa(void *obj) {
	if(obj) {
		void *obj2;
		obj2 = malloc(sizeof(char)*(strlen((char*)obj)+1));
		CopiaDato(obj2,0,obj,0,sizeof(char)*(strlen((char*)obj)+1));
		return obj2;
	} else {
		return NULL;
	}
}

int minore(int a, int b) {
	return (a < b ? 1 : 0);
}
int maggiore(int a, int b) {
	return (a > b ? 1 : 0);
}
void fprintfstringa(void *fd,void *obj) {
	if(obj)
		fprintf((FILE*)fd, "%s", (char*)obj);
	else
		fprintf((FILE*)fd, "NULL");
}
void fprintfintero(void *fd,void *obj) {
	if(obj)
		fprintf((FILE*)fd, "%d", *(int *)obj);
	else
		fprintf((FILE*)fd, "NULL");
}
void fprintfdouble(void *fd,void *obj) {
	if(obj)
		fprintf((FILE*)fd, "%lf", *(double*)obj);
	else
		fprintf((FILE*)fd, "NULL");
}
void fprintfpersona(void *fd,void *obj) {
	if(obj)
		fprintf((FILE*)fd, "< %s : %s >", (*(struct persona *)obj).nome, (*(struct persona *)obj).cognome);
	else
		fprintf((FILE*)fd, "NULL");
}
void* getinputpersona() {
	struct persona *pers;
	pers = malloc(sizeof(struct persona));
	pers->nome = getalfa(1,99999,"\nInserisci un nome: ");
	pers->cognome = getalfa(1,99999,"\nInserisci un cognome: ");
	return (void *)pers;
}
void* getinputstringa() {
	char *stringa;
	stringa = getalfa(1,99999,"\nInserisci una stringa: ");
	return (void *)stringa;
}
void* getinputintero() {
	int *intero;
	intero = malloc(sizeof(int));
	*intero = getnum(-999999999,999999999,"\nInserisci un numero intero: ");
	return (void *)intero;
}
void* getinputdouble() {
	double *decimale;
	decimale = malloc(sizeof(double));
	*decimale = getdouble(-999999999.9,999999999.9,"\nInserisci un numero decimale: ");
	return (void *)decimale;
}
void* getinputfloat() {
	float *decimale;
	decimale = malloc(sizeof(float));
	*decimale = (float) getdouble(-999999999.9,999999999.9,"\nInserisci un numero decimale: ");
	return (void *)decimale;	
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
void stampafloat(void *obj) {
	if(obj)
		printf("%f", *(float*)obj);
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
int cmpstr(void *a, void *b) {
	if(a == NULL || b == NULL) {
		return -1;
	}
	return strcmp((char *)a, (char *)b);
}
int cmpint(void *a, void *b) {
	int na, nb;
	na = 0;
	nb = 0;
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
	na = 0.0;
	nb = 0.0;
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
void dealloca_persona(void *s) {
	if(s != NULL) {
		free(((struct persona*)s)->nome);
		free(((struct persona*)s)->cognome);
		free(s);
	}
}
void dealloca_oggetto(void *s) {
	if(s != NULL)
		free(s);
}
void *string_to_int(char *s) {
	int *o;
	if(!s || !*s) {
		return (void *) NULL;
	}
	o = NULL;
	o = malloc(sizeof(int));
	sscanf(s, "%d", o);
	return (void *)o;
}

void *string_to_string(char *s) {
	char *o;
	if(!s || !*s) {
		return (void *) NULL;
	}
	o = malloc(sizeof(char)*(strlen(s)+1));
	strcpy(o,s);
	trim(o);
	return (void *)o;
}

void *string_to_double(char *s) {
	double *o;
	if(!s || !*s) {
		return (void *) NULL;
	}
	o = malloc(sizeof(double));
	sscanf(s, "%lf", o);
	return (void *)o;
}

void *string_to_persona(char *s) {
	struct persona *o;
	char *nome, *cognome;
	int i, j, count;
	if(!s || !*s) {
		return (void *) o;
	}
	o = malloc(sizeof(struct persona));
	nome = malloc(sizeof(char)*(strlen(s)));
	cognome = malloc(sizeof(char)*(strlen(s)));
	i = 0;
	j = 0;
	count = 0;
	for(i=0;i<strlen(s);i++) {
		if(s[i] == '<') {
			if(count==0) {
				count = 1;
				continue;
			} else {
				break;
			}
		}
		if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {
			if(count == 1)
				break;
		}
	}
	for(;i<strlen(s);i++) {
		if(s[i] == ':') {
			i++;
			break;
		}
		nome[j] = s[i];
		j++;
	}
	nome[j] = '\0';
	for(;i<strlen(s);i++) {
		if(s[i] != ' ' && s[i] != '\t' && s[i] != '\n') {
			break;
		}
	}
	j = 0;
	for(;i<strlen(s);i++) {
		if(s[i] == '>') {
			break;
		}
		cognome[j] = s[i];
		j++;
	}
	cognome[j] = '\0';
	trim(nome);
	trim(cognome);
	//realloca di dimensione giusta
	nome = realloc(nome, sizeof(char)*(strlen(nome)+1));
	cognome = realloc(cognome, sizeof(char)*(strlen(cognome)+1));
	o->nome = malloc(sizeof(char)*(strlen(nome)+1));
	strcpy(o->nome, nome);
	free(nome);
	o->cognome = malloc(sizeof(char)*(strlen(cognome)+1));
	strcpy(o->cognome, cognome);
	free(cognome);
	return (void *)o;
}

int explode(char ***arr_ptr, char *str, char *delimiter) {
	char *src = str, *end, *dst;
	char **arr;
	int size = 1, i;
	// Find number of strings
	while ((end = strstr(src, delimiter)) != NULL) {
		size = size +1;
		src = end + 1;
	}
	arr = malloc(size * sizeof(char *) + (strlen(str) + 1) * sizeof(char));
	src = str;
	dst = (char *) arr + size * sizeof(char *);
	for(i = 0; i < size; i++) {
		if((end = strstr(src, delimiter)) == NULL)
			end = src + strlen(src);
		arr[i] = dst;
		strncpy(dst, src, end - src);
		dst[end - src] = '\0';
		dst += end - src + 1;
		src = end + 1;
	}
	*arr_ptr = arr;
	return size;
}

void trim(char *s) {
	int i;
	int begin = 0;
	int end = strlen(s) - 1;
	while(isspace(s[begin]))
		begin++;
	while( end >= begin && isspace(s[end]))
		end--;
	for(i=begin;i <= end;i++)
		s[i-begin] = s[i];
	s[i-begin] = '\0';
}

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
int is_string(char *s) {
	if(s && *s) {
		if(strlen(s) > 0)
			return 1;
	}
	return 0;
}
int is_int(char *s) {
	if(s && *s) {
		if(strlen(s) > 0)
			return char_is_number(s, strlen(s), 0);
	}
	return 0;
}
int is_double(char *s) {
	if(s && *s) {
		if(strlen(s) > 0)
			return char_is_number(s, strlen(s), 1);
	}
	return 0;
}
int is_persona(char *s) {
	int i, z, nome, cognome;
	if(s && *s) {
		trim(s);
		if(strlen(s) > 4) {
			if(s[0] == '<' && s[strlen(s)-1] == '>') {
				i = 1;
				nome = 0;
				cognome = 0;
				z = 0;
				while(i < (strlen(s)-1)) {
					if(s[i] == ' ' || s[i] == '\t' || s[i] == '\n') {
						i++;
						continue;
					}
					if(s[i] == ':') {
						if(nome == 0) {
							nome = z;
							z = -1;
						}
					}
					z++;
					i++;
				}
				cognome = z;
				if(nome > 0 && cognome > 0)
					return 1;
			}
		}
	}
	return 0;
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

