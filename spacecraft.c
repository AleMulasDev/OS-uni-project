#include "spacecraft.h"

void spacecraft(int pipeOUT, int MAXY){
  bool quit = false;
  timeout(DELAY_MS);
  coordinate coords;
  coords.emitter=SPACECRAFT;
  coords.x = 0;
  coords.y = MAXY/2;
  fflush(stdin);
  while(!quit){
    int c = getch();
    switch(c){
      case KEY_UP:
        if(coords.y>0){       // Se non sono già in cima
          coords.y--;
        }
        break;
      case KEY_DOWN:
        if(coords.y<MAXY-1){  // Se non sono già in fondo
          coords.y++;
        }
        break;
      case 113: // q
        coords.x = -1;
        quit = true;
        break;
    }
    write(pipeOUT, &coords, sizeof(coordinate));
    napms(DELAY_MS);
  }
}