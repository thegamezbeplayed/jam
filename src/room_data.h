#ifndef ROOM_DATA_H
#define ROOM_DATA_H

#define ROOM_LEVEL_COUNT 3

typedef struct{
  int     wid, hei;
  Cell    start;
  Tiles   tiles[12][12];
  Tiles   slabs[12][12];
}level_layout_t;

static level_layout_t LVLS[LVL_ALL] = {
  {
    5, 5,
    {4,3},
    .tiles = {
      [4][0]    = TILE_URUZ,
      [0][4]  = TILE_FEHU
    },
    .slabs = {
      [3][1]  = SLAB_FEHU,
      [1][2] = SLAB_URUZ, 
    }
  },
  {
    8,5,
    {3,3},
    .tiles = {
      [5][0] = TILE_VOID,
      [5][1] = TILE_VOID,
      [5][2] = TILE_VOID,
      [5][3] = TILE_VOID,
      [5][4] = TILE_VOID,
      [5][5] = TILE_VOID,
      [1][1] = TILE_LAGUZ,
      [6][1] = TILE_SOL,
    },
    .slabs = {
      [4][3] = SLAB_SOL,
      [2][2] = SLAB_LAGUZ,
      [6][3] = SLAB_LAGUZ,
    }
  },
  {
    5, 8,
    {0,4},
    .tiles = {
      [3][5] = TILE_ISA,
    },
    .slabs = {
      [3][4] = SLAB_ISA,
      [1][3] = SLAB_URUZ,
      [3][1] = SLAB_THURISAZ
    }
  },
  {
    8, 8,
    {3, 1},
    .tiles = {
      [1][1] = TILE_THURISAZ,
      [4][7] = TILE_ANSUZ,
      [0][7] = TILE_VOID,
      [1][6] = TILE_VOID,
      [2][5] = TILE_VOID,
      [3][5] = TILE_VOID,
      [4][5] = TILE_VOID,
      [4][6] = TILE_VOID,
      [5][6] = TILE_VOID,
      [5][7] = TILE_VOID,
    },
    .slabs = {
      [2][2] = SLAB_ANSUZ,
      [3][2] = SLAB_THURISAZ
    }
  }  
};

#endif
