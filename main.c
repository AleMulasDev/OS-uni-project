#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <locale.h>
#include <fcntl.h>
#include "spacecraft.h"

// ------------------------------------------------------------
// DEFINIZIONE MACRO

#define DELETE_COLOR 1
#define SPACECRAFT_COLOR 2
#define BULLET_COLOR 3

// ------------------------------------------------------------
// DEFINIZIONE GLOBALI

char projectile = '#';

// ------------------------------------------------------------
// DEFINIZIONE PROTOTIPI

void game(int pipeIN, int pipeOUT, borders borders);

// ------------------------------------------------------------
// DEFINIZIONE MAIN

int main(){
  /* Inizializzazioni varie        */
  setlocale(LC_ALL, "");
  borders borders;

  int position_pipe[2];          /* Pipe processi -> main*/
  int hit_pipe[2];               /* Pipe main -> processi*/
  int PIDSpacecraft;             /* PID del processo Spacecraft */

  /* Inizializzazione pipe         */
  pipe(position_pipe);
  pipe(hit_pipe);
  int flags = fcntl(hit_pipe[0], F_GETFL, 0);
  fcntl(hit_pipe[0], F_SETFL, flags | O_NONBLOCK);  /* Imposto la lettura della pipe non bloccante */

  /* Inizializzazione ncurses      */
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, borders.maxy, borders.maxx);
  box(stdscr, ACS_VLINE, ACS_HLINE);

  /* Inizializzo il vettore del proiettile in base al rapporto di aspetto del terminale */
  /* Necessario per permettere di colpire anche i nemici in fondo a destra              */
  /* Aggiungo 2 per compensare per eventuali troncature di interi                       */
  if(borders.maxx > borders.maxy){
    RIGHT_UP.x = (borders.maxx/borders.maxy)+2;
    RIGHT_DOWN.x = RIGHT_UP.x;
  }else{
    RIGHT_UP.y = (borders.maxy/borders.maxx)+2;
    RIGHT_DOWN.y = RIGHT_UP.y;
  }
  // Compenso per la presenza del box
  borders.maxx-=2;
  borders.maxy-=2;

  /* Inizializzo i colori          */
  start_color();
  init_pair(DELETE_COLOR, COLOR_BLACK, COLOR_BLACK);
  init_pair(SPACECRAFT_COLOR, COLOR_GREEN, COLOR_BLACK);
  init_pair(BULLET_COLOR, COLOR_YELLOW, COLOR_RED);


  /* Creazione processo Spacecraft */
  PIDSpacecraft = fork();
  if(PIDSpacecraft == 0){
    close(position_pipe[0]);         /* Chiusura della lettura   */
    close(hit_pipe[1]);              /* Chiusura della scrittura */
    spacecraft(hit_pipe[0], position_pipe[1], borders);
  }else{
    close(position_pipe[1]);         /* Chiusura della scrittura */
    close(hit_pipe[0]);              /* Chiusura della lettura   */
    game(position_pipe[0], hit_pipe[1], borders);
    waitpid(PIDSpacecraft, NULL, 0); /* Attesa del processo Spacecraft */
    while(wait(NULL) > 0); // Attendo la terminazione dei processi figli
    endwin();
  }
  
}

// ------------------------------------------------------------
// FUNZIONE DI GIOCO

void game(int pipeIN, int pipeOUT, borders borders){
  int life = 3;
  int i=0;
  coordinate update;

  while(life > 0){
    
    read(pipeIN, &update, sizeof(coordinate));
    switch(update.emitter){
      case SPACECRAFT:
        if(update.x == -1) return; /* Se coordinata x = -1 allora il processo Spacecraft è terminato */
        
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<SPACECRAFT_SPRITE_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%6s", "");
        }

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(SPACECRAFT_COLOR));
        for(i=0; i<SPACECRAFT_SPRITE_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", spriteSpacecraft[i]);
        }             
      break;

      case BULLET:
        attron(COLOR_PAIR(DELETE_COLOR));
        mvprintw(update.prev_coordinate.y, update.prev_coordinate.x, "%c", " ");  /* Cancello la precedente posizione del proiettile */
        attron(COLOR_PAIR(BULLET_COLOR));
        mvprintw(update.y, update.x, "%c", projectile);                           /* Stampo nella nuova posizione del proiettile     */
    }

    refresh();
  }
}