#include "utils_struct.h"
#include <unistd.h>
#include <curses.h>
#include <stdbool.h>

// ------------------------------------------------------------
// DEFINIZIONE COSTANTI

#define DELAY_MS 33 // tempo di attesa in millisecondi
#define SPACECRAFT_SPRITE_SIZE 6
char spacecraftSprite[SPACECRAFT_SPRITE_SIZE] = "<===>";

// ------------------------------------------------------------
// DEFINIZIONE PROTOTIPI

void spacecraft(int pipeOUT, int MAXY);