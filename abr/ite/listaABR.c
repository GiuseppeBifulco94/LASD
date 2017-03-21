/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdlib.h>
#include "listaABR.h"
#include "abr.h"

LISTA *estrai_lista(LISTA *albero, int ids) {
	if(albero != NULL) {
		if(albero->id == ids) {
			return albero;
		} else {
			return estrai_lista(albero->next, ids);
		}
	} else {
		return NULL;
	}
}
LISTA *aggiungi_lista(LISTA *albero, int id, int tipo) {
	LISTA *nuovonodo = (LISTA *) malloc(sizeof(LISTA));
	nuovonodo->id = id;
	nuovonodo->next = albero;
	//nuovonodo->content = (struct abr *) malloc(sizeof(ABR));
	nuovonodo->content = NULL;
	nuovonodo->tipo = tipo;
	return nuovonodo;
}
