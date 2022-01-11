#include "utils_thread.h"

int index_hitBuffer = 0;
int index_posBuffer = 0;

void addHit(hitUpdate hitAction){
  sem_wait(&semHitBufferFull);
  pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */

  /* Buffer non pieno */
  hit_buffer[index_hitBuffer] = hitAction;
  index_hitBuffer++;
  sem_post(&semHitBuffer);
  
  pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
  return;
}

hitUpdate getHit(){
  sem_wait(&semHitBuffer);
  pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */
  index_hitBuffer--;
  hitUpdate hitAction = hit_buffer[index_hitBuffer];
  sem_post(&semHitBufferFull);
  pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
  return hitAction;
}

void addUpdate(coordinate update){
  sem_wait(&semPosBufferFull);
  pthread_mutex_lock(&positionMutex); /* positionMutex LOCK */

  /* Buffer non pieno */
  position_buffer[index_posBuffer] = update;
  index_posBuffer++;
  sem_post(&semPosBuffer);

  pthread_mutex_unlock(&positionMutex); /* positionMutex UNLOCK */
  return;
}

coordinate getUpdate(){
  sem_wait(&semPosBuffer);
  pthread_mutex_lock(&positionMutex); /* positionMutex LOCK */
  index_posBuffer--;
  coordinate update = position_buffer[index_posBuffer];
  sem_post(&semPosBufferFull);
  pthread_mutex_unlock(&positionMutex); /* positionMutex UNLOCK */
  return update;
}

void addEnemyUpdate(hitUpdate hitAction, enemyThread enemyThread){
  int semValue;
  sem_wait(&(enemyThread.semaphoreFull));
  pthread_mutex_lock(&(enemyThread.mutex)); /* enemyThread.mutex LOCK */
  sem_getvalue(&(enemyThread.semaphore), &semValue);
  /* Buffer non pieno */
  enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+semValue] = hitAction;
  sem_post(&(enemyThread.semaphore));

  pthread_mutex_unlock(&(enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
  return;
}

hitUpdate getEnemyUpdate(enemyThread enemyThread){
  int semValue;
  hitUpdate hitAction;
  sem_wait(&(enemyThread.semaphore));
  pthread_mutex_lock(&(enemyThread.mutex)); /* enemyThread.mutex LOCK */
  sem_getvalue(&(enemyThread.semaphore), &semValue);
  hitAction = enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+semValue];
  sem_post(&(enemyThread.semaphoreFull));
  pthread_mutex_unlock(&(enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
  return hitAction;
}

hitUpdate getEnemyUpdateNB(enemyThread enemyThread){
  int semValue;
  hitUpdate hitAction;
  if(sem_trywait(&(enemyThread.semaphore)) == 0){
    pthread_mutex_lock(&(enemyThread.mutex)); /* enemyThread.mutex LOCK */
    sem_getvalue(&(enemyThread.semaphore), &semValue);
    hitAction = enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+semValue];
    sem_post(&(enemyThread.semaphoreFull));
    pthread_mutex_unlock(&(enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
    return hitAction;
  }
  hitAction.hitting.x = -2;
  return hitAction;
}