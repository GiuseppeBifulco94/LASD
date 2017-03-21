/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
struct persona {
	char *nome;
	char *cognome;
};
int stringaparidispari(void *obj);
int intparidispari(void *obj);
int doubleparidispari(void *obj);
int personaparidispari(void *obj);
int stringaparidispari(void *obj);
void stampastringa(void *obj);
void stampadouble(void *obj);
void stampaintero(void *obj);
void stampapersona(void *obj);
void* getinputstringa();
void* getinputint();
void* getinputdouble();
void* getinputpersona();
int cmpstr(void *a, void *b);
int cmpint(void *a, void *b);
int cmpdouble(void *a, void *b);
int cmppers(void *a, void *b);
void **alloca_stringarray(void **array, int length);
void **alloca_intarray(void **array, int length);
void **alloca_doublearray(void **array, int length);
void *getvoidint(int a);

enum { //Lo si può tranquillamente cancellare
	T_STRING = 0,
	T_INT = 1,
	T_FLOAT = 2,
	T_STRUCT = 3
};
