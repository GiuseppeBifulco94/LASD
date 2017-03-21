#ifndef GAME_DEFINED
#include "struct_def.h"
#endif

void init_game(GAMESTATUS *game); /* Inizializza il gioco settando i valori di default della struct gamestatus */
int game_engine(FINESTRE *area, int glines, int gcols); /* Cuore del gioco, inizia turno, mostra turno, computa prossimo.. */
int deltatime(GAMESTATUS *game); /* Scandisce il timing */
void reset_delta(GAMESTATUS *game); /* Ripristina il delta time del turno a 0 */
void stop_game_key(GAMESTATUS *game); /* Rimuove il tasto premuto dalla memoria */
void start_new_turn(GAMESTATUS *game, CHARACTER *pac); /* Inizia un turno */
void compute_next_state(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac); /* Computa il prossimo stato */
void show_new_state(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac); /* Mostra il nuovo stato (labirinto, guardie, oggetti, main..) */
void reset_game(GAMESTATUS *game);
void level_up(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
int load_from_file_menu(FINESTRE *area, WINDOW *where, GAMESTATUS *game, CHARACTER *pac);
void increase_score_by_time(FINESTRE *area, GAMESTATUS *game);
void init_from_maze(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
void search_and_set_entrance_exit_enemies(GAMESTATUS *game, CHARACTER *pac);
void read_from_keyboard(FINESTRE *area, GAMESTATUS *game); /* Aggiorna il game->key in base al tasto premuto (se e' valido) */
int is_valid_key(int key); /* Controlla se un tasto e' associato ad una funzione nel gioco */
int is_once_key(int key); /* Controlla se un tasto e' associato ad una funzione da effettuare una volta nel gioco (come sparare) */
void init_maze(GAMESTATUS *game, int glines, int gcols);  /* Inizializza il labirinto */
void show_maze(WINDOW *where, struct labyrinth *lab, int remaining_items); /* Mostra nel box di gioco il labirinto */
void random_maze2(GAMESTATUS *game); /* Genera un labirinto (quindi ingresso, uscita, oggetti, guardie..) randomico */
void make_recursive_maze(struct labyrinth *lab, int x_ing, int y_ing); /* Genera un maze randomico (quindi mura e corridoi) */
/* Rompe howmany mura a partire da (x,y) in direzione dir */
void break_wall(char **maze, int x, int y, int dir, int howmany, int max_x, int max_y);
/* Funzione di appoggio che ritorna la coordinata (solo x o solo y) in direzione dir a partire da (x,y) */
int gotointhemaze(int x, int y, int dir, int howmany, int max);
void destroy_random_wall(struct labyrinth *lab, int howmany); /* Rompe howmany mura randomiche */
void add_random_door(GAMESTATUS *game); /* Aggiunge una porta in posizione randomica al labirinto */
void get_random_exit(struct labyrinth *lab, int *x, int *y); /* Genera un'uscita pseudo-casuale */
void add_random_object(GAMESTATUS *game); /* Aggiunge un oggetto pseudo-casuale in una posizione pseudo-casuale */
void add_random_bonus(GAMESTATUS *game); /* Aggiunge un oggetto bonus casuale in una posizione pseudo-casuale */
void add_object(GAMESTATUS *game, int item); /* Aggiunge l'oggetto specificato da ITEM */
void riempi_tutto(struct labyrinth *lab, int ch); /* Rimepie tutto il labirinto di 'ch' */
int through_door(GAMESTATUS *game, int x, int y); /* Controlla se puo passare attraverso una porta */
int rotate_door(GAMESTATUS *game, int x, int y); /* Cerca di ruotare la porta */
void rotate_hor_door(char **maze, int a, int b, int c, int d, int e, int f); /* Ruota la porta orizzontale */
void rotate_ver_door(char **maze, int a, int b, int c, int d, int e, int f); /* Ruota la porta verticale */
void remove_door(GAMESTATUS *game, int x, int y); /* Rimuove la porta */

int iswall(char c); /* Ritorna se e' un muro (| o -) */
int exists_and_ischar(GAMESTATUS *game, int x,  int y, char ch); /* Controlla se (x,y) e' compresa nel maze e se e' uguale a 'ch' */
int exists_and_iswall(GAMESTATUS *game, int x,  int y); /* Controlla se (x,y) e' compresa nel maze e se e' un muro */
