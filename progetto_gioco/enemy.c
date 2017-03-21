#include <stdlib.h>
#include <string.h> 
#include <math.h>
#include <unistd.h>
#include "maze.h"
#include "heap.h"
#include "utility.h"
#include "enemy.h" /* Include gia' <curses>, struct_def.h e <time> */
#include "pac.h"



void free_enemy(void *tmp) {
	if(((struct coppia *)tmp)->content) {
		if( ((ENEMY *) (((struct coppia *)tmp)->content))->character)
			free(((ENEMY *) (((struct coppia *)tmp)->content))->character);
		free(((struct coppia *)tmp)->content);
	}
	((struct coppia *)tmp)->content = NULL;
	if(tmp)
		free(tmp);
}

void kill_guard(GAMESTATUS *game, ENEMY *el) {
	if(!el)
		return;
	if(!el->character)
		return;
	el->character->duration = 0;
	/* Invece di deallocare volta per volta ogni guardia,
	la nascondo, non computo piu' alcun suo movimento,
	la dealloco al termine del livello */

	/* Rimuovi la rappresentazione grafica della guardia */
	game->lab->maze[el->character->x_curr][el->character->y_curr] = CORRIDOR;
}
void astar_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {

	int dadove[1], adove[1];
	int **dist;
	int ***pred;
	int i, j;
	int vai_in_x, vai_in_y;
	/* Se non sono stati inizializzati ancora guardia o main character.. esci */
	if(!enemy)
		return;
	if(!enemy->character)
		return;
	if(!pac)
		return;

	vai_in_x = -1;
	vai_in_y = -1;

	/* Inizializza dist, col, pred per A* */
	dist = malloc(sizeof(int*)*game->lab->righe);
	pred = malloc(sizeof(int**)*game->lab->righe); 
	for(i=0;i<game->lab->righe;i++) {
		dist[i] = malloc(sizeof(int)*game->lab->colonne);
		pred[i] = malloc(sizeof(int*)*game->lab->colonne);
		for(j=0;j<game->lab->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}
	/* Sorgente */
	dadove[0] = enemy->character->x_curr;
	dadove[1] = enemy->character->y_curr;

	/* Destinazione */
	adove[0] = pac->x_curr;
	adove[1] = pac->y_curr;

	/* Esegui A* sul labirinto */
	Astar(game->lab, dadove[0], dadove[1], pac->x_curr, pac->y_curr, dist, pred);

	/* Calcola delle coordinate dove andare */
	coordinate_successive(game->lab, dadove, adove, pred, &vai_in_x, &vai_in_y);

	if(!enemy_can_go(game, vai_in_x, vai_in_y)) { /* C'e' gia' una guardia la'! */
		vai_in_x = -1;
		vai_in_y = -1;
	}

	/* Se vai_in_x e' una coordinata contenuta all'interno del maze */
	if(vai_in_x > -1 && vai_in_y > -1 && vai_in_x < game->lab->righe-1 && vai_in_y < game->lab->colonne-1) {
		enemy->character->x_next = vai_in_x;
		enemy->character->y_next = vai_in_y;
	} else {
		//random move..
		random_move(game, enemy);
	}

	/* Dealloca tutto il materiale precedentemente allocato */
	for(i=0;i<game->lab->righe;i++) {
		free(dist[i]);
		for(j=0;j<game->lab->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(pred);


}

void dijkstra_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {

	int dadove[1], adove[1];
	int **dist;
	int ***pred;
	int i, j;
	int vai_in_x, vai_in_y;
	/* Se non sono stati inizializzati ancora guardia o main character.. esci */
	if(!enemy)
		return;
	if(!enemy->character)
		return;
	if(!pac)
		return;

	vai_in_x = -1;
	vai_in_y = -1;

	/* Inizializza dist, col, pred per Dijkstra */
	dist = malloc(sizeof(int*)*game->lab->righe);
	pred = malloc(sizeof(int**)*game->lab->righe); 
	for(i=0;i<game->lab->righe;i++) {
		dist[i] = malloc(sizeof(int)*game->lab->colonne);
		pred[i] = malloc(sizeof(int*)*game->lab->colonne);
		for(j=0;j<game->lab->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}
	/* Sorgente */
	dadove[0] = enemy->character->x_curr;
	dadove[1] = enemy->character->y_curr;

	/* Destinazione */
	adove[0] = pac->x_curr;
	adove[1] = pac->y_curr;

	/* Esegui Dijkstra sul labirinto */
	Dijkstra(game->lab, dadove[0], dadove[1], dist, pred);

	/* Calcola delle coordinate dove andare */
	coordinate_successive(game->lab, dadove, adove, pred, &vai_in_x, &vai_in_y);

	if(!enemy_can_go(game, vai_in_x, vai_in_y)) { /* C'e' gia' una guardia la'! */
		vai_in_x = -1;
		vai_in_y = -1;
	}

	/* Se vai_in_x e' una coordinata contenuta all'interno del maze */
	if(vai_in_x > -1 && vai_in_y > -1 && vai_in_x < game->lab->righe-1 && vai_in_y < game->lab->colonne-1) {
		enemy->character->x_next = vai_in_x;
		enemy->character->y_next = vai_in_y;
	} else {
		//random move..
		random_move(game, enemy);
	}

	/* Dealloca tutto il materiale precedentemente allocato */
	for(i=0;i<game->lab->righe;i++) {
		free(dist[i]);
		for(j=0;j<game->lab->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(pred);


}
void bfs_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	int dadove[1], adove[1];
	int **dist, **col;
	int ***pred;
	int i, j;
	int vai_in_x, vai_in_y;
	/* Se non sono stati inizializzati ancora guardia o main character.. esci */
	if(!enemy)
		return;
	if(!enemy->character)
		return;
	if(!pac)
		return;

	vai_in_x = -1;
	vai_in_y = -1;

	/* Inizializza dist, col, pred per la BFS */
	dist = malloc(sizeof(int*)*game->lab->righe);
	col = malloc(sizeof(int*)*game->lab->righe);
	pred = malloc(sizeof(int**)*game->lab->righe); 
	for(i=0;i<game->lab->righe;i++) {
		dist[i] = malloc(sizeof(int)*game->lab->colonne);
		col[i] = malloc(sizeof(int)*game->lab->colonne);
		pred[i] = malloc(sizeof(int*)*game->lab->colonne);
		for(j=0;j<game->lab->colonne;j++)
			pred[i][j] = malloc(sizeof(int)*2); 
	}
	/* Sorgente */
	dadove[0] = enemy->character->x_curr;
	dadove[1] = enemy->character->y_curr;

	/* Destinazione */
	adove[0] = pac->x_curr;
	adove[1] = pac->y_curr;

	/* Esegui BFS sul labirinto */
	BFS(game->lab, dadove, dist, col, pred);

	// Calcola delle coordinate dove andare //
	coordinate_successive(game->lab, dadove, adove, pred, &vai_in_x, &vai_in_y);

	if(!enemy_can_go(game, vai_in_x, vai_in_y)) { /* C'e' gia' una guardia la'! */
		vai_in_x = -1;
		vai_in_y = -1;
	}

	/* Se vai_in_x e' una coordinata contenuta all'interno del maze */
	if(vai_in_x > -1 && vai_in_y > -1 && vai_in_x < game->lab->righe-1 && vai_in_y < game->lab->colonne-1) {
		enemy->character->x_next = vai_in_x;
		enemy->character->y_next = vai_in_y;
	} else {
		//random move..
		random_move(game, enemy);
	}

	/* Dealloca tutto il materiale precedentemente allocato */
	for(i=0;i<game->lab->righe;i++) {
		free(dist[i]);
		free(col[i]);
		for(j=0;j<game->lab->colonne;j++)
			free(pred[i][j]);
		free(pred[i]);
	}
	free(dist);
	free(col);
	free(pred);
}
int coordinate_successive(struct labyrinth *lab, int *s, int *v, int ***pred, int *vai_in_x, int *vai_in_y) {
	int appoggio[1];
	int t[1];
	int done;
	*vai_in_x = -1;
	*vai_in_y = -1;
	done = 0;
	if(s[0] == v[0] && s[1] == v[1]) { // uguali.. resta fermo
		*vai_in_x = s[0];
		*vai_in_y = s[1];
	}
	t[0] = v[0];
	t[1] = v[1];
	while(!done) {
		if(pred[v[0]][v[1]][0] == -1) { //Non eiste il percorso
			*vai_in_x = -1;
			*vai_in_y = -1;
			done = 1;
		} else if(s[0] == v[0] && s[1] == v[1]) { // ora sono uguali
			done = 1;
		} else {
			*vai_in_x = v[0];
			*vai_in_y = v[1];
			appoggio[0] = pred[v[0]][v[1]][0];
			appoggio[1] = pred[v[0]][v[1]][1];
			v[0] = appoggio[0];
			v[1] = appoggio[1];
		}
	}
	v[0] = t[0];
	v[1] = t[1];
}

int getpeso_maze_for_enemy(struct labyrinth *lab, int x, int y) {
	int ret;
	if(x < 0 || x >= lab->righe || y < 0 || y >= lab->colonne) {
		//printf("Out of cordinate.\n");
		return -1;
	} else {
		ret = 1;
		switch(lab->maze[x][y]) {
			case WALLH:
			case WALLV:
			case DOOR:
				ret = INFINITO; /* La guardia non puo' percorrere questi blocchi */
			break;
		}
		return ret;
	}
}
int enemy_sees_you(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	if(!enemy)
		return 0;
	if(!enemy->character)
		return 0;
	if( abs(enemy->character->x_curr - pac->x_curr) <= enemy->visibilita && 
		abs(enemy->character->y_curr - pac->y_curr) <= enemy->visibilita)
		/* La guardia ti riesce a vedere perche' sei
		all'interno del suo riquadro di visibilita' */
		return 1;
	else
		/* La guardia non ti riesce a vedere perche'
		sei troppo lontano rispetto a quanto la guardia
		riesce a vedere */
		return 0;
}
void make_enemiesmove(GAMESTATUS *game, CHARACTER *pac) {
	int i;
	ENEMY *el;
	i = 0;
	while(i<game->enemy->heapsize) { /* Scorro tutte le guardie */
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content); /* Per ogni guardia.. */
		if(el->character->duration != 0) /* Se non e' inattiva.. */
			el->move_enemy(game, el, pac); /* ..la muovo */
		i++;
	}
}
void random_move(GAMESTATUS *game, ENEMY *enemy) {
	int dir;
	int tentativi;
	int x, y;
	dir = rand() % 4;
	dir -= 1;
	tentativi = 0;
	enemy->character->x_next = enemy->character->x_curr;
	enemy->character->y_next = enemy->character->y_curr;
	while(tentativi < 4) {
		dir = fix_circular(dir+1, 4);
		switch(dir) {
			case 0:
				x = enemy->character->x_curr-1;
				y = enemy->character->y_curr;
			break;
			case 1:
				x = enemy->character->x_curr;
				y = enemy->character->y_curr+1;
			break;
			case 2:
				x = enemy->character->x_curr+1;
				y = enemy->character->y_curr;
			break;
			default:
				x = enemy->character->x_curr;
				y = enemy->character->y_curr-1;
			break;
		}
		tentativi++;
		x = fix_circular(x, game->lab->righe);
		y = fix_circular(y, game->lab->colonne);
		if(game->lab->maze[x][y] == DOOR || game->lab->maze[x][y] == WALLH ||
			game->lab->maze[x][y] == WALLV || game->lab->maze[x][y] == EXIT)
			continue;
		if(!enemy_can_go(game,x,y))
			continue;
		enemy->character->x_next = x;
		enemy->character->y_next = y;
		return;
	}
}
void move_police1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	/* Scandisci i turni interni del personaggio */
	if(enemy->character->velocita_tmp > 1) {
		enemy->character->velocita_tmp -= 1;
		DecreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
		return;
	} else if(enemy->character->velocita == 0) {
		return;
	}
	enemy->character->velocita_tmp = enemy->character->velocita;
	if(enemy_sees_you(game, enemy, pac)) { /* Se sei nel riquadro di visibilita' della guardia.. */
		bfs_toward_pac(game, enemy, pac);
	} else { /* ..altrimenti */
		random_move(game, enemy); /* Muovi in modo casuale la guardia */
	}
	IncreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
}
void move_soldier1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	/* Scandisci i turni interni del personaggio */
	if(enemy->character->velocita_tmp > 1) {
		enemy->character->velocita_tmp -= 1;
		DecreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
		return;
	} else if(enemy->character->velocita == 0) {
		return;
	}
	enemy->character->velocita_tmp = enemy->character->velocita;
	if(enemy_sees_you(game, enemy, pac)) { /* Se sei nel riquadro di visibilita' della guardia.. */
		dijkstra_toward_pac(game, enemy, pac);
	} else { /* ..altrimenti */
		random_move(game, enemy); /* Muovi in modo casuale la guardia */
	}
	IncreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
}
void move_astar1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac) {
	/* Scandisci i turni interni del personaggio */
	if(enemy->character->velocita_tmp > 1) {
		enemy->character->velocita_tmp -= 1;
		DecreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
		return;
	} else if(enemy->character->velocita == 0) {
		return;
	}
	if(enemy->character->duration > 0)
		enemy->character->duration -= 1;
	enemy->character->velocita_tmp = enemy->character->velocita;
	if(enemy_sees_you(game, enemy, pac)) { /* Se sei nel riquadro di visibilita' della guardia.. */
		astar_toward_pac(game, enemy, pac);
	} else { /* ..altrimenti */
		random_move(game, enemy); /* Muovi in modo casuale la guardia */
	}
	IncreaseKey(game->enemy, heapgetnumfromel(game->enemy, enemy), enemy->character->velocita_tmp);
}

void stop_all_enemies(GAMESTATUS *game) {
	int i;
	ENEMY *el;
	i = 0;
	while(i<game->enemy->heapsize) { /* Scorro tutte le guardie */
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content); /* Per ogni guardia.. */
		if(el->character->duration != 0)
			stop(el->character);
		i++;
	}
}

int enemy_can_go(GAMESTATUS *game, int x, int y) {
	int i;
	ENEMY *el;
	i = 0;
	while(i<game->enemy->heapsize) { /* Scorro tutte le guardie */
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content); /* Per ogni guardia.. */
		if(el->character->duration == 0) {
			i++;
			continue;
		}
		/* Se la posizione della guardia corrente e' (x,y).. */
		if(el->character->x_curr == x && el->character->y_curr == y)
			return 0; /* Ritorna falso! */
		/* Se la posizione in cui si spostera' la guardia corrente e' (x,y).. */
		if(el->character->x_next == x && el->character->y_next == y)
			return 0; /* Ritorna falso! */

		i++;
	}
	/* Se arrivo fino a qui non
	ho incontrato nessuna guardia
	in quella coordinata, quindi
	la guardia puo' andarci */
	return 1;
	/* Dovro' poi verificare che non sia un muro! */
}
void blink_enemy(WINDOW *where, ENEMY *el) {
	/* Fa lampeggiare la guardia se ti sta inseguendo */
	wmove(where, el->character->x_next, el->character->y_next);
	wattron(where, COLOR_PAIR(el->character->color_pair) | A_BLINK | A_REVERSE);
	waddch(where, el->character->character);
	wattroff(where, COLOR_PAIR(el->character->color_pair) | A_BLINK | A_REVERSE);
}
void show_enemies(WINDOW *where, GAMESTATUS *game, CHARACTER *pac) {
	int i;
	ENEMY *el;
	if(!game->enemy)
		return;
	/* Stampa tutte le guardie nella loro posizione attuale! */
	i = 0;
	while(i<game->enemy->heapsize) {
		el = (ENEMY *) (((struct coppia *) (game->enemy->get_el(game->enemy, i)))->content);
		if(el->character->duration == 0) {
			i++;
			continue;
		}
		/* Rimuovi la rappresentazione grafica della guardia corrente */
		/* dalla posizione attuale.. */
		wmove(where, el->character->x_curr, el->character->y_curr);
		wattron(where,COLOR_PAIR(1)); 
		waddch(where, CORRIDOR); /* ..ripristinando il corridoio */
		wattroff(where,COLOR_PAIR(1));

		/* Inserisci la guardia nella nuova posizione */
		wmove(where, el->character->x_next, el->character->y_next);
		wattron(where, COLOR_PAIR(el->character->color_pair) | A_REVERSE);
		waddch(where, el->character->character);
		wattroff(where, COLOR_PAIR(el->character->color_pair) | A_REVERSE);

		if(enemy_sees_you(game, el, pac)) /* Se una guardia ti sta inseguendo, la guardia lampeggiera' */
			blink_enemy(where, el);

		i++;
	}
}
void get_random_enemy_xy(GAMESTATUS *game, int *x, int *y) {
	int max_tentativi;
	int range;
	/* Numero massimo di tentativi per aggiungere la guardia
	e' necessario per evitare loop nel caso
	di generazione di labirinti senza possibilita'
	di aggiunta di porte, oppure nel caso
	di voler aggiungere troppe porte */
	max_tentativi = 100; *x = game->x_ingresso; *y = game->y_ingresso;
	/* Range minimo lontano dal main character
	in cui posizionare la guardia  */
	range = 6;
	/* Calcola una coppia randomica (x,y) libera per poter posizionare la guardia */
	/* Per vedere se la coppia e' libera dobbiamo vedere che sia un corridoio.. */
	/* .. ed inoltre non vi devono essere altre guardie */
	while(game->lab->maze[*x][*y] != CORRIDOR ||
		!enemy_can_go(game,*x,*y) ||
		(*x == game->x_ingresso && *y == game->y_ingresso) ||
		abs(*x-game->x_ingresso) < range ||
		abs(*y-game->y_ingresso) < range) {
		max_tentativi--;
		if(max_tentativi < 0) {
			*x = -1;
			*y = -1;
			return;
		}
		*x = rand() % game->lab->righe; *y = rand() % game->lab->colonne;
		if(*x+1 < game->lab->righe) {
			/* Non collocare i nemici sopra l'apertura di una porta */
			if(game->lab->maze[*x+1][*y] == DOOR) {
				*x = game->x_ingresso; *y = game->y_ingresso;
			}
		}
		if(*x-1 > -1) {
			/* Non collocare i nemici sotto l'apertura di una porta */
			if(game->lab->maze[*x-1][*y] == DOOR) {
				*x = game->x_ingresso; *y = game->y_ingresso;
			}
		}
	}
}
void add_random_enemy(GAMESTATUS *game) {
	/* Sceglie in modo casuale quale nemico inserire nel labirinto */
	int r;
	int x;
	x = game->level * 10;
	if(x > 100)
		x = 100;
	r = rand() % (200 + x);
	if(r < 100 || r > 219)
		/* Si muove random se non sei nel suo raggio d'azione (5),
		altrimenti bfs verso di te */
		add_random_police1(game);
	else if(r < 180)
		/* Si muove random se non sei nel suo raggio d'azione (6),
		altrimenti dijkstra verso di te */
		add_random_soldier1(game);
	else if(r < 200)
		/* Si muove sempre verso di te con la bfs */
		add_random_sniper1(game);
	else if(r < 215)
		/* Si muove sempre verso di te con A star */
		add_random_astar1(game);
}
void add_enemy_here(int x, int y, GAMESTATUS *game, void *algo, int CHARx, int color_pair, int duration, int velocita, int visibilita) {
	ENEMY *new_enemy;

	new_enemy = malloc(sizeof(ENEMY));
	new_enemy->character = malloc(sizeof(CHARACTER));

	/* Id identificativo della guardia (per comparare se due guardie sono uguali) */
	new_enemy->id_enemy = game->enemy->heapsize;
	
	/* Puntatore a funzione dell'algoritmo che muove la guardia  */
	new_enemy->move_enemy = algo;

	/* Definizione grafica (carattere - schema di colori) della guardia */
	new_enemy->character->character = CHARx; new_enemy->character->color_pair = color_pair;

	/* Imposta le coordinate di partenza */
	new_enemy->character->x_curr = x; new_enemy->character->y_curr = y;

	/* Imposta le coordinate del prossimo turno */
	new_enemy->character->x_next = x; new_enemy->character->y_next = y;

	new_enemy->character->duration = duration; /* Dura all'infinito */

	/* Imposta la velocita' del personaggio (si muove ogni TOT turni) */
	/* In questo modo potremo accelerare o meno il personaggio e */
	/* possiamo creare guardie piu' veloci */
	new_enemy->character->velocita = velocita; new_enemy->character->velocita_tmp = velocita;

	/* Impostiamo la visibilita' della guardia */
	new_enemy->visibilita = visibilita;

	/* In base alla loro velocita' avranno piu' o menu priorita' */
	InsertKey(game->enemy, new_enemy, new_enemy->character->velocita);

}
void add_enemy(GAMESTATUS *game, void *algo, int CHARx, int color_pair, int duration, int velocita, int visibilita) {
	int x, y;

	x = -1; y = -1;
	get_random_enemy_xy(game, &x, &y);

	/* Se la coppia randomica generata e' (-1,-1)
	allora l'algoritmo ha ecceduto il numero di
	tentativi massimi */
	if(x == -1 && y == -1) return;

	add_enemy_here(x, y, game, algo, CHARx, color_pair, duration, velocita, visibilita);
}

void add_random_sniper1(GAMESTATUS *game) {
	/* Argomenti della funzione add_enemy:
	1. Struttura del gioco,
	2. Puntatore alla funzione contenente l'algoritmo di movimento,
	3. Carattere di rappresentazione,
	4. Coppia di colori per la rappresentazione,
	5. Durata (-1 infinito),
	6. Velocita',
	7. Visibilita' */
	add_enemy(game, &move_police1, SNIPER, 16, -1, 15, game->lab->righe > game->lab->colonne ? game->lab->righe-1 : game->lab->colonne-1);
}

void add_random_astar1(GAMESTATUS *game) {
	/* Argomenti della funzione add_enemy:
	1. Struttura del gioco,
	2. Puntatore alla funzione contenente l'algoritmo di movimento,
	3. Carattere di rappresentazione,
	4. Coppia di colori per la rappresentazione,
	5. Durata (-1 infinito),
	6. Velocita',
	7. Visibilita' */
	add_enemy(game, &move_astar1, ASTAR, 19, -1, 20, game->lab->righe > game->lab->colonne ? game->lab->righe-1 : game->lab->colonne-1);
}

void add_random_soldier1(GAMESTATUS *game) {
	/* Argomenti della funzione add_enemy:
	1. Struttura del gioco,
	2. Puntatore alla funzione contenente l'algoritmo di movimento,
	3. Carattere di rappresentazione,
	4. Coppia di colori per la rappresentazione,
	5. Durata (-1 infinito),
	6. Velocita',
	7. Visibilita' */
	add_enemy(game, &move_soldier1, SOLDIER, 18, -1, 20, 6);
}

void add_random_police1(GAMESTATUS *game) {
	/* Argomenti della funzione add_enemy:
	1. Struttura del gioco,
	2. Puntatore alla funzione contenente l'algoritmo di movimento,
	3. Carattere di rappresentazione,
	4. Coppia di colori per la rappresentazione,
	5. Durata (-1 infinito),
	6. Velocita',
	7. Visibilita' */
	add_enemy(game, &move_police1, POLICE, 13, -1, 15, 5);
}

int cmpenemy(void *a, void *b) {
	int id_a, id_b;
	if(a == NULL || b == NULL) {
		return -1;
	}
	id_a = (*(ENEMY *)a).id_enemy;
	id_b = (*(ENEMY *)b).id_enemy;
	if(id_a == id_b)
		return 0;
	else if(id_a > id_b)
		return 1;
	else
		return -1;
}
void init_enemy(GAMESTATUS *game) {
	game->enemy = alloca_heap(game->enemy);
	setHeap(game->enemy, 4, 2, 1);
	/* setHeap inizializza il tipo di Heap, in particolare:
	4: SOLO PER SCOPI INTERNI (non permette stampe e input da utente),
	2: implementazione come albero
	1: realizza un MIN HEAP */
	game->enemy->cmp = &cmpenemy;
	game->enemy->libera = &free_enemy;
	init_heap(game->enemy, 0); /* 0 e' la dimensione iniziale del MIN HEAP */
}