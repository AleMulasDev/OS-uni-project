#include "bullet.h"

void bullet(int pipeOUT, borders borders, vettore direzione, coordinate startingPoint){
  bool stop = false;

  /* Inizializzazione coordinate */
  coordinate coords = startingPoint;
  coords.PID = getpid();
  float mov_x = coords.x;
  float mov_y = coords.y;
  /* prev_coordinate già inizializzate dalla funzione chiamante */

  while(!stop){
    /* Aggiorno coordinate */
    mov_x += (direzione.x * direzione.speed);
    mov_y += (direzione.y * direzione.speed);
    coords.x = mov_x; 
    coords.y = mov_y;
    /* Controllo se è fuori dallo schermo */
    if(coords.x < 1 || coords.x >= borders.maxx || coords.y < 1 || coords.y >= borders.maxy){
      coords.x = -2;
      stop = true;
    }
    /* Scrivo coordinate nel pipe */
    write(pipeOUT, &coords, sizeof(coordinate));

    coords.prev_coordinate.x = coords.x;
    coords.prev_coordinate.y = coords.y;

    napms(DELAY_MS*DEFAULT_SPEED);
  }
}