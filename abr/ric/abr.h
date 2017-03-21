/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
#include <stdbool.h>
struct abr {
	void *val;
	struct abr *sx;
	struct abr *dx;
};
typedef struct abr ABR;

struct func {
	void (*print)(void*);
	void* (*getInput)(void);
	void* (*alloca_valore)(void*,void*);
	void (*libera)(void*);
	int (*cmp)(void*,void*);
	int (*condizionees5)(void*);
	void* (*oggetto_random)(void);
	int tipo;
};
typedef struct func FUNC;

static int const PREORDER = 0;
static int const INORDER = 1;
static int const POSTORDER = 2;

void setLib(FUNC *funz, int type);

void stampaABR(ABR *testa, int mode, FUNC funz);
void preorder(ABR *testa, FUNC funz);
void inorder(ABR *testa, FUNC funz);
void postorder(ABR *testa, FUNC funz);
int nnodi(ABR *testa);
ABR *inserisci(ABR *testa, void *valore, FUNC funz);
ABR *distruggi(ABR *testa, FUNC funz);
ABR *cancella(ABR *testa, FUNC funz);
ABR *staccamin(ABR *nodo, ABR *padre);
ABR *cancella_valore(ABR *testa, void *elem, FUNC funz);
ABR *es5(ABR *testa, int flag, void *a, void *b, FUNC funz);
ABR *insrand(ABR *testa, int n, FUNC funz);
ABR *duplica(ABR *testa, FUNC funz);
bool ABR_uguali(ABR *a, ABR *b, FUNC funz);
void stampa_array(void **array, int length, FUNC funz);
void riempi_array_da_ABR(ABR *testa, void **array, FUNC funz);
int riempi_array_da_ABRmain(ABR *testa, void ** array, int length, FUNC funz);
ABR *bilancia_da_arraymain(void **array, int inizio, int fine, FUNC funz);
ABR *bilancia(ABR *testa, FUNC funz);

void dealloca_nodo(ABR *nodo);
ABR *alloca_nodo(ABR *nodo);
void *alloca_stringa(void *nodo, void *elem);
void *alloca_intero(void *nodo, void *elem);
void *alloca_double(void *nodo, void *elem);
void *alloca_persona(void *nodo, void *elem);
void dealloca_persona(void *s);
void *random_struct(void);
void dealloca_array(void **array, int length, FUNC funz);