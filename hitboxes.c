#include "hitboxes.h"

coordinate* lastRecorded;
int dimHistory;

void initializeHistory(int numEnemies){
    /* Salvo la posizione di tutti i nemici + della navicella */
    dimHistory = numEnemies+1;
    lastRecorded = (coordinate*)malloc(sizeof(coordinate)*dimHistory);
    int i;
    for(i=0; i<dimHistory;i++){
        lastRecorded[i].PID = -1;
        /* Inizializzo a -1 il pid in caso non sia ancora stato usato */
    }
}

void updatePosition(coordinate newItem){
  int i;
  /* Non tengo conto dei proiettili */
  if(newItem.emitter==BULLET) return;
  for(i=0; i<dimHistory; i++){
    if(lastRecorded[i].PID == newItem.PID){
      lastRecorded[i] = newItem;
      return;
    }
  }
  /* Se arrivo qua allora non è ancora presente nell'array */
  for(i=0; i<dimHistory; i++){
    if(lastRecorded[i].PID == -1){
      lastRecorded[i] = newItem;
      return;
    }
  }
}

coordinate checkHitBox(coordinate newItem){
  int i;
  updatePosition(newItem);
  coordinate_base dim_hitbox;
  coordinate_base personal_hitbox = getHitBox(newItem);
  coordinate checking;
  for(i=0; i<dimHistory; i++){
    checking = lastRecorded[i];
    dim_hitbox = getHitBox(lastRecorded[i]);
    if(checking.PID == newItem.PID) continue;
    if(newItem.x >= checking.x && newItem.x <= checking.x + dim_hitbox.x){
      /* Le x collidono */
      if(newItem.y >= checking.y && newItem.y <= checking.y + dim_hitbox.y){
        /* Le y collidono */
        return checking;
      }
    }


  }
  checking.PID = -1;
  return checking;
}

coordinate_base getHitBox(coordinate item){
  coordinate_base toReturn;
  switch(item.emitter){
    case ENEMY:
      toReturn.x = ENEMY_SPRITE_1_WIDTH;
      toReturn.y = ENEMY_SPRITE_1_HEIGHT;
      break;
    case BULLET:
      toReturn.x = toReturn.y = 1;
      break;
    case SPACECRAFT:
      toReturn.x = SPACECRAFT_SPRITE_WIDTH;
      toReturn.y = SPACECRAFT_SPRITE_HEIGHT;
      break;
  }
  return toReturn;
}
