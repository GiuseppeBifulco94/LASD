#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "heap.h"
#include "game.h" /* Include gia' <curses>, struct_def.h e <time> */
#include "curses_and_texts.h"
#include "pac.h"
#include "enemy.h"

void reset_delta(GAMESTATUS *game) {
	game->delta = 0; /* Reset del delta dopo che un turno è stato eseguito */
}

int deltatime(GAMESTATUS *game) {
	/* La funzione permette di scandire i turni:
	ritorna 0 se il tempo passato dall'ultimo reset del delta e' minore del delay di gioco,
	ritorna 1 altrimenti.
	Va effettuato un reset del delta dopo che la funzione restituisce 1 */
	clock_t now;
	
	now = clock(); /* Numero di clock passati dall'inizio del programma */
	game->delta += now - game->lasttime; /* Aggiorna il numero di clock trascorsi dall'ultimo controllo */
	game->lasttime = now; /* Registra il corrente numero di clock */

	 /* Controlla se sono passati almeno 'game->delay' centesimi di secondi */
	if((100*game->delta)/CLOCKS_PER_SEC > game->delay) {
		return 1; /* Ritorna quindi 1 */
	}
	return 0; /* Altrimenti ritorna 0 */
}

void start_new_turn(GAMESTATUS *game, CHARACTER *pac) {
	int i;
	ENEMY *el;
	/* Setta a 0 l'intervallo di tempo passato tra il turno (appena eseguito) e il prossimo */
	reset_delta(game);

	i = 0;
	while(i<game->enemy->heapsize) { /* Scorro tutte le guardie */
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content); /* Per ogni guardia.. */

		if(el->character->duration != 0) {
			/* Aggiorna le coordinate della guardia corrente  */
			el->character->x_curr = el->character->x_next;
			el->character->y_curr = el->character->y_next;
		}
		i++; /* Procedi alla prossima guardia */
	}

	i = 0;
	while(i<game->bullets->heapsize) { /* Scorro tutti i proiettili */
		el = (ENEMY *) (((struct coppia *) (game->bullets->get_el(game->bullets, i)))->content); /* Per ogni proiettile.. */

		if(el->character->duration != 0) {
			/* Aggiorna le coordinate del proiettile corrente  */
			el->character->x_curr = el->character->x_next;
			el->character->y_curr = el->character->y_next;
		}
		i++; /* Procedi al prossimo proiettile */
	}

	/* Aggiorna le coordinate del main character */
	pac->x_curr = pac->x_next;
	pac->y_curr = pac->y_next;
}

void compute_next_state(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	/* Assegna punti bonus ogni 2 minuti */
	increase_score_by_time(area, game);

	/* Muove il main character nel gioco */
	make_pacmove(area, game, pac);
	
	/* Muove tutte le guardie attive nel gioco */
	make_enemiesmove(game, pac);

	/* Muove tutti i proiettili nel gioco */
	make_bulletsmove(area, game, pac);

	/* Controlla se ho raccolto oggetti */
	pac_on_items(area, game, pac);

	/* Controlla se ho raccolto tutti gli oggetti e se sono arrivato all'uscita */
	pac_on_exit(area, game, pac);

	/* Controlla se un proiettile colpisce una guardia */
	bullet_on_enemy(area, game, pac);

	/* Controlla se il main character collide su una guardia */
	pac_on_enemy(area, game, pac);
}

void show_new_state(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {

	/* Scrive il tempo di gioco aggiornato nel box di informazione */
	show_time_elapsed(area->info, game);

	/* Se ha imbrogliato, allora avvisalo di dover chiudere il gioco e riaprirlo */
	if(game->started == -1) 
		write_event(area->desc, game, "In the life cheating don't allow you go on. Close game, think about this and then play again.");

	/* Scrive il punteggio di gioco aggiornato nel box di informazione */
	show_score(area->info, game);

	/* Scrivere il livello di gioco aggiornato nel box di informazione */
	update_level(area->info, game);

	/* Stampa il labirinto aggiornato */
	show_maze(area->main, game->lab, game->remaining_items);

	/* Stampa il character */
	show_move(area->main, pac);

	/* Stampa le guardie */
	show_enemies(area->main, game, pac);

	/* Stampa i proiettili */
	show_bullets(area->main, game);

	/* Stampa pugnali e proiettili collezionati */
	show_inventory(area->info, game);

	/* Aggiorna a video il box di informazione */
	wrefresh(area->info);

	/* Aggiorna a video il box di gioco */
 	wrefresh(area->main);

}

void init_game(GAMESTATUS *game) {

	game->timepaused = 0; /* Tempo trascorso nella schermata di pausa (schermata di menu) */
	game->lasttimepaused = -1; /* Ultimo tempo di pausa salvato -1 perche' ancora non e' stato salvato nessun tempo */ 
	game->started = (long) time(NULL); /* Tempo attuale convertito in long */
	game->lasttimesaved = (long) -1; /* Ultimo tempo salvato -1 perche' ancora non e' stato salvato nessun tempo */
	game->score = 0; /* Punteggio 0 iniziale */
	game->over = 0; /* Il gioco non e' ancora finito */
	game->level = 1; /* Primo livello di gioco */
	game->delta = 0; /* Non e' passato ancora nessun istante tra il turno attuale e il precedente */
	game->lasttime = 0; /* Non e' mai stato effettuato nessun turno */
	game->delay = DELAY; /* Intervallo tra un turno e l'altro */
	/* Ancora non e' stato definito l'ingresso del labirinto */
	game->x_ingresso = -1;
	game->y_ingresso = -1;
	/* Ancora non e' stata definita l'uscita del labirinto */
	game->x_uscita = -1;
	game->y_uscita = -1;

	/* Ancora non e' stato premuto nessun tasto */
	stop_game_key(game);

	game->total_items = 0; /* Non sono ancora stati caricati gli oggetti da prelevare */
	game->remaining_items = 0; /* Non esistono oggetti rimanenti senza gli oggetti da prelevare */

	game->enemy = NULL; /* non c'e' ancora nessun nemico */
	game->bullets = NULL; /* non c'e' ancora nessun proiettile */

	game->knife = 0; /* Ancora e' stato raccolto nessun coltello */
	game->bullet = 0; /* Ancora e' stato raccolto nessun proiettile*/

}

int game_engine(FINESTRE *area, int glines, int gcols) {
	/* Il game engine dopo l'inizializzazione, iterativamente inizializza turno per turno,
	legge da tastiera, computa la mossa successiva, aggiorna a video il risultato */
	GAMESTATUS *game;
	CHARACTER *pac;
	if(!area) return 0;
	
	game = (GAMESTATUS *) malloc(sizeof(GAMESTATUS));
	pac = (CHARACTER *) malloc(sizeof(CHARACTER));
	init_maze(game, glines, gcols); /* alloca il labirinto */
	init_game(game); /* inizializza i valori dello stato del gioco */
	init_game_text(area, game); /* inizializza le scritte del gioco */
	random_maze2(game); /* Crea il labirinto in modo random */
	init_pac(pac, game->x_ingresso, game->y_ingresso); /* Inizializza il main character */

	while(!game->over) { /* Fin quando il gioco non e' terminato */
		if(deltatime(game)) { /* Se e' passato abbastanza tempo tra il turno precedente e quello attuale */
			start_new_turn(game, pac);
			compute_next_state(area, game, pac);
			show_new_state(area, game, pac);
    	}
	}

	/* Libera la struttura del gioco */
	if(game) {
		if(game->lab) distruggi_labyrinth(game->lab);
		if(game->enemy)	distruggi_heap(game->enemy);
		if(game->bullets) distruggi_heap(game->bullets);
		free(game);
	}
	if(pac)	free(pac);
	return 1;
}

int is_valid_key(int key) {
	if (key != KEY_LEFT && key != KEY_RIGHT && key != KEY_UP 
		&& key != KEY_DOWN  && key != ESC && key != SPACE &&
		key != FIRE && key != MENU)
		return 0;
	else
		return 1;
}

int is_once_key(int key) {
	if(key == FIRE)
		return 1;
	else
		return 0;
}

void read_from_keyboard(FINESTRE *area, GAMESTATUS *game) {
	int key;
	key = wgetch(area->main); /* Prende il tasto premuto */
	flushinp(); /* Svuota tutto l'input dal buffer della tastiera */
	
	/* Se il tasto premuto non corrisponde a nessun operazione
	nel gioco allora ritorna il tasto premuto precedente */
	if(!is_valid_key(key))
		game->key = game->lastkey;
	else {
		game->key = key;
		/* Se il tasto premuto e' un tasto da effettuare
		una sola volta (tipo il fuoco)
		allora mantieni il tasto precedente */
		if(!is_once_key(key))
			game->lastkey = key;
	}
}

void stop_game_key(GAMESTATUS *game) {
	game->key = -1; /* Non salvare nessun tasto premuto */
	game->lastkey = -1; /* Non salvare nessun tasto premuto come precedente */
}

void reset_game(GAMESTATUS *game) {

	game->score = 0; /* Punteggio 0 iniziale */
	game->started = (long) time(NULL); /* Tempo attuale convertito in long */
	game->lasttimesaved = (long) -1; /* Ultimo tempo salvato -1 perche' ancora non e' stato salvato nessun tempo */
	game->lasttime = 0; /* Non e' mai stato effettuato nessun turno */

	game->timepaused = 0; /* Tempo trascorso nella schermata di pausa (schermata di menu) */
	game->lasttimepaused = -1; /* Ultimo tempo di pausa salvato -1 perche' ancora non e' stato salvato nessun tempo */ 

	/* Inizializza l'inventario a 0 */
	game->knife = 0;
	game->bullet = 0;
}

void increase_score_by_time(FINESTRE *area, GAMESTATUS *game) {
	int inc;
	long calcola;
	char buffer[55];
	char numero[5];
	inc = 0;
	calcola = difftime(time(NULL),game->started);
	if(calcola < 60 || calcola == difftime(game->lasttimesaved, game->started))
		return;
	if(calcola%600 == 0)
		inc = 1500; //ogni dieci minuti
	else if(calcola%300 == 0)
		inc = 750; //ogni cinque minuti
	else if(calcola%180 == 0)
		inc = 350; //ogni tre minuti
	else if(calcola%120 == 0)
		inc = 150; //ogni due minuti
	if(inc != 0) {
		sprintf(numero, "%d", inc);
		strcpy(buffer, "Congratulations: you have just earned ");
		strcat(buffer, numero);
		strcat(buffer, " points!");
		modify_score(game, inc);
		write_event(area->desc, game, buffer);
	}
}

void level_up(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	game->remaining_items = 0;
	game->total_items = 0;
	game->score += 25 * (game->level); /* Aumenta il punteggio in proporzione al livello attuale */
	game->level += 1; /* Aumenta effettivamente il livello di 1 */

	riempi_tutto(game->lab, WALLH); /* Riempi tutto il labirinto di mura */
	random_maze2(game); /* Crea un nuovo labirinto */
	init_pac(pac, game->x_ingresso, game->y_ingresso); /* Inizializza il main character */

	/* Scrive la frase iniziale per ogni livello nel box di eventi */
	write_event(area->desc,game,"\0");

	/* Blocca il main character */
	stop_game_key(game);
}

int load_from_file_menu(FINESTRE *area, WINDOW *where, GAMESTATUS *game, CHARACTER *pac) {
	char c, *stringa;
	int i, j;
	i = 0; j = 15;
	int ret;
	char allwd[17] = "1234BKO0-#RCSFXD";
	stringa = malloc(sizeof(char)*j);
	while(1) {
		/* Poiche' sono bloccanti questi tipi di input
		utilizzando un approccio del tipo while(1)
		necessita l'aumento del timing anche in questo caso.. */
		aggiorna_timing_in_pausa(area,game);

		/* Poi blocca per l'input */
		c = wgetch(where);
		if(c != ERR) {
			if(c == '\n' || c == '\r') break;
			stringa[i] = c; i++;
			if(i+1 >= j-1) {
				j = j*2;
				stringa = realloc(stringa, j);
			}
		}
		stringa[i] = '\0';
		mvwprintw(where, 7, 0, "Path: %s", stringa, strlen(stringa));
	}
	stringa = realloc(stringa, strlen(stringa)+1);
	wclear(where);
	write_menu(where);
	if(strlen(stringa) > 0) {
		ret = load_maze_from_file(game->lab, stringa, allwd, 17, 20, 12, 35, 50);
		if(ret) init_from_maze(area, game, pac);
		if(stringa) free(stringa);
		if(ret == 0)
			mvwprintw(where, 7, 0, "Failed to parse the file!", stringa, strlen(stringa));			
		return ret;
	}
	else {
		if(stringa) free(stringa);
		mvwprintw(where, 7, 0, "Path can't be empty!", stringa, strlen(stringa));
		return 0;
	}
}

void init_from_maze(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	/* Elimina le finestre precedenti, e poi.. */
	delwin(area->info); delwin(area->main); delwin(area->desc);
	
	initscr(); /* Reinizializza lo schermo */
	clear(); /* Pulisce lo schermo */  
	refresh();

	/* ..crea di nuovo le finestre (adattate per le nuove dimensioni) */
	crea_area_di_gioco(area, game->lab->righe, game->lab->colonne);
	
	distruggi_heap(game->enemy); /* Distruggi il MIN HEAP precedente */
	init_enemy(game); /* Alloca il nuovo MIN HEAP */

	distruggi_heap(game->bullets); /* Distruggi il MIN HEAP precedente */
	init_bullets(game); /* Alloca il nuovo MIN HEAP */

	/* Stampa pugnali e proiettili collezionati */
	show_inventory(area->info, game);

	/* Cerca l'ingresso 'X' nel maze, altrimenti imposta ingresso randomico */
	/* Cerca l'uscita 'D' nel maze, altrimenti imposta uscita randomica */
	/* Cerca le guardie ('1','2','3','4') nel maze, e posiziona le guardie */
	search_and_set_entrance_exit_enemies(game, pac);

	init_game_text(area, game); /* inizializza le scritte del gioco */
	game->level = 1; /* Per scelta: se carico un livello personalmente riparto dal livello 1! */
	reset_game(game); /* Reimposta i valori di gioco a quelli di default */
	pac->velocita = PAC_VELOCITA; /* Reimposta la velocita' a quella di default */
	wclear(area->main); /* Svuota la finestra di gioco */
	wrefresh(area->main); /* Aggiorna la finestra di gioco */
}

void search_and_set_entrance_exit_enemies(GAMESTATUS *game, CHARACTER *pac) {
	int i, j;
	game->total_items = 0;
	game->remaining_items = 0;
	for(i=0;i<game->lab->righe;i++) {
		for(j=0;j<game->lab->colonne;j++) {
			switch(game->lab->maze[i][j]) {
				case 'X': /* Ingresso del gioco */
					game->lab->maze[i][j] = CORRIDOR;
					game->x_ingresso = i; game->y_ingresso = j;
					pac->x_curr = i; pac->y_curr = j; pac->x_next = i; pac->y_next = j;
				break;
				case EXIT:
					game->x_uscita = i; game->y_uscita = j;
				break;
				case ITEM_SLOW: case ITEM_FAST: case ITEM_COIN: case ITEM_REVERSE_DIR:
					game->total_items += 1; game->remaining_items += 1;
				break;
				case '1': /* POLICE */
					game->lab->maze[i][j] = CORRIDOR;
					add_enemy_here(i, j, game, &move_police1, POLICE, 13, -1, 30, 5);
				break;
				case '2': /* SOLDIER */
					game->lab->maze[i][j] = CORRIDOR;
					add_enemy_here(i, j, game, &move_soldier1, SOLDIER, 18, -1, 25, 6);
				break;
				case '3': /* SNIPER */
					game->lab->maze[i][j] = CORRIDOR;
					add_enemy_here(i, j, game, &move_police1, SNIPER, 16, -1, 20, game->lab->righe > game->lab->colonne ? game->lab->righe-1 : game->lab->colonne-1);
				break;
				case '4': /* A STAR forze speciali */
					game->lab->maze[i][j] = CORRIDOR;
					add_enemy_here(i, j, game, &move_astar1, ASTAR, 19, -1, 15, game->lab->righe > game->lab->colonne ? game->lab->righe-1 : game->lab->colonne-1);
				break;
			}
		}
	}
}

void show_maze(WINDOW *where, struct labyrinth *lab, int remaining_items) {
	int i, j;

	//scorre l'intera matrice del maze
	for(i=0;i<lab->righe;i++) {
		for(j=0;j<lab->colonne;j++) {
			//si posiziona all'interno della finestra alle coordinate correnti
			wmove(where, i, j);

			/* A seconda del carattere presente alla posizione maze[i][j]
			   stampa il carattere che lo rappresenta
			   con lo schema di colori associato a quel carattere */
			switch(lab->maze[i][j]) {
				case DOOR:
					wattron(where, COLOR_PAIR(1));
					waddch(where, DOOR);
					wattroff(where, COLOR_PAIR(1));
				break;
				case CORRIDOR:
					wattron(where, COLOR_PAIR(2));
					waddch(where, CORRIDOR);
					wattroff(where, COLOR_PAIR(2));
				break;
				case WALLH:
					wattron(where, COLOR_PAIR(5));
					waddch(where, WALLH);
					wattroff(where, COLOR_PAIR(5));
				break;
				case WALLV:
					wattron(where, COLOR_PAIR(5));
					waddch(where, WALLV);
					wattroff(where, COLOR_PAIR(5));
				break;
				case EXIT:
					if(remaining_items == 0) { //se ha raccolto tutti gli oggetti allora mostra l'uscita
						wattron(where, COLOR_PAIR(9));
						waddch(where, ACS_CKBOARD | A_REVERSE | A_BOLD);
						wattroff(where, COLOR_PAIR(9));
					} else { //altrimenti non la mostrare
						wattron(where, COLOR_PAIR(5));
						waddch(where, WALLV);
						wattroff(where, COLOR_PAIR(5));
					}
				break;
				case ITEM_COIN:
					wattron(where, COLOR_PAIR(10));
					waddch(where, ACS_BULLET | A_REVERSE | A_BOLD);
					wattroff(where, COLOR_PAIR(10));	
				break;
				case ITEM_FAST:
					wattron(where, COLOR_PAIR(11));
					waddch(where, ACS_BULLET | A_REVERSE | A_BOLD);
					wattroff(where, COLOR_PAIR(11));	
				break;
				case ITEM_REVERSE_DIR:
					wattron(where, COLOR_PAIR(12));
					waddch(where, ACS_BULLET | A_REVERSE | A_BOLD);
					wattroff(where, COLOR_PAIR(12));	
				break;
				case ITEM_SLOW:
					wattron(where, COLOR_PAIR(8));
					waddch(where, ACS_BULLET | A_REVERSE | A_BOLD);
					wattroff(where, COLOR_PAIR(8));	
				break;
				case BONUS_BOMB:
					wattron(where,  COLOR_PAIR(14));
					waddch(where, 'O' | A_BOLD);
					wattroff(where, A_REVERSE | COLOR_PAIR(14));	
				break;
				case BONUS_KNIFE:
					wattron(where,  COLOR_PAIR(15));
					waddch(where, '/' | A_BOLD);
					wattroff(where, A_REVERSE | COLOR_PAIR(15));	
				break;
				case BONUS_BULLET:
					wattron(where,  COLOR_PAIR(16));
					waddch(where, '*' | A_BOLD);
					wattroff(where, A_REVERSE | COLOR_PAIR(16));	
				break;
			}
		}
	}
}

void init_maze(GAMESTATUS *game, int glines, int gcols) {
	game->lab = alloca_labyrinth(game->lab);
	init_maze_labyrinth(game->lab, glines, gcols, WALLV);
}

int iswall(char c) {
	if(c == WALLV || c== WALLH)
		return 1;
	else
		return 0;
}

int exists_and_ischar(GAMESTATUS *game, int x,  int y, char ch) {
	if(x < 0 || y < 0 || x > game->lab->righe-1 || y > game->lab->colonne-1)
		return 0;
	if(game->lab->maze[x][y] == ch || game->lab->maze[x][y] == ch)
		return 1;
	else
		return 0;
}

int exists_and_iswall(GAMESTATUS *game, int x,  int y) {
	return (exists_and_ischar(game, x, y, WALLV) || exists_and_ischar(game, x, y, WALLH));
}

void add_random_door(GAMESTATUS *game) {
	int i, j;
	int x_start, y_start;
	int x, y;
	int max_tentativi;
	//analizzeremo tutto il labirinto ad esclusione dei bordi
	//e consideriamo come (x_start, y_start) tutte le coppie che possono essere centri di porte
	//ossia (x_start, y_start) e' il centro del tornello (che di default e' orizzontale)
	x_start = (rand() % (game->lab->righe-4))+2;
	y_start = (rand() % (game->lab->colonne-4))+2;

	/* Numero massimo di tentativi per aggiungere una porta
	e' necessario per evitare loop nel caso
	di generazione di labirinti senza possibilita'
	di aggiunta di porte, oppure nel caso
	di voler aggiungere troppe porte */
	max_tentativi = 100;

	y = y_start;
	for(i=0;i<game->lab->righe-2;i++) {
		x = fix_circular(x_start + i, game->lab->righe-2);
		if(x < 2)
			continue;
		for(j=0;j<game->lab->colonne-2;j++) {
			y = fix_circular(y_start + j, game->lab->colonne-2);
			max_tentativi--;
			if(max_tentativi < 0)
				return;
			if(y < 2 || (x == game->x_ingresso && y == game->y_ingresso))
				continue;
			if( game->lab->maze[x][y] != DOOR && /* se non e' gia' una porta */
				iswall(game->lab->maze[x][y-2]) && iswall(game->lab->maze[x][y+2]) &&
				iswall(game->lab->maze[x-2][y]) && iswall(game->lab->maze[x+2][y])
				) {
				//Crea la porta
				if(game->x_ingresso == x && (game->y_ingresso == y-1 || game->y_ingresso == y+1))
					continue;
				game->lab->maze[x][y-1] = DOOR;
				game->lab->maze[x][y] = DOOR;
				game->lab->maze[x][y+1] = DOOR;
				//Rompi l'eventuale muro per far spazio alla rotazione della porta
				game->lab->maze[x-1][y] = CORRIDOR;
				game->lab->maze[x+1][y] = CORRIDOR;
				game->lab->maze[x-1][y-1] = CORRIDOR;
				game->lab->maze[x+1][y-1] = CORRIDOR;
				game->lab->maze[x-1][y+1] = CORRIDOR;
				game->lab->maze[x+1][y+1] = CORRIDOR;
				//esci dalla funzione perche' ha raggiunto il suo scopo (Aggiungere una e una sola porta)
				return;
			}
		}
	}

}

void break_wall(char **maze, int x, int y, int dir, int howmany, int max_x, int max_y) {
	int i;
	for(i=0;i<howmany;i++) {
		switch(dir) {
			case 0: //su
				x--;
			break;
			case 1: //destra
				y++;
			break;
			case 2: //giu
				x++;
			break;
			case 3: //sinistra
				y--;
			break;
		}
		if(x >= 0 && y >= 0 && x < max_x && y < max_y)
			maze[x][y] = CORRIDOR;
	}
}

int gotointhemaze(int x, int y, int dir, int howmany, int max) {
	int i;
	int ret;
	if(y == -1)
		ret = 0;
	else
		ret= 1;
	for(i=0;i<howmany;i++) {
		switch(dir) {
			case 0: //su
				x--;
			break;
			case 1: //destra
				y++;
			break;
			case 2: //giu
				x++;
			break;
			case 3: //sinistra
				y--;
			break;
		}
	}
	if(ret == 0) {
		ret = x;
	} else {
		ret = y;
	}
	if(ret >= max || ret < 0)
		ret = -1;
	return ret;
	//più piccolo o uguale
}

void make_recursive_maze(struct labyrinth *lab, int x_ing, int y_ing) {
	int dir;
	int i;
	int x, y;
	int a, b;
	lab->maze[x_ing][y_ing] = CORRIDOR;
	dir = rand()%4;
	
	for(i=0;i<4;i++) {
		//se non e' la prima volta allora devo cambiare direzione
		//cerco in senso orario una direzione libera
		if(i!=0)
			dir = fix_circular(dir+1, 4);

		// (a,b) e' la cella di 1 avanti rispetto a (x_ing,y_ing)
		a = gotointhemaze(x_ing, -1, dir, 1, lab->righe);
		b = gotointhemaze(-1, y_ing, dir, 1, lab->colonne);

		// (x,y) e' la cella di 2 avanti rispetto a (x_ing,y_ing)
		x = gotointhemaze(x_ing, -1, dir, 2, lab->righe);
		y = gotointhemaze(-1, y_ing, dir, 2, lab->colonne);
		
		//se va out of the maze
		if(a == -1 || b == -1 || x == -1 || y == -1)
			continue; 
		//altrimenti se queste due caselle sono muri..
		if( (lab->maze[x][y] == WALLH || lab->maze[x][y] == WALLV) &&
			(lab->maze[a][b] == WALLH || lab->maze[a][b] == WALLV)) {
			//rompe questi muri
			break_wall(lab->maze, x_ing, y_ing, dir, 1, lab->righe, lab->colonne);

			//si richiama ricorsivamente sull'ultima cella appena distrutta
			make_recursive_maze(lab, x, y);
		}
	}
}

void destroy_random_wall(struct labyrinth *lab, int howmany) {
	int i, j;
	int rx, ry;
	int dir;
	int x, y;

	j = howmany;
	/* Analizza howmany coppie randomiche */
	for(i=1;i<j;i++) {
		rx = (rand() % (lab->righe-2)) + 1;
		ry = (rand() % (lab->colonne-2)) + 1;
		if(lab->maze[rx][ry] == CORRIDOR) { /* Se alle coordinate della coppia randomica c'e' un corridoio.. */
			/* ..allora controlla se o a nord - sud , oppure ad est - ovest , ci sono muri */
			/* La scelta e' randomica */
			dir = rand()%4;
			x = gotointhemaze(rx, -1, dir, 2, lab->righe);
			y = gotointhemaze(-1, ry, dir, 2, lab->colonne);
			/* Se c'e' un muro in una di queste due direzioni lo rompe */
			if(x != -1 && y != -1)
				break_wall(lab->maze, rx, ry, dir, 1, lab->righe, lab->colonne);
			/* Calcola la direzione duale */
			if(dir == 0 || dir == 2)
				dir += 2;
			else
				dir += 1;
			dir = fix_circular(dir, 4);
			x = gotointhemaze(rx, -1, dir, 2, lab->righe);
			y = gotointhemaze(-1, ry, dir, 2, lab->colonne);
			/* Se c'e' un muro nell'altra direzione lo rompe */
			if(x != -1 && y != -1)
				break_wall(lab->maze, rx, ry, dir, 1, lab->righe, lab->colonne);
			
		}
	}

}

void random_maze2(GAMESTATUS *game) {
	int i, max_righecolonne, min_righecolonne, x_ingresso, y_ingresso, tmp_x_uscita = 1, tmp_y_uscita = 0;

	// mura tutto il labirinto
	riempi_tutto(game->lab, WALLH);

	// genera una coppia (x,y) randomica e dispari all'interno del labirinto
	x_ingresso = (rand()%(game->lab->righe-2)+1);
	y_ingresso = (rand()%(game->lab->colonne-2)+1);
	if((x_ingresso % 2) == 0) x_ingresso++;
	if((y_ingresso % 2) == 0) y_ingresso++;

	// importa le coordinate di ingresso del labirinto
	// l'ingresso corrisponde al punto di partenza del main character
	game->x_ingresso = x_ingresso; game->y_ingresso = y_ingresso;

	// inizia a creare un labirinto perfetto con un algoritmo applicato al maze a partire da (x,y)
	make_recursive_maze(game->lab, x_ingresso, y_ingresso);

	//max_righecolonne contiene il massimo tra {game->lab->righe,game->lab->colonne}
	//serve a scopi pseudo-randomici

	if(game->lab->righe > game->lab->colonne) {
		max_righecolonne = game->lab->righe;
		min_righecolonne = game->lab->colonne;
	}
	else {
		max_righecolonne = game->lab->colonne;
		min_righecolonne = game->lab->righe;
	}

	//rompiamo n muri tra i percorsi
	//dove n e' randomico e n e' generato dall'espressione di cui sotto
	//la proprieta' del numero e' tale per assicurarsi un certo numero minimo di muri da analizzare
	destroy_random_wall(game->lab, (int)( (rand() % (max_righecolonne+1)) + max_righecolonne));

	//crea una coppia randomica per l'uscita
	get_random_exit(game->lab, &tmp_x_uscita, &tmp_y_uscita);

	// imposta le coordinate dell'uscita dal labirinto
	game->x_uscita = tmp_x_uscita; game->y_uscita = tmp_y_uscita;

	//inseriamo dei tornelli random (se possibile)
	//il numero dei tornelli dipende dal livello di gioco
	for(i=0;i<(int)((game->level/1.5)+1);i++) {
		if(i > (game->lab->righe*game->lab->colonne)/(max_righecolonne*1.2)) break;
		add_random_door(game);
	}

	//inseriamo degli oggetti random da raccogliere
	//il numero degli oggetti dipende dal livello di gioco
	for(i=0;i<(int)((game->level/2)+1);i++) {
		if(i > (game->lab->righe*game->lab->colonne)/(max_righecolonne*1.2)) break;
		add_random_object(game);
	}


	//inseriamo degli oggetti bonus random da raccogliere
	for(i=0;i<5;i++) {
		add_random_bonus(game);
	}

	distruggi_heap(game->enemy); /* Distruggi il MIN HEAP precedente */
	init_enemy(game); /* Alloca il nuovo MIN HEAP */

	distruggi_heap(game->bullets); /* Distruggi il MIN HEAP precedente */
	init_bullets(game); /* Alloca il nuovo MIN HEAP */

	//inseriamo delle guardie random
	//il numero e il tipo delle guardie dipende anche dal livello di gioco
	for(i=0;i<(int)((game->level/(min_righecolonne/2))+1);i++) {
		if(i > min_righecolonne/1.7) break;
			add_random_enemy(game);
	}

}

void get_random_exit(struct labyrinth *lab, int *x, int *y) {
	int tmp_x, tmp_y;
	int asse;
	int done;
	int i, j;
	done = 0;
	while(!done) {
		// seleziona l'asse su cui posizionare l'uscita
		asse = rand() % 4;

		// calcola la coppia (tmp_x, tmp_y) randomica fissata sull'asse
		// calcola la coppia (i,j) associata come l'ultima casella precedente all'uscita
		switch(asse) {
			default: case 0:
				tmp_x = 0;	tmp_y = rand() % lab->colonne;
				i = 1;	j = tmp_y;
			break;
			case 1:
				tmp_x = rand() % lab->righe;	tmp_y = lab->colonne-1;
				i = tmp_x;	j = tmp_y - 1;
			break;
			case 2:
				tmp_x = lab->righe-1;	tmp_y = rand() % lab->colonne;
				i = tmp_x - 1;	j = tmp_y;
			break;
			case 3:
				tmp_x = rand() % lab->righe;	tmp_y = 0;
				i = tmp_x;	j = 1;
			break;
		}
		//se nella coppia (i,j) non vi e' un muro l'uscita e' valida:
		if(!iswall(lab->maze[i][j]))
			done = 1;
	}
	lab->maze[tmp_x][tmp_y] = EXIT;
	*x = tmp_x;
	*y = tmp_y;
}

void add_object(GAMESTATUS *game, int item) {
	int rx, ry;
	int done;
	int tmp;
	int max_tentativi;
	done = 0;

	/* Numero massimo di tentativi per aggiungere una porta
	e' necessario per evitare loop nel caso
	di generazione di labirinti senza possibilita'
	di aggiunta di porte, oppure nel caso
	di voler aggiungere troppe porte */
	max_tentativi = 100;

	/* Fin quando non inserisce l'oggetto.. */
	while(!done) {
		max_tentativi--;
		if(max_tentativi < 0)
			return;
		/* Genera una coppia randomica (rx,ry) t.c. in (rx,ry) c'e' un corridoio, */
		rx = rand() % game->lab->righe;
		ry = rand() % game->lab->colonne;
		if(game->lab->maze[rx][ry] == CORRIDOR && game->x_ingresso != rx) { /* e non e' la coordinata di partenza */
			tmp = rx-1;
			if(tmp > -1)
				if(game->lab->maze[tmp][ry] == DOOR)
					continue;
			tmp = rx+1;
			if(tmp < game->lab->righe)
				if(game->lab->maze[tmp][ry] == DOOR)
					continue;
			game->lab->maze[rx][ry] = item;
			done = 1; //Termina il ciclo
			if(item != BONUS_BULLET && item != BONUS_KNIFE && item != BONUS_BOMB) {
				game->remaining_items += 1; //Aggiorna il numero corretto di oggetti rimanenti
				game->total_items += 1; //Aggiorna il numero corretto di oggetti totali
			}
		}
	}
}

void add_random_object(GAMESTATUS *game) {
	int max_kind_of_item;
	int random;
	
	/* La funzione aggiunge un oggetto casuale
	tra una scelta pero' limitata in base
	al livello attuale di gioco. Piu' si va avanti
	nel gioco piu' sara' vasta la scelta (pur sempre
	randomica) di oggetti da inserire */
	max_kind_of_item = (int)(game->level*1.5);
	random = rand() % max_kind_of_item;
	//In random vi e' il numero associato all'oggetto da inserire
	if(random%2 == 0) {
		add_object(game, ITEM_COIN);
	} else if(random%3 == 0) {
		add_object(game, ITEM_FAST);
	} else if(random%5 == 0) {
		add_object(game, ITEM_REVERSE_DIR);
	} else if(random%7 == 0) {
		add_object(game, ITEM_SLOW);
	} else {
		add_object(game, ITEM_COIN);
	}
}

void add_random_bonus(GAMESTATUS *game) {
	int random;
	
	/* La funzione aggiunge un oggetto BONUS casuale
	non per forza deve inserire un oggetto (proprio perché bonus)
	*/
	random = rand() % 60;
	//In random vi e' il numero associato all'oggetto da inserire
	if(random < 10) {
		add_object(game, BONUS_BOMB);
	} else if(random < 20) {
		add_object(game, BONUS_KNIFE);
	} else if(random < 30) {
		add_object(game, BONUS_BULLET);
	}
}

void rotate_hor_door(char **maze, int a, int b, int c, int d, int e, int f) {
	/* Rotate_door e le funzioni precedenti si sono gia' assicurate
	che le coppie (a,b) , (c,d) , (e,f) appartengono al maze
	e che sono tutte e tre delle porte. Possiamo quindi
	senza commettere errori, basarci su quest'affermazione */
	maze[c-1][d] = DOOR;
	maze[c+1][d] = DOOR;
	maze[a][b] = CORRIDOR;
	maze[e][f] = CORRIDOR;
}

void rotate_ver_door(char **maze, int a, int b, int c, int d, int e, int f) {
	/* Rotate_door e le funzioni precedenti si sono gia' assicurate
	che le coppie (a,b) , (c,d) , (e,f) appartengono al maze
	e che sono tutte e tre delle porte. Possiamo quindi
	senza commettere errori, basarci su quest'affermazione */
	maze[c][d-1] = DOOR;
	maze[c][d+1] = DOOR;
	maze[a][b] = CORRIDOR;
	maze[e][f] = CORRIDOR;
}

int rotate_door(GAMESTATUS *game, int x, int y) {
	int a, b, c, d, e, f;
	/* Orizzontale? */
	a = x;    c = x;    e = x;
	b = y;    d = y+1;  f = y+2;
	if(f < game->lab->colonne) {
		if(game->lab->maze[c][d] == DOOR && game->lab->maze[e][f] == DOOR) { //avevo preso il lato sinistro
			if(enemy_can_go(game, c-1, d) && enemy_can_go(game, c+1, d) && /* se ruotando non sbatto su nessun nemico */
			exists_and_iswall(game, c-2, d) && exists_and_iswall(game, c+2, d)) { /* se esiste ancora il muro dove ruota */
				rotate_hor_door(game->lab->maze, a, b, c, d, e, f); //ruota
				return 1; //e ritorna vero
			} else { //altrimenti
				return 0; //ritorna solo falso
			}
		}
	}

	b = y-2;  d = y-1;  f = y;
	if(b > -1) {
		if(game->lab->maze[c][d] == DOOR && game->lab->maze[e][f] == DOOR) { //avevo preso il lato destro
			if(enemy_can_go(game, c-1, d) && enemy_can_go(game, c+1, d) && /* se ruotando non sbatto su nessun nemico */
			exists_and_iswall(game, c-2, d) && exists_and_iswall(game, c+2, d)) { /* se esiste ancora il muro dove ruota */
				rotate_hor_door(game->lab->maze, a, b, c, d, e, f); //ruota
				return 1; //e ritorna vero
			} else { //altrimenti
				return 0; //ritorna solo falso
			}
		}
	}
	/* Se arrivo fino a qui
	(ossia se non vi e' stato un return)
	allora la porta non era orizzontale ma verticale */
	a = x;    c = x+1;    e = x+2;
	b = y;    d = y;      f = y;
	if(a < game->lab->righe) {
		if(game->lab->maze[c][d] == DOOR && game->lab->maze[e][f] == DOOR) { //avevo preso il lato di sotto
			if(enemy_can_go(game, c, d-1) && enemy_can_go(game, c, d+1) && /* se ruotando non sbatto su nessun nemico */
			exists_and_iswall(game, c, d-2) && exists_and_iswall(game, c, d+2)) { /* se esiste ancora il muro dove ruota */
				rotate_ver_door(game->lab->maze, a, b, c, d, e, f); //ruota
				return 1; //e ritorna vero
			} else { //altrimenti
				return 0; //ritorna solo falso
			}
		}
	}
	a = x-2;    c = x-1;    e = x;
	if(a > -1) {
		if(game->lab->maze[c][d] == DOOR && game->lab->maze[e][f] == DOOR) { //avevo preso il lato di sopra
			if(enemy_can_go(game, c, d-1) && enemy_can_go(game, c, d+1) && /* se ruotando non sbatto su nessun nemico */
			exists_and_iswall(game, c, d-2) && exists_and_iswall(game, c, d+2)) { /* se esiste ancora il muro dove ruota */
				rotate_ver_door(game->lab->maze, a, b, c, d, e, f); //ruota
				return 1; //e ritorna vero
			} else { //altrimenti
				return 0; //ritorna solo falso
			}
		}
	}
}

int through_door(GAMESTATUS *game, int x, int y) {
	int a, b, c, d;
	/* Se dove voglio andare non e' una porta, allora ritorna vero, */
	if(game->lab->maze[x][y] != DOOR)
		return 1;
	else if(game->lab->maze[x][y] == DOOR) { /* altrimenti .. */
		/* controlla se non sto sbattendo
		nel centro della porta */

		/* Caso 1: porta orizzontale */
		a = x; b = y-1; c = x; d = y+1;
		if(b > -1 && d < game->lab->colonne) { // le porte non possono stare a meta' tra lo schermo del labirinto
			if(game->lab->maze[a][b] == DOOR && game->lab->maze[c][d] == DOOR) { // allora sto sbattendo nel centro della porta
				return 0;
			} else if(game->lab->maze[a][b] == DOOR || game->lab->maze[c][d] == DOOR) { // altrimenti, e' porta e ruota la porta
				return rotate_door(game, x, y);
			}
		}

		/* Caso 2: porta verticale */
		a = x-1; b = y; c = x+1; d = y;
		if(a < 0 || c >= game->lab->righe)
			return 0; // le porte non possono stare a meta' tra lo schermo del labirinto
		if(game->lab->maze[a][b] == DOOR && game->lab->maze[c][d] == DOOR) { // allora sto sbattendo nel centro della porta
			return 0;
		} else if(game->lab->maze[a][b] == DOOR || game->lab->maze[c][d] == DOOR) { // altrimenti ruota la porta orizzonale
			return rotate_door(game, x, y);
		}

	}
} 

void remove_door(GAMESTATUS *game, int x, int y) {
	/* Esci se sono fuori dal range */
	if(x < 0 || y < 0 || x > game->lab->righe-1 || y > game->lab->colonne-1)
		return;
	/* Esci se non gli passo come coordinate (x,y) un 'pezzo' di porta */
	if(game->lab->maze[x][y] != DOOR)
		return;
	game->lab->maze[x][y] = CORRIDOR; /* Comincia a mettere un corridoio.. */

	/* Poi cerca il continuo della porta, ed elimina i due restanti 'pezzi' di porta */
	if(exists_and_ischar(game, x-2, y, DOOR) && exists_and_ischar(game, x-1, y, DOOR)) {
		game->lab->maze[x-2][y] = CORRIDOR;
		game->lab->maze[x-1][y] = CORRIDOR;
	} else if(exists_and_ischar(game, x-1, y, DOOR) && exists_and_ischar(game, x+1, y, DOOR)) {
		game->lab->maze[x-1][y] = CORRIDOR;
		game->lab->maze[x+1][y] = CORRIDOR;
	} else if(exists_and_ischar(game, x+1, y, DOOR) && exists_and_ischar(game, x+2, y, DOOR)) {
		game->lab->maze[x+1][y] = CORRIDOR;
		game->lab->maze[x+2][y] = CORRIDOR;
	} else {
		if(exists_and_ischar(game, x, y-2, DOOR) && exists_and_ischar(game, x, y-1, DOOR)) {
			game->lab->maze[x][y-2] = CORRIDOR;
			game->lab->maze[x][y-1] = CORRIDOR;
		} else if(exists_and_ischar(game, x, y-1, DOOR) && exists_and_ischar(game, x, y+1, DOOR)) {
			game->lab->maze[x][y-1] = CORRIDOR;
			game->lab->maze[x][y+1] = CORRIDOR;
		} else if(exists_and_ischar(game, x, y+1, DOOR) && exists_and_ischar(game, x, y+2, DOOR)) {
			game->lab->maze[x][y+1] = CORRIDOR;
			game->lab->maze[x][y+2] = CORRIDOR;
		}
	}
}

void riempi_tutto(struct labyrinth *lab, int ch) {
	int i, j;
	/* Riempie tutta la matrice di caratteri con
	il carattere specificato da 'ch' */
	for(i=0;i<lab->righe;i++) {
		for(j=0;j<lab->colonne;j++)
			lab->maze[i][j] = ch;
	}
}