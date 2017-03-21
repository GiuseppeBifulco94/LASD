/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
struct lista {
	int id;
	struct abr *content;
	struct lista *next;
	int tipo;
};

typedef struct lista LISTA;

LISTA *estrai_lista(LISTA *albero, int ids);
LISTA *aggiungi_lista(LISTA *albero, int id, int tipo);
