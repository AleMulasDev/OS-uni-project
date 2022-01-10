#ifndef ENEMY_H
#define ENEMY_H

#include "bullet.h"
#include "utils_thread.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE COSTANTI                                         */

extern int ENEMY_SPRITE_1_WIDTH;
extern int ENEMY_SPRITE_2_WIDTH;
extern int ENEMY_SPRITE_1_HEIGHT;
extern int ENEMY_SPRITE_2_HEIGHT;
extern char* sprite1Enemy[];
extern char* sprite2Enemy[];
#define ENEMY_MOV_SPEED 7 /* Velocità di movimento, più basso = più veloce */
#define BOMB_SPAWN_CHANCE 0.03 /* Probabilità di spawn di una bomba */
#define BOMB_SPAWN_DELAY 8000  /* Dopo quanti ms dalla prima bomba ne deve spawnare un'altra */
#define N_HIT_LV2_ENEMY 2

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void *enemy(void* args);

#endif