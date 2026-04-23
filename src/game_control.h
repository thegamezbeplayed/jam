#ifndef __GAME_CTRL__
#define __GAME_CTRL__

typedef struct action_key_s action_key_t;
typedef BehaviorStatus (*ActionKeyCallback)(struct ent_s* e, action_key_t a, KeyboardKey k);
  
struct action_key_s{
  ActionType        action;
  int               num_keys;
  KeyboardKey       keys[12];
  ActionKeyCallback fn;
  int               binding;
};

typedef struct{
  ent_t*          owner;
  bool            active, redo;
  int             turn;
  action_key_t    actions[ACT_DONE];
}game_input_t;
extern game_input_t player_input;

void InitInput(ent_t* player);
void InputSync(int turn);
bool InputCheck(int turn);
static void InputToggle(void){
  player_input.active = !player_input.active;
}

#endif
