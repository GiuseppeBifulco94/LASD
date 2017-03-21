/* MIRKO ALICASTRO N86/1437 */
/* LIBRERIA ABR GENERICI CON (VOID*) */
int getnum(int start, int end, char *text);
double getdouble(double start, double end, char *text);
char *getalfa(int start, int end, char *text);
void *random_int(void);
void *random_double(void);
void *random_string(void);
void dealloca_oggetto(void *s);
int random_num(int a, int b);
void CopiaDato(void *dest,int dpos, void *src,int spos, int dim);
int is_double(char *s);
int length_int(int i);
int char_is_number(char *s, int length, int withdot);