

struct persona {
	char *nome;
	char *cognome;
};


int length_int(int i);
int getnum(int start, int end, char *text);
double getdouble(double start, double end, char *text);
char *getalfa(int start, int end, char *text);
void dealloca_oggetto(void *s);
void dealloca_persona(void *s);
void CopiaDato(void *dest,int dpos, void *src,int spos, int dim);
void stampapersona(void *obj);
void stampastringa(void *obj);
void stampaintero(void *obj);
void stampadouble(void *obj);
void* getinputpersona();
void* getinputstringa();
void* getinputintero();
void* getinputdouble();
int cmpstr(void *a, void *b);
int cmpint(void *a, void *b);
int cmpdouble(void *a, void *b);
int cmppers(void *a, void *b);
int minore(int a, int b);
int maggiore(int a, int b);
