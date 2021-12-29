#include <stdio.h>
#include <stdlib.h>
#include "spacecraft.h"
#include <sys/wait.h>

// ------------------------------------------------------------
// DEFINIZIONE MACRO

#define DELETE_COLOR 1
#define SPACECRAFT_COLOR 2

// ------------------------------------------------------------
// DEFINIZIONE GLOBALI

int MAXX, MAXY;
int spriteHeight = 3;
int spriteWidth = 6;

// ------------------------------------------------------------
// DEFINIZIONE PROTOTIPI

void game(int pipeIN);

// ------------------------------------------------------------
// DEFINIZIONE MAIN

int main(){
  int position_pipe[2];          /* Pipe */
  int PIDSpacecraft;             /* PID del processo Spacecraft */

  /* Inizializzazione pipe */
  pipe(position_pipe);

  /* Inizializzazione ncurses */
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, MAXY, MAXX);
  /* Inizializzo i colori */
  start_color();
  init_pair(DELETE_COLOR, COLOR_BLACK, COLOR_BLACK);
  init_pair(SPACECRAFT_COLOR, COLOR_GREEN, COLOR_BLACK);


  /* Creazione processo Spacecraft */
  PIDSpacecraft = fork();
  if(PIDSpacecraft == 0){
    close(position_pipe[0]);         /* Chiusura della lettura */
    spacecraft(position_pipe[1], MAXY);
  }else{
    close(position_pipe[1]);         /* Chiusura della scrittura */
    game(position_pipe[0]);
    waitpid(PIDSpacecraft, NULL, 0); /* Attesa del processo Spacecraft */
    endwin();
  }
}

// ------------------------------------------------------------
// FUNZIONE DI GIOCO

void game(int pipeIN){
  int life = 3;
  coordinate update;
  coordinate oldUpdate;

  while(life > 0){
    
    read(pipeIN, &update, sizeof(coordinate));
    if(update.emitter == SPACECRAFT){
      if(update.x == -1) return;                      /* Se coordinata x = -1 allora il processo Spacecraft è terminato */

      attron(COLOR_PAIR(DELETE_COLOR));
      mvprintw(oldUpdate.y,oldUpdate.x, "%5s", " ");  /* Cancello la precedente posizione della nave */
      attron(COLOR_PAIR(SPACECRAFT_COLOR));
      mvprintw(update.y, update.x, "%s", spacecraftSprite);     /* Stampo nella nuova posizione della nave */
    }

    oldUpdate = update;
    refresh();
    napms(DELAY_MS);
  }
}


// ------------------------------------------------------------
// FUNZIONE DI UTILITÀ
