#ifndef __GAME_COMMON__
#define __GAME_COMMON__

#include "raylib.h"
#include "game_tools.h"
#include "game_enum.h"
#include "game_lists.h"

#define CELL_WIDTH 27
#define CELL_HEIGHT 30

static inline bool LESS_THAN(int a, int b){
  return a<b;
}

static inline bool GREATER_THAN(int a, int b){
  return a>b;
}

static inline bool NOT_EQUAL_TO(int a, int b){
  return a!=b;
}


static inline bool EQUAL_TO(int a, int b){
  return a==b;
}

static inline bool ALWAYS(int a, int b){
  return true;
}

static inline bool NEVER(int a, int b){
  return false;
}
struct ent_s;   // forward declaration

typedef enum{
  STAT_POINTS,
  STAT_TURNS,
  STAT_ENT,
  STAT_MAX_COLOR,
  STAT_MAX_TYPE,
  STAT_COLOR_MUL,
  STAT_TYPE_MUL,
  STAT_COMBO_MUL,
  STAT_BLANK
}StatType;

struct stat_s;
typedef bool (*StatOwnerCallback)(struct ent_s* owner);
typedef void (*StatCallback)(struct ent_s* owner, float old, float cur);
typedef float (*StatGetter)(struct stat_s* self);
typedef struct stat_s{
  StatType  attribute;
  float     min;
  float     max;
  float     current;
  float     increment;
  StatGetter ratio;
  StatCallback on_stat_change;
  StatOwnerCallback on_stat_empty;
} stat_t;

stat_t* InitStatOnMin(StatType attr, float min, float max);
stat_t* InitStatOnMax(StatType attr, float val);
stat_t* InitStatEmpty(void);
stat_t* InitStat(StatType attr,float min, float max, float amount);
bool StatIncrementValue(stat_t* attr,bool increase);
bool StatChangeValue(struct ent_s* owner, stat_t* attr, float val);
void StatMaxOut(stat_t* s);
void StatEmpty(stat_t* s);
float StatGetRatio(stat_t *self);
//<====STATS

typedef bool (*StateComparator)(int a, int b);

typedef struct{
  int             state;
  StateComparator can;
  int             required;
}state_change_requirement_t;

static state_change_requirement_t CAN_CHANGE[STATE_STUCK+1] = {
  {STATE_NONE, NEVER, STATE_END},
  {STATE_SPAWN, LESS_THAN, STATE_SPAWN},
  {STATE_IDLE, LESS_THAN, STATE_PLACED},
  {STATE_PUSHING, LESS_THAN, STATE_PUSHING},
  {STATE_SHIFTING, LESS_THAN, STATE_PUSHING},
  {STATE_PLACED, ALWAYS, STATE_END},
  {STATE_SOLVED, LESS_THAN, STATE_PLACED},
  {STATE_DIE, LESS_THAN, STATE_DIE},
  {STATE_END, EQUAL_TO, STATE_DIE},
  {STATE_STUCK, LESS_THAN, STATE_PLACED}
};
#endif
