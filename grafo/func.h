#ifndef funcall
#define funcall ;
struct grafo;

struct func {
	int impl_grafo;
	int tipo_el;
	void* (*getInput)(void);
	void (*print)(void*);
	int (*cmp)(void*,void*);
	void (*libera)(void*);
	void* (*allocaval)(void*);

	void (*initAdj)(struct grafo*, int);
	void (*init)(struct grafo*, int, struct func);
	void** (*getMappa)(struct grafo*);
	void** (*getAdj)(void*,void*,int*,int,struct func);
	float (*getPeso)(struct grafo*,int,int,struct func);
	void (*printAllAdj)(struct grafo*, struct func);
	void (*printAllVertex)(struct grafo*, struct func);
	int (*addVertex)(struct grafo*,void*,struct func);
	int (*addEdge)(struct grafo*,void*,void*,float,struct func);
	void (*removeVertex)(struct grafo*,void*,struct func);
	void (*removeEdge)(struct grafo*,void*,void*,struct func);
	void (*trasposta)(struct grafo*,struct func);
	void (*convertiRappresentazione)(struct grafo*,struct func);
	void (*deallocaAdj)(struct grafo*,struct func);

	int (*checkType)(char*);
	void* (*stringToType)(char*);
	void (*fprintf)(void*,void*);
};

void setlib(struct func *funz, int impl_grafo, int tipo_el);
void switch_func(struct func *funz);

#endif