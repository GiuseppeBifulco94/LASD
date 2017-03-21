#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "utility.h"
#include "heap.h"
#include "maze.h"
#include "curses_and_texts.h" /* include gia' <curses>, struct_def.h e <time> */
#include "game.h"
#include "pac.h"
#include "enemy.h"

WINDOW *create_boxed_newwin(int height, int width, int starty, int startx) {
	/* Crea una finestra contornata con "height" linee, "width" colonne. */
	WINDOW *local_win;

	/* Crea una finestra per il riquadro di delimitazione */
	local_win = newwin(height, width, starty, startx);

	box(local_win, BOX_VERTICAL, BOX_HORIZONTAL);   /* Crea il riquadro di delimitazione */
	wrefresh(local_win);   /* Mostra tutto */

	/* Crea una nuova finestra dentro il riquadro di delimitazione */
	local_win = newwin(height-2, width-2, starty+1, startx+1); 
	wrefresh(local_win);   /* Mostra tutto */
	nodelay(local_win,TRUE);   /* Le letture dalla tastiera saranno di tipo non-bloccante nella finestra */
	keypad(local_win, TRUE);   /* Consente il controllo dei tasti della tastiera */
	return local_win;
}
void init_color_curses() {
	start_color(); /* Inizializza i colori */

	/* Definisce delle coppie di colori <foreground,background> per il testo stampato a schermo */
	init_pair(1, COLOR_WHITE, COLOR_BLACK); /* Bianco su nero */
	init_pair(2, COLOR_BLACK, COLOR_BLACK); /* Nero su nero */
	init_pair(3, COLOR_RED, COLOR_WHITE);   /* Rosso su bianco */
	init_pair(4, COLOR_RED, COLOR_BLACK);   /* Rosso su nero */
	init_pair(5, COLOR_WHITE, COLOR_WHITE);   /* Bianco su bianco */
	init_pair(6, COLOR_BLACK, COLOR_WHITE); /* Nero su bianco */
	init_pair(7, COLOR_WHITE, COLOR_RED);   /* Bianco su rosso */
	init_pair(8, COLOR_BLACK, COLOR_RED);   /* Nero su rosso */
	init_pair(9, COLOR_YELLOW, COLOR_YELLOW);   /* Giallo su giallo */
	init_pair(10, COLOR_BLACK, COLOR_YELLOW);   /* Nero su Giallo */
	init_pair(11, COLOR_BLACK, COLOR_GREEN);   /* Nero su Verde */
	init_pair(12, COLOR_BLACK, COLOR_MAGENTA);   /* Nero su Viola */
	init_pair(13, COLOR_YELLOW, COLOR_BLACK);   /* Giallo su Nero */
	init_pair(14, COLOR_RED, COLOR_BLACK);   /* Rosso su Nero */
	init_pair(15, COLOR_GREEN, COLOR_BLACK);   /* Verde su Nero */
	init_pair(16, COLOR_CYAN, COLOR_BLACK);   /* Ciano su Nero */
	init_pair(17, COLOR_BLACK, COLOR_CYAN);   /* Nero su Ciano */
	init_pair(18, COLOR_GREEN, COLOR_BLACK);   /* Verde su Nero */
	init_pair(19, COLOR_MAGENTA, COLOR_BLACK);   /* Magenta su Nero */
}
int init_curses(void) {
	/* Inizializza lo schermo e la keyboard */
	initscr(); /* Inizializza lo schermo */
	clear(); /* Pulisce lo schermo */  
	keypad(stdscr, TRUE); /* Consente il controllo dei tasti della tastiera */
	curs_set(0); /* Nasconde il cursore */
	timeout(0); /* Non aspetta il getch() */
	noecho(); /* Nasconde l'echo dell'utente */
	cbreak(); /* Disabilita il buffer di linea */
 	if(!has_colors()) {
		endwin(); /* Se la console non consente di utilizzare colori allora chiude la finestra */
		printf("Il tuo terminale non supporta i colori.\n"); /* Stampa un messaggio di errore a video */
		return 0; /* Ritorna errore */
	}
	init_color_curses();
	/* ESCDELAY = 20; */ /* Inutile perche' non uso il tasto ESC */
	return 1; /* Ritorna OK */
}
void crea_area_di_gioco(FINESTRE *area, int glines, int gcols) {
	/* Crea la finestra per il gioco */
	area->main = create_boxed_newwin(glines+2,gcols+2,0,0);

	/* Crea la finestra per le informazioni */
	area->info = create_boxed_newwin(glines+2,BOX_COLS,0,gcols+2);
	
	/* Crea la finestra per la descrizione degli eventi accaduti */
	area->desc = create_boxed_newwin(5,gcols+2+BOX_COLS,glines+2,0);

	/* Verifica che entrambe le finestre siano state creata correttamente */
	if(area->main && area->info && area->desc) 
		area->success = 1;
	else
		area->success = 0;
}
void close_curses() {
	attroff(COLOR_PAIR(1)); /* Ripristina i colori dello schermo */
	endwin();   /* Chiude la finestra */
}
void print_w_effect(WINDOW *where, char *stringa) {
	int i, max, k;
	char *nstring;
	wclear(where); /* Svuota la finestra */
	i = 0;
	max = strlen(stringa);
	nstring = malloc(sizeof(char)*(max+1));
	wattron(where, COLOR_PAIR(1));
	/* Crea un effetto macchina da scrivere */
	while(i<max) {
		if(stringa[i] == ' ') {
			i++;
			continue;
		}
		k = 0;
		while(k<i) {
			nstring[k] = stringa[k];
			k++;
		}
		nstring[k] = '\0';
		mvwprintw(where,0,0,nstring);
		wrefresh(where);
		usleep(6500);
		i++;
	}
	if(nstring)
		free(nstring);
	wattroff(where, COLOR_PAIR(1));
}
void show_intro(int glines, int gcols){
	int key, back1, back2;
	WINDOW *intro;


	/* Crea la finestra per l'intro che occupa tutto lo di gioco (cosi' nascosto) */
	intro = create_boxed_newwin(glines+2+5,gcols+2+BOX_COLS,0,0);

	/* Inizializza le variabili di ausilio per il menu' */
	timeout(1000); /* La lettura del tasto blocca per millisecondi e ritorna ERR se nessun tasto e' premuto */

	back1 = 0; /* Non ha ancora premuto SPAZIO */
	back2 = 0; /* Non ha ancora premuto SPAZIO */
	print_w_effect(intro, "Hello, let me explain you some things before hand:\n1. What's the goal of the game?\nYou need to collect all the items without being catched by guards.\n2. What are the items?\nItems are colored dots and different colors mean different effects.\n3. What are bonus items?\nBonus items are not dots.\nCollect bonus items isn't mandatory to complete the level.\nThey're bombs, knives and bullets.\nPress SPACE to continue to read.");
	flushinp();
	while(!back1) {
		while(!back2) {
			key = wgetch(intro); /* Prende il tasto premuto */
			if(key == SPACE) /* Se l'utente preme SPAZIO.. */ {
				back2 = 1; /* ..allora continua a leggere */
				print_w_effect(intro, "4. What do bonus items do?\nWalking on a bomb will destroy surroundings, guards too.\nIf you have a knife, you can go towards a guard to defeat him.\nKnives are disposable.\nIf you have a bullet, you can fire it in your direction.\nThe first object (wall,door,guard) on its path will be destroyed.\n4. What do you have to do after collecting all the items?\nThe exit will appear and you've to reach it.\n5. What happens when a guard catches you?\nYour score decreases of 100 points.\nIf it's positive, you restart the level.\nIf it's negative, you have to restart from the first level.\nTips: play for long times, you'll earn points for the dedication.\nThat's all, press SPACE to start the game!");
				flushinp();
				key = -1;
			}
		}
		key = wgetch(intro); /* Prende il tasto premuto */
		if(key == SPACE) /* Se l'utente preme SPAZIO.. */
			back1 = 1; /* ..allora esci dall'intro */
	}
	timeout(0); /* Non aspettare piu' il getch */
	delwin(intro); /* Elimina la finestra */
}
void init_game_text(FINESTRE *area, GAMESTATUS *game) {
	/* Scrive i comandi di gioco nel box di informazione */
	wattron(area->info, COLOR_PAIR(1));
	mvwprintw(area->info,0,0,"Press 'x' to fire (capslock off)\nUse the keyboard arrow to move\nPress SPACE to stop\nPress '1' to show the menu");
	wattroff(area->info, COLOR_PAIR(1));
	
	/* Scrive la frase iniziale per ogni livello nel box di eventi */
	write_event(area->desc,game,"\0");
	
	show_author(area->info, (game->lab->righe/2)); /* Scrive i crediti nel box di informazione */
	
	/* Aggiorna a video il box di informazione */
	wrefresh(area->info);
}
char *string_time_elapsed(GAMESTATUS *game) {
	long approssimazione;
	char ore[3], minuti[3], secondi[3], stringa[16], *ret;
	double diff_time;
	if(
		(difftime(time(NULL), game->lasttimesaved) > 9 && game->lasttimesaved != -1)
		|| game->started == -1
		|| difftime(time(NULL), game->lasttimesaved) < 0) {
		strcpy(stringa,"Don't cheat");
		game->started = -1;
	} else {
		/* Esegue la sottrazione tra il time() attuale e quello di inizio gioco */
		diff_time = difftime(difftime(time(NULL), game->started), game->timepaused);
		approssimazione = (long) diff_time; /* Cast a long */

		game->lasttimesaved = time(NULL);

		sprintf(secondi, "%ld", approssimazione%60); approssimazione /= 60;
		sprintf(minuti, "%ld", approssimazione%60); approssimazione /= 60;
		sprintf(ore, "%ld", approssimazione%24); approssimazione /= 24;

		/* Aggiunge uno zero iniziale alle ore se le ore di gioco sono ad una cifra */
		if(strlen(ore) == 1) {
			strcpy(stringa, "0");
			strcat(stringa, ore);
		} else 
			strcpy(stringa, ore);

		strcat(stringa, ":");
		/* Aggiunge uno zero iniziale ai minuti se i minuti di gioco sono ad una cifra */
		if(strlen(minuti) == 1)
			strcat(stringa, "0");

		strcat(stringa, minuti);
		strcat(stringa, ":");
		/* Aggiunge uno zero iniziale ai secondi se i secondi di gioco sono ad una cifra */
		if(strlen(secondi) == 1)
			strcat(stringa, "0");

		strcat(stringa, secondi);
	}
	ret = (char *) malloc(sizeof(char)*(strlen(stringa)+1)); /* Alloca la stringa da restituire */
	strcpy(ret, stringa); /* Copia il contenuto composto sino ad ora nella stringa appena allocata */
	return ret; /* Restituisce questa stringa appena formattata */
}
void show_time_elapsed(WINDOW *where, GAMESTATUS *game) {
	char *s;
	/* Se l'utente ha imbrogliato sul tempo allora e' inutile effettuare i calcoli successivi.. */
	if(game->started == -1) {
		riempi_tutto(game->lab, CORRIDOR);
		return;
	}

	s = string_time_elapsed(game); /* La stringa 's' contiene la formattazione del tempo di gioco trascorso  */

	/* Scrive l'informazione nel box di informazione */
	mvwprintw(where,game->lab->righe-2,0,"Time elapsed: ");
	if(strcmp(s,"Don't cheat") == 0) {
			distruggi_heap(game->enemy); /* Distruggi il MIN HEAP precedente */
	init_enemy(game); /* Alloca il nuovo MIN HEAP */

	distruggi_heap(game->bullets); /* Distruggi il MIN HEAP precedente */
	init_bullets(game); /* Alloca il nuovo MIN HEAP */

		wattron(where, A_BLINK | A_BOLD | COLOR_PAIR(7));
		mvwprintw(where,game->lab->righe-2,BOX_COLS-strlen(s)-2,s);		
		wattroff(where, A_BLINK | A_BOLD | COLOR_PAIR(7));
	} else {
		mvwprintw(where,game->lab->righe-2,BOX_COLS-strlen(s)-2,s);
	}
	if(s)
		free(s); /* Libera la stringa formattata */
}
char *string_score(GAMESTATUS *game) {
	char *stringa, scorestr[16];
	int i;

	stringa = malloc(sizeof(char)*16); /* Alloca la stringa da restituire */
	stringa[0] = '\0';
	sprintf(scorestr, "%d", game->score); /* Converte in stringa il punteggio da intero */

	/* Formatta la stringa del punteggioin una stringa di 15 caratteri con
	(laddove il punteggio non avesse abbastanza cifre) tanti zero iniziali */
	for(i=0;i<15-strlen(scorestr);i++)
		strcat(stringa, "0");

	strcat(stringa, scorestr);
	return stringa; /* Ritorna la stringa appena formattata */
}
void show_score(WINDOW *where, GAMESTATUS *game) {

	char *s;
	s = string_score(game); /* La stringa 's' contiene la formattazione del punteggio corrente  */

	/* Scrive l'informazione nel box di informazione */
	mvwprintw(where,game->lab->righe-1,0,"Score: ");
	mvwprintw(where,game->lab->righe-1,BOX_COLS-17,s);

	if(s)
		free(s); /* Libera la stringa formattata */
}
void show_author(WINDOW *where, int x) {
	wattron(where, A_REVERSE | A_BOLD | COLOR_PAIR(8)); /* Imposta lo schema di colori 4 (Red on Black) */ 

	/* Scrive i crediti nel box di informazione */
	mvwprintw(where,x,0,"    Mirko Alicastro N86/1437     ");
	
	wattroff(where, A_REVERSE | A_BOLD | COLOR_PAIR(8)); /* Ripristina lo schema di colori */
}
void update_level(WINDOW *where, GAMESTATUS *game) {
	char buff[5];

	sprintf(buff, "%d", game->level); /* Converte l'intero contente l'informazione del livello corrente in una stringa */

	/* Scrive l'informazione nel box di informazione */
	mvwprintw(where,game->lab->righe-3,0,"Level:                          ");
	mvwprintw(where,game->lab->righe-3,BOX_COLS-strlen(buff)-2,buff);
}
int jump_to_level_menu(FINESTRE *area, WINDOW *where, GAMESTATUS *game, CHARACTER *pac) {
	char c, *stringa;
	int i, j, lvl;
	i = 0; j = 15; lvl = 1;
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
			if(!char_is_digit(c) || (i == 0 && c == '0') || (i >= 6)) continue;
			stringa[i] = c; i++;
		}
		stringa[i] = '\0';
		mvwprintw(where, 7, 0, "Jump to level: %s", stringa);
		mvwprintw(where, 9, 0, "Max: 999999");
	}
	stringa = realloc(stringa, strlen(stringa)+1);
	if(strlen(stringa) > 0)
		sscanf(stringa, "%d", &lvl);
	else {
		if(stringa) free(stringa);
		wclear(where);
		write_menu(where);
		mvwprintw(where, 7, 0, "Level can't be empty!", stringa);		
		return 0;
	}
	jump_to_level(area, game, pac, lvl);
	if(stringa) free(stringa);
	return 1;
}
void write_menu(WINDOW *where) {
	/* Svuota la window */
	wclear(where);

	/* Crea il menu visivo */
	wattron(where, A_BOLD | A_UNDERLINE | COLOR_PAIR(4));
	mvwprintw(where,0,0,"Menu\n");
	wattroff(where, A_BOLD | A_UNDERLINE | COLOR_PAIR(4));
	mvwprintw(where,2,0,"1 Back to game\n2 Load maze from txt\n3 Jump to level\n4 Close game");
	wattroff(where, A_BOLD);
}
void show_menu(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	WINDOW *menu;
	int key; int back;
	
	/* Crea la finestra per il menu */
	menu = create_boxed_newwin(game->lab->righe+2,game->lab->colonne+2,0,0);
	/* Inizializza le variabili di ausilio per il menu' */
	back = 0;
	timeout(1000); /* La lettura del tasto blocca per millisecondi e ritorna ERR se nessun tasto e' premuto */
	game->lasttimepaused = time(NULL); /* Nuovo time salvato in pausa */

	write_menu(menu); /* Scrive il menu grafico */

	while(!back) {
		/* Poiche' sono bloccanti questi tipi di input
		utilizzando un approccio del tipo while(1)
		necessita l'aumento del timing anche in questo caso.. */
		aggiorna_timing_in_pausa(area,game);
		
		key = wgetch(menu); /* Prende il tasto premuto */
		stop_game_key(game); /* Non salvare nessun tasto premuto */
		switch(key) {
			case 50: /* Load maze from txt */
				if(game->started == -1) 
					continue; /* Se l'utente ha imbrogliato non puo' fare quest'operazione */
				write_menu(menu); /* Scrive il menu grafico */
				back = load_from_file_menu(area, menu, game, pac);
			break;
			case 51: /* Jump to level */
				if(game->started == -1) 
					continue; /* Se l'utente ha imbrogliato non puo' fare quest'operazione */
				write_menu(menu); /* Scrive il menu grafico */
				/* Se inserisco un livello valido esci anche dal menu */
				back = jump_to_level_menu(area, menu, game, pac);
			break;
			case 52: /* Close the game */
				write_menu(menu); /* Scrive il menu grafico */
				bye_bye(menu, game); /* Stampa a video un saluto e termina il gioco */
				back = 1;
			break;
			case MENU: /* Tasto menu (ossia tasto 1) */
				back = 1; /* Esce dal menu */
			break;
		}
	}
	timeout(0); /* Non aspettare piu' il getch */
	wclear(menu); /* Svuota la finestra */
	delwin(menu); /* Elimina la finestra */
}
void show_inventory(WINDOW *where, GAMESTATUS *game) {
	char buff[5];
	/* Converte l'intero contente l'informazione del numero di coltelli corrente in una stringa */
	sprintf(buff, "%d", game->knife);
	/* Scrive con la corretta formattazione il carattere del coltello */
	wmove(where, game->lab->righe-4, BOX_COLS-strlen(buff)-5);
	wattron(where,  COLOR_PAIR(15));
	waddch(where, '/' | A_BOLD);
	wattroff(where, A_REVERSE | COLOR_PAIR(15));
	wattron(where, COLOR_PAIR(1));
	mvwprintw(where,game->lab->righe-4,BOX_COLS-strlen(buff)-23,"Disposable knives");
	wattroff(where, COLOR_PAIR(1));
	wattron(where, COLOR_PAIR(1));
	mvwprintw(where,game->lab->righe-4,BOX_COLS-strlen(buff)-4,": %s",buff);
	wattroff(where, COLOR_PAIR(1));

	/* Converte l'intero contente l'informazione del numero di coltelli corrente in una stringa */
	sprintf(buff, "%d", game->bullet);
	/* Scrive con la corretta formattazione il carattere del proiettile */
	wmove(where, game->lab->righe-5, BOX_COLS-strlen(buff)-5);
	wattron(where,  COLOR_PAIR(16));
	waddch(where, '*' | A_BOLD);
	wattroff(where, A_REVERSE | COLOR_PAIR(16));
	wattron(where, COLOR_PAIR(1));
	mvwprintw(where,game->lab->righe-5,BOX_COLS-strlen(buff)-13,"Bullets");
	wattroff(where, COLOR_PAIR(1));
	wattron(where, COLOR_PAIR(1));
	mvwprintw(where,game->lab->righe-5,BOX_COLS-strlen(buff)-4,": %s",buff);
	wattroff(where, COLOR_PAIR(1));
}
void write_event_important(WINDOW *where, GAMESTATUS *game, char *stringa) {
	wattron(where, A_BLINK);
	write_event(where, game, stringa);
	wattroff(where, A_BLINK);
}
void write_event(WINDOW *where, GAMESTATUS *game, char *stringa) {
	int i, j, x, y;
	char defaultstring[71];
	/* Questa funzione splitta il testo su righe
	aggiungendo un '-' prima dello '\n'. Serve solo
	per scopi estetici e consiste in una stampa
	carattere per carattere della stringa 'stringa'
	all'interno della funzione where. Il testo
	viene inoltre formattato */
	wattron(where, A_BOLD | A_REVERSE | COLOR_PAIR(11));
	if(strlen(stringa) < 1) {
		strcpy(defaultstring, "Collect all the items to unlock the exit and switch to the next level!");
		stringa = defaultstring;
	}
	i = 0; j = strlen(stringa); x = 0; y = 0;
	wclear(where); /* Svuota la window */
	while(i<j) {
		mvwprintw(where, x, y, "%c", stringa[i]);
		i++;
		y++;
		if(y >= (game->lab->colonne+BOX_COLS-1) && i<j) {
			if(stringa[i-1] != ' ' && stringa[i] != ' ')
				mvwprintw(where, x, y, "-"); /* Se non c'era gia' uno spazio tra le parole che vanno accapo */
				while(stringa[i] == ' ') { /* Avanza di tutti gli spazi che ci sono */
					i++;
					if(i >= j)
						break;
				}
			x += 1; y=0;
		}
	}
	wattroff(where, A_BOLD | A_REVERSE | COLOR_PAIR(11));
	wrefresh(where);
}
void aggiorna_timing_in_pausa(FINESTRE *area, GAMESTATUS *game) {
	if(deltatime(game)) { /* Aggiorna i clock ugualmente */
		reset_delta(game); /* Ripristina il delta */
		game->timepaused = difftime(time(NULL), game->lasttimepaused) + game->timepaused;
		game->lasttimepaused = time(NULL);
		show_time_elapsed(area->info, game); /* Aggiorna il timing */
		wrefresh(area->info); /* Mostra il timing aggiornato */
	}
}
void jump_to_level(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac, int level) {
	/* Salta al livello 'level'
	mantenendo lo score attuale */
	int tmp_score;
	if(level < 0)
		level = 1; //underflow
	if(level > 999999)
		level = 999999; //overflow (stack smashing detected)
	tmp_score = game->score;
	game->level = level-1;
	level_up(area, game, pac);
	game->score = tmp_score;
}
void you_lose(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {

	WINDOW *game_over;

	/* Crea la finestra */
	game_over = create_boxed_newwin(game->lab->righe+2,game->lab->colonne+2,0,0);

	/* Crea l'aspetto grafico */
	wattron(game_over, A_BOLD | A_BLINK | COLOR_PAIR(13));
	mvwprintw(game_over,0,0,"    __                \n   / /  ___   ___ ___ \n  / /__/ _ \\ (_-</ -_)\n /____/\\___//___/\\__/ \n");
	wattroff(game_over, A_BOLD | A_BLINK | COLOR_PAIR(13));
	wrefresh(game_over);

	/* Aspetta 4 secondi */
	sleep(4);

	reset_game(game);

	game->level = 0; /* Setta il livello a 0 e poi.. */
	level_up(area, game, pac); /* ..sali di livello! */

	/* Disabilita l'ultimo tasto premuto */
	stop_game_key(game);

	/* Elimina la window creata */
	delwin(game_over);
}
void bye_bye(WINDOW *where, GAMESTATUS *game) {
	wattron(where, A_BOLD | A_BLINK | COLOR_PAIR(4));
	mvwprintw(where, 7, 0, "Bye bye! =)\n");
	wattroff(where, A_BOLD | A_BLINK | COLOR_PAIR(4));
	wrefresh(where);
	sleep(3); /* Richiede unistd.h */
	game->over = 1;
}
