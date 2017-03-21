#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "maze.h"
#include "heap.h"
#include "pac.h" /* Include gia' <curses>, struct_def.h e <time> */
#include "game.h"
#include "curses_and_texts.h"
#include "enemy.h"

void pac_on_items(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	int ret, score;

	/* Se non ci sono piu' oggetti da raccogliere ERA inutile andare avanti.. 
	perché adesso ci sono anche oggetti bonus non obbligatori da raccogliere! */
//	if(game->remaining_items == 0)
//		return;
	ret = -1;
	score = 0;
	switch(game->lab->maze[pac->x_curr][pac->y_curr]) {
		case ITEM_COIN:
			score = 10;
			write_event(area->desc,game,"You have collected a coin!");
		break;
		case ITEM_FAST:
			score = 15;
			write_event(area->desc,game,"You drank a 'fast poison': don't run too fast!");
			pac->velocita = (pac->velocita - 2) > 0 ? pac->velocita - 2 : 1;
		break;
		case ITEM_REVERSE_DIR:
			score = 25;
			write_event(area->desc,game,"A strange item have confused you, where are you going?!");
			reverse_pac_dir(game, pac->character);
		break;
		case ITEM_SLOW:
			score = 35;
			write_event(area->desc,game,"You have eaten too much.. Don't you feel heavy?!");
			pac->velocita = pac->velocita + 2;
		break;
		case BONUS_BOMB:
			score = 100;
			write_event(area->desc,game,"BOOOOOOOOM!");
			bomb(game, pac);
			ret = 0; /* Gli oggetti bonus non sono obbligatori da raccogliere */
		break;
		case BONUS_BULLET:
			score = 20;
			game->bullet += 1;
			write_event(area->desc,game,"You have collected a bullet, use it against guards!");
			ret = 0; /* Gli oggetti bonus non sono obbligatori da raccogliere */
		break;
		case BONUS_KNIFE:
			score = 5;
			game->knife += 1;
			write_event(area->desc,game,"You have found a disposable knife, cut the head to a guard!");
			ret = 0; /* Gli oggetti bonus non sono obbligatori da raccogliere */
		break;
		default:
			ret = 0; // se non trovo nessun oggetto conosciuto non tolgo nulla agli oggetti rimanenti
		break;
	}
	modify_score(game, score);
	game->remaining_items += ret;
	if(game->remaining_items == 0 && ret != 0)
			write_event_important(area->desc,game,"Come on, run to the exit!");
	game->lab->maze[pac->x_curr][pac->y_curr] = CORRIDOR;
}

void move_pac(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac, int dir) {
	int x, y;

	pac->lastdir = dir; /* Aggiorna l'ultima direzione del main character */
	x = pac->x_curr;
	y = pac->y_curr;
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
	x = fix_circular(x, game->lab->righe);
	y = fix_circular(y, game->lab->colonne);

	if(iswall(game->lab->maze[x][y]) || !through_door(game,x,y) || !can_exit(game,x,y)) {
		pac->x_next = pac->x_curr;
		pac->y_next = pac->y_curr;
	} else {
		pac->x_next = x;
		pac->y_next = y;
	}
	switch(dir) {
		case 0:
			pac->character = '^';
		break;
		case 1:
			pac->character = '>';
		break;
		case 2:
			pac->character = 'v';
		break;
		case 3:
			pac->character = '<';
		break;
	}
}

void make_pacmove(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	int dir;
	/* Scandisci i turni interni del personaggio */
	if(pac->velocita_tmp > 1) {
		pac->velocita_tmp -= 1;
		return;
	} else if(pac->velocita == 0) {
		return;
	}
	pac->velocita_tmp = pac->velocita;
	read_from_keyboard(area, game); /* Legge la tastiera */
	/* Muovi il main character in base al tasto premuto */
	/* Calcola la direzione dell'eventuale movimento */
	dir = -1;
	switch(game->key) {
		case KEY_UP:
			dir = 0;
		break;
		case KEY_RIGHT:
			dir = 1;
		break;
		case KEY_DOWN:
			dir = 2;
		break;
		case KEY_LEFT:
			dir = 3;
		break;
	}
	/* Se dir != -1 allora il tasto premutO
	   e' un tasto di movimento del main character */
	if(dir != -1) {
		move_pac(area, game, pac, dir);
		return;
	}
	/* Altrimenti in base al tasto premuto si richiama
	   una funzione che traduce l'operazione richiesa */
	switch(game->key) {
	case SPACE : /* Ferma il main character alla posizione attuale */
		stop(pac);
	break;
	case MENU: /* Mostra il menu */
		show_menu(area, game, pac);
	break;
	case FIRE: /* Spara un proiettile */
		pac_fire(game, pac);
	break;
	}
}

void show_move(WINDOW *where, CHARACTER *pac) {
	/* Rimuovi la rappresentazione grafica del main character */
	/* dalla posizione attuale.. */
	wmove(where, pac->x_curr, pac->y_curr);
	wattron(where,COLOR_PAIR(1)); 
	waddch(where, CORRIDOR); /* ..ripristinando il corridoio */
	wattroff(where,COLOR_PAIR(1));

	/* Inserisci il main character nella nuova posizione */
	wmove(where, pac->x_next, pac->y_next);
	wattron(where, COLOR_PAIR(pac->color_pair) | A_REVERSE);
	waddch(where, pac->character);
	wattroff(where, COLOR_PAIR(pac->color_pair) | A_REVERSE);

}

void init_pac(CHARACTER *pac, int x, int y) {
	pac->character = 'v'; /* Imposta la shape iniziale */
	pac->color_pair = 4; /* Coppia di colori per rappresentare il character (vd init_pair) */
	/* NOTA BENE: la coppia viene 'reversata' per aumentare la vivacita' del colore */

	/* Imposta le coordinate di partenza */
	pac->x_curr = x;
	pac->y_curr = y;

	/* Imposta le coordinate del prossimo turno */
	pac->x_next = x;
	pac->y_next = y;

	pac->duration = -1; /* Dura all'infinito */

	/* Imposta la velocita' del personaggio (si muove ogni 5 turni) */
	/* In questo modo potremo accelerare o meno il personaggio e */
	/* possiamo creare guardie piu' veloci */
	pac->velocita = PAC_VELOCITA;
	pac->velocita_tmp = pac->velocita;

	pac->lastdir = 2; /* Inizialmente il main character 'guarda' verso il basso */

}

void stop(CHARACTER *pac) {
	/* Blocca la posizione del character 'pac' */
	pac->x_next = pac->x_curr;
	pac->y_next = pac->y_curr;
}


void reverse_pac_dir(GAMESTATUS *game, char character) {
	/* Inverti la direzione corrente del main character */
	switch(character) {
		case '<':
			game->key = KEY_RIGHT;
		break;
		case '>':
			game->key = KEY_LEFT;
		break;
		case '^':
			game->key = KEY_DOWN;
		break;
		default:	case 'v':
			game->key = KEY_UP;
		break;
	}
	game->lastkey = game->key;
}

int can_exit(GAMESTATUS *game, int x, int y) {
	/* Controlla se puo' andare sulla casella
	dell'uscita o se deve sbatterci contro come un muro */
	if(game->lab->maze[x][y] == EXIT) {
		if(game->remaining_items == 0)
			return 1;
		else
			return 0;
	} else
		return 1;
}

void pac_on_exit(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	if(game->remaining_items == 0 && game->x_uscita == pac->x_curr && game->y_uscita == pac->y_curr) {
		/* Aumenta di livello */
		level_up(area, game, pac);
	}
}

void modify_score(GAMESTATUS *game, int item) {
	game->score += item;
	if(game->score < 0)
		game->score = 0;
}

void get_random_pac_xy(GAMESTATUS *game, int *x, int *y) {
	/* Questa funzione e' molto importante perche' dopo che
	una guardia cattura il main character, decide le coordinate
	(x,y) in modo randomiche tali che, siano il piu lontane possibile
	(<= range) dalle guardie del gioco, per evitare di essere preso
	appena inizio di nuovo a giocare. Essendo randomico, e non 
	essendo detto che sia possibile, nel caso fallisse posiziona
	il main character nella sua posizione di partenza del livello. */
	int k, range, max_tentativi, t_x, t_y, err_range;
	ENEMY *el;
	range = 4; max_tentativi = 50;
	while(1) {
		if(max_tentativi < 0) { /* Se ho fatto gia' 50 tentativi allora */
			if(range < 1) { /* Se il range e' troppo piccolo purtroppo metto le coordinate come quelle di partenza liv. */
				*x = game->x_ingresso; *y = game->y_ingresso;
				return; /* ed esco */
			} else { /* altrimenti */
				range--; /* restringo il range */ max_tentativi = 50; /* e ripristino il numero di tentativi */
			}
		}
		max_tentativi--; /* diminuisco il num. di tentativi a disposizione */
		
		/* Posso creare coppia randomica */
		t_x = (rand() % (game->lab->righe-4))+2; /* Non bordi */
		t_y = (rand() % (game->lab->colonne-4))+2; /* Non bordi */

		/* Se la coppia randomica e' porta o muro.. */
		if(game->lab->maze[t_x][t_y] == DOOR || game->lab->maze[t_x][t_y] == WALLV || game->lab->maze[t_x][t_y] == WALLH)
			continue; /* Ripeto direttamente il ciclo */

		/* altrimenti */
		k = 0; err_range = 0;
		while(k<game->enemy->heapsize && !err_range) { /* Scorro tutte le guardie */
			el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, k)))->content); /* Per ogni guardia.. */
			if(el->character->duration != 0 && /* Se e' una guardia attiva */
				abs(el->character->x_curr - t_x) < range &&  /* Se la coordinata x, e poi la y, sono fuori dal range */
				abs(el->character->y_curr - t_y) < range) {
				err_range = 1; /* metto ad 1 l'errore */
			}
			k++;
		}
		if(!err_range) { /* se non vi sono stati errori allora */
			*x = t_x; *y = t_y;
			return; /* esci, salvando gli ultimi valori come corretti */
		}
		/* altrimenti riparti dall'inizio */
	}
}

void pac_fire(GAMESTATUS *game, CHARACTER *pac) {
	ENEMY *bullet;
	int x, y;
	/* Se non ha proiettili, esci */
	if(game->bullet < 1)
		return;

	/* Posizione di partenza del proiettile */
	x = pac->x_curr;
	y = pac->y_curr;

	/* Diminuisci il numero di proiettili a disposizione */
	game->bullet -= 1;

	/* Ferma il personaggio */
	stop_game_key(game);

	bullet = malloc(sizeof(ENEMY));
	bullet->character = malloc(sizeof(CHARACTER));

	/* Id identificativo della guardia (per comparare se due guardie sono uguali) */
	bullet->id_enemy = game->bullets->heapsize;
	
	/* Puntatore a funzione dell'algoritmo che muove il proiettile
	(inutile perche' tutti i proiettili) seguono lo stesso algoritmo */
	bullet->move_enemy = NULL;

	/* Definizione grafica (carattere - schema di colori) del proiettile */
	bullet->character->character = BULLET; bullet->character->color_pair = 17;

	/* Imposta le coordinate di partenza */
	bullet->character->x_curr = x; bullet->character->y_curr = y;

	/* Imposta le coordinate del prossimo turno */
	bullet->character->x_next = x; bullet->character->y_next = y;

	/* Imposta la velocita' del proiettile (si muove ogni 2 turni) */
	bullet->character->velocita = 2; bullet->character->velocita_tmp = 0;

	/* Dura fin quando non rompe qualcosa (in un giro completo)
	nella direzione in cui e' stato sparato */
	if(pac->lastdir == 0 || pac->lastdir == 2)
		bullet->character->duration = game->lab->righe*bullet->character->velocita;
	else
		bullet->character->duration = game->lab->colonne*bullet->character->velocita;

	/* Impostiamo la visibilita' della guardia */
	bullet->visibilita = pac->lastdir;

	InsertKey(game->bullets, bullet, bullet->character->velocita);
}

void pac_on_enemy(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	int k;
	int x, y;
	ENEMY *el;
	if(is_pac_on_enemy(game,pac)) { /* Se e' avvenuta la collisione */
		reset_delta(game);
		if(game->knife > 0) { /* Se ho un coltello.. */
			k = 0;
			/* Poi scorro tutte le guardie per trovare quella che sto uccidendo */
			while(k<game->enemy->heapsize) { /* Scorro tutte le guardie */
				el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, k)))->content); /*Per ogni guardia*/
				if(el->character->x_curr == pac->x_curr && el->character->y_curr == pac->y_curr) {
					if(el->character->duration != 0) {
						game->knife -= 1; /* Diminuisco i miei coltelli perche' 'usa e getta' */
						kill_guard(game, el);
						write_event(area->desc, game, "Killing is a crime.. but not in games!");
						modify_score(game, 100);
						return;
					}
				}
				k++; /* Procedi alla prossima guardia */
			}
		} else {
			stop_all_enemies(game);
			stop(pac);
			write_event_important(area->desc, game, "The guard has just caught you!");
			sleep(2);
			x = game->x_ingresso; y = game->y_ingresso;
			get_random_pac_xy(game, &x, &y);
			pac->x_curr = x;
			pac->y_curr = y;
			pac->x_next = pac->x_curr;
			pac->y_next = pac->y_curr;
			stop_game_key(game); /* Non salvare nessun tasto premuto */
			write_event(area->desc, game, "");
			if(game->score < abs(PENALITY)) {
				you_lose(area, game, pac);
			} else {
				modify_score(game, PENALITY);
			}
		}
	}
}

int is_pac_on_enemy(GAMESTATUS *game, CHARACTER *pac) {
	int i;
	ENEMY *el;
	i = 0;
	while(i<game->enemy->heapsize) { /* Scorro tutte le guardie */
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content); /* Per ogni guardia.. */
		 /* Se la posizione della guardia corrente e'
		 la stessa posizone del main character .. 
		 ed inoltre se il personaggio e' diverso
		 da quello dell'argomento della funzione
		 (per poter controllare se nemici diversi
		 si sovrappongono */
		if(el->character->x_curr == pac->x_curr && el->character->y_curr == pac->y_curr && el->character != pac && el->character->duration != 0)
			return 1; /* Ritorna vero! */
		i++;
	}
	return 0;
}

int getpeso_maze_for_pac(struct labyrinth *lab, int x, int y) {
	int ret;
	if(x < 0 || x >= lab->righe || y < 0 || y >= lab->colonne) {
		//printf("Out of cordinate.\n");
		return -1;
	} else {
		ret = 1;
		switch(lab->maze[x][y]) {
			case WALLH:
			case WALLV:
				ret = INFINITO; /* Il main character non puo' percorrere questi blocchi */
			break;
		}
		return ret;
	}
}

void bomb(GAMESTATUS *game, CHARACTER *pac) {
	int x, y;
	int i,j;
	int k;
	int range;
	ENEMY *el;
	x = pac->x_curr;
	y = pac->y_curr;
	range = 5;
	/* Distruggi tutte le mura (tranne le porte) nel range */
	for(i=x-range;i<x+range;i++) {
		/* Se esco fuori dal range del labirinto non eseguire l'operazione */
		if(i < 0 || i > game->lab->righe-1)
			continue;
		for(j=y-range;j<y+range;j++) {
			/* Se esco fuori dal range del labirinto non eseguire l'operazione */
			if(j < 0 || j > game->lab->colonne-1)
				continue;
			if(game->lab->maze[i][j] == DOOR) /* ..se e' una porta */
				remove_door(game, i, j); /* la distrugge.. */
			if(game->lab->maze[i][j] == WALLH || game->lab->maze[i][j] == WALLV) /* ..se e' un muro */
				game->lab->maze[i][j] = CORRIDOR; /* ..lo distrugge */
			/* Elimina i nemici presenti in tutte le coordinate 'fatte esplodere' */
			k = 0;
			while(k<game->enemy->heapsize) { /* Scorro tutte le guardie */
				el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, k)))->content); /*Per ogni guardia*/
				if(el->character->x_curr == i && el->character->y_curr == j && el->character->duration != 0) {
					kill_guard(game, el);
					/* Inutile continuare perche' gia'
					mi sono assicurato che non ci sono
					piu' guardie nella stessa posizione */
					break;
				}
				k++; /* Procedi alla prossima guardia */
			}
		}
	}
	/* Rimuovo le porte al di fuori di quel range */
	for(i=x-range-1;i<x+range+1;i++) {
		for(j=y-range-1;j<y+range+1;j++) {
				remove_door(game, i, j); /* In remove_door mi assicuro che non vado fuori range del maze */
		}
	}
}

void bullet_on_enemy(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	int i;
	int k;
	ENEMY *bull, *el;
	if(!game->bullets)
		return;
	i = 0;
	while(i<game->bullets->heapsize) { /* Scorri tutti i proiettili */
		bull = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->bullets, i)))->content); /* Per ogni proiettile */
		if(bull->character->duration == 0) { /* , se non e' attivo saltalo.. */
			i++;
			continue;
		}
		/* ..altrimenti vedo se: */
		k = 0;
		while(k<game->enemy->heapsize) { /* All'interno di tutte le guardie, */
			el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, k)))->content);
				/* se c'e' una guardia che collide con il proiettile, */
				if(el->character->x_curr == bull->character->x_curr
					&& el->character->y_curr == bull->character->y_curr
					&& el->character->duration != 0) {
					bull->character->duration = 0; /* Termina il proiettile.. */
					kill_guard(game, el); /* .. e 'uccide' la guardia */
					/* Inutile continuare perche' gia'
					mi sono assicurato che non ci sono
					piu' guardie nella stessa posizione */
					write_event(area->desc, game, "Should I check if you have a gun license?!");
					modify_score(game, 150);
					break;
				}
				k++; /* Procedi alla prossima guardia */
			}
		i++; /* Procedi al prossimo proiettile */
	}
}

void show_bullets(WINDOW *where, GAMESTATUS *game) {
	int i;
	char dirch;
	ENEMY *el;
	if(!game->bullets)
		return;
	/* Stampa tutti i proiettili nella loro posizione attuale! */
	i = 0;
	while(i<game->bullets->heapsize) {
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->bullets, i)))->content);
		if(el->character->duration == 0) {
			i++;
			continue;
		}
		switch(el->visibilita) {
			case 0:
				dirch = '^';
			break;
			case 1:
				dirch = '>';
			break;
			case 2:
				dirch = 'v';
			break;
			default:
				dirch = '<';
			break;
		}
		/* Rimuovi la rappresentazione grafica del proiettile corrente */
		/* dalla posizione attuale.. */
		wmove(where, el->character->x_curr, el->character->y_curr);
		wattron(where,COLOR_PAIR(1)); 
		waddch(where, CORRIDOR); /* ..ripristinando il corridoio */
		wattroff(where,COLOR_PAIR(1));

		/* Inserisci il proiettile nella nuova posizione */
		wmove(where, el->character->x_next, el->character->y_next);
		wattron(where, COLOR_PAIR(el->character->color_pair) | A_REVERSE);
		waddch(where, dirch);
		wattroff(where, COLOR_PAIR(el->character->color_pair) | A_REVERSE);

		i++;
	}
}

void move_bullet(WINDOW *where, GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	int x, y;
	/* Scandisci i turni interni del personaggio */
	enemy->character->duration -= 1;
	if(enemy->character->velocita_tmp > 1) {
		enemy->character->velocita_tmp -= 1;
		return;
	} else if(enemy->character->velocita == 0) {
		return;
	}
	enemy->character->velocita_tmp = enemy->character->velocita;
	x = enemy->character->x_curr;
	y = enemy->character->y_curr;
	switch(enemy->visibilita) {
		case 0:
			x--;
		break;
		case 1:
			y++;
		break;
		case 2:
			x++;
		break;
		case 3:
			y--;
		break;
	}
	x = fix_circular(x, game->lab->righe);
	y = fix_circular(y, game->lab->colonne);
	enemy->character->x_next = x;
	enemy->character->y_next = y;
	if(iswall(game->lab->maze[x][y])) {
		game->lab->maze[x][y] = CORRIDOR;
		game->lab->maze[enemy->character->x_curr][enemy->character->y_curr] = CORRIDOR;
		write_event(where, game, "Let me understand.. I've built this perfect maze and you are destroying it?!");
		enemy->character->duration = 0;

		/* Elimino un eventuale porta che era incastrata nel muro che ho appena distrutto */
		remove_door(game, x-1, y); /* In remove_door mi assicuro che non vado fuori range del maze */
		remove_door(game, x+1, y); /* In remove_door mi assicuro che non vado fuori range del maze */
		remove_door(game, x, y-1); /* In remove_door mi assicuro che non vado fuori range del maze */
		remove_door(game, x, y+1); /* In remove_door mi assicuro che non vado fuori range del maze */
	} else if(game->lab->maze[x][y] == DOOR) {
		game->lab->maze[enemy->character->x_curr][enemy->character->y_curr] = CORRIDOR;
		remove_door(game, x, y);
		write_event(where, game, "Let me understand.. I've built this perfect maze and you are destroying it?!");
		enemy->character->duration = 0;
	}
}

void make_bulletsmove(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac) {
	int i;
	ENEMY *el;
	i = 0;
	while(i<game->bullets->heapsize) { /* Scorro tutti i proiettili */
		el = (ENEMY *) (((struct coppia *) (game->bullets->get_el(game->bullets, i)))->content); /* Per ogni proiettile.. */
		if(el->character->duration != 0) /* Se non e' inattiva.. */
			move_bullet(area->desc, game, el, pac); /* ..la muovo */
		i++;
	}
}

void init_bullets(GAMESTATUS *game) {
	game->bullets = alloca_heap(game->bullets);
	setHeap(game->bullets, 4, 2, 1);
	/* setHeap inizializza il tipo di Heap, in particolare:
	4: SOLO PER SCOPI INTERNI (non permette stampe e input da utente),
	2: implementazione come albero
	1: realizza un MIN HEAP */
	game->bullets->cmp = &cmpenemy;
	game->bullets->libera = &free_enemy;
	init_heap(game->bullets, 0); /* 0 e' la dimensione iniziale del MIN HEAP */
}