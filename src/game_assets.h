#ifndef __GAME_ASSETS__
#define __GAME_ASSETS__

//#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include "game_utils.h"

#define MAX_SONGS 4
#define MAX_ANIM_FRAMES 4
#define MAX_SPRITES 128
#define MAX_LAYER_SPRITES 64


#define FLOAT_TEXT_SIZE 54
#define FLOAT_TEXT_SPACING 2

typedef struct ent_s ent_t;
typedef struct sprite_s sprite_t;
typedef struct sprite_slice_s sprite_slice_t;

typedef struct {
  int   anim_seq;
  int   anim_index;
  int   anim_dur;
}anim_data_t;

typedef struct{
  int     vfx_id;
  VFXType type;
  float   offx;
  float   offy;
  Color   col;
  float   scale;
  float   rot;
}texture_vfx_t;

typedef struct sub_texture_s {
  int           tag;
  int           originX, originY;
  int           positionX, positionY;
  int           sourceWidth, sourceHeight;
  anim_data_t   adat;
  texture_vfx_t vfx;
  float         scale;
} sub_texture_t;

typedef struct{
  char        text[MAX_NAME_LEN];
  Vector2     pos;
  int         size;
  Color       color;
  int         duration;
}render_text_t;

render_text_t* InitRenderText(const char*, Vector2, int, Color, int);
void AddFloatingText(render_text_t *rt);

void InitResources();

typedef void (*OnAnimCallback)(ent_t*, sprite_t*);
typedef void (*AnimSliceCallback)(sprite_slice_t*);
typedef struct{
  int               num_seq, cur_index;
  CharacterSprite   frames[12];
  int               duration;
  int               elapsed;
  float             speed;
  OnAnimCallback    on_step, on_end_seq, on_complete;
  AnimSliceCallback on_start, on_end;
}anim_t;

typedef struct{
  AnimType         type;
  int              num_seq, cur_seq;
  anim_t           *anims[ANIM_SEQ_ALL];
  sprite_slice_t   *sequences[ANIM_SEQ_ALL][MAX_ANIM_FRAMES];
}anim_player_t;

typedef struct{
  int     duration;
  int     elapsed;
  float   vol;
  bool    increase;
}audio_fade_t;

typedef struct{
  Music        music;
  events_t     *events;
  audio_fade_t *fade;
}music_track_t;

typedef struct{
  unsigned int  index;
  int           num_songs;
  unsigned int  current_index;
  float         volume;
  music_track_t track[MAX_SONGS];
}music_group_t;

typedef enum{
  SFX_ALL,
  SFX_UI,
  SFX_ACTION,
  SFX_IMPORTANT,
  SFX_DONE
}SfxGroup;

typedef enum {
  ACTION_HOVER,
  ACTION_PLACE,
  END_SFX
}SfxType;

typedef struct{
  SfxType     type;
  SfxGroup    group;
  int         num_sounds;
  const char* paths[5];
}sfx_info_t;

typedef struct{
  SfxType   type;
  int       num_sounds;
  Sound     *sounds;
}sfx_sound_t;

static sfx_info_t sfx_catalog[]={
  {ACTION_PLACE,SFX_ACTION,4,{"switch_001.ogg","switch_002.ogg","switch_003.ogg","switch_004.ogg"}},
  {ACTION_HOVER,SFX_ACTION,5,{"click_001.ogg","click_002.ogg","click_003.ogg","click_004.ogg","click_005.ogg"}},
};

typedef struct{
  int         num_types;
  float       volume;
  sfx_sound_t items[END_SFX];
}sfx_group_t;

typedef struct{
  music_group_t   tracks[6];
  float           volume;
  int             current_track;
  int             concurrent_track;
  int             num_tracks;
  sfx_group_t     sfx[SFX_DONE];
  events_t        *timers[SFX_DONE];
}audio_manager_t;

void InitAudio();
int AudioBuildMusicTracks(const char* subdir);
void AudioStep();
void AudioPlaySfx(SfxGroup group, SfxType type, int index);
void AudioPlayRandomSfx(SfxGroup group, SfxType type);
void AudioPlayMusic(int index);
bool AudioPlayNext(music_group_t* t);
void AudioMusicFade(music_track_t* t);

typedef enum{
  SHEET_UI,
  SHEET_ICON,
  SHEET_TILE,
  SHEET_CHAR,
  SHEET_VFX,
  SHEET_ALL
}SheetID;

typedef enum{
  LAYER_ROOT = -1,
  LAYER_BG,
  LAYER_MAIN,
  LAYER_TOP,
  LAYER_DONE
}RenderLayer;

struct sprite_slice_s{
  int               id;
  game_object_uid_i gouid;
  SheetID           sheet;
  int       sequence_index;
  Vector2   center, pos;
  Rectangle bounds;
  Vector2   offset;
  float     scale, rot;
  Color     color;
};

typedef struct{
  int             num_sprites;
  sprite_slice_t  *sprites[128];
  Texture2D       *sprite_sheet;
}sprite_sheet_data_t;

static sub_texture_t* TEXTURES[SHEET_ALL];
static sprite_sheet_data_t SHEETS[SHEET_ALL];
void SpriteLoadSubTextures(sub_texture_t* data, SheetID id, int sheet_cap);

void SpriteLoadSlicedTextures();
//SPRITE_T===>
struct sprite_s{
  game_object_uid_i gouid;
  int               suid;
  AnimSequence      state;
  anim_player_t     *anim;
  texture_vfx_t     *vfx;
  Texture2D         *sheet;
  sprite_slice_t*   root, *slice, *fx;
  bool              is_visible;
  float             rot;
  Vector2           pos, offset, dest;
  RenderLayer       layer;
  struct ent_s      *owner;
};

void DrawSlice(sprite_slice_t*, Vector2 position,float rot);
sprite_t* InitSpriteByID(int id, SheetID);
sprite_t* InitSpriteByIndex(int index, sprite_sheet_data_t* spritesheet);
sprite_t* InitAnimationByID(int, int, SheetID);
sprite_t* InitAnimationVec(int id, SheetID s, int dur);
bool FreeSprite(sprite_t* s);
void DrawSprite(sprite_t* s);
void DrawSpriteAtPos(sprite_t*s , Vector2 pos);
void SpriteSync(struct ent_s* e, sprite_t *spr);
bool SpriteSetAnimSequence(sprite_t* spr, AnimSequence s);
bool SpriteCanChangeState(sprite_t*, AnimSequence old, AnimSequence s);
sprite_slice_t* SpriteAnimate(sprite_t *spr);
bool SpriteCanAnimateTo(sprite_t *spr, Cell from, Cell to);
void SpriteAnimateTo(sprite_t *spr, Cell from, Cell to);
void SpritePreprocessImg(Image *img, Texture2D* out);
void DrawTextExOutlined(Font font, const char *text, Vector2 pos, float fontSize, float spacing, Color textColor, Color outlineColor);

typedef struct{
  param_t     asset;
}render_context_t;

typedef struct{
  hash_map_t    context[LAYER_DONE];
}layer_renderer_t;

typedef struct{
  int                 num_tiles, tile_cap;
  render_context_t*   tiles;
  int                 counts[LAYER_DONE];
  layer_renderer_t*   layers;
}asset_manager_t;

extern asset_manager_t AssMan;
void InitAssetManager(int);
void AssetRender(void);
void AssetAdd(sprite_t*, RenderLayer);
void AssetAddTile(sprite_t* s);
void AssetReset(void);
//====SPRITE_T>>
#endif
