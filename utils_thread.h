#ifndef UTILS_THREAD_H
#define UTILS_THREAD_H

#include "utils_struct.h"


#define BUFFER_SIZE 64
#define ENEMY_BUFFER_SIZE 8

coordinate* position_buffer;
extern int index_posBuffer;
hitUpdate* hit_buffer;
extern int index_hitBuffer;
hitUpdate* enemiesBuffer;
pthread_t* toDelete;

/* Dichiarazione mutex           */
pthread_mutex_t	positionMutex;
pthread_mutex_t	getPositionMutex;
pthread_mutex_t	hitMutex;
pthread_mutex_t toDeleteMutex;
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
void addEnemyUpdate(hitUpdate hitAction, enemyThread enemyThread);
hitUpdate getEnemyUpdate(enemyThread enemyThread);
hitUpdate getEnemyUpdateNB(enemyThread enemyThread); /* NB = non-blocking */
bool checkDeletion(pthread_t threadID);
void addToDelete(pthread_t threadID);

#endif