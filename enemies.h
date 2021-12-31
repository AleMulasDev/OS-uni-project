#ifndef ENEMIES_H
#define ENEMIES_H

#include "utils_struct.h"
#include "enemy.h"
#include <time.h>
#define MAX_ENEMIES 40

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void enemies(int pipeIN, int pipeOUT, borders borders, int max_enemies, coordinate_base startingPoint);

#endif