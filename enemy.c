#include "enemy.h"

int ENEMY_SPRITE_1_WIDTH=6;
int ENEMY_SPRITE_2_WIDTH=4;
int ENEMY_SPRITE_1_HEIGHT=4;
int ENEMY_SPRITE_2_HEIGHT=2;
char* sprite1Enemy[] = {"<\\==\\ ","  /][\\","  \\][/","</==/ "};
char* sprite2Enemy[] = {"<\\=\\","</=/"};

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint){
  coordinate report;
  coordinate tmpReport;
  hitUpdate receivedUpdate;
  int elapsed = 0;
  bool firstBombSpawned = false;
  int bombElapsed = 0;
  int level = 1;
  int hitCount[4] = {0,0,0,0};
  int PID;
  int index;
  coordinate bombSpawnPoint;
  vettore bombDirection;
  report.PID = getpid();
  report.emitter = ENEMY;
  report.prev_coordinate.x = startingPoint.x;
  report.prev_coordinate.y = startingPoint.y;
  report.x = startingPoint.x;
  report.y = startingPoint.y;
  bool stop = false;
  srand(getpid());
  while(!stop){
    report.prev_coordinate.x = report.x;
    report.prev_coordinate.y = report.y;
    while(elapsed < DELAY_MS*ENEMY_MOV_SPEED){
      if(read(pipe.pipeIN, &receivedUpdate, sizeof(hitUpdate)) > 0){
      if(receivedUpdate.hitting.x == -1 && receivedUpdate.hitting.emitter == SPACECRAFT){
        close(pipe.pipeIN);
        close(pipe.pipeOUT);
        return;
      }
      switch(receivedUpdate.hitting.emitter){
        case SPACECRAFT:
          report.x = -1;
          stop = true;
          close(pipe.pipeIN);
          write(pipe.pipeOUT, &report, sizeof(coordinate));
          close(pipe.pipeOUT);
          return;
        case BULLET:
          if(level == 1){
            /* Cancello la nave di 1 livello */
            level++;
            report.x = -1;
            write(pipe.pipeOUT, &report, sizeof(coordinate));
            /* Riporto la coordinata all'originale per i nemici di secondo livello */
            report.x = report.prev_coordinate.x;
            report.emitter = ENEMY_LV2;
          }else{
            /* Calcolo quale dei 4 nemici di secondo livello è riferito nell'update */
            /*
            ALTO SINISTRA index = 0    ALTO DESTRA index = 1
            BASSO SINISTRA index = 2   BASSO DESTRA index = 3
            */
            index = (receivedUpdate.hitting.x-report.x)%ENEMY_SPRITE_2_WIDTH;
            index += 2*((receivedUpdate.hitting.y-report.y)*ENEMY_SPRITE_2_HEIGHT);
            hitCount[index]++;
            if(hitCount[index] == 2){
              /* Cancello l'enemy di secondo livello */
              report.x = -1;
              write(pipe.pipeOUT, &report, sizeof(coordinate));
              /* Riporto la coordinata all'originale per i nemici di secondo livello */
              report.x = report.prev_coordinate.x;
            }
            /* Controllo se sono rimasti nemici in vita */
            for(index=0;index<4;index++){
              if(hitCount[index] < 2){
                break;
              }
            }
            stop = true;
          }
          break;

        case ENEMY:
          direzione.y = direzione.y * -1;
          break;
        }
      }
      elapsed += (DELAY_MS*ENEMY_MOV_SPEED)/11;
      napms((DELAY_MS*ENEMY_MOV_SPEED)/11); /* Piccola attesa per restare al corrente degli update */
    }
    bombElapsed += elapsed;
    elapsed = 0;
    
    /* Le bombe spawnano in modo random la prima volta, con una percentuale di probabilità
    ** definita in BOMB_SPAWN_CHANGE, dopo di ché spawnano ogni BOMB_SPAWN_DELAY
    ** per calcolare il delay sommo l'attesa a ogni ciclo principale + l'attesa effettuata 
    ** qua sopra, in attesa di aggiornamenti dal processo padre
    */
    if(rand() % 100 < BOMB_SPAWN_CHANCE*100 && !firstBombSpawned){
      firstBombSpawned = true;
      bombElapsed = 0;
      PID = fork();
      if(PID == 0){
        bombSpawnPoint = report;
        bombSpawnPoint.emitter = BOMB;
        bombSpawnPoint.x--;
        bombDirection = RIGHT_UP;
        bombDirection.y = 0;
        bombDirection.x = bombDirection.x * -1;
        bullet(pipe.pipeOUT, borders, bombDirection, bombSpawnPoint);
        return;
      }
    }

    if(firstBombSpawned && bombElapsed < BOMB_SPAWN_DELAY){
      bombElapsed += (DELAY_MS*ENEMY_MOV_SPEED);
    }else if(firstBombSpawned && bombElapsed >= BOMB_SPAWN_DELAY){
      bombElapsed = 0;
      PID = fork();
      if(PID == 0){
        bombSpawnPoint = report;
        bombSpawnPoint.emitter = BOMB;
        bombSpawnPoint.x--;
        bombDirection = RIGHT_UP;
        bombDirection.y = 0;
        bombDirection.x = bombDirection.x * -1;
        bullet(pipe.pipeOUT, borders, bombDirection, bombSpawnPoint);
        return;
      }
    }
    
    if(report.y + direzione.y < 2 || report.y + direzione.y > borders.maxy - ENEMY_SPRITE_1_HEIGHT){
      direzione.y = direzione.y * -1;
    }

    if(!stop){
      report.y += direzione.y;
      report.x += direzione.x;
    }

    if(level == 1){
      write(pipe.pipeOUT, &report, sizeof(coordinate));
      if(stop) close(pipe.pipeOUT);
    }else{
      for(index=0;index<4;index++){
        tmpReport = report;
        tmpReport.prev_coordinate.x += (index%2)*ENEMY_SPRITE_2_WIDTH + (index%2)*2;
        tmpReport.prev_coordinate.y += (index/2)*ENEMY_SPRITE_2_HEIGHT + (index/2)*2;
        tmpReport.x += (index%2)*ENEMY_SPRITE_2_WIDTH + (index%2)*2;
        tmpReport.y += (index/2)*ENEMY_SPRITE_2_HEIGHT + (index/2)*2;
        if(hitCount[index] < 2) write(pipe.pipeOUT, &tmpReport, sizeof(coordinate));
        if(stop) close(pipe.pipeOUT);
      }
    }
    napms(DELAY_MS*ENEMY_MOV_SPEED);
  }
}