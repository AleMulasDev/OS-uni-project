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
  int pid;

  int** enemies_pipes = (int**)malloc(sizeof(int*)*max_enemies);
  enemyPipes* enemiesPipes = (enemyPipes*)malloc(sizeof(enemyPipes)*max_enemies);
  coordinate_base numEnemies = calculateNumEnemies(border, startingPoint);

  vettore randDirection = generateRandomDirection();

  hitUpdate update;
  enemyPipes toChild;
  vettore direzione;

  /* -------------- Inizializzo le pipes  -------------- */
  for(i=0; i<max_enemies; i++){
    enemies_pipes[i] = (int*)malloc(sizeof(int)*2);
    pipe(enemies_pipes[i]);
    int flags = fcntl(enemies_pipes[i][0], F_GETFL, 0);
    fcntl(enemies_pipes[i][0], F_SETFL, flags | O_NONBLOCK);  /* Imposto la lettura della pipe non bloccante */
    enemiesPipes[i].pipeOUT = pipeOUT;
    enemiesPipes[i].pipeIN = enemies_pipes[i][0];
  }

  /* ----------------- Spawno i nemici ----------------- */
  int enemyCount = 0;
  coordinate_base offset_spawn = {0,0};
  while(enemyCount < max_enemies){
    toChild.pipeIN = enemiesPipes[enemyCount].pipeIN;
    toChild.pipeOUT = enemiesPipes[enemyCount].pipeOUT;
    pipeToClose = enemies_pipes[enemyCount][1];
    direzione = randDirection;
    pid = fork();
    if(pid == 0){
      /* Nemico */
      close(pipeToClose);
      for(i=0; i<max_enemies; i++){
        free(enemies_pipes[i]);
      }
      free(enemies_pipes);
      free(enemiesPipes);
      coordinate_base startingEnemyPoint = {startingPoint.x + offset_spawn.x, startingPoint.y + offset_spawn.y};
      enemy(toChild, border, direzione, startingEnemyPoint);
      return;
    }else{
      enemiesPipes[enemyCount].PID_child = pid; /* Salvo il PID del processo figlio */
      close(enemies_pipes[enemyCount][0]);
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
    read(pipeIN, &update, sizeof(hitUpdate));
    if(update.hitting.x == -1 && update.hitting.emitter == SPACECRAFT){
      /* Chiudo tutti i processi figli */
      enemyCount=0;
      for(i=0; i<max_enemies; i++){
        if(enemiesPipes[i].PID_child != -1) write(enemies_pipes[i][1], &update, sizeof(hitUpdate));
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
            if(update.beingHit.PID == enemiesPipes[i].PID_child){
              write(enemies_pipes[i][1], &update, sizeof(hitUpdate));
              break;
            }
          }
        }
      }
    }
  }

   /* ------------- Chiusura del processo  ------------- */
  while(wait(NULL) > 0); /* Attendo la terminazione dei processi figli */

  for(i=0; i<max_enemies; i++){
    free(enemies_pipes[i]);
  }
  free(enemies_pipes);
  free(enemiesPipes);
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