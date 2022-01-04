#ifndef ENEMY_H
#define ENEMY_H

#include "utils_struct.h"
#include "bullet.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE COSTANTI                                         */

extern int ENEMY_SPRITE_1_WIDTH;
extern int ENEMY_SPRITE_2_WIDTH;
extern int ENEMY_SPRITE_1_HEIGHT;
extern int ENEMY_SPRITE_2_HEIGHT;
extern char* sprite1Enemy[];
extern char* sprite2Enemy[];
#define ENEMY_MOV_SPEED 5 /* Velocità di movimento, più basso = più veloce */
#define BOMB_SPAWN_CHANCE 0.01 /* Probabilità di spawn di una bomba */

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint);

#endif