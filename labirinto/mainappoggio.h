void print_menu();
int getpeso_maze(struct labyrinth *graph, int x, int y);
void stampa_tutti_adiacenti(struct labyrinth *mygraph);
void aggiungi_vertice(struct labyrinth *mygraph);
void rimuovi_arco(struct labyrinth *mygraph);
void rimuovi_vertice(struct labyrinth *mygraph);
void aggiungi_arco(struct labyrinth *mygraph);
void carica_da_file(struct labyrinth *mygraph);
void colora_con_bfs(struct labyrinth *mygraph);
void stampa_percorso_minimo_bfs(struct labyrinth *mygraph);
void stampa_la_dfs(struct labyrinth *mygraph);
void colora_con_dijkstra(struct labyrinth *mygraph);
void colora_con_astar(struct labyrinth *mygraph);