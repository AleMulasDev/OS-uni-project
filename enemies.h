#ifndef ENEMIES_H
#define ENEMIES_H

#include "utils_struct.h"
#include "enemy.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE MACRO                                            */

#define MAX_ENEMIES 40
#define SPACE_BETWEEN 4

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void enemies(int pipeIN, int pipeOUT, borders borders, int max_enemies, coordinate_base startingPoint);

#endif