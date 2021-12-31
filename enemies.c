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
  /*int enemies_pipes[12][2];
  enemyPipes enemiesPipes[12];*/
  enemyPipes toChild;
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
  while(enemyCount < max_enemies){
    toChild.pipeIN = enemiesPipes[enemyCount].pipeIN;
    toChild.pipeOUT = enemiesPipes[enemyCount].pipeOUT;
    pid = fork();
    if(pid == 0){
      /* Nemico */
      for(i=0; i<max_enemies; i++){
        free(enemies_pipes[i]);
      }
      free(enemies_pipes);
      free(enemiesPipes);
      coordinate_base startingEnemyPoint = {startingPoint.x + offset_spawn.x, startingPoint.y + offset_spawn.y};
      enemy(toChild, borders, generateRandomDirection(), startingEnemyPoint);
      return;
    }else{
      enemiesPipes[enemyCount].PID_child = pid; // Salvo il PID del processo figlio

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
  /* Aggiungo 2 alla grandezza dello sprite per lasciare un bordo vuoto tra le navi */
  numEnemies.x = (borders.maxx - 1) / (ENEMY_SPRITE_1_WIDTH + 2);
  numEnemies.y = (borders.maxy - 1) / (ENEMY_SPRITE_1_HEIGHT + 2);
  return numEnemies;
}

vettore generateRandomDirection(){
  vettore direzione;
  direzione.x = (rand() % 2) ? 1 : -1;
  direzione.y = (rand() % 2) ? 1 : -1;
  direzione.speed = 1;
  return direzione;
}