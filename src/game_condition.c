#include "game_process.h"
#include "game_types.h"
#include "game_helpers.h"

void OnSignalEvent(event_t* ev, void* user){
  signal_interaction_d* sigint = user;
  signal_condition_t sigcon = SIGCON[sigint->condition];

  if(ev->type != sigcon.state)
    return;

  param_t p = WorldGetParam(DATA_ENT, sigint->user);

  param_t other = EMPTY_PARAM;

  switch(ev->type){
    case EVENT_ENTER_CELL:
      break;
    case EVENT_TILE_MOVE:
      ent_t* o = ev->data;
      other = ParamMakeObj(DATA_ENT, o->gouid, 0);
      break;
    case EVENT_TILE_DIST:
      if(ev->payload.type != DATA_INT ||
          sigint->val.type != DATA_INT)
        return;

      int dist = ParamReadInt(&ev->payload);
      int val = ParamReadInt(&sigint->val);
      if(!sigcon.fn(dist, val))
        return;

      map_cell_t* mc = ev->data;
      switch(sigint->condition){
        case COND_NEAR:
          if(!mc->occupant)
            return;

          other = ParamMakeObj(DATA_ENT, mc->occupant->gouid, mc->occupant);
          break;
        default:
          return;
          break;
      }
      break;
    default:
      return;
      break;
  }
  if(sigint->wait == 0)    
  sigint->action(p, other);

  ent_t* e = p.data;
  e->action = sigint->action;
  e->action_param = other;
  LevelScheduleEvent(EVENT_SIGNAL_ACTION, e, ev->iuid, TF_UPDATE, sigint->wait ); 
}

void InitSignalEvent(game_object_uid_i gouid, signal_interaction_d* sint,
    signal_condition_t* scond){

  if(scond->targeted)
    LevelTargetSubscribe(scond->state, OnSignalEvent, sint, gouid);
  else
    LevelSubscribe(scond->state, OnSignalEvent, sint);
}

void InitSignals(game_object_uid_i gouid, Signals signals){
  while(signals){
    Signal sig = signals & -signals;
    signals &= signals -1;

    signal_interaction_d* base = SignalsGetEntry(sig);
    if(!base)
      continue;


    signal_interaction_d* sigint = GameCalloc("InitSignal", 1, sizeof(signal_interaction_d));
        memcpy(sigint, base, sizeof(signal_interaction_d));

    sigint->val = ParamMake(DATA_INT, sizeof(int), &sigint->raw);
    sigint->user = gouid;
    if(!sigint || sigint->signal != sig)
      continue;

    signal_condition_t sigcon = SIGCON[sigint->condition];
    switch(sigcon.type){
      case DATA_EVENT:
        InitSignalEvent(gouid, sigint, &sigcon);
      default:
        continue;
        break;
    }
  }
}

bool ActionGlide(param_t user, param_t other){
  if(user.type != DATA_ENT)
   return false;

  ent_t* e = user.data;

  int x = e->facing.x + e->pos.x;
  int y = e->facing.y + e->pos.y;

  bool out = false;
  if(EntGridStep(e, e->facing) < TILE_ISSUES)
    out = true;

  map_cell_t* mc = MapGetTile(WorldGetMap(), e->pos);
  LevelScheduleEvent(EVENT_LEVEL_CHECK, e, mc->gouid, TF_UPDATE, 4);

  return out;
}


bool ActionRepel(param_t user, param_t other){
  if(other.type != DATA_ENT && user.type != DATA_ENT)
   return false;

  ent_t* e = user.data;

  ent_t* tar = other.data;

  if(!CanSignalEvent(EVENT_SIGNAL_ACTION, tar->signals))
    return false;


  Cell dir = cell_moore_dir(e->pos, tar->pos);
  bool out = EntGridStep(tar, dir) < TILE_ISSUES;

  map_cell_t* mc = MapGetTile(WorldGetMap(), tar->pos);
  LevelScheduleEvent(EVENT_LEVEL_CHECK, tar, mc->gouid, TF_UPDATE, 12);

  return out;
}
