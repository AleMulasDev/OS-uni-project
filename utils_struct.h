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
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

/* ------------------------------------------------------------ */
/* Costanti comuni a più file                                   */

#define DELAY_MS 33        /* tempo di attesa in millisecondi */
#define DEFAULT_SPEED 0.06 /* velocità di default del vettore */
#define MAX_ENEMIES 20     /* massimo numero di nemici        */

/* ------------------------------------------------------------ */
/* Enumerazione per riconoscere l'emettitore delle coordinate   */

typedef enum {
  SPACECRAFT,
  ENEMY,
  ENEMY_LV2,
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
  pthread_t threadID;
  int x;
  int y;
  coordinate_base prev_coordinate;
  emitter_type emitter;
} coordinate;

typedef struct {
  coordinate beingHit;
  coordinate hitting;
} hitUpdate;

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
  pthread_mutex_t* mutex;
  sem_t* semaphore;
  sem_t* semaphoreFull;
  pthread_t threadID_Child;
  int enemyNumber;
  int* index;
} enemyThread;

/* ------------------------------------------------------------ */
/* Thread arguments                                             */

typedef struct {
  borders border;
  int max_enemies;
  coordinate_base startingPoint;
} enemiesArguments;

typedef struct {
  borders border;
  vettore direzione;
  coordinate_base startingPoint;
  enemyThread self;
} enemyArguments;

typedef struct {
  borders border;
  vettore direzione;
  coordinate startingPoint;
} bulletArguments;

#endif