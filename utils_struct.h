
typedef enum {
  SPACECRAFT,
  ENEMY,
  BULLET,
  BOMB
}emitter_type;

// -------------------------------------------------------------
// Gestione coordinate

typedef struct {
  emitter_type emitter;
  int x;
  int y;
} coordinate;

