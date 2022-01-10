#include "utils_thread.h"

void addHit(hitUpdate hitAction){
  pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */

  if(index_hitBuffer == BUFFER_SIZE-1){
    /* Buffer pieno, uso il semaforo per attendere che si liberi spazio */
    pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
    sem_wait(&semHitBufferFull);
    pthread_mutex_lock(&hitMutex); /* HITMUTEX LOCK */
  }

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
  hitUpdate hitAction = hit_buffer[index_hitBuffer-1];
  index_hitBuffer--;
  sem_post(&semHitBufferFull);
  pthread_mutex_unlock(&hitMutex); /* HITMUTEX UNLOCK */
  return hitAction;
}

void addUpdate(coordinate update){
  pthread_mutex_lock(&positionMutex); /* positionMutex LOCK */
  
  if(index_posBuffer == BUFFER_SIZE-1){
    /* Buffer pieno, uso il semaforo per attendere che si liberi spazio */
    pthread_mutex_unlock(&positionMutex); /* positionMutex UNLOCK */
    sem_wait(&semPosBufferFull);
    pthread_mutex_lock(&positionMutex); /* positionMutex LOCK */
  }

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
  coordinate update = position_buffer[index_posBuffer-1];
  index_posBuffer--;
  sem_post(&semPosBufferFull);
  pthread_mutex_unlock(&positionMutex); /* positionMutex UNLOCK */
  return update;
}