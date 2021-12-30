#include "bullet.h"
#include <math.h>

void bullet(int pipeOUT, borders borders, vettore direzione, coordinate startingPoint){
  bool stop = false;
  int i=0;

  /* Inizializzazione coordinate */
  coordinate coords = startingPoint;
  coords.PID = getpid();
  double mov_x = coords.x;
  double mov_y = coords.y;
  /* prev_coordinate già inizializzate dalla funzione chiamante */
  int numMovimenti = abs(direzione.x) + abs(direzione.y); /* Numero di passi da eseguire */
  int changeAfter;
  if(abs(direzione.x) > abs(direzione.y)){
    changeAfter = abs(direzione.x/direzione.y);
  }else{
    changeAfter = abs(direzione.y/direzione.x);
  }
  while(!stop){
    /* Aggiorno coordinate */
    if(i == changeAfter){
      if(abs(direzione.x) > abs(direzione.y)){
        if(direzione.y < 0) coords.y--; else coords.y++;
      }else{
        if(direzione.x < 0) coords.x--; else coords.x++;
      }
      i=0;
    }else{
      i++;
      if(abs(direzione.x) > abs(direzione.y)){
        if(direzione.x < 0) coords.x--; else coords.x++;
      }else{
        if(direzione.y < 0) coords.y--; else coords.y++;
      }
    }
    
    /* Controllo se è fuori dallo schermo */
    if(coords.x < 1 || coords.x >= borders.maxx || coords.y < 1 || coords.y >= borders.maxy){
      coords.x = -2;
      stop = true;
    }
    /* Scrivo coordinate nel pipe */
    write(pipeOUT, &coords, sizeof(coordinate));

    coords.prev_coordinate.x = coords.x;
    coords.prev_coordinate.y = coords.y;

    napms(DELAY_MS*(DEFAULT_SPEED*numMovimenti));
  }
}