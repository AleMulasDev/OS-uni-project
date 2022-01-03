#include "enemies.h"

/* ------------------------------------------------------------ */
/* Funzione di utilità per calcolare il numero di nemici che ci */
/* stanno nello schermo                                         */
coordinate_base calculateNumEnemies(borders borders, coordinate_base startingPoint);
vettore generateRandomDirection();

/* ------------------------------------------------------------ */
/* Funzione principale                                          */
void enemies(int pipeIN, int pipeOUT, borders borders, int max_enemies, coordinate_base startingPoint){
  int** enemies_pipes = (int**)malloc(sizeof(int*)*max_enemies);
  enemyPipes* enemiesPipes = (enemyPipes*)malloc(sizeof(enemyPipes)*max_enemies);
  hitUpdate update;
  enemyPipes toChild;
  vettore direzione;
  int pipeToClose;
  int i;
  int pid;
  borders.maxx = borders.maxx - startingPoint.x - ENEMY_SPRITE_1_WIDTH;

  for(i=0; i<max_enemies; i++){
    enemies_pipes[i] = (int*)malloc(sizeof(int)*2);
    pipe(enemies_pipes[i]);
    int flags = fcntl(enemies_pipes[i][0], F_GETFL, 0);
    fcntl(enemies_pipes[i][0], F_SETFL, flags | O_NONBLOCK);  /* Imposto la lettura della pipe non bloccante */
    enemiesPipes[i].pipeOUT = pipeOUT;
    enemiesPipes[i].pipeIN = enemies_pipes[i][0];
  }
  coordinate_base numEnemies = calculateNumEnemies(borders, startingPoint);
  /* Spawno i nemici */
  int enemyCount = 0;
  coordinate_base offset_spawn = {0,0};
  while(enemyCount < max_enemies || enemyCount < numEnemies.x*numEnemies.y){
    toChild.pipeIN = enemiesPipes[enemyCount].pipeIN;
    toChild.pipeOUT = enemiesPipes[enemyCount].pipeOUT;
    pipeToClose = enemies_pipes[enemyCount][1];
    direzione = generateRandomDirection();
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
      enemy(toChild, borders, direzione, startingEnemyPoint);
      return;
    }else{
      enemiesPipes[enemyCount].PID_child = pid; /* Salvo il PID del processo figlio */
      close(enemies_pipes[enemyCount][0]);
      /* sposto il punto di spawn della prossima navicella nemica */
      if(offset_spawn.x + ENEMY_SPRITE_1_WIDTH+2 >= borders.maxx){
        offset_spawn.y += (ENEMY_SPRITE_1_HEIGHT+2);
        offset_spawn.x = 0;
      }else{
        offset_spawn.x += (ENEMY_SPRITE_1_WIDTH+2);
      }
      enemyCount++;
    }
  }

  while(enemyCount > 0){
    read(pipeIN, &update, sizeof(hitUpdate));
    if(update.hitting.x == -1 && update.hitting.emitter == SPACECRAFT){
      /* Chiudo tutti i processi figli */
      enemyCount=0;
      for(i=0; i<max_enemies; i++){
        if(enemiesPipes[i].PID_child != -1) write(enemies_pipes[i][1], &update, sizeof(hitUpdate));
      }
      break;
    }else{
      /* Controllo se è un update di chiusura processo figlio */
      if(update.beingHit.x == -1 && update.beingHit.emitter == ENEMY){
        for(i=0; i<max_enemies; i++){
          if(enemiesPipes[i].PID_child == update.beingHit.PID){
            enemiesPipes[i].PID_child = -1;
          }
        }
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

  while(wait(NULL) > 0); /* Attendo la terminazione dei processi figli */

  for(i=0; i<max_enemies; i++){
    free(enemies_pipes[i]);
  }
  free(enemies_pipes);
  free(enemiesPipes);
}


/* ------------------------------------------------------------ */
/* Funzioni di utilità                                          */
coordinate_base calculateNumEnemies(borders borders, coordinate_base startingPoint){
  coordinate_base numEnemies;
  borders.maxx -= startingPoint.x;
  borders.maxy -= startingPoint.y;
  borders.maxx -= ENEMY_SPRITE_1_WIDTH;
  /* Aggiungo 2 alla grandezza dello sprite per lasciare un bordo vuoto tra le navi */
  numEnemies.x = (borders.maxx - 1) / (ENEMY_SPRITE_1_WIDTH + 2);
  numEnemies.y = (borders.maxy - 1) / (ENEMY_SPRITE_1_HEIGHT + 2);
  return numEnemies;
}

vettore generateRandomDirection(){
  vettore direzione;
  direzione.x = 0;
  direzione.y = (rand() % 100) > 50 ? 1 : -1;
  direzione.speed = 1;
  return direzione;
}