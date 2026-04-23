#ifndef __GAME_GEN__ 
#define __GAME_GEN__ 
#include "game_tools.h"
#include "game_enum.h"
#include "game_assets.h"
#include "game_types.h"

#define MAX_OPTIONS 512
#define MAX_CELLS   256 
#define GRID_WIDTH 128
#define GRID_HEIGHT 128
  
typedef struct map_grid_s map_grid_t;
typedef struct choice_pool_s choice_pool_t;

typedef struct level_s level_t;
typedef uint64_t run_seed;
extern run_seed RUN_SEED;
//#include "game_utils.h"
typedef enum{
  GEN_NONE,
  GEN_DONE,
  GEN_RUNNING,
  GEN_ISSUES,
}GenStatus;
GenStatus MapGetStatus(void);

typedef enum{
  TILEFLAG_NONE        = 0,
  TILEFLAG_BORDER      = 1 << 0,
  TILEFLAG_EMPTY       = 1 << 1,   // blocks movement
  TILEFLAG_SOLID       = 1 << 2,   // blocks movement
  TILEFLAG_DEBRIS      = 1 << 7,
  TILEFLAG_DECOR       = 1 << 10,
  TILEFLAG_OBSTRUCT    = 1 << 11,
  TILEFLAG_FLOOR       = 1 << 13,
  TILEFLAG_WALL        = 1 << 14,
  TILEFLAG_DOOR        = 1 << 15,
  TILEFLAG_START       = 1 << 16,
  TILEFLAG_INTERACT    = 1 << 17,
  TILEFLAG_EXIT        = 1 << 19,
}TileFlag;

typedef uint64_t TileFlags;
static const TileFlags MapTileFlags[TILE_DONE] = {
 
}; 

typedef struct {
    Cell min, center, max;
} cell_bounds_t;
    
typedef struct map_cell_s map_cell_t;

typedef void (*MapCellFn)(map_grid_t*, map_cell_t* mc, ent_t* e);
void MapCellOccupied(map_grid_t*, map_cell_t* mc, ent_t* e);

struct map_cell_s{
  game_object_uid_i   gouid;
  int                 index;
  Tiles               tile;
  sprite_t*           sprite;
  Cell                coords;
  TileStatus          status;
  TileFlags           flags;
  ent_t*              occupant;
  Signals             signals;
  MapCellFn           on_enter;
};

map_cell_t* InitMapCell(map_grid_t*, Tiles, Cell);
struct map_grid_s{
  //MapID        id;
  map_cell_t   **tiles;
  hash_map_t   hash;
  int          x,y,width,height;
  int          step_size;
  Color        floor;
};
void MapRender(map_grid_t* m);

static Rectangle MapBounds(map_grid_t* m){
  return (Rectangle){m->x, m->y, m->width, m->height};
}
map_grid_t* InitMapGrid(level_t*);
void MapTurnStep(map_grid_t* m);
TileStatus MapChangeOccupant(map_grid_t* m, ent_t* e, Cell old, Cell c);
TileStatus MapSetOccupant(map_grid_t* m, ent_t* e, Cell c);
TileStatus MapForceOccupant(map_grid_t* m, ent_t* e, Cell c);
ent_t* MapGetOccupant(map_grid_t* m, Cell c, TileStatus* status);
map_cell_t* MapGetTile(map_grid_t* map,Cell tile);
TileStatus MapTileAvailable(map_grid_t* m, Cell c);
TileStatus MapRemoveOccupant(map_grid_t* m, Cell c);
TileStatus MapSetTile(map_grid_t* m, Tiles, Cell c);

TileStatus MapCheckMoveOptions(map_grid_t*, ent_t*);

typedef bool (*TileBlock)(map_cell_t *c);
typedef bool (*TileFlagHasFn)(TileFlags f);

static int MapGetNeighborsByStatus(map_grid_t* m, Cell pos, map_cell_t* nei[8], TileStatus status){

  int count = 0;
  for (int x = pos.x -1; x < pos.x+1; x++)
    for (int y = pos.y -1; y < pos.y+1; y++){
      map_cell_t* n = &m->tiles[x][y];

      if(n->status != status)
        continue;

      nei[count++] = n;
    }

  return count;
}
bool HasLOS(map_grid_t* m, Cell c0, Cell c1);

static inline bool TileHasAnyFlags(Tiles t, TileFlags flags) {
    return (MapTileFlags[t] & flags) != 0;
}

static inline bool TileHasFlag(Tiles t, TileFlag flag) {
    return (MapTileFlags[t] & flag) != 0;
}

bool IsDiagBlocked(map_grid_t* m, map_cell_t* cc, map_cell_t* nc, TileBlock fn);
int NumNeighborsCell(map_grid_t* m, map_cell_t* c, TileBlock fn);

static bool TileFlagHas(TileFlags f, TileFlags has){
  return ((f & has) > 0);
}
static bool TileFlagHasAccess(TileFlags f) {

  return (f & TILEFLAG_DOOR) || (f & TILEFLAG_FLOOR) || (f & TILEFLAG_EMPTY);
}

static bool TileFlagBlocksMovement(TileFlags f)
{
    // Treat ANY of these as blocking
    return
        (f & TILEFLAG_SOLID)   ||
        (f & TILEFLAG_BORDER)  ||
        (f & TILEFLAG_WALL);
        // Add more if needed
}

bool TileBlocksMovement(map_cell_t *c);

bool TileBlocksSight(map_cell_t *c);

#endif
