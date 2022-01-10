#ifndef UTILS_THREAD_H
#define UTILS_THREAD_H

#include "utils_struct.h"
#define BUFFER_SIZE 32

coordinate* position_buffer;
int index_posBuffer = 0;
int presenti_posBuffer = 0;
hitUpdate* hit_buffer;
int index_hitBuffer = 0;
int presenti_hitBuffer = 0;

/* Dichiarazione mutex           */
pthread_mutex_t	positionMutex;
pthread_mutex_t	hitMutex;
/* Dichiarazione semafori        */
sem_t semPosBuffer;
sem_t semPosBufferFull;
sem_t semHitBufferFull;
sem_t semHitBuffer;


/* ------------------------------------------------------------ */
/* Prototipi funzioni                                           */
void addHit(hitUpdate hitAction);
hitUpdate getHit();
void addUpdate(coordinate update);
coordinate getUpdate();

#endif