#ifndef HITBOXES_H
#define HITBOXES_H

#include "utils_struct.h"
#include "enemies.h"
#include "spacecraft.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void initializeHistory(int numEnemies);
void updatePosition(coordinate newItem);
coordinate checkHitBox(coordinate newItem);
coordinate_base getHitBox(coordinate item);
bool areThereEnemies();

/* ------------------------------------------------------------ */
/* DEFINIZIONE GLOBALI                                          */
extern coordinate* lastRecorded;

#endif
