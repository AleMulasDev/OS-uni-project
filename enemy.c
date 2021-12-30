#include "enemy.h"

int ENEMY_SPRITE_1_WIDTH=7;
int ENEMY_SPRITE_2_WIDTH=3;
int ENEMY_SPRITE_1_HEIGHT=4;
int ENEMY_SPRITE_2_HEIGHT=2;
char* sprite1Enemy[] = {"   /\\ ","/-|--|","\\-|--|","   \\/ "};
char* sprite2Enemy[] = {"/==|","\\==|"};

void enemy(enemyPipes pipe, borders borders, vettore direzione, coordinate_base startingPoint){
  coordinate report;
  report.PID = getpid();
  report.emitter = ENEMY;
  report.prev_coordinate.x = startingPoint.x;
  report.prev_coordinate.y = startingPoint.y;
  report.x = startingPoint.x;
  report.y = startingPoint.y;
  write(pipe.pipeOUT, &report, sizeof(report));
}