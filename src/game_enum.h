#ifndef __GAME_ENUM__ 
#define __GAME_ENUM__ 

#define BIT64(n) (1ULL << (n))

typedef enum{
  EVENT_TILE_COLLISION,
  EVENT_TILE_INSERT,
  EVENT_TILE_MOVE,
  EVENT_TILE_DIST,
  EVENT_SHIFT_SLAB,
  EVENT_SIGNAL_ACTION,
  EVENT_TILE_END,
  EVENT_GAME_PROCESS,
  EVENT_INTERACTION,
  EVENT_PLAY_SFX,
  EVENT_SONG_END,
  EVENT_SONG_FADE_IN,
  EVENT_SONG_FADE_OUT,
  EVENT_WAIT,
  EVENT_FINISH,
  EVENT_ENT_ACTION,
  EVENT_ENT_DIE,
  EVENT_ENTER_CELL,
  EVENT_LEVEL_TURN_END,
  EVENT_LEVEL_SHIFT,
  EVENT_LEVEL_CHECK,
  EVENT_LEVEL_SOLVED,
  EVENT_LEVEL_CLEAR,
  EVENT_LEVEL_END,
  EVENT_LEVEL_STUCK,
  EVENT_NONE,
  MAX_EVENTS,
} EventType;

typedef enum{
  ENT_NONE,
  ENT_PLAYER,
  ENT_TILE,
  ENT_DONE
}EntityType;

typedef enum{
  STATE_NONE,//if ent_t is properly initalized to {0} this is already set
  STATE_SPAWN,//Should only be set after NONE
  STATE_IDLE, //should be able to move freely between these ==>
  STATE_PUSHING,
  STATE_SHIFTING,
  STATE_PLACED,
  STATE_SOLVED,
  STATE_DIE,//<===== In MOST cases. Should not be able to go down from DIE
  STATE_END,//sentinel entity state should never be this or greater
  STATE_STUCK
}EntityState;

typedef enum{
  PARAM_NONE,
  PARAM_TURN,
  PARAM_SCORE,
  PARAM_LEVEL,
  PARAM_ENT,
  PARAM_POS,
  PARAM_NAME,
  PARAM_STATE,
  PARAM_ALL,
}GameObjectParam;

typedef enum{
  TF_NONE,
  TF_TURN,
  TF_UPDATE,
}TimeFrame;

typedef enum {
  DATA_NONE,
  DATA_ARRAY,
  DATA_PARAM,
  DATA_INT,
  DATA_FLOAT,
  DATA_BOOL,
  DATA_PTR,
  DATA_CELL,
  DATA_UINT64,
  DATA_GOUID,
  DATA_STRING,
  DATA_ENT,
  DATA_EVENT,
  DATA_MAP_CELL,
  DATA_ELEM,
  DATA_LEVEL,
  DATA_CD,
  DATA_SPRITE,
  DATA_SLICE,
  DATA_ALL,
} DataType;

typedef enum{
  CHAR_NONE,
  CHAR_DOWN_0,
  CHAR_DOWN_1,
  CHAR_DOWN_2,
  CHAR_LEFT_0,
  CHAR_LEFT_1,
  CHAR_LEFT_2,
  CHAR_RIGHT_0,
  CHAR_RIGHT_1,
  CHAR_RIGHT_2,
  CHAR_UP_0,  
  CHAR_UP_1,  
  CHAR_UP_2,
  CHAR_DONE  
}CharacterSprite;

typedef enum{
  VFX_EMPTY,
  VFX_SHADOW_SQUARE,
  VFX_SHADOW_OBLONG,
  VFX_ALL
}VFXSprite;

typedef enum{
  VFX_NONE,
  VFX_SHAD,
  VFX_DONE
}VFXType;

typedef enum{
  ANIM_SEQ_NONE,
  ANIM_SEQ_GO_L,
  ANIM_SEQ_GO_U,
  ANIM_SEQ_GO_R,
  ANIM_SEQ_GO_D,
  ANIM_SEQ_ALL
}AnimSequence;

typedef enum{
  AT_NONE,
  AT_SPRITE,
  AT_VEC,
}AnimType;

typedef enum{
  TILE_VOID = -1,
  TILE_BLANK,    

  SLAB_ALGIZ,    
  SLAB_ANSUZ,    
  SLAB_BERKANA,  
  SLAB_DAGAZ,    
  SLAB_EHWAZ,    
  SLAB_FEHU,     
  SLAB_GEBO,     
  SLAB_HAGALAZ,  

  SLAB_INGUZ,    
  SLAB_INGWAZ,   
  SLAB_ISA,      
  SLAB_JERA,     
  SLAB_KUANAZ,   
  SLAB_LAGUZ,    
  SLAB_MANNAZ,   
  SLAB_NAUTHIZ,  
  SLAB_OTHILA,   
  SLAB_PEORTH,   

  SLAB_RAIDO,    
  SLAB_SOL,      
  SLAB_TEIWAZ,   
  SLAB_THURISAZ, 
  SLAB_URUZ,     
  SLAB_WUNJO,    
  SLAB_YR,       
  TILE_ANSUZ,    
  TILE_FEHU,     
  TILE_GEBO,     
  TILE_LAGUZ,    
  TILE_MANNAZ,   
  TILE_RAIDO,    
  TILE_SOL,      
  TILE_THURISAZ, 
  TILE_URUZ,     
  TILE_YR,       
  TILE_ISA,
  TILE_DONE
}Tiles;

typedef enum{
  LVL_TUT,
  LVL_BASIC,
  LVL_SLIP,
  LVL_VOIDS,
  LVL_ALL,
}Levels;

typedef enum{
  TILE_EMPTY,
  TILE_SUCCESS,
  TILE_ISSUES,
  TILE_NO_ENTRY,
  TILE_COLLISION,
  TILE_OCCUPIED,
  TILE_REACHABLE,
  TILE_BORDER,
  TILE_OUT_OF_BOUNDS,
  TILE_ERROR,
  TILE_TIMING,
  TILE_UNREACHABLE
}TileStatus;

typedef enum{
  ACT_NONE,
  ACT_MOVE,
  ACT_DONE
}ActionType;

typedef enum{
  EVS_NONE,
  EVS_KILL
}EventStatus;

typedef enum{
  COND_NONE,
  COND_ENTER,
  COND_NEAR,
  COND_ALL,
}Condition;

typedef enum{
  SOLV_NO,
  SOLV_TRUE,
  SOLV_STUCK,
  SOLV_END,
}SolveStatus;
#endif

