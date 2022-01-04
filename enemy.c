#include "enemy.h"

int ENEMY_SPRITE_1_WIDTH=6;
int ENEMY_SPRITE_2_WIDTH=3;
int ENEMY_SPRITE_1_HEIGHT=4;
int ENEMY_SPRITE_2_HEIGHT=2;
char* sprite1Enemy[] = {"<\\==\\ ","  /][\\","  \\][/","</==/ "};
char* sprite2Enemy[] = {"/==|","\\==|"};

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint){
  coordinate report;
  hitUpdate receivedUpdate;
  int elapsed = 0;
  int level = 1;
  int PID;
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
        write(pipe.pipeOUT, &report, sizeof(coordinate));
        return;
      }
      switch(receivedUpdate.hitting.emitter){
        case BULLET:
          report.x = -1;
          stop = true;
          close(pipe.pipeIN);
          write(pipe.pipeOUT, &report, sizeof(coordinate));
          close(pipe.pipeOUT);
          return;
          break;

        case ENEMY:
          direzione.y = direzione.y * -1;
          break;
        }
      }
      elapsed += (DELAY_MS*ENEMY_MOV_SPEED)/11;
      napms((DELAY_MS*ENEMY_MOV_SPEED)/11); /* Piccola attesa per restare al corrente degli update */
    }
    elapsed = 0;
    
    if(rand() % 100 < BOMB_SPAWN_CHANCE*100){
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
    }

    if(level == 1){
      write(pipe.pipeOUT, &report, sizeof(coordinate));
      if(stop) close(pipe.pipeOUT);
    }else{
      
    }
    napms(DELAY_MS*ENEMY_MOV_SPEED);
  }
}