#include "game_process.h"

payload_t* InitPayload(int count, param_t params[count]){
  payload_t* p = GameCalloc("InitPayload", 1, sizeof(payload_t));

  p->params =  GameCalloc("InitPayload", 2, sizeof(param_t));

  for(int i = 0; i < count; i++){
    param_t* e = &p->params[p->count++];

    *e = params[i];
  }

  return p;
}

payload_t* InitPayloadSingle(param_t param){
  payload_t* p = GameCalloc("InitPayloadSingle", 1, sizeof(payload_t));

  p->params = GameCalloc("InitPayload", 1, sizeof(payload_t));

  param_t* e = &p->params[p->count++];
  *e = param;

  return p;
}

static interaction_t interactions[MAX_INTERACTIONS];
static bool interaction_used[MAX_INTERACTIONS]; 

const interaction_t default_interaction = {
  .source_uid = -1,
  .target_uid = -1,
  .timer = NULL
};

int InitInteractions(){
  int i;
  for (i=0; i < MAX_INTERACTIONS; i++){
    interactions[i] = default_interaction;
    interaction_used[i] = false;
  }

  return i;
}

interaction_t* EntInteraction(unsigned int source, unsigned int target, int duration){
 interaction_t* result = malloc(sizeof(interaction_t));

 *result = (interaction_t){
   .source_uid = source,
   .target_uid = target,
   .timer = InitCooldown(duration,EVENT_INTERACTION,NULL,NULL)
 };

 return result;
}

cooldown_t* InitCooldown(int dur, EventType type, CooldownCallback on_end_callback, void* params){
  cooldown_t* cd = malloc(sizeof(cooldown_t)); 

  *cd = (cooldown_t){
    .type = type,
      .is_complete = false,
      .duration = dur,
      .elapsed = 0,
      .on_end_params = params,
      .on_end = on_end_callback// ? on_end_callback : DO_NOTHING
  };
  
  return cd;
}
void UnloadCooldown(cooldown_t* cd){
  if(!cd)
    return;

  *cd = (cooldown_t){0};
}

void UnloadEvents(events_t* ev){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(!ev->cooldown_used[i])
      continue;
    UnloadCooldown(&ev->cooldowns[i]);
    ev->cooldown_used[i] = false;
  }
}

bool AddInteraction(interaction_t* inter){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(interaction_used[i])
      continue;

    interaction_used[i] = true;

    interactions[i] = *inter;

    return true;
  }

  return false;
}

bool CanInteract(int source, int target){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(interactions[i].source_uid != source)
      continue;

    if(interactions[i].target_uid == target)
      return false;
  }

  return true;
}

int GetInteractions(int source){
  int count = 0;
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(interactions[i].source_uid != source)
      continue;

    count++;
  }

  return count;
}

void FreeInteraction(interaction_t* item) {
  int index = item - interactions;
  if (index >= 0 && index < MAX_INTERACTIONS) {
    FreeInteractionByIndex(index);
  }
}

void FreeInteractions(){
  for (int i = 0; i < MAX_INTERACTIONS; i++)
    FreeInteractionByIndex(i);
}

void FreeInteractionByIndex(int i) {
  if (i < 0 && i > MAX_INTERACTIONS)
    return;

  interaction_used[i] = false;
  interactions[i] = default_interaction;
}

void InteractionStep(){
  for (int i = 0; i < MAX_INTERACTIONS; i++){
    if(!interaction_used[i])
      continue;

    if(interactions[i].timer == NULL){
      FreeInteractionByIndex(i);
      continue;
    }

    if(interactions[i].timer->elapsed >= interactions[i].timer->duration){
      FreeInteractionByIndex(i);
      continue;
    }

    interactions[i].timer->elapsed++;
  }
}

int AddEvent(events_t* pool, cooldown_t* cd){
  if(CheckEvent(pool,cd->type))
    return -1;
  
  for (int i = 0; i < MAX_EVENTS; i++){
    if(pool->cooldown_used[i]){
      if(pool->cooldowns[i].type == EVENT_NONE){
        pool->cooldowns[i] = *cd;
        pool->cooldown_used[i] = true;
        return i;
      }
      else
        continue;
    }
    else{
      pool->cooldowns[i] = *cd;
      pool->cooldown_used[i] = true;
      return i;
    }
  }

  return -1;
}

events_t* InitEvents(){
  events_t* ev = malloc(sizeof(events_t));
  *ev =  (events_t) { 0 };

  for(int i = 0; i < MAX_EVENTS; i++){
    ev->cooldown_used[i] = false;
    ev->cooldowns[i].type = EVENT_NONE;
  }

  return ev;
}

int GetEventIndex(events_t* pool, EventType type){
  for(int i = 0; i<MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;

    if(pool->cooldowns[i].type != type)
      continue;


    return i;
  }
  
  return -1;
}


bool CheckEvent(events_t* pool, EventType type){
  for(int i = 0; i<MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;

    if(pool->cooldowns[i].type != type)
      continue;

    if(pool->cooldowns[i].is_complete)
      return false;

    return true;
  }

  return false;
}

void ResetEvent(events_t* pool, EventType type){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;

    if(pool->cooldowns[i].type != type)
      continue;

    pool->cooldowns[i].is_complete = false;
    pool->cooldowns[i].elapsed = 0;
  }

}

void StartEvent(events_t* pool, EventType type){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;
 
    if(pool->cooldowns[i].type != type)
      continue;
  }
}

void StepEvents(events_t* pool){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(!pool->cooldown_used[i])
      continue;

    if(pool->cooldowns[i].type == EVENT_NONE)
      continue;

    if(pool->cooldowns[i].is_complete && !pool->cooldowns[i].is_recycled){
      pool->cooldowns[i]=(cooldown_t){0};
      pool->cooldowns[i].type = EVENT_NONE;
      pool->cooldown_used[i] = false;
      continue;
    }
   /* 
    if(pool->cooldowns[i].is_complete){
      if(!pool->cooldowns[i].is_recycled)
        continue;

      pool->cooldowns[i].is_complete = false;
      pool->cooldowns[i].elapsed = 0;

      continue;
    }
    */
    if(pool->cooldowns[i].elapsed >= pool->cooldowns[i].duration){
      pool->cooldowns[i].is_complete = true;
      pool->cooldowns[i].elapsed = 0;
      if(pool->cooldowns[i].on_end){
        pool->cooldowns[i].on_end(pool->cooldowns[i].on_end_params);
      }
      continue;
    }

    if(pool->cooldowns[i].is_complete && pool->cooldowns[i].is_recycled)
      continue;

    pool->cooldowns[i].elapsed++;
    if(pool->cooldowns[i].on_step)
      pool->cooldowns[i].on_step(pool->cooldowns[i].on_step_params);
  }
}

event_bus_t* InitEventBus(int cap){
  event_bus_t* bus = GameCalloc("InitEventBus",1,sizeof(event_bus_t));
  *bus = (event_bus_t){
    .cap = cap,
      .subs = GameCalloc("InitEventBus", cap, sizeof(event_sub_t))
  };

  HashInit(&bus->scheduled, next_pow2_int(cap*2));
  return bus;
}

void EventBusEnsureCap(event_bus_t* bus){
  if (bus->count < bus->cap)
    return;

  int new_cap = bus->cap + 64;
  bus->subs = GameRealloc("EventBusEnsureCap", bus->subs, new_cap * sizeof(event_sub_t));
  bus->cap = new_cap;
}

void EventBusStep(event_bus_t* bus){
  hash_iter_t iter;
  HashStart(&bus->scheduled, &iter);

  hash_slot_t* s;

  while((s = HashNext(&iter))){
    event_t* e = s->value;

    switch(e->timing){
      case TF_TURN:
        if(e->scheduled > WorldGetTurn())
          continue;
        break;
      case TF_UPDATE:
        if(e->scheduled > WorldGetTime())
          continue;
        break;
      default:
        break;

    }

    EventEmit(bus, e);
    HashRemove(&bus->scheduled, e->gouid);
  }
}

event_sub_t* EventSubscribe(event_bus_t* bus, EventType event, EventCallback cb, void* u_data){
  EventBusEnsureCap(bus);

  event_sub_t* sub =  &bus->subs[bus->count++];
  *sub = (event_sub_t){
    .event = event,
      .cb = cb,
      .user_data = u_data
  };

  return sub;
}

void EventRemove(event_bus_t* bus, uint64_t id){
  if (!bus) return;

  int index = -1;
  for (int i = 0; i < bus->count; i++) {
    if (bus->subs[i].uid != id)
      continue;
    index = i;
    break;
  }

  if (index < 0) return; // not found

  bus->subs[index] = bus->subs[bus->count - 1];
  bus->count--;

}

void EventUnsubscribe(event_bus_t* bus, event_sub_t* sub)
{
  if (!bus || !sub) return;

  // Find the subscription in the array
  int index = -1;
  for (int i = 0; i < bus->count; i++) {
    if (&bus->subs[i] == sub) {
      index = i;
      break;
    }
  }

  if (index < 0) return; // not found

  // Move last element into the removed slot
  bus->subs[index] = bus->subs[bus->count - 1];
  bus->count--;
}

void EventEmit(event_bus_t* bus, event_t* e){
  for (int i = 0; i < bus->count; i++) {
    if (bus->subs[i].event != e->type)
      continue;

    if(e->max != -1 && e->calls >= e->max)
      break;

    if (bus->subs[i].uid != -1
       && bus->subs[i].uid != e->iuid)
      continue;

    bus->subs[i].cb(e, bus->subs[i].user_data);
    e->calls++;
  }

  if(e->max != -1 && e->calls >= e->max)
    EventRemove(bus, e->iuid);
}

game_object_uid_i EventSchedule(event_bus_t* bus, event_t* e){
  game_object_uid_i gouid = hash_combine_64(hash_string_64("EVENT"),
      hash_combine_64(e->iuid, WorldGetTime()));

  e->gouid = gouid;

  HashPut(&bus->scheduled, gouid, e);
  return gouid;
}
