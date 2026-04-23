#include <stdio.h>
#include <unistd.h>
#include "game_process.h"
#include "game_tools.h"
#include "game_helpers.h"
#include "game_utils.h"
#include "game_ui.h"
#include "game_control.h"
#include "game_strings.h"

world_t world;
game_process_t game_process;
ent_t* player = NULL;

void Subscribe(EventType event, EventCallback cb, void* data){
  event_sub_t* sub = EventSubscribe(world.events, event, cb, data);
  sub->uid = -1;
}


void ScheduleEvent(EventType type, void* data, uint64_t uid, TimeFrame tf, int step){
  switch(tf){
    case TF_TURN:
      step += WorldGetTurn();
      break;
    case TF_UPDATE:
      step += WorldGetTime();
      break;
    default:
      return;
      break;
  }

  event_t* event = GameCalloc("ScheduleEvent",1, sizeof(event_t));

  *event = (event_t){
    .type = type,
    .data = data,
    .iuid = uid,
    .max  = -1,
    .timing = tf,
    .scheduled = step,
  };

  EventSchedule(world.events, event);
}


bool TogglePause(ui_menu_t* m){
  if(game_process.state[SCREEN_GAMEPLAY] == GAME_READY)
    GameSetState(GAME_PAUSE);
  else if (game_process.state[SCREEN_GAMEPLAY] == GAME_PAUSE)
    GameSetState(GAME_READY);

  return true;
}

void GameSetState(GameState state){
  if(game_process.screen != SCREEN_GAMEPLAY)
    return;

  game_process.state[SCREEN_GAMEPLAY] = state;
}

void WorldLevelReset(bool restart){
  int stage = restart? world.stage : world.stage+1;
  world.num_ent = 0;

  if(stage >= LVL_ALL){
    GameTransitionScreen();
    return;
  }
  HashClear(&world.ent_map);
  HashClear(&world.tile_map);

  HashInit(&world.ent_map, MAX_ENTS * 2);
  HashInit(&world.tile_map, MAX_CELLS * 2);


  AssetReset();
  GameFree("WorldReset", world.levels[world.stage]->events);
  UnloadEvents( game_process.events);
  world.stage = stage;
  world.levels[world.stage] = InitLevel(stage);
  LevelSubscribe(EVENT_LEVEL_END, OnWorldEvent, world.levels[stage]);
  game_process.state[SCREEN_GAMEPLAY] = GAME_READY;
  LevelReady(WorldGetLevel());

  if(!restart)
  Subscribe(EVENT_LEVEL_STUCK, OnWorldEvent, world.levels[stage]);
}

void OnWorldEvent(event_t *e, void* user){
  level_t* l = e->data;

  switch(e->type){
    case EVENT_LEVEL_END:
      ScheduleEvent(EVENT_LEVEL_CLEAR, l, l->id, TF_UPDATE, 48);

      //l->events->count = 0;
      break;
    case EVENT_LEVEL_CLEAR:
      WorldLevelReset(false);
      break;
    case EVENT_LEVEL_STUCK:
      WorldLevelReset(true);
      break;
  }
}

void GameReady(){

  WorldInitOnce();
  world.stage = 0;
  world.levels[0] = InitLevel(0);
  LevelReady(WorldGetLevel());
  LevelSubscribe(EVENT_LEVEL_END, OnWorldEvent, world.levels[0]);
  Subscribe(EVENT_LEVEL_CLEAR, OnWorldEvent, world.levels[0]);
  game_process.state[SCREEN_GAMEPLAY] = GAME_READY;
  game_process.children[SCREEN_GAMEPLAY].state[PROCESS_LEVEL] = GAME_READY;
  Subscribe(EVENT_LEVEL_STUCK, OnWorldEvent, world.levels[0]);

}

render_text_t* InitRenderText(const char* text, Vector2 pos,
    int size, Color col, int dur){

  render_text_t* rt = GameCalloc("IntRenderText", 1, sizeof(render_text_t));

  *rt = (render_text_t){
    .pos = pos,
    .size = size,
    .color = col,
    .duration = dur
  };

  strcpy(rt->text, text);

  return rt;
}
void AddFloatingText(render_text_t *rt){
  for (int i = 0; i < MAX_EVENTS; i++){
    if(world.floatytext_used[i])
      continue;

    world.floatytext_used[i] = true;
    world.texts[i] = rt;
    return;
  }
}

void WorldAnnounce(EventType type, Vector2 pos){
   render_text_t* rt = InitRenderText(StringGetAnnouncement(type), pos, 14, BLUE, 48);
   AddFloatingText(rt);
}

int WorldGetEnts(ent_t** results,EntFilterFn fn, void* params){
  int num_results = 0;
  for(int i = 0; i < world.num_ent; i++){
    if(!fn(world.ents[i],params))
      continue;

    results[num_results] = world.ents[i];
    num_results++;
  }

  return num_results;
}

int RemoveEnt(int index){
  int last_pos = world.num_ent -1;

  /*
  if(!FreeEnt(world.ents[index]))
    return 0;
*/
  world.num_ent--;
  if(index!=last_pos){
    world.ents[index] = world.ents[last_pos];
    return 1;
  }

  return 0;

}

int AddEnt(ent_t *e) {
  if (world.num_ent < MAX_ENTS) {
    int index = world.num_ent;
    world.ents[index] = e;
    world.num_ent++;

    return CURRENT_ENT_IDENTIFIER++;
  }
  return -1;
} 

game_object_uid_i RegisterEnt( ent_t *e, Cell pos){
  e->uid = AddEnt(e);

  int index = IntGridIndex(pos.x, pos.y);
  e->pos = pos;
  game_object_uid_i gouid = GameObjectMakeUID("ENTITY", index, WorldGetTime());

  if(e->sprite){
    game_object_uid_i souid =  GameObjectMakeUID("ENTITY_SPRITE", index, WorldGetTime());
    e->sprite->gouid = souid;  
    AssetAdd(e->sprite, e->type);

  }

  if(e->signals >0){
    InitSignals(gouid, e->signals);
    LevelTargetSubscribe(EVENT_SIGNAL_ACTION, EntSignalAction, e, gouid);
  }

  HashPut(&world.ent_map, gouid, e);
  return gouid;
}

game_object_uid_i RegisterMapCell(map_cell_t* mc){
  static int count = 0;
  mc->sprite->is_visible = true;
  game_object_uid_i gouid =  GameObjectMakeUID("MAP_CELL", IntGridIndex(mc->coords.x, mc->coords.y), WorldGetTime());
  game_object_uid_i souid =  GameObjectMakeUID("TILE_SPRITE", count++, WorldGetTime());

  /*
  if(mc->sprite->slice)
    AssetAddTile(mc->sprite);
  */

  if(mc->signals >0)
    InitSignals(gouid, mc->signals);

  HashPut(&world.tile_map, gouid, mc);
  return gouid;
}

void WorldInitOnce(){
  InteractionStep();

  InitInput(player);

}

void WorldPreUpdate(){
  EventBusStep(world.events);
  InteractionStep();
  InputCheck(WorldGetTurn());
}

void WorldFixedUpdate(){
  for(int i = 0; i < world.num_ent; i++){
    switch(world.ents[i]->state){
      case STATE_END:
        i-=RemoveEnt(i);
        break;
      default:
        EntSync(world.ents[i]);
        break;
    }
  }
}

void WorldPostUpdate(){
  for(int i = 0; i < MAX_EVENTS; i++){
    if(!world.floatytext_used[i])
      continue;

    if(world.texts[i]->duration <=0){
      world.floatytext_used[i] =false;
      continue;
    }

    world.texts[i]->duration--;
  
  }
}


void FreeWorld(){
  for (int i = 0; i < world.num_ent; i++){
    RemoveEnt(i);
  }
  world.num_ent = 0;
}

void WorldRender(){
  level_t* l = WorldGetLevel();
  if(l)
  MapRender(l->map);
  
  AssetRender();

  for(int i = 0; i < MAX_EVENTS; i++){
    if(!world.floatytext_used[i])
      continue;
    render_text_t rt = *world.texts[i];
    DrawTextExOutlined(ui.font,rt.text, rt.pos,rt.size,FLOAT_TEXT_SPACING,rt.color,BLACK);
  }

}

void InitGameProcess(){
  for(int s = 0; s<SCREEN_DONE; s++){
    game_process.album_id[s] = -1;
    for(int u = 0; u<UPDATE_DONE;u++){
      game_process.update_steps[s][u] = DO_NOTHING;
    
    }
    game_process.children[s].process= PROCESS_NONE;
    for(int p = 0; p < PROCESS_DONE; p++)
      game_process.children[s].state[p]=GAME_NONE;
  }

  game_process.next[SCREEN_TITLE] = SCREEN_GAMEPLAY;
  game_process.init[SCREEN_TITLE] = InitTitleScreen;
  game_process.finish[SCREEN_TITLE] = UnloadTitleScreen;
  game_process.update_steps[SCREEN_TITLE][UPDATE_DRAW] = DrawTitleScreen;
  game_process.update_steps[SCREEN_TITLE][UPDATE_FRAME] = UpdateTitleScreen;

  game_process.next[SCREEN_GAMEPLAY] = SCREEN_ENDING;
  game_process.init[SCREEN_GAMEPLAY] = InitGameplayScreen;
  game_process.finish[SCREEN_GAMEPLAY] = UnloadGameplayScreen;
  game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_FIXED] = FixedUpdate;
  game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_PRE] = PreUpdate;
  game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_DRAW] = DrawGameplayScreen;
  game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_FRAME] = UpdateGameplayScreen;
  game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_POST] = PostUpdate;
  game_process.album_id[SCREEN_GAMEPLAY] = AudioBuildMusicTracks("bingbong");

  game_process.children[SCREEN_GAMEPLAY].update_steps[PROCESS_LEVEL][UPDATE_FIXED] = LevelFixedUpdate; 
  game_process.children[PROCESS_LEVEL].process = PROCESS_LEVEL;
  game_process.next[SCREEN_ENDING] = SCREEN_TITLE;
  game_process.init[SCREEN_ENDING] = InitEndScreen;
  game_process.finish[SCREEN_ENDING] = UnloadEndScreen;
  game_process.update_steps[SCREEN_ENDING][UPDATE_DRAW] = DrawEndScreen;
  game_process.update_steps[SCREEN_ENDING][UPDATE_FRAME] = UpdateEndScreen;

  game_process.screen = SCREEN_TITLE;
  game_process.state[SCREEN_GAMEPLAY] = GAME_LOADING;
  game_process.events = InitEvents();
  game_process.init[SCREEN_TITLE]();

}

void InitGameEvents(){
  world_data_t wdata = {0};

  game_process.children[SCREEN_GAMEPLAY].process = PROCESS_LEVEL;
  game_process.game_frames = 0; 
  HashInit(&world.ent_map, MAX_ENTS *2);
  HashInit(&world.tile_map, MAX_CELLS *2);

  RegisterSignals();

  world.events = InitEventBus(64);
  GameReady();
}

bool GameTransitionScreen(){
  GameScreen current = game_process.screen;
  GameScreen prepare = game_process.next[current];
  if(game_process.state[current] >= GAME_FINISHED)
    return false;
  game_process.init[prepare]();
  game_process.state[current] = GAME_FINISHED;
  game_process.finish[current]();
  game_process.screen = prepare;
  game_process.state[prepare] = GAME_LOADING;
  AudioPlayMusic(game_process.album_id[prepare]);

  return true;
}

void GameProcessStep(){
  if(game_process.screen != SCREEN_GAMEPLAY)
    return;

  game_process.game_frames++;
  if(game_process.events)
    StepEvents(game_process.events);
}

void GameProcessSync(bool wait){
  if(game_process.state[game_process.screen] > GAME_READY){

    game_process.update_steps[SCREEN_GAMEPLAY][UPDATE_DRAW]();
  
    return;
  }
  
  for(int i = 0; i <UPDATE_DONE;i++){
    if(i > UPDATE_DRAW && wait)
      return;
    game_process.update_steps[game_process.screen][i]();
  }

  for(int i = 0; i < PROCESS_DONE;i++){
    if(game_process.children[game_process.screen].process==PROCESS_NONE)
      continue;
    child_process_t* kids = &game_process.children[game_process.screen];
    for(int j = 0; j < UPDATE_DONE; j++)
      if(kids->update_steps[i][j]!=NULL)
        kids->update_steps[i][j]();
  }
}

void GameProcessEnd(){
  UnloadEvents(game_process.events);
  FreeWorld();
  FreeInteractions();
}

void GameProcessAddEvent(ProcessType t, cooldown_t* cd){
  AddEvent(game_process.events, cd);
}

ent_t* WorldPlayer(void){
  return player;
}

const char* GetLevelString(void){
  level_t* l = WorldGetLevel();
  if(!l)
    return "";
  return TextFormat("Level: %i", l->id);
}
  
map_cell_t* WorldGetTile(hash_key_t key){
  return HashGet(&world.tile_map, key);
} 

ent_t* WorldGetEnt(hash_key_t key){
  return HashGet(&world.ent_map, key);
} 

void WorldRemoveEnt(hash_key_t key){
  HashRemove(&world.ent_map, key);
}

param_t WorldGetParam(DataType type, hash_key_t key){
  param_t p = EMPTY_PARAM;

  switch(type){
    case DATA_ENT:
      ent_t* e = WorldGetEnt(key);
      if(e)
      p = ParamMakeObj(type, key, e);
      break;
    case DATA_MAP_CELL:
      map_cell_t* mc = WorldGetTile(key);
      if(mc)
      p = ParamMakeObj(type, key, mc);
      break;
  }

  return p;
}
