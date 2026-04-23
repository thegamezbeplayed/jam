#ifndef __GAME_DEFINITION__
#define __GAME_DEFINITION__
#include "game_enum.h"

#define BCTZL(uint) (int){__builtin_ctzll(uint)}


typedef struct{
  EntityType      id;
  CharacterSprite anims[12];
  CharacterSprite base;
}ent_d;

typedef enum{
  SIG_NONE      = 0,
  SIG_ALGIZ     = BIT64(0), //protection
  SIG_ANSUZ     = BIT64(1), //Gods, air
  SIG_BERKANA   = BIT64(2), //Tree
  SIG_DAGAZ     = BIT64(3), //Day
  SIG_EHWAZ     = BIT64(4), //Harmony, teamwork
  SIG_FEHU      = BIT64(5), //wealth
  SIG_GEBO      = BIT64(6), //Gift
  SIG_HAGALAZ   = BIT64(7), //Hail, destruction
  SIG_INGUZ     = BIT64(8), //remove
  SIG_INGWAZ    = BIT64(9), //Gestation, movement
  SIG_ISA       = BIT64(10), //ice
  SIG_JERA      = BIT64(11), //year, harvest
  SIG_KUANAZ    = BIT64(12), //ulcer, torch
  SIG_LAGUZ     = BIT64(13), //water
  SIG_MANNAZ    = BIT64(14), //man
  SIG_NAUTHIZ   = BIT64(15), //need, distress
  SIG_OTHILA    = BIT64(16), //legacy
  SIG_PEORTH    = BIT64(17), //myster, chance, destiny
  SIG_RAIDO     = BIT64(18), //journey, wagon
  SIG_SOL       = BIT64(19), //sun
  SIG_TEIWAZ    = BIT64(20), //justice, victory, sacrifice
  SIG_THURISAZ  = BIT64(21), //Thorn, conflict
  SIG_URUZ      = BIT64(22), // strength, ox
  SIG_WUNJO     = BIT64(23), //joy, fellowship
  SIG_YR        = BIT64(24), // Death
}Signal;
typedef uint64_t Signals;

typedef struct{
  EventType   event;
  Signals     no;
}signal_events_d;

static signal_events_d SIGNAL_EVENTS[EVENT_TILE_END] = {
  {EVENT_TILE_COLLISION},
  {EVENT_TILE_INSERT, SIG_ANSUZ},
  {EVENT_TILE_MOVE},
  {EVENT_TILE_DIST},
  {EVENT_SHIFT_SLAB},
  {EVENT_SIGNAL_ACTION, SIG_URUZ},
};


static Signals TILE_SIGNALS[TILE_DONE] = {
  [SLAB_ALGIZ]    = SIG_ALGIZ,
  [SLAB_ANSUZ]    = SIG_ANSUZ,
  [SLAB_BERKANA]  = SIG_BERKANA,
  [SLAB_DAGAZ]    = SIG_DAGAZ,
  [SLAB_EHWAZ]    = SIG_EHWAZ,
  [SLAB_FEHU]     = SIG_FEHU,
  [SLAB_GEBO]     = SIG_GEBO,
  [SLAB_HAGALAZ]  = SIG_HAGALAZ,
  [SLAB_INGUZ]    = SIG_INGUZ,
  [SLAB_INGWAZ]   = SIG_INGWAZ,
  [SLAB_ISA]      = SIG_ISA,
  [SLAB_JERA]     = SIG_JERA,
  [SLAB_KUANAZ]   = SIG_KUANAZ,
  [SLAB_LAGUZ]    = SIG_LAGUZ,
  [SLAB_MANNAZ]   = SIG_MANNAZ,
  [SLAB_NAUTHIZ]  = SIG_NAUTHIZ,
  [SLAB_OTHILA]   = SIG_OTHILA,
  [SLAB_PEORTH]   = SIG_PEORTH,
  [SLAB_RAIDO]    = SIG_RAIDO,
  [SLAB_SOL]      = SIG_SOL,
  [SLAB_TEIWAZ]   = SIG_TEIWAZ,
  [SLAB_THURISAZ] = SIG_THURISAZ,
  [SLAB_URUZ]     = SIG_URUZ,
  [SLAB_WUNJO]    = SIG_WUNJO,
  [SLAB_YR]       = SIG_YR,

  [TILE_ANSUZ]    = SIG_ANSUZ,
  [TILE_FEHU]     = SIG_FEHU,
  [TILE_GEBO]     = SIG_GEBO,
  [TILE_LAGUZ]    = SIG_LAGUZ,
  [TILE_MANNAZ]   = SIG_MANNAZ,
  [TILE_RAIDO]    = SIG_RAIDO,
  [TILE_SOL]      = SIG_SOL,
  [TILE_THURISAZ] = SIG_THURISAZ,
  [TILE_URUZ]     = SIG_URUZ,
  [TILE_YR]       = SIG_YR,
  [TILE_ISA]      = SIG_ISA
};
#endif

