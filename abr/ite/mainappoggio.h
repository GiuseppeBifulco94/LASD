/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
/* FUNZIONI PER MAIN */
struct lista *scegli_albero(struct lista *lista, int *current_ABR, int *num_ABR);
void mostra_menu(int current_ABR, int num_ABR);
int scelta_utente();
void stampa_albero(struct abr *albero, struct func funz);
void termina(LISTA *lista, struct func funz);
struct abr *inserisci_elem(struct abr *albero, struct func funz);
struct abr *inserisci_casuali(struct abr *albero, struct func funz);
struct abr *cancella_elem(struct abr *albero, struct func funz);
struct abr *esegui_es5(struct abr *albero, struct func funz);
void array_ordinato(struct abr *albero, struct func funz);
void verifica_abr_uguali(struct abr *albero, struct lista *lista, int current_ABR, int num_ABR, struct func funz);
struct lista *duplicazione(struct lista *lista, int *num_ABR, struct abr *albero, struct func funz);
struct abr *bilanciamento(struct abr *albero, struct func funz);

enum {
CASE_TERMINA = 0,
CASE_STAMPA = 1,
CASE_INSERISCI_ELEMENTO = 2,
CASE_INSERISCI_CASUALI = 3,
CASE_CANCELLA_ELEMENTO = 4,
CASE_CANCELLA_PARI_DISPARI_TRA_A_E_B = 5,
CASE_DISTRUGGI = 6,
CASE_VERIFICA_ABR_UGUALI = 7,
CASE_DUPLICAZIONE = 8,
CASE_ARRAY_ORDINATO_DA_ABR = 9,
CASE_PERFETTAMENTE_BILANCIATO = 10,
CASE_CAMBIA_ALBERO = 11
};

