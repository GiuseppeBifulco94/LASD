#ifndef GAME_DEFINED
#include "struct_def.h"
#endif

void init_pac(CHARACTER *pac, int x, int y);
void show_move(WINDOW *where, CHARACTER *pac);
void make_pacmove(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
void move_pac(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac, int dir);
void pac_on_items(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
void modify_score(GAMESTATUS *game, int item);
void pac_on_exit(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
int can_exit(GAMESTATUS *game, int x, int y);
void reverse_pac_dir(GAMESTATUS *game, char character);
void get_random_pac_xy(GAMESTATUS *game, int *x, int *y);
void stop(CHARACTER *pac);
void pac_fire(GAMESTATUS *game, CHARACTER *pac);
void pac_on_enemy(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
int is_pac_on_enemy(GAMESTATUS *game, CHARACTER *pac);
int getpeso_maze_for_pac(struct labyrinth *lab, int x, int y);
void bomb(GAMESTATUS *game, CHARACTER *pac);
void bullet_on_enemy(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
void init_bullets(GAMESTATUS *game);
void make_bulletsmove(FINESTRE *area, GAMESTATUS *game, CHARACTER *pac);
void move_bullet(WINDOW *where, GAMESTATUS *game, ENEMY *enemy, CHARACTER *pac);
void show_bullets(WINDOW *where, GAMESTATUS *game);