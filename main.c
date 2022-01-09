#include <stdio.h>
#include <locale.h>
#include <string.h>
#include "spacecraft.h"
#include "enemies.h"
#include "hitboxes.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE MACRO                                            */

#define SCOREBOARD_HEIGHT 3
#define ENEMY_LV1_POINT 50
#define ENEMY_LV2_POINT 100

#define DELETE_COLOR 1
#define SPACECRAFT_COLOR 2
#define BULLET_COLOR 3
#define ENEMY_COLOR 4
#define BKGD_COLOR 5
#define SCOREBOARD_COLOR 6
#define BOMB_COLOR 7

/* ------------------------------------------------------------ */
/* DEFINIZIONE GLOBALI                                          */

char projectile = 'O';
int score = 0;
bool invincible = false;

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

int game(int pipeIN, int pipeOUT, borders borders);
/*
  Intero ritornato da game:
  -1: utente ha premuto Q per chiudere il gioco
  0 : sconfitta
  1 : vittoria
*/
void endgame(borders border, int gameEndingReason);

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
  int gameEndingReason;          /* Risultato della partita     */

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
  init_pair(BULLET_COLOR, COLOR_RED, COLOR_YELLOW);
  init_pair(ENEMY_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(BKGD_COLOR, COLOR_WHITE, COLOR_BLACK);
  init_pair(SCOREBOARD_COLOR, COLOR_CYAN, COLOR_WHITE);
  init_pair(BOMB_COLOR, COLOR_YELLOW, COLOR_RED);
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
      startingPoint.x = border.maxx - (border.maxx/4);
      enemies(hit_pipe[0], position_pipe[1], border, MAX_ENEMIES, startingPoint);
    }else{
      close(position_pipe[1]);         /* Chiusura della scrittura */
      close(hit_pipe[0]);              /* Chiusura della lettura   */
      gameEndingReason = game(position_pipe[0], hit_pipe[1], border);
      kill(PIDSpacecraft, SIGKILL);
      endgame(border, gameEndingReason);
      while(wait(NULL) > 0); /* Attendo la terminazione dei processi figli */
      endwin();
    }
  } 
}

/* ------------------------------------------------------------ */
/* FUNZIONE DI GIOCO                                            */

int game(int pipeIN, int pipeOUT, borders border){
  borders realBorder;
  getmaxyx(stdscr, realBorder.maxy, realBorder.maxx);
  int life = 3;
  int i, j;
  i = j = 0;
  coordinate update;
  coordinate isHit;
  hitUpdate hitAction;           /* Struttura per l'aggiornamento delle hit */

  attron(COLOR_PAIR(BKGD_COLOR));
  move(border.maxy, 0);
  addch(ACS_LTEE);
  for(i=1;i<realBorder.maxx; i++){
    move(border.maxy, i);
    addch(ACS_HLINE);
  }
  move(border.maxy, border.maxx+1);
  addch(ACS_RTEE);

  while(life > 0){
    read(pipeIN, &update, sizeof(coordinate));
    switch(update.emitter){
      case SPACECRAFT:
        if(update.x == -1){
          /* Se coordinata x = -1 allora il processo Spacecraft è terminato */
          hitAction.hitting = update;
          write(pipeOUT, &hitAction ,sizeof(hitUpdate));
          return -1;
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
          /* Se coordinata x = -1 allora la nave di primo livello è morta */
          hitAction.beingHit = update;
          /* Avviso il processo enemies che uno dei suoi figli è aumentato di livello */
          write(pipeOUT, &hitAction ,sizeof(hitUpdate)); 
          break;
        } 

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(ENEMY_COLOR));
        for(i=0; i<ENEMY_SPRITE_1_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", sprite1Enemy[i]);
        }
        break;
      case ENEMY_LV2:
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<ENEMY_SPRITE_2_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%4s", " ");
        }

        if(update.x == -1){
          /* Se coordinata x = -1 allora una nave di secondo livello è morta */
          hitAction.beingHit = update;
          /* Avviso il processo enemies che una nave di secondo livello è morta */
          write(pipeOUT, &hitAction ,sizeof(hitUpdate)); 
          break;
        } 

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(ENEMY_COLOR));
        for(i=0; i<ENEMY_SPRITE_2_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", sprite2Enemy[i]);
        }
        break;
      case BOMB:
        attron(COLOR_PAIR(DELETE_COLOR));
        /* Cancello la precedente posizione del proiettile */
        mvprintw(update.prev_coordinate.y, update.prev_coordinate.x, "%c", ' ');  
        attron(COLOR_PAIR(BOMB_COLOR));
        /* Stampo nella nuova posizione del proiettile     */
        mvprintw(update.y, update.x, "%c", projectile);                           
        break; 
    }

    /* ------------------------------------------------------------ */
    /* Controllo HITBOX                                             */

    if(update.x == 1 && (update.emitter == ENEMY || update.emitter == ENEMY_LV2)){
      /* Il nemico ha toccato il bordo sinistro */
      life=0;
      beep();
    }

    isHit = checkHitBox(update);
    if(isHit.PID != -1){
      /* Se PID diverso da -1 ho una hit */
      hitAction.beingHit = isHit;
      hitAction.hitting = update;
      switch (isHit.emitter) {
        case ENEMY_LV2:
        case ENEMY:
          if(update.emitter == ENEMY || update.emitter == ENEMY_LV2){
            /* Collisione tra 2 nemici, avviso entrambi per farli rimbalzare */
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            hitAction.beingHit = update;
            hitAction.hitting = isHit;
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
          }else{
            write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            if(update.emitter == BULLET){
              if(isHit.emitter == ENEMY)     score += ENEMY_LV1_POINT;
              if(isHit.emitter == ENEMY_LV2) score += ENEMY_LV2_POINT;
              kill(update.PID, SIGKILL); /* Cancello il proiettile */
              attron(COLOR_PAIR(DELETE_COLOR));
              mvprintw(update.y, update.x, "%c", ' ');
            }
          }
          break;
        case SPACECRAFT:
          if(update.emitter == BOMB){
            life--;
            beep();
            kill(update.PID, SIGKILL);
            attron(COLOR_PAIR(DELETE_COLOR));
            mvprintw(update.y, update.x, "%c", ' ');
            if(life == 0){
              hitAction.hitting.emitter = SPACECRAFT;
              hitAction.hitting.x = -1;
              attron(COLOR_PAIR(DELETE_COLOR));
              write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            }
          }else{
            if(update.emitter == ENEMY || update.emitter == ENEMY_LV2){
              /* Forzo la chiusura dei nemici perché uno collide con la navicella madre */
              life=0;
              beep();
              hitAction.hitting = isHit;
              hitAction.beingHit = update;
              hitAction.hitting.x = -1;
              write(pipeOUT, &hitAction ,sizeof(hitUpdate));
            }
          }
      }
    }
    if(invincible) life = 3;
    if(life==0){
      /* Se la navicella è morta, avviso il processo enemies che è terminato */
      hitAction.hitting.emitter = SPACECRAFT;
      hitAction.hitting.x = -1;
      write(pipeOUT, &hitAction ,sizeof(hitUpdate));
    }
    if(update.x == -1 && update.emitter == ENEMY_LV2){
      /* Un nemico è morto, controllo gli altri */
      if(areThereEnemies() == false){
        return 1; /* Vittoria */
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
    mvprintw(border.maxy+2, border.maxx/2-8, "LIFE:");
    for(j=0; j<life; j++){
      for(i=0; i<SPACECRAFT_SPRITE_HEIGHT; i++){
        mvprintw(border.maxy+i+1, border.maxx/2+j*(SPACECRAFT_SPRITE_WIDTH+4), "%s", spriteSpacecraft[i]);
      }
    }

    refresh();
  }
  return 0; /* Sconfitta */
} /* Fine funzione game */

/* ------------------------------------------------------------ */
/* FUNZIONE DI ENDGAME                                          */

void endgame(borders border, int gameEndingReason){
  if(gameEndingReason == -1) return;
  int i;
  int j;
  char pushToCloseString[] = "Premere un tasto per chiudere...";
  int pushToCloseStringLength = strlen(pushToCloseString);
  char scoreObtainedString[] = "Punteggio ottenuto: %d";
  int scoreObtainedStringLength = strlen(scoreObtainedString);
  char youLost[] = "Hai perso...";
  int youLostLength = strlen(youLost);
  char youWin[] = "Hai vinto!";
  int youWinLength = strlen(youWin);
  int xStarting = (border.maxx/2)/2;
  int yStarting = (border.maxy/2)/2;

  /* Pulisco la zona centrale dello schermo */
  for(i=0;i<border.maxx/2; i++){
    for(j=0; j<border.maxy/2; j++){
      attron(COLOR_PAIR(SCOREBOARD_COLOR));
      mvprintw(yStarting+j, xStarting+i, "%c", ' ');
    }
  }

  if(gameEndingReason == 0){
    /* Sconfitta */
    mvprintw((border.maxy/2)-2, (border.maxx/2)-(youLostLength/2), youLost);
  }else if(gameEndingReason == 1){
    /* Vittoria */
    mvprintw((border.maxy/2)-2, (border.maxx/2)-(youWinLength/2), youWin);
  }  
  mvprintw((border.maxy/2)-1, (border.maxx/2)-(pushToCloseStringLength/2), pushToCloseString);
  mvprintw((border.maxy/2)+1, (border.maxx/2)-(scoreObtainedStringLength/2), scoreObtainedString, score);
  refresh();
  timeout(-1);
  fflush(stdin);
  getch();
}