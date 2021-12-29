#ifndef SPACECRAFT_H
#define SPACECRAFT_H

#include "utils_struct.h"
#include "bullet.h"

// ------------------------------------------------------------
// DEFINIZIONE COSTANTI

#define SPACECRAFT_SPRITE_SIZE 6
extern int spriteSpacecraftHeight;
extern int spriteSpacecraftWidth;
extern char spriteSpacecraft[SPACECRAFT_SPRITE_SIZE];

// ------------------------------------------------------------
// DEFINIZIONE PROTOTIPI

void spacecraft(int pipeIN, int pipeOUT, borders borders);

#endif