#include "game_types.h"
#include "game_tools.h"
#include "game_math.h"
#include "game_process.h"
#include "game_helpers.h"
#include "game_control.h"

MAKE_ADAPTER(StepState, ent_t*);

ent_t* InitEnt(EntityType t){
  ent_t* e = GameCalloc("InitEnt", 1, sizeof(ent_t));

  ent_d def = DEF_ENT[t];
  if(def.id != t)
    return NULL;

  e->type = t;
  e->sprite = InitAnimationByID(def.base, def.end, SHEET_CHAR);
  e->sprite->owner = e;
  SetState(e,STATE_SPAWN,NULL);
  return e;
}

ent_t* InitEntStatic(EntityType t, Tiles r){
  ent_t* e = GameCalloc("InitEnt", 1, sizeof(ent_t));

  e->type = t;
  e->sprite = InitAnimationVec(r, SHEET_TILE, 4);
  e->sprite->owner = e;
  e->signals = TILE_SIGNALS[r];
  SetState(e,STATE_SPAWN,NULL);

  return e;
}

void EntDestroy(ent_t* e){
  if(!e || !SetState(e, STATE_END,NULL))
    return;

  WorldRemoveEnt(e->gouid);
  if(e->sprite!=NULL){
    e->sprite->is_visible = false;
    e->sprite->owner = NULL;
  }
}

bool FreeEnt(ent_t* e){
  if(!e)
    return false;

  GameFree("FreeEnt",e);
  return true;
}

void EntSync(ent_t* e){
  SpriteSync(e, e->sprite);
}
/*
void EntRender(ent_t* e){
  if(!e->sprite)
    DrawSprite(e->base);
  else
    DrawSprite(e->sprite);
}
*/

bool SetState(ent_t *e, EntityState s,StateChangeCallback callback){
  if(CanChangeState(e->state,s)){
    EntityState old = e->state;
    e->state = s;

    if(callback!=NULL)
      callback(e,old,s);

    OnStateChange(e,old,s);
    return true;
  }

  return false;
}

void StepState(ent_t *e){
  SetState(e, e->state+1,NULL);
}

bool CanChangeState(EntityState old, EntityState s){
  state_change_requirement_t req = CAN_CHANGE[s];
  if(req.can)
  return req.can(old, req.required);

  return false;
} 

void OnStateChange(ent_t *e, EntityState old, EntityState s){
  switch(s){
    case STATE_DIE:
      EntDestroy(e);
      LevelScheduleEvent(EVENT_ENT_DIE, e, e->gouid, TF_TURN, 1);
      break;
    default:
      break;
  }

  switch(old){
    case STATE_SHIFTING:
      map_cell_t* mc = MapGetTile(WorldGetMap(), e->pos);
      LevelScheduleEvent(EVENT_LEVEL_CHECK, e, mc->gouid, TF_UPDATE, 3);
      
      break;
    default:
      break;
  }
}

bool CheckEntAvailable(ent_t* e){
  if(!e)
    return false;

  return (e->state < STATE_DIE);
}

void EntCheckStatus(ent_t* e){
  if(MapCheckMoveOptions(WorldGetMap(), e) < TILE_ISSUES)
    return;

  SetState(e, STATE_STUCK, NULL);
}

TileStatus EntGridStep(ent_t *e, Cell step){
  Cell newPos = CellInc(e->pos,step);

  e->facing = step;
  TileStatus status = MapSetOccupant(WorldGetMap(),e,newPos);

  if(status < TILE_ISSUES){
    Cell oldPos = e->pos;


    SpriteAnimateTo(e->sprite, oldPos, newPos);
    //WorldDebugCell(e->pos, YELLOW);
    e->pos = newPos;
    e->old_pos = oldPos;
    LevelEvent(EVENT_ENTER_CELL, e, e->gouid);
  }

  return status;
}


void OnShiftEvent(event_t* e, void* user){
  ent_t* p = user;
  ent_t* slab = e->data;

  if(p->state != STATE_PUSHING || slab->state != STATE_SHIFTING)
    return;

  if(EntGridStep(slab, p->facing) < TILE_ISSUES)
    if(EntGridStep( p, p->facing) < TILE_ISSUES)
      LevelEvent(EVENT_LEVEL_SHIFT, slab, p->gouid);

  SetState(p, STATE_IDLE, NULL);
  SetState(slab, STATE_IDLE, NULL);
}

void OnStaticCollide( event_t* e, void* user){
  ent_t* slab = user;
  ent_t* other = e->data;

  if(!SetState(other, STATE_PUSHING, NULL))
    return;

  if(SetState(slab, STATE_SHIFTING, NULL))
  LevelEvent(EVENT_SHIFT_SLAB, slab, other->gouid);
}

void EntSignalAction(event_t* ev, void* user){
  ent_t* e = user;

  if(!e->action)
    return;
  e->action(ParamMakeObj(DATA_ENT, e->gouid, e), e->action_param);

  e->action = NULL;
  e->action_param = EMPTY_PARAM;

}
