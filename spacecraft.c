#include "spacecraft.h"

int spriteSpacecraftHeight = 3;
int spriteSpacecraftWidth = 6;
char spriteSpacecraft[SPACECRAFT_SPRITE_SIZE] = "<>=<>";

void spacecraft(int pipeIN, int pipeOUT, borders borders){
  bool quit = false;
  int pid;
  timeout(DELAY_MS);

  /* Inizializzazione coordinate */
  coordinate coords;
  coords.emitter=SPACECRAFT;
  coords.x = 0;
  coords.y = borders.maxy/2;
  coords.PID = getpid();
  coords.prev_coordinate.x = coords.x;
  coords.prev_coordinate.y = coords.y;

  fflush(stdin);
  // ciclo principale
  while(!quit){
    int c = getch();
    switch(c){
      case KEY_UP:
        if(coords.y>0){       // Se non sono già in cima
          coords.y--;
        }
        break;
      case KEY_DOWN:
        if(coords.y<borders.maxy-1){  // Se non sono già in fondo
          coords.y++;
        }
        break;
      case 32: // Spazio
        pid = fork();
        if(pid == 0){
          coords.x = coords.x + spriteSpacecraftWidth;
          coords.prev_coordinate.x = coords.x;
          bullet(pipeOUT, borders, RIGHT_UP, coords);
          return;
        }else{
          pid = fork();
          if(pid == 0){
            coords.x = coords.x + spriteSpacecraftWidth;
            coords.prev_coordinate.x = coords.x;
            bullet(pipeOUT, borders, RIGHT_DOWN, coords);
            return;
          }
        }
        break;
      case 113: // q
        coords.x = -1;
        quit = true;
        break;
    }
    write(pipeOUT, &coords, sizeof(coordinate));
    coords.prev_coordinate.y = coords.y;
    napms(DELAY_MS);
  }
}