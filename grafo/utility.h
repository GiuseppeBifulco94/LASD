int getnum(int start, int end, char *text);
double getdouble(double start, double end, char *text);
char *getalfa(int start, int end, char *text);
void stampapersona(void *obj);
void stampastringa(void *obj);
void stampaintero(void *obj);
void stampafloat(void *obj);
void stampadouble(void *obj);
void* getinputpersona();
void* getinputstringa();
void* getinputintero();
void* getinputdouble();
void* getinputfloat();
int cmpstr(void *a, void *b);
int cmpint(void *a, void *b);
int cmpdouble(void *a, void *b);
int cmppers(void *a, void *b);
int minore(int a, int b);
int maggiore(int a, int b);

void* allocaintero(void *obj);
void* allocadouble(void *obj);
void* allocafloat(void *obj);
void* allocapersona(void *obj);
void* allocastringa(void *obj);

void CopiaDato(void *dest,int dpos, void *src,int spos, int dim);

void dealloca_persona(void *s);
void dealloca_oggetto(void *s);
void fprintfstringa(void *fd,void *obj);
void fprintfintero(void *fd,void *obj);
void fprintfdouble(void *fd,void *obj);
void fprintfpersona(void *fd,void *obj);
void *string_to_int(char *s);
void *string_to_string(char *s);
void *string_to_double(char *s);
void *string_to_persona(char *s);

int explode(char ***arr_ptr, char *str, char *delimiter);
void trim(char *s);
int length_int(int i);
int char_is_number(char *s, int length, int withdot);
int is_string(char *s);
int is_int(char *s);
int is_double(char *s);
int is_persona(char *s);