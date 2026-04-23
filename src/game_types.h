#ifndef __GAME_TYPES__
#define __GAME_TYPES__

#include "game_common.h"
#include "game_assets.h"
#include "game_utils.h"
#include "raylib.h"
#include "game_math.h"
#include "game_tools.h"
#include "game_systems.h"

#define GRID_SIZE 128
#define GRID_STEP 32

#define MAX_ENTS 128  

typedef struct ent_s ent_t;
typedef struct bounds_s {
  Vector2     pos,offset;
  float       radius;
  float       width,height;
  //BoundsCheckCall col_check;
} bounds_t;

//===ENT_T===>
struct ent_s{
  int                   uid;
  game_object_uid_i     gouid;
  EntityType            type;
  Cell                  pos, old_pos, facing;
  EntityState           state;
  sprite_t              *sprite;
  Signals               signals;
  SignalAction          action;  
  param_t               action_param;
};

void EntSignalAction(event_t*, void*);
ent_t* InitEnt(EntityType id);
ent_t* InitEntStatic(EntityType t, Tiles r);

void EntToggleTooltip(ent_t* e);

void EntSync(ent_t* e);
void EntAddPoints(ent_t* e,EntityState old, EntityState s);
void EntPrepStep(ent_t *e);
static void EntSetPos(ent_t *e, Cell pos){
  e->pos = pos;
}

bool FreeEnt(ent_t*);
void EntRender(ent_t* e);
void EntControlStep(ent_t *e);
typedef void (*StateChangeCallback)(ent_t *e, EntityState old, EntityState s);

void EntCheckStatus(ent_t* e);
bool CheckEntAvailable(ent_t* e);
bool SetState(ent_t *e, EntityState s,StateChangeCallback callback);
void StepState(ent_t *e);
void OnStateChange(ent_t *e, EntityState old, EntityState s);
bool CanChangeState(EntityState old, EntityState s);

TileStatus EntGridStep(ent_t *e, Cell step);
void OnShiftEvent(event_t* e, void* user);
void OnStaticCollide( event_t* e, void* user);
#endif
