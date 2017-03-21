struct tree {
	struct coppia *val;
	struct tree * sx;
	struct tree * dx;
};

void preorder(struct tree *testa, struct heap *funz);
struct tree *alloca_nodo(struct tree *nodo);
struct tree *instree(struct tree *testa, int indice, struct coppia *el);