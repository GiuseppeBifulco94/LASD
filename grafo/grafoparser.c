#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafoparser.h"
#include "func.h"
#include "grafo.h"
#include "utility.h"

void write_grafo(void *graph, char *percorso, struct func funz) {
	void **mappa, **array;
	int i, j, numadj;
	float peso;
	FILE *fd;
	fd = fopen(percorso, "w+");
	if(!fd) {
		printf("Errore: Impossibile aprire il file %s\n", percorso);
	} else {
		fprintf(fd, "(%d) (%d) { ", funz.tipo_el, ((struct grafo *)graph)->nnodi);
		mappa = funz.getMappa(((struct grafo *)graph));
		for(i=0;i<((struct grafo *)graph)->nnodi;i++) {
			funz.fprintf(fd, mappa[i]);
			array = funz.getAdj(((struct grafo *)graph)->adiacenze, mappa[i], &numadj, ((struct grafo *)graph)->nnodi, funz);
			if(numadj > 0)
				fprintf(fd, " -> ");
			for(j=0;j<numadj;j++) {
				peso = funz.getPeso(((struct grafo *)graph),i,getnumfromvertex(mappa, array[j], ((struct grafo *)graph)->nnodi, funz),funz);
				if(peso > .0 || peso < .0)
					fprintf(fd, "(%f)", peso);
				else
					fprintf(fd, "(NULL)");
				funz.fprintf(fd, array[j]);
				if(j!=numadj-1)
					fprintf(fd, ", ");
			}
			fprintf(fd, "; ");
			if(numadj > 0)
				free(array);
		}
		fprintf(fd, " } \n");
		printf("Salvataggio del grafo completato con successo.\n");
		fclose(fd);
		if(mappa)
			free(mappa);
	}
}

void read_grafo(void *graph, char *percorso, int impl_grafo, struct func *funz, int *error) {
	int tipo_el, nnodi, err;
	FILE *fd;
	err = 0;
	*error = 0;
	fd = fopen(percorso, "r");
	if(fd) {
		printf("Apertura del file completata con successo.\n");
		funz->getMappa = NULL;
		if(crea_il_grafo(graph, fd, &tipo_el, &nnodi) == 1) {
			printf("Recuperato tipo e numero elementi del grafo.\n");
			funz->tipo_el = tipo_el;
			((struct grafo *)graph)->nnodi = 0;
			setlib(funz, impl_grafo, funz->tipo_el);
			funz->initAdj(((struct grafo*)graph),0);
			costruisci_vertici(graph, fd, *funz, &err);
			if(err == 1) {
				err = 3;
			} else {
				err = 0;
				if(((struct grafo *)graph)->nnodi != nnodi) {
					err = 2;
				} else {
					costruisci_archi(graph, fd, *funz, &err);
					if(err == 1) {
						err = 4;
					} else {
						fclose(fd);
						return;
					}
				}
			}
		} else {
			err = 1;
		}
	} else {
		printf("Errore: Impossibile accedere al file %s\n(Verificare che il percorso esiste e di avere i permessi necessari per aprirlo)\n", percorso);
		*error = 1;
		return;
	}
	printf("Errore (%d): Il file %s non aderisce alla grammatica del grafo.\n", err, percorso);
	distruggi_grafo(graph, *funz);
	graph = alloca_grafo(graph);
	*error = 1;
}

int crea_il_grafo(void *graph, FILE *file, int *tipo_el, int *nnodi) {
	int controllo, off;
	char c;
	rewind(file);
	*tipo_el = num_da_parentesi(file,&controllo);
	if(controllo == 1) {
		*nnodi = num_da_parentesi(file,&controllo);
		if(controllo == 1) {
			if(match_reverse(file, '}') == 1 && match(file, '{', &off) == 1) {
				fseek(file, off+1, SEEK_CUR); //mi posiziono dopo la prima graffa
				return 1;
			}
		}
	}
	return 0;
}

int num_da_parentesi(FILE *file, int *ok) {
	int off, length, ret;
	char *stringa;
	off = 0;
	if(match(file, '(', &off) == 1) {
		off = 0;
		length = 0;
		stringa = leggi_compreso(file, 1, ')', &length, &off); //1 = salta la prima cifra significativa
		if(off > 0) {
			if(length > 0 && char_is_number(stringa, length, 0) == 1) {
				fseek(file,off,SEEK_CUR);
				sscanf(stringa, "%d", &ret);
				free(stringa);
				*ok = 1;
				return ret;
			}
		}
	}
	*ok = 0;
	return -1;
}

char *leggi_compreso(FILE *file, int da, char a, int *length, int *off) {
	char c;
	char *buffer;
	char *ret;
	int i,r,k;
	i = 0;
	r = 10;
	*off = 0;
	k = -1;
	buffer = malloc(sizeof(char)*r);
	while((c = fgetc(file)) == '\t' || c == '\n' || c == ' ') {
		*off = *off + 1;
	}
	ungetc(c,file);
	*off = *off - 1;
	while((c=fgetc(file)) != a && c != EOF) {
		if(i>r-2) {
			r = i*2;
			buffer = realloc(buffer, sizeof(char)*r);
		}
		k++;
		if(k >= da) {
		buffer[i] = c;
		i++;
		}
		*off = *off + 1;
	}
	if(c == EOF) {
		*length = -1;
		*off = 0;
		return (char *) NULL;
	}
	ret = malloc(sizeof(char)*(i+1));
	r = 0;
	while(r < i) {
		ret[r] = buffer[r];
		r++;
	}
	ret[r] = '\0';
	free(buffer);
	*length = r;
	return ret;
}
int match(FILE *file, char s, int *off) {
	char c;
	int i;
	i = 0;
	while((c = fgetc(file)) == '\t' || c == '\n' || c == ' ') {
		i++;
	}
	*off = i;
	ungetc(c,file);
	if(s == c) {
		return 1;
	} else {
		*off = 0;
		return 0;
	}
}

int match_reverse(FILE *file, char s) {
	char c;
	int prima, i, trovato;
	trovato = 0;
	prima = ftell(file);
	fseek(file,0,SEEK_END);
	i = -1;
	while(ftell(file) > 1) {
	fseek(file,i,SEEK_CUR);
	c = fgetc(file);
	ungetc(c, file);
	if(c != ' ' && c != '\t' && c != '\n' && c != s) {
		trovato = 0;
		break; //il carattere trovato non coincide
	} else if(c == s) {
		trovato = 1;
		break;
	}
	i--;
	}
	fseek(file, prima, SEEK_SET);
	return trovato;
}

void costruisci_vertici(void *graph, FILE *file, struct func funz, int *err) {
	int size1, size2, i, j, length, prima, test;
	char *tutto, **arr1, **arr2;
	void *nuovonodo;
	tutto = NULL;
	arr1 = NULL;
	prima = ftell(file);
	tutto = leggi_compreso(file,0,'}', &length, &i);
	*err = 0;
	if(length < 1) {
		return;
	}
	size1 = explode(&arr1, tutto, ";");
	for(i=0;i<size1;i++) {
		arr2 = NULL;
		size2 = explode(&arr2, arr1[i], "->");
		if(size2 > 0) {
			trim(arr2[0]);
			if(strlen(arr2[0]) > 0) {
				if(funz.checkType(arr2[0]) == 0) {
					*err = 1;
					break;
				}
				nuovonodo = NULL;
				nuovonodo = funz.stringToType(arr2[0]);
				test = funz.addVertex(((struct grafo *)graph), nuovonodo, funz);
				if(test == 0) {
					*err = 1;
					break;
				}
			}
		}
		if(arr2)
			free(arr2);
	}
	if(arr1)
		free(arr1);
	if(tutto)
		free(tutto);
	//ok
	fseek(file, prima, SEEK_SET);
}

void costruisci_archi(void *graph, FILE *file, struct func funz, int *err) {
	int size1, size2, size3, i, j, k, z, length, test;
	char *tutto, **arr1, **arr2, **arr3, *peso;
	void *nodo1, *nodo2, *nodotmp;
	float fl;
	tutto = NULL;
	arr1 = NULL;
	*err = 0;
	tutto = leggi_compreso(file,0,'}', &length, &i);
	if(length < 2) {
		return;
	}
	size1 = explode(&arr1, tutto, ";");
	for(i=0;i<size1;i++) {
		arr2 = NULL;
		size2 = explode(&arr2, arr1[i], "->");
		if(size2 > 1) { //almeno 1 adiacente
			trim(arr2[0]);
			if(strlen(arr2[0]) > 0) {
				nodo1 = NULL;
			//	nodo1 = funz.stringToType(arr2[0]); invece di allocarlo qui, lo alloco direttamente quando lo passo alla funzione addEdge
				arr3 = NULL;
				size3 = explode(&arr3, arr2[1], ",");
				if(size3 > 0) {
					for(k=0;k<size3;k++) {
						trim(arr3[k]);
						if(strlen(arr3[k]) > 0) {
							nodo2 = NULL;
							peso = strchr(arr3[k],'(');
							arr3[k] = strchr(arr3[k],')'); //fix per peso NULL or void*
							if(arr3[k] != NULL) {
								peso = peso + 1;
								z = 0;
								while(z<strlen(peso)) {
									if(peso[z] == ')') {
										peso[z] = '\0';
										break;
									}
									z++;
								}
								trim(peso);
								if(strcmp(peso,"NULL") == 0) {
									fl = .0;
								} else {
									if(char_is_number(peso, strlen(peso), 1) == 0) {
										fl = .0; //errore peso invalido
										*err = 1;
									} else {
										fl = (float) atof(peso);
									}
								}
								arr3[k] = arr3[k]+1;
								nodo2 = funz.stringToType(arr3[k]);
								test = 0;
								if(nodo2 && fl) {
									nodotmp = funz.stringToType(arr2[0]);
									test = funz.addEdge(((struct grafo *)graph), nodotmp, nodo2, fl, funz);
								}
								if(test == 0)
									*err = 1;
							}
						}
						if(*err == 1)
							goto exit;
					}
				}
				if(arr3)
					free(arr3);
			}
		}
		if(arr2)
			free(arr2);
	}
	if(arr1)
		free(arr1);
	if(tutto)
		free(tutto);
	//ok
return;

	exit:
		if(arr3)
			free(arr3);
		if(arr2)
			free(arr2);
		if(arr1)
			free(arr1);
		if(tutto)
			free(tutto);
		//ok
		//e' inutile eseguire tutto se c'è un *err=1 quindi anticipo la deallocazione e l'uscita 
		return;
}
//avanza di q: fseek(file,q,SEEK_CUR);
