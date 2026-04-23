#ifndef __GAME_LIST__
#define __GAME_LIST__
#include "game_define.h"

static ent_d DEF_ENT[ENT_DONE] = {
  [ENT_PLAYER] = {ENT_PLAYER, 
    { CHAR_DOWN_0,  CHAR_DOWN_1,  CHAR_DOWN_2,
      CHAR_LEFT_0,  CHAR_LEFT_1,  CHAR_LEFT_2,
      CHAR_RIGHT_0,  CHAR_RIGHT_1,  CHAR_RIGHT_2,
      CHAR_UP_0,  CHAR_UP_1,  CHAR_UP_2,
    },
    CHAR_DOWN_0,
  }
};

#endif

