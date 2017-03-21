#include <time.h>
#include <curses.h>

struct finestre {
	WINDOW *main;
	WINDOW *info;
	WINDOW *desc;
	int success;
};
typedef struct finestre FINESTRE;

struct gamestatus {
	long started; /* Contiene il long del time() di quando si inizia a giocare */
	long lasttimesaved; /* Contiene il long dell'ultimo time() salvato per evitare imbrogli di tempo */
	long timepaused; /* Contiene il tempo di gioco 'in pausa' per poter contare il tempo di gioco effettivo */
	long lasttimepaused; /* Contiene il long dell'ultimo time() mentre si stava in pausa per poter scandire il tempo */
	int score; /* Punteggio */
	int over; /* Vale 1 se il gioco è finito, 0 altrimenti */
	int level; /* Livello di gioco */
	clock_t delay; /* Delay tra i round */
	clock_t lasttime; /*  Numero di clock dall'ultimo controllo */
	float delta; /* Tempo passato dall'ultimo round */
	struct labyrinth *lab; /* Struttura del labirinto con la matrice di char */
	int x_uscita; /* Coordinata della riga dell'uscita */
	int y_uscita; /* Coordinata della colonna dell'uscita */
	int x_ingresso; /* Coordinata della riga di partenza del main character */
	int y_ingresso; /* Coordinata della colonna di partenza del main character */
	int remaining_items; /* Numero di oggetti rimanenti da raccogliere */
	int total_items; /* Numero totale di oggetti da prelevare assegnati al livello */
	int key; /* Tasto premuto corrente */
	int lastkey; /* Tasto precedente premuto */
	struct heap *enemy; /* Ho deciso di implementare 'il raccoglitore' di nemici del livello attuale come un MIN HEAP */
	int knife; /* Numero di coltelli collezionati */
	int bullet; /* Numero di proiettili collezionati */
	struct heap *bullets; /* Proiettile correntemente sparato */
};
typedef struct gamestatus GAMESTATUS;

struct characterstatus {
	char character; /* Rappresentazione grafica (tramite carattere) del personaggio */
	short color_pair; /* Numero identificativo del colore di fondo/sfondo del carattere */
	int x_next; /* Prossima x */
	int y_next; /* Prossima y */
	int x_curr; /* X corrente */
	int y_curr; /* Y corrente */
	int duration; /* Durata del carattere (-1 per durata infinita) */
	int velocita; /* Ogni quanti turni si deve muovere:
					 0 per non muoversi mai,
					 1 per muoversi ad ogni turno,
					 'i' per muoversi ad ogni 'i' turno ) */
	int velocita_tmp; /* Serve per gestire la velocita':
						 ad ogni turno diminuisce di 1,
						 fino a valere 1,
						 quando vale 1 si muove e ritorna a valere 'velocita'  */
	int lastdir; /* Ultima direzione del personaggio */
};
typedef struct characterstatus CHARACTER;

/* (ogni guardia puo' avere così il suo algoritmo di rincorsa) */
struct enemystatus {
	CHARACTER *character;
	void (*move_enemy)(GAMESTATUS *, struct enemystatus *, CHARACTER *pac); /* Puntatore all'algoritmo di inseguimento */
	int id_enemy;
	int visibilita;
};
typedef struct enemystatus ENEMY;

#define GAME_DEFINED 1 /* Controlla se sono state definite le costanti e le strutture */

#define BOX_VERTICAL 0 /* Carattere default per il box */
#define BOX_HORIZONTAL 0 /* Carattere default per il box */

#define BOX_COLS 35 /* Dimensione fissata per la larghezza del box di informazione */

/* Indica l'intervallo di centesimi di secondi tra un turno e l'altro */
#define DELAY 7

/* Indica la velocità di default del main character (ogni quanti turni si muove) */
#define PAC_VELOCITA 3

/* Indica il punteggio da sottrarre quando una guardia ti raggiunge.
Se non hai abbastaza punti da farti sottrarre allora hai perso */
#define PENALITY -100

/* Definizione dei caratteri rappresentanti la struttura del labirinto */
#define WALLH '-' /* Carattere per il muro - orizzontale */
#define WALLV '|' /* Carattere per il muro - verticale */
#define DOOR '#' /* Carattere per la porta girevole */
#define CORRIDOR ' ' /* Carattere per il corridoio */
#define EXIT 'D' /* Carattere per l'uscita */

/* Definizione dei caratteri rappresentanti gli oggetti da raccogliere */
#define ITEM_COIN 'C' /* Carattere per l'oggetto monetina */
#define ITEM_FAST 'F' /* Carattere per l'oggetto che aumenta di velocita' al main character */
#define ITEM_SLOW 'S' /* Carattere per l'oggetto che rallenta il main character */
#define ITEM_REVERSE_DIR 'R' /* Carattere per l'oggetto che inverte la direzione corrente del main character */

/* Definizione dei caratteri rappresentanti gli oggetti bonus (non necessariamente da raccogliere) */
#define BONUS_BULLET 'B'
#define BONUS_KNIFE 'K'
#define BONUS_BOMB 'O'

/* Definizione del carattere proiettile */
#define BULLET 'P'

/* Definizione dei caratteri rappresentanti le guardie */
#define POLICE '!' /* Carattere per la guardia 'poliziotto' */
#define SOLDIER '!' /* Carattere per la guardia 'soldato' */
#define SNIPER '!' /* Carattere per la guardia 'cecchino' */
#define ASTAR 'A' /* Carattere per la guardia A* */

/* Identificativi dei tasti */
#define ESC 27
#define SPACE 32
#define MENU 49
#define FIRE 'x'

