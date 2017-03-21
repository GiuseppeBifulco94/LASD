struct coda {
	void * content;
	struct coda * next;
};
struct coda *accoda_coda(struct coda *st, void *el);
struct coda *decoda_coda(struct coda *st);
void *testa_coda(struct coda *st);
struct coda *svuota_coda(struct coda *st);