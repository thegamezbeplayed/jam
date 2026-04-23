#ifndef __GAME_UTIL__
#define __GAME_UTIL__

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "game_common.h"
#include <stdint.h>

#define MAX_PLAYERS 32

#define MAX_BEHAVIOR_TREE 12
#define MAX_NAME_LEN 128

#define ParamRead(o, T) ((T*)((o)->data))

#define COMBO_KEY(a, b) ((a << 8) | b)
#define CALL_FUNC(type, ptr, ...) ((type)(ptr))(__VA_ARGS__)
#define MAKE_ADAPTER(name, T) \
    static void name##_Adapter(void *p) { name((T)p); }
#pragma once
#ifdef __EMSCRIPTEN__
  #include <emscripten/emscripten.h>
#else
  #define EMSCRIPTEN_KEEPALIVE
#endif
void UploadScore(void);

//====FILE & STRINGS====>
char* GetFileStem(const char* filename);
//<==========
//
static inline void DO_NOTHING(void){}
static inline bool BOOL_DO_NOTHING(){return false;}
static inline const char* CHAR_DO_NOTHING(){return "\0";}

typedef struct{
  game_object_uid_i gouid;
  DataType          type;
  void*             data;
  size_t            size;
}param_t;

typedef struct{
  int     count;
  param_t *params;
}payload_t;
payload_t* InitPayload(int count, param_t[count]);
payload_t* InitPayloadSingle(param_t);
static param_t EMPTY_PARAM = {0,DATA_NONE};
static param_t ParamCopyObj(DataType type, game_object_uid_i uid, const void* src, size_t size){
  param_t o;
  o.type = type;
  o.size = size;
  o.gouid = uid;

  if (src && size > 0)
  {
    o.data = GameMalloc("ParamCopyObj", size);
    memcpy(o.data, src, size);
  }
  else
  {
    o.data = NULL;
  }

  return o;
}

static param_t ParamMakeObj(DataType type, game_object_uid_i uid, void* src) {
  param_t o;
  o.type = type;
  o.size = 0;
  o.gouid = uid;
  o.data = src;
  return o;
}

static param_t ParamMakeArray(DataType type, game_object_uid_i uid, void* src, int count, size_t size){

  param_t p = {0};
  p.gouid = uid;
  p.type = type;
  p.size  =  count;
  p.data = src;
  return p;

  if( src && count > 0)
    p.data = GameMalloc("ParamMakeArray", size * count);
    memcpy(p.data, src, size * count);

  return p;

}

static void ParamUpdate(param_t* p, DataType type, size_t size, const void* src)
{
  if (!p) return;

  p->type = type;

  // If size changed, reallocate
  if (p->size != size)
  {
    GameFree("ParamUpdate", p->data);              // must exist in your allocator
    p->data = GameMalloc("ParamUpdate", size);
    p->size = size;
  }

  memcpy(p->data, src, size);
}

static param_t ParamMake(DataType type, size_t size, const void* src) {
  param_t o;
  o.type = type;
  o.size = size;

  o.gouid = -1;
  o.data = GameMalloc("ParamMake", o.size);
  memcpy(o.data, src, o.size);
  return o;
}


static inline param_t ParamMakePtr(DataType type, void* ptr){
  return (param_t){
    .type = type,
    .data = ptr
  };
}

static param_t ParamMakeString(const char* s) {
  size_t len = strlen(s)+1;
  return ParamMake(DATA_STRING, len, s);
}

static inline const char* ParamReadString(param_t* o){
  assert(o);
  assert(o->type == DATA_STRING);
  assert(o->data);
  assert(o->size > 0);

  return (const char*)o->data;
}

static inline float ParamReadFloat(const param_t* o) {
    assert(o->type == DATA_FLOAT);
    assert(o->size == sizeof(float));
    return *(float*)o->data;
}

static inline int ParamReadInt(const param_t* o) {
    assert(o->type == DATA_INT);
    assert(o->size == sizeof(int));
    return *(int*)o->data;
}

static inline Cell ParamReadCell(const param_t* o){
  assert(o->type == DATA_CELL);
  return *(Cell*)o->data;

}

static inline game_object_uid_i ParamReadGOUID(const param_t* o){
  assert(o->type == DATA_GOUID);
  assert(o->size == sizeof(game_object_uid_i));
  return *(game_object_uid_i*)o->data;

}

static param_t ParamClone(const param_t* src) {
    param_t p = *src;

    if (src->size > 0 && src->data) {
        p.data = GameMalloc("ParamClone", src->size);
        memcpy(p.data, src->data, src->size);
    }

    return p;
}

typedef bool (*ParamCompareFn)(param_t *a, param_t *b);
static bool ParamCompareGreater(param_t *a, param_t *b){
  int aval = 0, bval = 0;
  int adec = 0, bdec = 0;

  switch(a->type){
    case DATA_FLOAT:
      Cell afval = float_to_ints(*ParamRead(a, float));
      Cell bfval = float_to_ints(*ParamRead(b, float));
      aval = afval.x;
      adec = afval.y;

      bval = bfval.x;
      bdec = bfval.y;
      aval = aval*100 + adec;
      bval = bval*100 + bdec;
      break;
    case DATA_INT:
      aval = *ParamRead(a, int);
      bval = *ParamRead(b, int);
      break;
    default:
      return false;
      break;
  }
  return GREATER_THAN(aval, bval);
}

static bool ParamCompareAnd(param_t *a, param_t *b){
  uint64_t abits = *ParamRead(a, uint64_t);
  uint64_t bbits = *ParamRead(b, uint64_t);

  return ((abits & bbits) > 0);
}

static bool ParamCompareLesser(param_t *a, param_t *b){
  return LESS_THAN(*ParamRead(a, int) ,*ParamRead(b, int));
}

static bool ParamCompare(param_t *a, param_t *b, ParamCompareFn fn){
  assert(a->type == b->type);

  return fn(a ,b);

}

//====EVENTS===>
typedef void (*CooldownCallback)(void* );


typedef struct{
  uint64_t          uid;
  EventType         type;
  int               duration;
  int               elapsed;
  bool              is_complete;
  bool              is_recycled;
  void*             on_end_params;
  void*             on_step_params;
  CooldownCallback  on_end;
  CooldownCallback  on_step;
}cooldown_t;

cooldown_t* InitCooldown(int dur, EventType type, CooldownCallback on_end_callback, void* params);
void UnloadCooldown(cooldown_t* cd);

typedef struct{
  cooldown_t  cooldowns[MAX_EVENTS];
  bool        cooldown_used[MAX_EVENTS];
}events_t;

events_t* InitEvents();
void UnloadEvents(events_t* ev);
int AddEvent(events_t* pool, cooldown_t* cd);
void StepEvents(events_t* pool);
void StartEvent(events_t* pool, EventType type);
void ResetEvent(events_t* pool, EventType type);
bool CheckEvent(events_t* pool, EventType type);

//<===BEHAVIOR TREES

//forward declare
typedef enum{
  BEHAVIOR_SUCCESS,
  BEHAVIOR_FAILURE,
  BEHAVIOR_RUNNING
}BehaviorStatus;

typedef struct{
  DataType        type;
  int             state;
  StateComparator fn;
  bool            targeted;
}signal_condition_t;

static signal_condition_t SIGCON[COND_ALL] = {
  [COND_ENTER] = {DATA_EVENT, EVENT_ENTER_CELL, EQUAL_TO, true},
  [COND_NEAR] = {DATA_EVENT, EVENT_TILE_DIST, EQUAL_TO, true}
};

typedef bool (*SignalAction)(param_t user, param_t other);
typedef struct{
  Signal            signal;
  Signals           interacts;
  Condition         condition;
  SignalAction      action;
  int               wait;
  int               raw;
  param_t           val;
  game_object_uid_i user;
}signal_interaction_d;

void InitSignals(game_object_uid_i, Signals);
bool ActionGlide(param_t user, param_t other);
bool ActionRepel(param_t user, param_t other);

static signal_interaction_d SLAB_SIGINT[TILE_DONE] = {
  {SIG_ISA, SIG_NONE, COND_ENTER, ActionGlide, 12},
  {SIG_THURISAZ, SIG_NONE, COND_NEAR, ActionRepel,3, 1},
};

typedef struct{
  int         cap, count;
  hash_map_t  map;
}signal_pool_t;
extern signal_pool_t SigPool;
void RegisterSignals(void);
signal_interaction_d* SignalsGetEntry(Signal);

typedef struct choice_s choice_t;
typedef struct choice_pool_s choice_pool_t;
typedef void (*OnChosen)(choice_pool_t* pool, choice_t* self);
  
void DiscardChoice(choice_pool_t* pool, choice_t* self);

typedef choice_t* (*ChoiceFn)(choice_pool_t *pool);
choice_t* ChooseByWeight(choice_pool_t* pool);
void ChoiceReduceScore(choice_pool_t* pool, choice_t* self);
  
struct choice_s{
  unsigned int id, cat;
  int          score, orig_score, cost;
  void*        context;
  uint64_t     flags;
  OnChosen     cb;
};
  
choice_t* ChoiceById(choice_pool_t* pool, int id);
  
struct choice_pool_s{
  unsigned int  id;
  int           cap, desired, count,budget, total;
  uint64_t      flags;
  ChoiceFn      choose;
  choice_t      *choices;
};

static void ShuffleChoices(choice_pool_t* pool){
  for (int i = pool->count - 1; i > 0; i--) {
    int j = rand() % (i + 1);

    choice_t tmp = pool->choices[i];
    pool->choices[i] = pool->choices[j];
    pool->choices[j] = tmp;
  }
}

choice_pool_t* StartChoice(choice_pool_t** pool, int size, ChoiceFn fn, bool* result);
void EndChoice(choice_pool_t* pool, bool reset);
choice_pool_t* InitChoicePool(int size, ChoiceFn fn);
bool AddChoice(choice_pool_t *pool, int id, int score, void *ctx, OnChosen fn);  

#endif
