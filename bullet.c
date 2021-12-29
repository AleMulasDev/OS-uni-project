#include "bullet.h"

void bullet(int pipeOUT, borders borders, vettore direzione, coordinate startingPoint){
  bool stop = false;
  
  /* Inizializzazione coordinate */
  coordinate coords = startingPoint;
  coords.emitter=BULLET;
  coords.PID = getpid();
  // prev_coordinate già inizializzate dalla funzione chiamante

  while(!stop){
    /* Aggiorno coordinate */
    coords.x += direzione.x;
    coords.y += direzione.y;
    /* Controllo se è fuori dallo schermo */
    if(coords.x < 0 || coords.x >= borders.maxx || coords.y < 0 || coords.y >= borders.maxy){
      stop = true;
    }
    /* Scrivo coordinate nel pipe */
    write(pipeOUT, &coords, sizeof(coordinate));

    coords.prev_coordinate.x = coords.x;
    coords.prev_coordinate.y = coords.y;

    napms(DELAY_MS);
  }
}