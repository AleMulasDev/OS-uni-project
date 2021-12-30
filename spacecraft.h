#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include "utils_struct.h"
#include "bullet.h"

/* ------------------------------------------------------------ */
/* DEFINIZIONE COSTANTI                                         */

extern float fireRate;
extern const int SPACECRAFT_SPRITE_WIDTH;
extern const int SPACECRAFT_SPRITE_HEIGHT;
extern char* spriteSpacecraft[];

/* ------------------------------------------------------------ */
/* DEFINIZIONE PROTOTIPI                                        */

void spacecraft(int pipeIN, int pipeOUT, borders borders);

#endif