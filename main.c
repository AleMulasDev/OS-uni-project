#include <stdio.h>
#include <locale.h>
#include "spacecraft.h"
#include "enemies.h"
#include "hitboxes.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE MACRO                                            */

#define SCOREBOARD_HEIGHT 3
#define ENEMY_LV1_POINT 50

#define DELETE_COLOR 1
#define SPACECRAFT_COLOR 2
#define BULLET_COLOR 3
#define ENEMY_COLOR 4
#define BKGD_COLOR 5
#define SCOREBOARD_COLOR 6

/* ------------------------------------------------------------ */
/* DEFINIZIONE GLOBALI                                          */

char projectile = '#';

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void game(int pipeIN, int pipeOUT, borders borders);

/* ------------------------------------------------------------ */
/* DEFINIZIONE MAIN                                             */

int main(){
  /* Inizializzazioni varie        */
  setlocale(LC_ALL, "");
  borders border;
  srand(time(NULL));
  initializeHistory(MAX_ENEMIES);

  int position_pipe[2];          /* Pipe processi -> main       */
  int hit_pipe[2];               /* Pipe main -> processi       */
  int PIDSpacecraft;             /* PID del processo Spacecraft */
  int PIDenemies;                /* PID del processo enemies    */

  /* Inizializzazione pipe         */
  pipe(position_pipe);
  pipe(hit_pipe);
  /*int flags = fcntl(hit_pipe[0], F_GETFL, 0);
  fcntl(hit_pipe[0], F_SETFL, flags | O_NONBLOCK);  /* Imposto la lettura della pipe non bloccante */

  /* Inizializzazione ncurses      */
  initscr();
  noecho();
  curs_set(0);
  keypad(stdscr, TRUE);
  getmaxyx(stdscr, border.maxy, border.maxx);
  box(stdscr, ACS_VLINE, ACS_HLINE);

  /* Inizializzo il vettore del proiettile in base al rapporto di aspetto del terminale */
  /* Necessario per permettere di colpire anche i nemici in fondo a destra              */
  /* Aggiungo 2 per compensare per eventuali troncature di interi                       */
  if(border.maxx > border.maxy){
    RIGHT_UP.x = (border.maxx/border.maxy)+2;
    RIGHT_DOWN.x = RIGHT_UP.x;
  }else{
    RIGHT_UP.y = (border.maxy/border.maxx)+2;
    RIGHT_DOWN.y = RIGHT_UP.y;
  }
  /* Compenso per la presenza del box */
  border.maxx-=2;
  border.maxy-=2;

  /* Compenso per la presenza del tabellone */
  border.maxy-=SCOREBOARD_HEIGHT;

  /* Inizializzo i colori          */
  start_color();
  init_pair(DELETE_COLOR, COLOR_BLACK, COLOR_BLACK);
  init_pair(SPACECRAFT_COLOR, COLOR_GREEN, COLOR_BLACK);
  init_pair(BULLET_COLOR, COLOR_YELLOW, COLOR_RED);
  init_pair(ENEMY_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(BKGD_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(SCOREBOARD_COLOR, COLOR_CYAN, COLOR_WHITE);
  bkgd(COLOR_PAIR(BKGD_COLOR));

  /* Creazione processo Spacecraft */
  PIDSpacecraft = fork();
  if(PIDSpacecraft == 0){
    close(position_pipe[0]);         /* Chiusura della lettura   */
    close(hit_pipe[1]);              /* Chiusura della scrittura */
    spacecraft(hit_pipe[0], position_pipe[1], border);
  }else{
    PIDenemies = fork();
    if(PIDenemies == 0){
      close(position_pipe[0]);         /* Chiusura della lettura   */
      close(hit_pipe[1]);              /* Chiusura della scrittura */
      coordinate_base startingPoint;
      startingPoint.y = 1; /* 1 per il bordo che è a y=0 */
      startingPoint.x = border.maxx/2;
      enemies(hit_pipe[0], position_pipe[1], border, MAX_ENEMIES, startingPoint);
    }else{
      close(position_pipe[1]);         /* Chiusura della scrittura */
      close(hit_pipe[0]);              /* Chiusura della lettura   */
      game(position_pipe[0], hit_pipe[1], border);
      while(wait(NULL) > 0); /* Attendo la terminazione dei processi figli */
      endwin();
    }
  } 
}

/* ------------------------------------------------------------ */
/* FUNZIONE DI GIOCO                                            */

void game(int pipeIN, int pipeOUT, borders border){
  borders realBorder;
  getmaxyx(stdscr, realBorder.maxy, realBorder.maxx);
  int life = 3;
  int score = 0;
  int i, j;
  i = j = 0;
  coordinate update;
  coordinate isHit;
  hitUpdate hitAction;           /* Struttura per l'aggiornamento delle hit */

  while(life > 0){
    attron(COLOR_PAIR(BKGD_COLOR));
    move(border.maxy, 0);
    addch(ACS_LTEE);
    for(i=1;i<realBorder.maxx; i++){
      move(border.maxy, i);
      addch(ACS_HLINE);
    }
    move(border.maxy, border.maxx+1);
    addch(ACS_RTEE);

    read(pipeIN, &update, sizeof(coordinate));
    switch(update.emitter){
      case SPACECRAFT:
        if(update.x == -1){
          /* Se coordinata x = -1 allora il processo Spacecraft è terminato */
          hitAction.hitting = update;
          write(pipeOUT, &hitAction ,sizeof(hitUpdate));
          return;
        }
        
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<SPACECRAFT_SPRITE_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%7s", " ");
        }

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(SPACECRAFT_COLOR));
        for(i=0; i<SPACECRAFT_SPRITE_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", spriteSpacecraft[i]);
        }             
        break;

      case BULLET:
        attron(COLOR_PAIR(DELETE_COLOR));
        /* Cancello la precedente posizione del proiettile */
        mvprintw(update.prev_coordinate.y, update.prev_coordinate.x, "%c", ' ');  
        attron(COLOR_PAIR(BULLET_COLOR));
        /* Stampo nella nuova posizione del proiettile     */
        mvprintw(update.y, update.x, "%c", projectile);                           
        break;
        
      case ENEMY:
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<ENEMY_SPRITE_1_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%7s", " ");
        }

        if(update.x == -1){
          /* Se coordinata x = -1 allora il processo enemies è terminato */
          hitAction.beingHit = update;
          write(pipeOUT, &hitAction ,sizeof(hitUpdate)); /* Avviso il processo enemies che uno dei suoi figli è terminato */
          break;
        } 

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(ENEMY_COLOR));
        for(i=0; i<ENEMY_SPRITE_1_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", sprite1Enemy[i]);
        }
        break;     
    }

    /* ------------------------------------------------------------ */
    /* Controllo HITBOX                                             */

    isHit = checkHitBox(update);
    if(isHit.PID != -1){
      /* Se PID diverso da -1 ho una hit */
      hitAction.beingHit = isHit;
      hitAction.hitting = update;
      switch (isHit.emitter) {
        case ENEMY:
          if(update.emitter == ENEMY){
            /* Collisione tra 2 nemici, avviso entrambi per farli rimbalzare */
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            hitAction.beingHit = update;
            hitAction.hitting = isHit;
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
          }else{
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            if(update.emitter == BULLET){
              score += ENEMY_LV1_POINT;
              kill(update.PID, SIGKILL);
              attron(COLOR_PAIR(DELETE_COLOR));
              mvprintw(update.y, update.x, "%c", ' ');
            }
          }
          break;
      }
    }

    /* ------------------------------------------------------------ */
    /* Aggiornamento SCOREBOARD                                     */

    attron(COLOR_PAIR(SCOREBOARD_COLOR));
    for(j=1; j<=SCOREBOARD_HEIGHT; j++){
      for(i=1;i<realBorder.maxx-1; i++){
        move(border.maxy+j, i);
        addch(' ');
      }
    }
    mvprintw(border.maxy+2, 2, "SCORE: %8d", score);

    refresh();
  }
}
