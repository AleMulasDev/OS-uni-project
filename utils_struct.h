#ifndef UTILS_STRUCT_H
#define UTILS_STRUCT_H

/* ------------------------------------------------------------ */
/* Include comuni a più file                                    */

#include <stdbool.h>
#include <unistd.h>
#include <curses.h>
#include <malloc.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>

/* ------------------------------------------------------------ */
/* Costanti comuni a più file                                   */

#define DELAY_MS 33        /* tempo di attesa in millisecondi                                                */
#define DEFAULT_SPEED 0.06 /* velocità di default del vettore */

/* ------------------------------------------------------------ */
/* Enumerazione per riconoscere l'emettitore delle coordinate   */

typedef enum {
  SPACECRAFT,
  ENEMY,
  BULLET,
  BOMB
}emitter_type;

/* ------------------------------------------------------------ */
/* Gestione coordinate                                          */

typedef struct {
  int x;
  int y;
} coordinate_base;

typedef struct {
  int PID;
  int x;
  int y;
  coordinate_base prev_coordinate;
  emitter_type emitter;
} coordinate;

typedef struct {
  int maxx;
  int maxy;
} borders;

typedef struct {
  int x;
  int y;
  float speed;
} vettore;

extern vettore RIGHT_UP;
extern vettore RIGHT_DOWN;

typedef struct {
  int pipeIN;
  int pipeOUT;
  int PID_child;
} enemyPipes;

#endif