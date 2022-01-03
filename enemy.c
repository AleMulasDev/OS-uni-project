#include "enemy.h"

int ENEMY_SPRITE_1_WIDTH=7;
int ENEMY_SPRITE_2_WIDTH=3;
int ENEMY_SPRITE_1_HEIGHT=4;
int ENEMY_SPRITE_2_HEIGHT=2;
char* sprite1Enemy[] = {"   /\\ ","/-|--|","\\-|--|","   \\/ "};
char* sprite2Enemy[] = {"/==|","\\==|"};

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint){
  coordinate report;
  hitUpdate receivedUpdate;
  int level = 1;
  report.PID = getpid();
  report.emitter = ENEMY;
  report.prev_coordinate.x = startingPoint.x;
  report.prev_coordinate.y = startingPoint.y;
  report.x = startingPoint.x;
  report.y = startingPoint.y;
  bool stop = false;
  while(!stop){
    if(read(pipe.pipeIN, &receivedUpdate, sizeof(hitUpdate)) > 0){
      if(receivedUpdate.hitting.x == -1 && receivedUpdate.hitting.emitter == SPACECRAFT){
        close(pipe.pipeIN);
        close(pipe.pipeOUT);
        return;
      }
      switch(receivedUpdate.hitting.emitter){
        case BULLET:
          report.x = -1;
          stop = true;
          close(pipe.pipeIN);
          break;

        case ENEMY:
          direzione.y = direzione.y * -1;
          break;
      }
    }
    
    if(report.y + direzione.y > 1 && report.y + direzione.y < borders.maxy - ENEMY_SPRITE_1_HEIGHT && !stop){
      report.prev_coordinate.x = report.x;
      report.prev_coordinate.y = report.y;
      /*report.y += direzione.y;*/
    }
    if(level == 1){
      write(pipe.pipeOUT, &report, sizeof(coordinate));
      if(stop) close(pipe.pipeOUT);
    }else{
      
    }
    napms(DELAY_MS);
  }
  /*
  report.prev_coordinate.x = startingPoint.x;
  report.prev_coordinate.y = startingPoint.y;
  report.x = startingPoint.x;
  report.y = startingPoint.y;
  write(pipe.pipeOUT, &report, sizeof(report));
  */
}