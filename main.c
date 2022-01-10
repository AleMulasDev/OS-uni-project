#include <stdio.h>
#include <locale.h>
#include <string.h>
#include "spacecraft.h"
#include "enemies.h"
#include "hitboxes.h"
#include "utils_thread.h"

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

int game(borders borders);
/*
  Intero ritornato da game:
  -2: errore nella creazione del thread
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

  /* Inizializzazioni buffer       */
  position_buffer = (coordinate*)malloc(sizeof(coordinate)*BUFFER_SIZE);
  hit_buffer = (hitUpdate*)malloc(sizeof(hitUpdate)*BUFFER_SIZE);
  enemiesBuffer = (hitUpdate*)malloc(sizeof(hitUpdate)*MAX_ENEMIES*ENEMY_BUFFER_SIZE);

  /* Dichiarazioni variabili       */
  pthread_t TIDSpacecraft;       /* Thread ID del processo Spacecraft */
  pthread_t TIDenemies;          /* Thread ID del processo enemies    */
  int gameEndingReason;          /* Risultato della partita           */

  /* Inizializzazioni mutex        */
  pthread_mutex_init(&positionMutex, NULL);
  pthread_mutex_init(&hitMutex, NULL);

  /* Inizializzazioni semafori     */
  sem_init(&semPosBuffer, 0, 0);
  sem_init(&semHitBufferFull, 0, BUFFER_SIZE);
  sem_init(&semPosBufferFull, 0, BUFFER_SIZE);
  sem_init(&semHitBuffer, 0, 0);

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

  /* ---------------- CREAZIONE THREAD ---------------- */
  
  /* Creazione thread Spacecraft */
  if(pthread_create(&TIDSpacecraft, NULL, spacecraft, (void*)&border)){
    endgame(border, -2);
    return; /* TODO AGGIUNGERE CONTROLLO CHE ENTRAMBI SIANO CHIUSI (enemies e spacecraft) */
  }
  
  /* Creazione thread enemies */
  enemiesArguments enemiesArg;
  enemiesArg.border = border;
  enemiesArg.max_enemies = MAX_ENEMIES;
  enemiesArg.startingPoint.y = 1;
  enemiesArg.startingPoint.x = border.maxx - (border.maxx/4);
  if(pthread_create(&TIDenemies, NULL, enemies, (void*)&enemiesArg)){
    endgame(border, -2);
    return; /* TODO AGGIUNGERE CONTROLLO CHE ENTRAMBI SIANO CHIUSI (enemies e spacecraft) */
  }

  gameEndingReason = game(border);
  pthread_join(TIDSpacecraft, NULL);
  /*pthread_join(TIDenemies, NULL);*/
  pthread_mutex_destroy(&positionMutex);
  pthread_mutex_destroy(&hitMutex);

  free(position_buffer);
  free(hit_buffer);
  free(enemiesBuffer);

  return;
}

/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE DEL GIOCO                                */
/* ------------------------------------------------------------ */
int game(borders border){

  /* ------------- Dichiarazioni variabili ------------- */
  borders realBorder;
  getmaxyx(stdscr, realBorder.maxy, realBorder.maxx);
  int life = 3;
  int lv1Killed = 0;
  int lv2Killed = 0;
  int i, j;
  i = j = 0;
  coordinate update;
  coordinate isHit;
  hitUpdate hitAction;           /* Struttura per l'aggiornamento delle hit */
  int mapSize = border.maxx - (border.maxx/4);
  mapSize += ((ENEMY_SPRITE_1_WIDTH+SPACE_BETWEEN_X)*MAX_ENEMIES);

  /* ------- Bordo tra il gioco e la scoreboard  ------- */
  attron(COLOR_PAIR(BKGD_COLOR));
  move(border.maxy, 0);
  addch(ACS_LTEE);
  for(i=1;i<realBorder.maxx; i++){
    move(border.maxy, i);
    addch(ACS_HLINE);
  }
  move(border.maxy, border.maxx+1);
  addch(ACS_RTEE);

  /* --------------------------------------------------- */
  /* ------------ CICLO DI GIOCO PRINCIPALE ------------ */
  /* --------------------------------------------------- */
  while(life > 0){
    sem_wait(&semPosBuffer);
    pthread_mutex_lock(&positionMutex);
    update = position_buffer[index_posBuffer];
    index_posBuffer--;
    pthread_mutex_unlock(&positionMutex);

    /* ---------- STAMPA DEGLI AGGIORNAMENTI  ---------- */
    switch(update.emitter){
      /*-------------------------------------------------*/
      case SPACECRAFT:  /* --------- SPACECRAFT ---------*/
        if(update.x == -1){
          /* Se coordinata x = -1 allora il processo Spacecraft è terminato */
          hitAction.hitting = update;
          addHit(hitAction);
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

      /*-------------------------------------------------*/
      case BULLET:  /* ----------- PROIETTILE -----------*/
        attron(COLOR_PAIR(DELETE_COLOR));
        /* Cancello la precedente posizione del proiettile */
        mvprintw(update.prev_coordinate.y, update.prev_coordinate.x, "%c", ' ');  
        attron(COLOR_PAIR(BULLET_COLOR));
        /* Stampo nella nuova posizione del proiettile     */
        mvprintw(update.y, update.x, "%c", projectile);                           
        break;

      /*-------------------------------------------------*/
      case ENEMY:  /* ------------- NEMICO  -------------*/
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<ENEMY_SPRITE_1_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%7s", " ");
        }

        if(update.x == -1){
          /* Se coordinata x = -1 allora la nave di primo livello è morta */
          hitAction.beingHit = update;
          lv1Killed++;
          /* Avviso il processo enemies che uno dei suoi figli è aumentato di livello */
          addHit(hitAction);
          break;
        } 

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(ENEMY_COLOR));
        for(i=0; i<ENEMY_SPRITE_1_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", sprite1Enemy[i]);
        }
        break;

      /*-------------------------------------------------*/
      case ENEMY_LV2:  /* --------- NEMICO LV2  ---------*/
        /* Cancello la precedente posizione della nave     */
        attron(COLOR_PAIR(DELETE_COLOR));
        for(i=0; i<ENEMY_SPRITE_2_HEIGHT; i++){
          mvprintw(update.prev_coordinate.y+i, update.prev_coordinate.x, "%4s", " ");
        }

        if(update.x == -1){
          /* Se coordinata x = -1 allora una nave di secondo livello è morta */
          hitAction.beingHit = update;
          lv2Killed++;
          /* Avviso il processo enemies che una nave di secondo livello è morta */
          addHit(hitAction);
          break;
        } 

        /* Stampo nella nuova posizione della nave         */
        attron(COLOR_PAIR(ENEMY_COLOR));
        for(i=0; i<ENEMY_SPRITE_2_HEIGHT; i++){
          mvprintw(update.y+i, update.x, "%s", sprite2Enemy[i]);
        }
        break;
      
      /*-------------------------------------------------*/
      case BOMB:  /* -------------- BOMBA  --------------*/
        attron(COLOR_PAIR(DELETE_COLOR));
        /* Cancello la precedente posizione del proiettile */
        mvprintw(update.prev_coordinate.y, update.prev_coordinate.x, "%c", ' ');  
        attron(COLOR_PAIR(BOMB_COLOR));
        /* Stampo nella nuova posizione del proiettile     */
        mvprintw(update.y, update.x, "%c", projectile);                           
        break; 
    }


    /* ------------------------------------------------------------ */
    /* Controllo Bordo sinistro                                     */

    if(update.x == 1 && (update.emitter == ENEMY || update.emitter == ENEMY_LV2)){
      /* Il nemico ha toccato il bordo sinistro */
      life=0;
      beep();
    }


    /* ------------------------------------------------------------ */
    /* Controllo HITBOX                                             */
    isHit = checkHitBox(update);
    if(isHit.threadID != -1){
      /* Se PID diverso da -1 ho una hit */
      hitAction.beingHit = isHit;
      hitAction.hitting = update;
      switch (isHit.emitter) {
        case ENEMY_LV2:
        case ENEMY:
          if(update.emitter == ENEMY || update.emitter == ENEMY_LV2){
            /* Collisione tra 2 nemici, avviso entrambi per farli rimbalzare */
            addHit(hitAction);
            hitAction.beingHit = update;
            hitAction.hitting = isHit;
            addHit(hitAction);
          }else{
            /* Qualcosa collide con un nemico di diverso da altri nemici */
            addHit(hitAction);
            if(update.emitter == BULLET){
              if(isHit.emitter == ENEMY)     score += ENEMY_LV1_POINT;
              if(isHit.emitter == ENEMY_LV2) score += ENEMY_LV2_POINT;
              /*kill(update.PID, SIGKILL); /* Cancello il proiettile */
              attron(COLOR_PAIR(DELETE_COLOR));
              mvprintw(update.y, update.x, "%c", ' ');
            }
          }
          break;
        case SPACECRAFT:
          if(update.emitter == BOMB){
            /* Spacecraft colpita da una bomba */
            life--;
            beep();
            /*kill(update.PID, SIGKILL);*/
            attron(COLOR_PAIR(DELETE_COLOR));
            mvprintw(update.y, update.x, "%c", ' ');
            if(life == 0){
              hitAction.hitting.emitter = SPACECRAFT;
              hitAction.hitting.x = -1;
              attron(COLOR_PAIR(DELETE_COLOR));
              addHit(hitAction);
            }
          }else{
            if(update.emitter == ENEMY || update.emitter == ENEMY_LV2){
              /* Forzo la chiusura dei nemici perché uno collide con la navicella madre */
              life=0;
              beep();
              hitAction.hitting = isHit;
              hitAction.beingHit = update;
              hitAction.hitting.x = -1;
              addHit(hitAction);
            }
          }
      }
    }
    if(invincible) life = 3;


    /* ------------------------------------------------------------ */
    /* CONTROLLO FINE GIOCO                                         */
    if(life==0){
      /* Se la navicella è morta, avviso il processo enemies che è terminato */
      hitAction.hitting.emitter = SPACECRAFT;
      hitAction.hitting.x = -1;
      addHit(hitAction);
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
    mvprintw(border.maxy+2, 2, "SCORE: %5d", score);
    mvprintw(border.maxy+1, 20, "NEMICI LV 1: %2d/%2d", lv1Killed,MAX_ENEMIES);
    mvprintw(border.maxy+3, 20, "NEMICI LV 2: %2d/%2d", lv2Killed,MAX_ENEMIES*4);
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
  char error[] = "Si è creato un errore critico";
  int errorLength = strlen(error);
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

  if(gameEndingReason == -1){
    mvprintw((border.maxy/2)-2, (border.maxx/2)-errorLength, error);
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