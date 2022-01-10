#include "enemy.h"


/* ------------------------------------------------------------ */
/* DEFINIZIONE COSTANTI                                         */
int ENEMY_SPRITE_1_WIDTH=6;
int ENEMY_SPRITE_2_WIDTH=3;
int ENEMY_SPRITE_1_HEIGHT=6;
int ENEMY_SPRITE_2_HEIGHT=2;
char* sprite1Enemy[] = {"      ", "<\\==\\ ","  /][\\","  \\][/","</==/ ","      "};
char* sprite2Enemy[] = {"<\\\\","<//"};


/* ------------------------------------------------------------ */
/* PROTOTIPI FUNZIONI DI UTILITÀ                                */
coordinate_base getOffset(int index);


/* ------------------------------------------------------------ */
/* FUNZIONE PRINCIPALE                                          */
void *enemy(void* args){
  enemyArguments enemyArgs = *((enemyArguments*) args);
  borders border = enemyArgs.border;
  coordinate_base startingPoint = enemyArgs.startingPoint;
  vettore direzione = enemyArgs.direzione;
  enemyThread self = enemyArgs.self;
  self.threadID_Child = pthread_self();
  coordinate report;
  coordinate tmpReport;
  hitUpdate receivedUpdate;
  coordinate_base offset;
  bulletArguments bulletArgs;
  int elapsed = 0;
  bool firstBombSpawned = false;
  int bombElapsed = 0;
  int toWait = 0;
  int level = 1;
  pthread_t bombThreadID;
  int hitCount[4] = {0,0,0,0};
  int index;
  int lastYBounce = 0;
  coordinate bombSpawnPoint;
  vettore bombDirection;
  report.threadID = pthread_self();
  report.emitter = ENEMY;
  report.prev_coordinate.x = startingPoint.x;
  report.prev_coordinate.y = startingPoint.y;
  report.x = startingPoint.x;
  report.y = startingPoint.y;
  bool stop = false;
  srand(getpid());

  /* Mando un singolo update, anche se fuori dalla zona visibile, per registrare il nemico nelle hitbox */
  if(report.x > border.maxx) addUpdate(report);
  
  /* ---------------------------------------------------------------------------- */
  /* -------------- Ciclo di attesa di essere nella zona visibile  -------------- */
  while(report.x >= border.maxx - ENEMY_SPRITE_1_WIDTH){
    /* Controllo se arriva una richiesta di chiusura forzata e avanzo verso sinistra
      finché non arrivo alla zona visibile */
    receivedUpdate = getEnemyUpdateNB(self);
    if(receivedUpdate.hitting.x != -2){
      if(receivedUpdate.hitting.x == -1 && receivedUpdate.hitting.emitter == SPACECRAFT){
        return NULL;
      }
    }
    report.x += direzione.x;
    napms(DELAY_MS*ENEMY_MOV_SPEED);
  }

  /* --------------------------------------------------- */
  /* ---------------- Ciclo principale  ---------------- */
  while(!stop){
    report.prev_coordinate.x = report.x;
    report.prev_coordinate.y = report.y;
    while(elapsed < DELAY_MS*ENEMY_MOV_SPEED){
      receivedUpdate = getEnemyUpdateNB(self);
      if(receivedUpdate.hitting.x != -2){
      if(receivedUpdate.hitting.x == -1 && receivedUpdate.hitting.emitter == SPACECRAFT){
        return NULL;
      }
      switch(receivedUpdate.hitting.emitter){
        case SPACECRAFT:
          /* Collisione con la spacecraft */
          report.x = -1;
          stop = true;
          addUpdate(report);
          return NULL;
        case BULLET:
          /* Collisione con un proiettile */
          if(level == 1){
            /* Cancello la nave di 1 livello */
            level++;
            report.x = -1;
            addUpdate(report);
            /* Riporto la coordinata all'originale per i nemici di secondo livello */
            report.x = report.prev_coordinate.x;
            report.emitter = ENEMY_LV2;
          }else{
            /* Calcolo quale dei 4 nemici di secondo livello è riferito nell'update */
            /*
            ALTO SINISTRA index = 0    ALTO DESTRA index = 1
            BASSO SINISTRA index = 2   BASSO DESTRA index = 3
            */
            index = abs(receivedUpdate.hitting.x-report.x)/(ENEMY_SPRITE_2_WIDTH+2);
            index += 2*(abs(receivedUpdate.hitting.y-report.y)/(ENEMY_SPRITE_2_HEIGHT+2));
            hitCount[index]++;
            if(hitCount[index] == N_HIT_LV2_ENEMY){
              /* Cancello l'enemy di secondo livello */
              tmpReport = report;
              tmpReport.x = -1;
              offset = getOffset(index);
              tmpReport.prev_coordinate.x += offset.x;
              tmpReport.prev_coordinate.y += offset.y;
              addUpdate(report);
            }
            /* Controllo se sono rimasti nemici in vita */
            stop = true;
            for(index=0;index<4;index++){
              if(hitCount[index] < N_HIT_LV2_ENEMY){
                stop = false;
              }
            }
            if(stop){
              return NULL;
            }
          }
          break;

        case ENEMY_LV2:
        case ENEMY:
          /* Salvo la Y del rimbalzo per impedire di cambiare 2 volte la pozione */
          /* Cosa che può succedere con 2 navicelle lv 2 affiancate              */
          if(lastYBounce != report.y){
            direzione.y = direzione.y * -1;
            lastYBounce = report.y;
          }
          break;
        }
      }
      elapsed += (DELAY_MS*ENEMY_MOV_SPEED)/11;
      napms((DELAY_MS*ENEMY_MOV_SPEED)/11); /* Piccola attesa per restare al corrente degli update */
    }
    bombElapsed += elapsed;
    toWait = (DELAY_MS*ENEMY_MOV_SPEED) - elapsed;
    elapsed = 0;

    /* Le bombe spawnano in modo random la prima volta, con una percentuale di probabilità
    ** definita in BOMB_SPAWN_CHANGE, dopo di ché spawnano ogni BOMB_SPAWN_DELAY
    ** per calcolare il delay sommo l'attesa a ogni ciclo principale + l'attesa effettuata 
    ** qua sopra, in attesa di aggiornamenti dal processo padre
    */
    if(rand() % 100 < BOMB_SPAWN_CHANCE*100 && !firstBombSpawned){
      firstBombSpawned = true;
      bombElapsed = 0;

      bombSpawnPoint = report;
      bombSpawnPoint.emitter = BOMB;
      bombSpawnPoint.x--;
      if(level == 2){
        for(index=0;index<4;index++){
          if(hitCount[index] < N_HIT_LV2_ENEMY){
            bombSpawnPoint.x += getOffset(index).x;
            bombSpawnPoint.y += getOffset(index).y;
            break;
          }
        }
      }
      bombDirection = RIGHT_UP;
      bombDirection.y = 0;
      bombDirection.x = bombDirection.x * -1;

      bulletArgs.border = border;
      bulletArgs.direzione = bombDirection;
      bulletArgs.startingPoint = bombSpawnPoint;
      if(pthread_create(&bombThreadID, NULL, bullet, (void*)&bulletArgs)){
        return NULL;
      }
    }

    if(firstBombSpawned && bombElapsed < BOMB_SPAWN_DELAY){
      bombElapsed += (DELAY_MS*ENEMY_MOV_SPEED);
    }else if(firstBombSpawned && bombElapsed >= BOMB_SPAWN_DELAY){
      bombElapsed = 0;
      bombSpawnPoint = report;
      bombSpawnPoint.emitter = BOMB;
      bombSpawnPoint.x--;
      bombDirection = RIGHT_UP;
      bombDirection.y = 0;
      bombDirection.x = bombDirection.x * -1;
      bulletArgs.border = border;
      bulletArgs.direzione = bombDirection;
      bulletArgs.startingPoint = bombSpawnPoint;
      if(pthread_create(&bombThreadID, NULL, bullet, (void*)&bulletArgs)){
        return NULL;
      }
    }
    
    if(report.y + direzione.y < 2 || report.y + direzione.y > border.maxy - ENEMY_SPRITE_1_HEIGHT){
      direzione.y = direzione.y * -1;
    }

    if(!stop){
      report.y += direzione.y;
      report.x += direzione.x;
    }

    if(level == 1){
      addUpdate(report);
    }else{
      for(index=0;index<4;index++){
        tmpReport = report;
        offset = getOffset(index);
        tmpReport.prev_coordinate.x += offset.x;
        tmpReport.prev_coordinate.y += offset.y;
        tmpReport.x += offset.x;
        tmpReport.y += offset.y;
        if(hitCount[index] < N_HIT_LV2_ENEMY) addUpdate(report);
      }
    }
    napms(toWait);
  }
}


/* ------------------------------------------------------------ */
/* FUNZIONE DI UTILITÀ                                          */

coordinate_base getOffset(int index){
  coordinate_base offset;
  if(index == 0){
    offset.x = offset.y = 0;
  }else if(index == 1){
    offset.x = ENEMY_SPRITE_2_WIDTH + 2;
    offset.y = 0;
  }else if(index == 2){
    offset.x = 0;
    offset.y = ENEMY_SPRITE_2_HEIGHT + 2;
  }else if(index == 3){
    offset.x = ENEMY_SPRITE_2_WIDTH + 2;
    offset.y = ENEMY_SPRITE_2_HEIGHT + 2;
  }
  return offset;
}