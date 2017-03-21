
struct heap {
	void *content;
	int heapsize;
	int real_allocated;
	void (*print)(void*);
	void* (*getInput)(void);
	int (*cmp)(void*,void*);
	struct coppia* (*get_el)(struct heap*,int);
	int (*checkminormax)(int,int);
	void (*libera)(void *);
	int tipo;
	int arrayortree;
	int minormax;
};

struct coppia {
	void *content;
	float priority;
};

void distruggi_heap(struct heap *ogg);

struct heap *alloca_heap(struct heap *ogg);

struct coppia *get_el_tree(struct heap *ogg, int indice);
struct tree *get_nodo(struct heap *ogg, int indice);
struct coppia *get_el_array(struct heap *ogg, int indice);

struct heap *init_heap(struct heap *ogg, int dimensione);
struct heap *init_heap_array(struct heap *ogg, int dimensione);
struct heap *init_heap_tree(struct heap *ogg, int dimensione);

struct heap *realloca_heap_array(struct heap *ogg);

void stampa_heap(struct heap *ogg);

void build_heap(struct heap *ogg);

void Heapify(struct heap *ogg, int i);
struct coppia *extractHeap(struct heap *ogg);
void DecreaseKey(struct heap *ogg, int indice, float priorita);
void IncreaseKey(struct heap *ogg, int indice, float priorita);
void DeleteHeap(struct heap *ogg, int indice);
struct coppia *gettop(struct heap *ogg);
void InsertKey(struct heap *ogg, void *el, float priorita);

void swap(struct coppia *a, struct coppia *b);
void printelcoda(struct coppia *el, struct heap *funz);

int heapgetnumfromel(struct heap *ogg, void *el);


void setHeap(struct heap *funz, int type, int arrayortree, int minormax);

void freecoppia(void *);