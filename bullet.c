#include "bullet.h"
#include <math.h>


/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE                                          */
/* ------------------------------------------------------------ */
// void bullet(int pipeOUT, border border, vettore direzione, coordinate startingPoint)
void *bullet(void* args){
  bulletArguments bulletArgs = *((bulletArguments*)args);
  coordinate startingPoint = bulletArgs.startingPoint;
  vettore direzione = bulletArgs.direzione;
  borders border = bulletArgs.border;
  bool stop = false;
  int i=0;
  border.maxx--;
  /* Inizializzazione coordinate */
  coordinate coords = startingPoint;
  coords.threadID = pthread_self();
  /* prev_coordinate già inizializzate dalla funzione chiamante */
  int numMovimenti = abs(direzione.x) + abs(direzione.y); /* Numero di passi da eseguire */
  int changeAfter;
  if(direzione.y == 0){
    changeAfter = border.maxx;
  }else{
    if(abs(direzione.x) > abs(direzione.y)){
      changeAfter = abs(direzione.x/direzione.y);
    }else{
      changeAfter = abs(direzione.y/direzione.x);
    }
  }
  coords.prev_coordinate.x = coords.x;
  coords.prev_coordinate.y = coords.y;
  
  /* ---------------- Ciclo principale ---------------- */
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
    if(coords.x < 1 || coords.x >= border.maxx || coords.y < 1 || coords.y >= border.maxy){
      coords.x = -2;
      stop = true;
    }
    /* Scrivo coordinate nel pipe */
    addUpdate(coords);

    coords.prev_coordinate.x = coords.x;
    coords.prev_coordinate.y = coords.y;

    napms(DELAY_MS*(DEFAULT_SPEED*numMovimenti));
  }
}