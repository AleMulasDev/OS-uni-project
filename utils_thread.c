#include "utils_thread.h"

int insert_hitBuffer = 0;
int remove_hitBuffer = 0;
int insert_posBuffer = 0;
int remove_posBuffer = 0;

void initThreadUtils(){
  /* Inizializzazioni buffer       */
  position_buffer = (coordinate*)malloc(sizeof(coordinate)*BUFFER_SIZE);
  hit_buffer = (hitUpdate*)malloc(sizeof(hitUpdate)*BUFFER_SIZE);
  enemiesBuffer = (hitUpdate*)malloc(sizeof(hitUpdate)*MAX_ENEMIES*ENEMY_BUFFER_SIZE);

  /* Inizializzazioni mutex        */
  pthread_mutex_init(&positionMutex, NULL);
  pthread_mutex_init(&getPositionMutex, NULL);
  pthread_mutex_init(&hitMutex, NULL);

  /* Inizializzazioni semafori     */
  sem_init(&semPosBuffer, 0, 0);
  sem_init(&semHitBuffer, 0, 0);
  sem_init(&semHitBufferFull, 0, BUFFER_SIZE-1);
  sem_init(&semPosBufferFull, 0, BUFFER_SIZE-1);
}

void addHit(hitUpdate hitAction){
  sem_wait(&semHitBufferFull);
  pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */

  /* Buffer non pieno */
  hit_buffer[insert_hitBuffer] = hitAction;
  insert_hitBuffer = (insert_hitBuffer+1)%BUFFER_SIZE;
  sem_post(&semHitBuffer);
  
  pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
  return;
}

hitUpdate getHit(){
  sem_wait(&semHitBuffer);
  pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */
  hitUpdate hitAction = hit_buffer[remove_hitBuffer];
  remove_hitBuffer = (remove_hitBuffer+1)%BUFFER_SIZE;
  sem_post(&semHitBufferFull);
  pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
  return hitAction;
}

void addUpdate(coordinate update){
  sem_wait(&semPosBufferFull);
  pthread_mutex_lock(&positionMutex); /* positionMutex LOCK */
  /* Buffer non pieno */
  position_buffer[insert_posBuffer] = update;
  insert_posBuffer = (insert_posBuffer+1)%BUFFER_SIZE;
  sem_post(&semPosBuffer);
  pthread_mutex_unlock(&positionMutex); /* positionMutex UNLOCK */
  return;
}

coordinate getUpdate(){
  coordinate update;
  sem_wait(&semPosBuffer);
  pthread_mutex_lock(&getPositionMutex); /* getPositionMutex LOCK */
  update = position_buffer[remove_posBuffer];
  remove_posBuffer = (remove_posBuffer+1)%BUFFER_SIZE;
  sem_post(&semPosBufferFull);
  pthread_mutex_unlock(&getPositionMutex); /* getPositionMutex UNLOCK */
  return update;
}

void addEnemyUpdate(hitUpdate hitAction, enemyThread enemyThread){
  int* index;
  sem_wait((enemyThread.semaphoreFull));
  pthread_mutex_lock((enemyThread.mutex)); /* enemyThread.mutex LOCK */
  index = enemyThread.index;
  /* Buffer non pieno */
  enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+*index] = hitAction;
  *index = (*index+1)%ENEMY_BUFFER_SIZE;
  sem_post((enemyThread.semaphore));

  pthread_mutex_unlock((enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
  return;
}

hitUpdate getEnemyUpdate(enemyThread enemyThread){
  int* index;
  hitUpdate hitAction;
  sem_wait((enemyThread.semaphore));
  pthread_mutex_lock((enemyThread.mutex)); /* enemyThread.mutex LOCK */
  index = enemyThread.index;
  hitAction = enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+(*index)];
  *index = (*index+1)%ENEMY_BUFFER_SIZE;
  sem_post((enemyThread.semaphoreFull));
  pthread_mutex_unlock((enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
  return hitAction;
}

hitUpdate getEnemyUpdateNB(enemyThread enemyThread){
  int* index;
  hitUpdate hitAction;
  if(sem_trywait((enemyThread.semaphore)) == 0){
    pthread_mutex_lock((enemyThread.mutex)); /* enemyThread.mutex LOCK */
    index = enemyThread.index;
    hitAction = enemiesBuffer[(enemyThread.enemyNumber*ENEMY_BUFFER_SIZE)+(*index)];
    *index=(*index+1)%ENEMY_BUFFER_SIZE;
    sem_post((enemyThread.semaphoreFull));
    pthread_mutex_unlock((enemyThread.mutex)); /* enemyThread.mutex UNLOCK */
    return hitAction;
  }
  hitAction.hitting.x = -2;
  return hitAction;
}