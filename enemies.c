#include "enemies.h"

int SPACE_BETWEEN_X = 16;
int SPACE_BETWEEN_Y = 8;

/* ------------------------------------------------------------ */
/* PROTOTIPI                                                    */
/* ------------------------------------------------------------ */

/* Funzione di utilità per calcolare il numero di nemici che ci */
/* stanno nello schermo                                         */
coordinate_base calculateNumEnemies(borders border, coordinate_base startingPoint);

/* Funzione per generare una direzione random                   */
vettore generateRandomDirection();



/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE                                          */
/* ------------------------------------------------------------ */
void *enemies(void *args){
  enemiesArguments enArgs = *((enemiesArguments*)args);
  int max_enemies = enArgs.max_enemies;
  borders border = enArgs.border;
  coordinate_base startingPoint = enArgs.startingPoint;
  int i;
  int pipeToClose;

  enemyThread* enemiesThreads = (enemyThread*)malloc(sizeof(enemyThread)*max_enemies);
  coordinate_base numEnemies = calculateNumEnemies(border, startingPoint);

  vettore randDirection = generateRandomDirection();

  hitUpdate update;
  vettore direzione;

  /* ---------- Inizializzo i semafori/mutex  ---------- */
  for(i=0; i<max_enemies; i++){
    pthread_mutex_init(&(enemiesThreads[i].mutex), NULL);
    sem_init(&(enemiesThreads[i].semaphore), 0, 0);
    sem_init(&(enemiesThreads[i].semaphoreFull), 0, ENEMY_BUFFER_SIZE);
  }

  /* ----------------- Spawno i nemici ----------------- */
  int enemyCount = 0;
  coordinate_base offset_spawn = {0,0};
  coordinate_base startingEnemyPoint;
  enemyArguments enemyArgs;
  while(enemyCount < max_enemies){
    enemiesThreads[enemyCount].enemyNumber = enemyCount;
    startingEnemyPoint.x = startingPoint.x + offset_spawn.x;
    startingEnemyPoint.y = startingPoint.y + offset_spawn.y;
    enemyArgs.border = border;
    enemyArgs.startingPoint = startingEnemyPoint;
    enemyArgs.direzione = randDirection;
    enemyArgs.self = enemiesThreads[enemyCount];

    if(pthread_create(&(enemiesThreads[enemyCount].threadID_Child), NULL, enemy, (void*)&enemyArgs)){
      /* Errore nella creazione del thread Nemico */
      return NULL;
    }else{
      /* sposto il punto di spawn della prossima navicella nemica */
      enemyCount++;
      if(enemyCount%numEnemies.y == 0){
        offset_spawn.x += (ENEMY_SPRITE_1_WIDTH+SPACE_BETWEEN_X);
        offset_spawn.y = 0;
      }else{
        offset_spawn.y += (ENEMY_SPRITE_1_HEIGHT+SPACE_BETWEEN_Y);
      }
    }
  }

  /* ---------------- Comunico i messaggi tra il main e i singoli nemici ---------------- */
  while(enemyCount > 0){
    update = getHit();
    if(update.hitting.x == -1 && update.hitting.emitter == SPACECRAFT){
      /* Chiudo tutti i processi figli */
      enemyCount=0;
      for(i=0; i<max_enemies; i++){
        if(enemiesThreads[i].threadID_Child != -1) addEnemyUpdate(update, enemiesThreads[i]);
      }
    }else{
      /* Controllo se è un update di eliminazione nave di primo livello */
      if(update.beingHit.x == -1 && update.beingHit.emitter == ENEMY){
        enemyCount+=3; /* -1 nave di primo livello + 4 navi di secondo livello */
      }else{
        if(update.beingHit.x == -1 && update.beingHit.emitter == ENEMY_LV2){
          enemyCount--;
        }else{
          /* Cerco il figlio colpito e gli invio l'aggiornamento */
          for(i=0; i<max_enemies; i++){
            if(update.beingHit.threadID == enemiesThreads[i].threadID_Child){
              addEnemyUpdate(update, enemiesThreads[i]);
              break;
            }
          }
        }
      }
    }
  }

   /* ------------- Chiusura del processo  ------------- */
  while(wait(NULL) > 0); /* Attendo la terminazione dei processi figli */

  free(enemiesThreads);
}



/* ------------------------------------------------------------ */
/* FUNZIONI DI UTILITÀ                                          */
/* ------------------------------------------------------------ */

coordinate_base calculateNumEnemies(borders border, coordinate_base startingPoint){
  coordinate_base numEnemies;
  border.maxx = border.maxx - startingPoint.x;
  border.maxy = border.maxy - startingPoint.y;
  numEnemies.x = (border.maxx) / (ENEMY_SPRITE_1_WIDTH + SPACE_BETWEEN_X);
  numEnemies.y = (border.maxy) / (ENEMY_SPRITE_1_HEIGHT + SPACE_BETWEEN_Y);
  return numEnemies;
}

vettore generateRandomDirection(){
  vettore direzione;
  direzione.x = -1;
  direzione.y = (rand() % 100) > 50 ? 1 : -1;
  direzione.speed = 1;
  return direzione;
}