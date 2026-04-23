#include "game_gen.h"
#include "game_types.h"
#include "game_process.h"

void OnMapCellEvent(event_t* ev, void* user){
  map_cell_t* mc = user;
  
  mc->occupant = NULL;
  if(mc->status == TILE_COLLISION)
    mc->status = TILE_EMPTY;
}

void InitMap(void){
  for(int i = 0; i < TILE_DONE; i++){
    sprite_t* s = InitSpriteByID(i, SHEET_TILE);
  }
}

map_grid_t* InitMapGrid(level_t* l){
  map_grid_t* m = GameMalloc("InitMapGrid", sizeof(map_grid_t));

  *m = (map_grid_t){0};

  m->step_size = CELL_WIDTH;

  m->x = 0;
  m->y = 0;
  level_layout_t ld = LVLS[l->id];

  m->width = ld.wid;
  m->height = ld.hei;
  m->floor = DARKBROWN;

  ScreenSetOffset(CELL_NEW(ld.wid * 4, ld.hei/2));
  int area = ld.wid * ld.hei;
  HashInit(&m->hash, next_pow2_int(1+area));
  m->tiles = GameMalloc("InitMapGrid", m->width * sizeof(map_cell_t*));
  for(int x = 0; x < m->width; x++)
    m->tiles[x] = GameCalloc("InitMapGrid", m->height, sizeof(map_cell_t)); 

  for(int y = 0; y < m->height; y++){
    for(int x = 0; x < m->width; x++){
    Tiles t = ld.tiles[x][y];

      Cell pos = CELL_NEW(x,y);
      map_cell_t* mc = InitMapCell(m, t, pos);

      mc->gouid = RegisterMapCell(mc);

      mc->on_enter = MapCellOccupied;      
      HashPut(&m->hash, mc->gouid, mc);
      
      if(t <= TILE_BLANK){
        if(t== TILE_VOID)
        mc->status = TILE_NO_ENTRY;
        continue;
      }

      Signals sig = TILE_SIGNALS[t];
      PuzzleRegisterSolution(l->puzzle, mc, sig);
    }
  }
  
  return m;
}

map_cell_t* InitMapCell(map_grid_t* m, Tiles t, Cell pos){
  map_cell_t* mc = &m->tiles[pos.x][pos.y];

  mc->coords = pos;
  mc->tile = t;
  if(t > 0)
  mc->signals = TILE_SIGNALS[t];

  mc->sprite = InitSpriteByID(t, SHEET_TILE);
  mc->sprite->pos = CellToVector2(pos, CELL_WIDTH);


  return mc;
}


void MapCellRender(map_cell_t* mc){
  DrawSprite(mc->sprite);

}

void MapRender(map_grid_t* m){
  for(int x = 0; x < m->width; x++){
    for(int y = 0; y < m->height; y++){
      map_cell_t* mc = &m->tiles[x][y];

      if(mc->tile > TILE_VOID)
      MapCellRender(mc);
    }
 }
}

void MapSync(map_grid_t* m){
}

TileStatus MapChangeOccupant(map_grid_t* map,ent_t* e, Cell old, Cell c){
  Cell bounds = CELL_NEW(map->width,map->height);
  if(!cell_in_bounds(c,bounds))
    return TILE_OUT_OF_BOUNDS;

  if(map->tiles[c.x][c.y].occupant)
    return TILE_OCCUPIED;

  if(map->tiles[old.x][old.y].occupant)
    MapRemoveOccupant(map,old);

  return MapSetOccupant(map,e,c);
}


TileStatus MapSetTile(map_grid_t* m, Tiles t, Cell c){
  Cell bounds = CELL_NEW(m->width,m->height);
  if(!cell_in_bounds(c,bounds))
    return TILE_OUT_OF_BOUNDS;

  map_cell_t* mc = &m->tiles[c.x][c.y];

  mc->tile = t;
  mc->sprite = InitSpriteByID(mc->tile, SHEET_TILE);
  mc->sprite->pos = CellToVector2(c, CELL_WIDTH);

  if(TileHasFlag(t, TILEFLAG_SOLID))
    mc->status = TILE_COLLISION;
  else if(TileHasFlag(t, TILEFLAG_BORDER))
    mc->status = TILE_BORDER;
  else
    mc->status = TILE_EMPTY;

  
  return TILE_SUCCESS;
}

TileStatus MapForceOccupant(map_grid_t* m, ent_t* e, Cell c){
Cell bounds = CELL_NEW(m->width,m->height);
  if(!cell_in_bounds(c,bounds)) 
    return TILE_OUT_OF_BOUNDS;
  
  map_cell_t* mc = &m->tiles[c.x][c.y];

  if(mc->status > TILE_NO_ENTRY){
    if(mc->occupant)
      LevelEvent(EVENT_TILE_COLLISION, e, mc->occupant->gouid);
    return TILE_OCCUPIED;
  }
  
  MapRemoveOccupant(m,e->pos);
  mc->occupant =e;
  
  if(mc->on_enter)
    mc->on_enter(m, mc, e);

  return TILE_SUCCESS;

}

TileStatus MapSetOccupant(map_grid_t* m, ent_t* e, Cell c){
  if(!e)
    return TILE_ERROR;

  Cell bounds = CELL_NEW(m->width,m->height);
  if(!cell_in_bounds(c,bounds))
    return TILE_OUT_OF_BOUNDS;

  map_cell_t* mc = &m->tiles[c.x][c.y];

  if(!mc)
    return TILE_OUT_OF_BOUNDS;

  if(mc->status == TILE_NO_ENTRY && e->type == ENT_TILE){
    LevelEvent(EVENT_TILE_INSERT, e, mc->gouid);
    return TILE_NO_ENTRY;
  }
  else if(mc->status > TILE_ISSUES){
    if(mc->occupant)
      LevelEvent(EVENT_TILE_COLLISION, e, mc->occupant->gouid);
    return TILE_OCCUPIED;
  }
  
  MapRemoveOccupant(m,e->pos);
  mc->occupant =e;
  mc->status = TILE_OCCUPIED;
  if(mc->on_enter)
    mc->on_enter(m, mc, e);

  return TILE_SUCCESS;
}

void MapCellOccupied(map_grid_t* m, map_cell_t* mc, ent_t* e){
  LevelEvent(EVENT_TILE_MOVE, e, mc->gouid);
 
  hash_iter_t iter;
  HashStart(&m->hash, &iter);

  hash_slot_t* s;
  while((s = HashNext(&iter))){
    map_cell_t* other = s->value;
    int dist = cell_distance_euc(other->coords, mc->coords);

    param_t p = ParamMake(DATA_INT, sizeof(int), &dist);
    LevelEventPayload(EVENT_TILE_DIST, other, mc->occupant->gouid, p);
  }
    LevelTargetSubscribe(EVENT_ENT_DIE, OnMapCellEvent, mc, e->gouid);
}

map_cell_t* MapGetTile(map_grid_t* map,Cell tile){
  Cell bounds = CELL_NEW(map->width,map->height);

  if(!cell_in_bounds(tile,bounds))
    return NULL;

  return &map->tiles[tile.x][tile.y];

}

TileStatus MapTileAvailable(map_grid_t* m, Cell c){
  Cell bounds = CELL_NEW(m->width,m->height);

  if(!cell_in_bounds(c,bounds))
    return TILE_OUT_OF_BOUNDS;
  map_cell_t* mc = &m->tiles[c.x][c.y];

  return mc->status;
}

TileStatus MapRemoveOccupant(map_grid_t* m, Cell c){
  Cell bounds = CELL_NEW(m->width,m->height);
  if(!cell_in_bounds(c,bounds))
    return TILE_OUT_OF_BOUNDS;

  m->tiles[c.x][c.y].occupant = NULL;

  m->tiles[c.x][c.y].status = TILE_EMPTY;

  return TILE_SUCCESS;
}

ent_t* MapGetOccupant(map_grid_t* m, Cell c, TileStatus* status){
  map_cell_t tile = m->tiles[c.x][c.y];

  *status = tile.status;

  return tile.occupant;
}

TileStatus MapCheckMoveOptions(map_grid_t* m, ent_t* e){
  Cell corners[4];
  corners[0] = CELL_EMPTY;
  corners[1] = CELL_NEW(0, m->height -1) ;
  corners[0] = CELL_NEW(m->width - 1, 0);
  corners[0] = CELL_NEW(m->width-1, m->height -1);
  for (int i = 0; i < 4; i++){
    Cell corner = corners[i];

    if(cell_compare(e->pos, corner))
      return TILE_BORDER;
  }

  return TILE_SUCCESS;
}
