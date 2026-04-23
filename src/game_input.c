#include "game_types.h"
#include "game_process.h"
#include "game_control.h"

game_input_t player_input;

BehaviorStatus InputActionMove(ent_t* e, action_key_t akey, KeyboardKey k){
  Cell dir = CELL_UNSET;

  switch(k){
    case KEY_A:
    case KEY_H:
    case KEY_LEFT:
      dir = CELL_LEFT;
      break;
    case KEY_D:
    case KEY_L:
    case KEY_RIGHT:
      dir = CELL_RIGHT;
      break;
    case KEY_W:
    case KEY_J:
    case KEY_UP:
      dir = CELL_UP;
      break;
    case KEY_S:
    case KEY_K:
    case KEY_DOWN:
      dir = CELL_DOWN;
      break;
    default:
      break;
  }

  if(cell_compare(dir, CELL_UNSET))
    return BEHAVIOR_FAILURE;

  if(EntGridStep(e, dir) > TILE_ISSUES)
  return BEHAVIOR_FAILURE;

  return BEHAVIOR_SUCCESS;
}

void InitInput(ent_t* player){
  player_input.owner = player;

  player_input.actions[ACT_MOVE] = (action_key_t){
    ACT_MOVE,12,{KEY_H, KEY_J, KEY_K, KEY_L, KEY_D,KEY_A,KEY_W,KEY_S,KEY_LEFT, KEY_RIGHT,KEY_UP,KEY_DOWN},InputActionMove, 0};

  player_input.active = true;
}

void InputSync(int turn){
  if(turn == player_input.turn)
    return;

  player_input.turn  = turn;
}

bool InputCheck(int turn){
  if(turn == player_input.turn)
    return false;

  if(!player_input.active)
    return false;

  for(int i = 0; i < ACT_DONE; i++){
    action_key_t akey = player_input.actions[i];

    for (int j = 0; j< akey.num_keys; j++){
      KeyboardKey k = akey.keys[j];


      if(!IsKeyDown(k))
        continue;


      if(akey.fn(player, akey, k) == BEHAVIOR_SUCCESS){
        LevelEvent(EVENT_ENT_ACTION, player, player->gouid);
        
        player_input.turn = WorldGetTurn();
      }
    }
  }
}
