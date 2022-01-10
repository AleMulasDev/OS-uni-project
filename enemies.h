#ifndef ENEMIES_H
#define ENEMIES_H

#include "utils_thread.h"
#include "enemy.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE MACRO                                            */

extern int SPACE_BETWEEN_X;
extern int SPACE_BETWEEN_Y;



/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void *enemies(void *args);

#endif