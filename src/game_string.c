#include "game_strings.h"

const char* StringGetAnnouncement(EventType type){
  announce_string_p *asp = &ANNOUNCE_STRINGS[type];

  bool ready = false;
  choice_pool_t* cp = StartChoice(&asp->picker, asp->count, ChooseByWeight, &ready);

  if(!ready){
    for( int i = 0; i < asp->count; i++){
      AddChoice(cp, i, asp->pool[i].weight, &asp->pool[i], NULL);

    }
  }
  
  ShuffleChoices(cp);
  choice_t* c = cp->choose(cp);

  if(!c || !c->context)
    return ANNOUNCE_STRINGS[type].pool[0].str;

  return ANNOUNCE_STRINGS[type].pool[c->id].str;
}

