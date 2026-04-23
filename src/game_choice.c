#include "game_utils.h"

choice_pool_t* StartChoice(choice_pool_t** pool, int size, ChoiceFn fn, bool* result){
  if(*pool==NULL){
    *pool = InitChoicePool(size, fn);
    *result = false;
  }
  else{
    (*pool)->choose = fn;
    *result = (*pool)->count > 0;
  }
  return *pool;
}

choice_t* ChoiceById(choice_pool_t* pool, int id){
  if(!pool || pool->count == 0)
    return NULL;

  for (int i = 0; i < pool->count; i++){
    if(pool->choices[i].id == id)
      return &pool->choices[i];
  }

  return NULL;
}

bool ChoiceEnsureCap(choice_pool_t* p){
  if(p->count < p->cap)
    return true;

  int new_cap = p->cap + 4;

  p->choices = GameRealloc("ChoiceEnsureCap", p->choices, new_cap * sizeof(choice_t));
  p->cap = new_cap;

  return true;
}

void EndChoice(choice_pool_t* pool, bool reset){
  pool->total = 0;
  for(int i = 0; i < pool->count; i++)
    pool->total+= pool->choices[i].score;

  if(pool->total < pool->count || reset){
    pool->total = 0;
    for(int i = 0; i < pool->count; i++){
      choice_t* c = &pool->choices[i];
      c->score = c->orig_score;
      pool->total+=c->score;
    }
  }
}

choice_pool_t* InitChoicePool(int size, ChoiceFn fn){
  // Allocate the pool
  choice_pool_t *pool = GameCalloc("InitChoicePool", 1, sizeof(choice_pool_t));
  if (!pool) return NULL;

  // Store selection callback
  pool->choose = fn;

  pool->cap = size;
  // Ensure count starts at zero
  pool->count = 0;

  // Allocate choice entries (NULL-initialized if you prefer lazy allocation)
  pool->choices = GameCalloc("InitChoicePool choices", size, sizeof(choice_t));

  return pool;
}

void DiscardChoice(choice_pool_t* pool, choice_t* self){
  if (!pool || !self || pool->count <= 0)
    return;

  for (int i = 0; i < pool->count; i++) {
    if (pool->choices[i].id == self->id) {
      // Remove from choices by swapping last element
      pool->choices[i] = pool->choices[--pool->count];
      return;
    }
  }
}

void ChoiceReduceScore(choice_pool_t* pool, choice_t* self){
  int avg = pool->total/pool->count;
  self->score -= avg/2;
  if(self->score < 1)
    self->score = 1;
  pool->total -= avg/2;
}


bool AddChoice(choice_pool_t *pool, int id, int score, void *ctx, OnChosen fn){
  if (!pool) return false;

  // Ensure we do not exceed capacity

  if (!ChoiceEnsureCap(pool))
    return false;

  // Allocate a new choice
    choice_t *c = &pool->choices[pool->count++];

    c->score   = score;
    c->orig_score   = score;
    c->context = ctx;
    c->id = id;
    c->cb = fn;
    // Store in pool
    pool->total+= score;
    return true;
}

choice_t* ChooseByWeight(choice_pool_t* pool){
  if(!pool || pool->count == 0)
    return NULL;

  // 1. Compute total weight
  int total = 0;
  for (int i = 0; i < pool->count; i++) {
    choice_t* c = &pool->choices[i];

    int w = c->score;
    if (w > 0) total += w;
  }

  if (total <= 0)
    return NULL; // all weights were zero or negative

  // 2. Pick random number in range
  int r = rand() % total;

  // 3. Find the weighted entry
  int running = 0;
  for (int i = 0; i < pool->count; i++) {
    int w = pool->choices[i].score;
    if (w <= 0) continue;

    running += w;
    if (r >= running)
      continue;

    choice_t* out = &pool->choices[i];
    if(pool->choices[i].cb)
      pool->choices[i].cb(pool,&pool->choices[i]);
    return out;
  }

  return NULL;
}
