#ifndef GAME_DEFINED
#include "struct_def.h"
#endif

void crea_area_di_gioco(FINESTRE *area, int glines, int gcols); /* Crea l'aria di gioco */
WINDOW *create_boxed_newwin(int height, int width, int starty, int startx); /* Crea una finestra contornata */
void init_color_curses(); /* Inizializza tutte le coppie di colori del gioco */
void close_curses(); /* Termina curses */
int init_curses(); /* Inizializza curses */
void print_w_effect(WINDOW *where, char *stringa); /* Scrive effetto macchina da scrivere nell'intro */
void show_intro(int glines, int gcols); /* Mostra l'intro */
void init_game_text(FINESTRE *area, GAMESTATUS *game); /* Mostra i testi di base del testo (crediti, istruzioni, ecc.) */
char *string_time_elapsed(GAMESTATUS *game); /* Ritorna la stringa formattata del tempo di gioco trascorso */
void show_time_elapsed(WINDOW *where, GAMESTATUS *game); /* Mostra il tempo di gioco trascorso nel box di informazioni */
char *string_score(GAMESTATUS *game); /* Ritorna la stringa contenente il punteggio */
void show_score(WINDOW *where, GAMESTATUS *game); /* Mostra il punteggio nel box di informazioni */
void update_level(WINDOW *where, GAMESTATUS *game); /* Mostra il livello nel box di informazioni */
void show_author(WINDOW *where, int x); /* Mostra i crediti nel box di informazioni */
void write_menu(WINDOW *where); /* Scrive le voci del menu' */
int jump_to_level_menu(FINESTRE *area, WINDOW *where, GAMESTATUS *game, CHARACTER *pac); /* Chiede a che livello andare */
void show_menu(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac); /* Mostra il menu' SUL box di gioco */
void show_inventory(WINDOW *where, GAMESTATUS *game); /* Mostra l'inventario (num. coltelli e proiettili) nel box di informazioni */
void write_event_important(WINDOW *where, GAMESTATUS *game, char *stringa); /* Scrive la stringa (lampeggiante) nel box di descrizione */
void write_event(WINDOW *where, GAMESTATUS *game, char *stringa); /* Scrive la stringa nel box di descrizione*/
void aggiorna_timing_in_pausa(FINESTRE *area, GAMESTATUS *game); /* Aggiorna il tempo trascorso in pausa */
void jump_to_level(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac, int level); /* FUnzione di appoggio per saltare ad un livello */
void you_lose(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac); /* Mostra ascii art con scritto 'Lose' */
void bye_bye(WINDOW *where, GAMESTATUS *game); /* Stampa a video un saluto e imposta il game over */