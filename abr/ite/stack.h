struct stack {
	void * content;
	struct stack * next;
};
struct stack *pushst(struct stack *st, void *el);
struct stack *popst(struct stack *st);
void *topst(struct stack *st);
struct stack *svuotastackst(struct stack *st);