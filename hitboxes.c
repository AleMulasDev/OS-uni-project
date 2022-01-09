#include "hitboxes.h"

coordinate* lastRecorded;
int dimHistory;

void initializeHistory(int numEnemies){
    /* Salvo la posizione di tutti i nemici (*4 per il secondo livello) + della navicella */
    dimHistory = (numEnemies*5)+1;
    lastRecorded = (coordinate*)malloc(sizeof(coordinate)*dimHistory);
    int i;
    for(i=0; i<dimHistory;i++){
        lastRecorded[i].PID = -1;
        /* Inizializzo a -1 il pid per specificare che non è ancora stato usato */
    }
}

void updatePosition(coordinate newItem){
  int i;
  /* Non tengo conto dei proiettili */
  if(newItem.emitter==BULLET || newItem.emitter==BOMB) return;
  for(i=0; i<dimHistory; i++){
    if(lastRecorded[i].PID == newItem.PID){
      if (newItem.x == -1 && newItem.emitter == ENEMY && lastRecorded[i].emitter == ENEMY){
        /* Enemy lv 1 da eliminare */
        lastRecorded[i].PID = -1;
        return;
      }else{
        if(newItem.x == -1 && newItem.emitter == ENEMY_LV2){
          /* Enemy lv 2 da eliminare */
          if(newItem.prev_coordinate.x == lastRecorded[i].x && newItem.prev_coordinate.y == lastRecorded[i].y){
            /* Le coordinate corrispondono, è l'enemy lv2 giusto da eliminare */
            lastRecorded[i].PID = -1;
            return;
          }
        }else{
          /* Update generico di posizione                                                                      */
          /* Verifico che sia un update dell'oggetto corretto (gli enemy lv2 sono 4x entità con lo stesso pid) */
          if(lastRecorded[i].x == newItem.prev_coordinate.x && lastRecorded[i].y == newItem.prev_coordinate.y){
            lastRecorded[i] = newItem;
            return;
          }
        }
      }
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
  if (newItem.x == -1){
    /* Oggetto da eliminare */
    newItem.PID = -1;
    return newItem;
  }
  coordinate_base dim_hitbox;
  coordinate_base personal_hitbox = getHitBox(newItem);
  coordinate checking;
  for(i=0; i<dimHistory; i++){
    checking = lastRecorded[i];
    dim_hitbox = getHitBox(checking);
    if(checking.PID == -1 || checking.PID == newItem.PID) continue;
    if(newItem.x >= checking.x && newItem.x <= checking.x + dim_hitbox.x){
      /* Le x collidono */
      if(newItem.y >= checking.y && newItem.y <= checking.y + dim_hitbox.y){
        /* Le y collidono */
        return checking;
      }
    }
    if(checking.x >= newItem.x && checking.x <= newItem.x + personal_hitbox.x){
      /* Le x collidono */
      if(checking.y >= newItem.y && checking.y <= newItem.y + personal_hitbox.y){
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
    case BOMB:
    case BULLET:
      toReturn.x = toReturn.y = 0;
      break;
    case SPACECRAFT:
      toReturn.x = SPACECRAFT_SPRITE_WIDTH;
      toReturn.y = SPACECRAFT_SPRITE_HEIGHT;
      break;
    case ENEMY_LV2:
      toReturn.x = ENEMY_SPRITE_2_WIDTH;
      toReturn.y = ENEMY_SPRITE_2_HEIGHT;
      break;
  }
  return toReturn;
}


bool areThereEnemies(){
  int i;
  for(i=0; i<dimHistory; i++){
    if(lastRecorded[i].PID != -1 && (lastRecorded[i].emitter == ENEMY || lastRecorded[i].emitter == ENEMY_LV2)) return true;
  }
  return false;
}