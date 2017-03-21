#include <stdlib.h>
#include "curses_and_texts.h" /* include gia' <curses>, struct_def.h e <time> */
#include "game.h"
#include "pac.h"
#include "enemy.h"

int main() {
	FINESTRE *area; /* Puntatore alla struttura che contiene le finestre dello schermo */
	int glines, gcols;
	srand(time(NULL)); /* Mescoliamo i random.. */
	glines = 21; /* Righe */
	gcols = 37; /* Colonne */
	/* Perche' i valori sono dispari? L'approccio scelto per la generaz. tramite 'dfs' dei labirinti
	richiede che i valori siano dispari affinché, partendo da una coppia di coordinate randomiche e 
	dispari, si possa generare un labirinto che sia murato lungo i bordi */
	if (init_curses()) { /* Verifica che l'inizializzazione di curses sia andata a buon fine */
		area = (FINESTRE *) malloc(sizeof(FINESTRE));
		show_intro(glines, gcols); /* Mostra l'intro prima di accedere al gioco vero e proprio */
		crea_area_di_gioco(area, glines, gcols); /* Crea le finestre */
		if (area->success) {  /* Se tutte le aree sono state create con successo, allora inizia il gioco */

			game_engine(area, glines, gcols); /* Avvia il cuore del gioco */

			/* Elimina le finestre e libera la memoria */
			delwin(area->info);
			delwin(area->main);
			delwin(area->desc);
			free(area);
		}
		close_curses(); /* Termina curses */
	}
	return 0;
}
