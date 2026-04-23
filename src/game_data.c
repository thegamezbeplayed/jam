#include <raylib.h>
#include "game_tools.h"
#include "game_process.h"
#include "game_strings.h"

signal_pool_t SigPool;
#define RATIO(s) ((s).ratio(&(s)))

hash_key_t SignalHash(Signal s){
  const char* name = SIG_STR[BCTZL(s)];

  uint64_t hashstr = hash_string_64(name);

  return hash_combine_64(hashstr, s);
}

void RegisterSignal(signal_interaction_d* sig){
  hash_key_t key = SignalHash(sig->signal);

  HashPut(&SigPool.map, key, sig);
  SigPool.count++;
}

void RegisterSignals(void){
    SigPool.cap = TILE_DONE;

    HashInit(&SigPool.map, next_pow2_int(TILE_DONE*2));

    for(int i = 0; i < TILE_DONE; i++){
      signal_interaction_d sig = SLAB_SIGINT[i];
      if(sig.signal == SIG_NONE)
        break;

      RegisterSignal(&SLAB_SIGINT[i]);
    }
}

signal_interaction_d* SignalsGetEntry(Signal s){
  hash_key_t key = SignalHash(s);

  return HashGet(&SigPool.map, key);
}

// Allocates a copy of the filename without extension
char* GetFileStem(const char* filename) {
    const char* dot = strrchr(filename, '.');
    size_t len = dot ? (size_t)(dot - filename) : strlen(filename);

    char* stem = malloc(len + 1);
    if (!stem) return NULL;
    memcpy(stem, filename, len);
    stem[len] = '\0';
    return stem;
}

stat_t* InitStatOnMin(StatType attr, float min, float max){
 stat_t* s = malloc(sizeof(stat_t));
 *s =(stat_t){
    .attribute = attr,
      .min = min,
      .max = max,
      .current = min,
      .ratio = StatGetRatio,
      .increment = 1.0f
  }; return s;
}

stat_t* InitStatOnMax(StatType attr, float val){}
stat_t* InitStatEmpty(void){}
stat_t* InitStat(StatType attr,float min, float max, float amount){
 stat_t* s = malloc(sizeof(stat_t));
 *s =(stat_t){
    .attribute = attr,
      .min = min,
      .max = max,
      .current = amount,
      .ratio = StatGetRatio,
      .increment = 1.0f
  }; return s;

}

bool StatIncrementValue(stat_t* attr,bool increase){
  float inc = attr->increment;
  if(!increase)
    inc*=-1;

  float old = attr->current;
  attr->current+=inc;
  attr->current = CLAMPF(attr->current,attr->min, attr->max);
  float cur = attr->current;
  return true;
}

bool StatChangeValue(struct ent_s* owner, stat_t* attr, float val){}
void StatMaxOut(stat_t* s){}
void StatEmpty(stat_t* s){
  s->current = s->min;
}

float StatGetRatio(stat_t *self){}

anim_set_d DEF_ANIM[ANIM_SEQ_ALL] = {
  {ANIM_SEQ_GO_L, {CHAR_LEFT_0, CHAR_LEFT_1, CHAR_LEFT_2}},
  {ANIM_SEQ_GO_U, {CHAR_UP_0, CHAR_UP_1, CHAR_UP_2}},
  {ANIM_SEQ_GO_R, {CHAR_RIGHT_0, CHAR_RIGHT_1, CHAR_RIGHT_2}},
  {ANIM_SEQ_GO_D, {CHAR_DOWN_0, CHAR_DOWN_1, CHAR_DOWN_2}}
};
