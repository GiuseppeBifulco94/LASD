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

void stampaABR_iterativo(ABR *testa, int mode, FUNC funz);
void preorder_iterativo(ABR *testa, FUNC funz);
void inorder_iterativo(ABR *testa, FUNC funz);
void postorder_iterativo(ABR *testa, FUNC funz);
int nnodi_iterativo(ABR *testa);
ABR *inserisci_iterativo(ABR *testa, void *valore, FUNC funz);
ABR *distruggi_iterativo(ABR *testa, FUNC funz);
ABR *cancella_iterativo(ABR *testa, FUNC funz);
ABR *staccamin_iterativo(ABR *nodo, ABR *padre);
ABR *cancella_valore_iterativo(ABR *testa, void *elem, FUNC funz);
ABR *es5_iterativo(ABR *testa, int flag, void *a, void *b, FUNC funz);
ABR *insrand_iterativo(ABR *testa, int n, FUNC funz);
ABR *duplica_iterativo(ABR *testa, FUNC funz);
bool ABR_uguali_iterativo(ABR *a, ABR *b, FUNC funz);
void stampa_array(void **array, int length, FUNC funz);
void riempi_array_da_ABR_iterativo(ABR *testa, void **array, FUNC funz);
ABR *bilancia_da_arraymain_iterativo(void **array, int num_nodi, FUNC funz);
ABR *bilancia_iterativo(ABR *testa, FUNC funz);

void dealloca_nodo(ABR *nodo);
ABR *alloca_nodo(ABR *nodo);
void *alloca_stringa(void *nodo, void *elem);
void *alloca_intero(void *nodo, void *elem);
void *alloca_double(void *nodo, void *elem);
void *alloca_persona(void *nodo, void *elem);
void dealloca_persona(void *s);
void *random_struct(void);
void dealloca_array(void **array, int length, FUNC funz);
