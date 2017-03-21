#ifndef GAME_DEFINED
#include "struct_def.h"
#endif

void random_move(GAMESTATUS *game, ENEMY *enemy);
void bfs_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void dijkstra_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void astar_toward_pac(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
int coordinate_successive(struct labyrinth *lab, int *s, int *v, int ***pred, int *vai_in_x, int *vai_in_y);
void init_enemy(GAMESTATUS *game);
int cmpenemy(void *a, void *b);
void add_random_enemy(GAMESTATUS *game);
void add_random_police1(GAMESTATUS *game);
void add_random_soldier1(GAMESTATUS *game);
void show_enemies(WINDOW *where, GAMESTATUS *game, CHARACTER *pac);
int enemy_can_go(GAMESTATUS *game, int x, int y);
void free_enemy(void *tmp);
void stop_all_enemies(GAMESTATUS *game);
void move_police1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void move_soldier1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void move_astar1(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void make_enemiesmove(GAMESTATUS *game, CHARACTER *pac);
int enemy_sees_you(GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
int getpeso_maze_for_enemy(struct labyrinth *lab, int x, int y);
void blink_enemy(WINDOW *where, ENEMY *el);
void kill_guard(GAMESTATUS *game, ENEMY *el);
void add_enemy_here(int x, int y, GAMESTATUS *game, void *algo, int CHARx, int color_pair, int duration, int velocita, int visibilita);
void add_enemy(GAMESTATUS *game, void *algo, int CHARx, int color_pair, int duration, int velocita, int visibilita);
void add_random_sniper1(GAMESTATUS *game);
void add_random_astar1(GAMESTATUS *game);