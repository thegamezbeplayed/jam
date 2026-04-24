#include "game_assets.h"
#include "game_utils.h"
#include "game_math.h"
#include "game_tools.h"
#include "screens.h"
#include "vfx_assets.h"
#include "character_asset.h"
#include "tile_assets.h"

asset_manager_t AssMan;

void InitAssetManager(int cap){

  AssMan.layers = GameCalloc("InitAssMan", 1, sizeof(layer_renderer_t));

  AssMan.tile_cap = MAX_SPRITES;
  AssMan.tiles = GameCalloc("InitAssMan", MAX_SPRITES, sizeof(render_context_t));

  for(int i = 0; i < LAYER_DONE; i++)
    HashInit(&AssMan.layers->context[i], next_pow2_int(cap*2));
}

void AssetAdd(sprite_t* s, RenderLayer l){
  AssMan.counts[l]++;
  
  param_t ctx = ParamMakeObj(DATA_SPRITE, s->gouid, s);

  render_context_t* sctx = GameCalloc("AssetAdd", 1, sizeof(render_context_t));
  sctx->asset = ctx;
  HashPut(&AssMan.layers->context[l], s->gouid, sctx);

  if(!s->fx)
    return;

  RenderLayer r = l-1;

  param_t fx = ParamMakeObj(DATA_SLICE, s->fx->gouid, s->fx);

  render_context_t* vtx = GameCalloc("AssetAdd", 1, sizeof(render_context_t));
  vtx->asset = fx;
  HashPut(&AssMan.layers->context[r], s->fx->gouid,  vtx);

}

void AssetReset(void){
  for(int i = 0; i < LAYER_DONE; i++){
    HashClear(&AssMan.layers->context[i]);
    HashInit(&AssMan.layers->context[i], AssMan.layers->context[i].cap);
    AssMan.counts[i] = 0;
  }
}

void AssetAddTile(sprite_t* s){
  render_context_t* ctx = &AssMan.tiles[AssMan.num_tiles++];


  ctx->asset = ParamMakeObj(DATA_SPRITE, s->gouid, s);
}

void AssetRender(void){
  for(int i = 0; i < AssMan.num_tiles; i++){
    render_context_t* ctx = &AssMan.tiles[AssMan.num_tiles];
    if(ctx->asset.type != DATA_SPRITE)
      continue;

    sprite_t* sprite = ctx->asset.data;
    DrawSprite(sprite);

  }
  hash_iter_t iter;  
  for(int i = 0; i < LAYER_DONE; i++){
    HashStart(&AssMan.layers->context[i], &iter);

    hash_slot_t* s;
    while ((s = HashNext(&iter))){
      render_context_t* ctx = s->value;
      switch(ctx->asset.type){
        case DATA_SLICE:
          sprite_slice_t* sl = ctx->asset.data;
          DrawSlice(sl, sl->pos, sl->rot);
          break;
        case DATA_SPRITE:
          sprite_t* spr = ctx->asset.data;
          DrawSprite(spr);
          break;
      }
  
    }
  }
}

void InitResources(){
  TEXTURES[SHEET_CHAR] = CHAR_SPRITES;
  TEXTURES[SHEET_TILE] = TILE_SPRITES;
  Image spritesImg = LoadImage(TextFormat("resources/%s",CHAR_IMAGE_PATH)); 
  SHEETS[SHEET_CHAR].sprite_sheet = GameMalloc("",sizeof(Texture2D));
  SpriteLoadSubTextures(CHAR_SPRITES,SHEET_CHAR,CHAR_DONE);
  *SHEETS[SHEET_CHAR].sprite_sheet = LoadTextureFromImage(spritesImg);

  Image tilesImg = LoadImage(TextFormat("resources/%s",TILE_IMAGE_PATH)); 
  SHEETS[SHEET_TILE].sprite_sheet = GameMalloc("",sizeof(Texture2D));
  SpriteLoadSubTextures(TILE_SPRITES,SHEET_TILE,TILE_DONE);
  *SHEETS[SHEET_TILE].sprite_sheet = LoadTextureFromImage(tilesImg);

  TEXTURES[SHEET_VFX] = VFX_SPRITES;
  Image vfxImg = LoadImage(TextFormat("resources/%s",VFX_IMAGE_PATH)); 
  SHEETS[SHEET_VFX].sprite_sheet = GameMalloc("",sizeof(Texture2D));
  SpriteLoadSubTextures(VFX_SPRITES,SHEET_VFX,VFX_DONE);
  *SHEETS[SHEET_VFX].sprite_sheet = LoadTextureFromImage(vfxImg);

}

sprite_slice_t* InitSliceByID(int id, SheetID s){
  sprite_slice_t* spr = GameCalloc("InitSlice", 1, sizeof(sprite_slice_t));
  sprite_sheet_data_t* data = &SHEETS[s];

  for (int i = 0; i < data->num_sprites; i++){
    if(data->sprites[i]->id != id)
      continue;

    spr = data->sprites[i];
    return spr;
  }

  return NULL;

}

sprite_slice_t* InitSpriteVfx(sprite_t* spr, texture_vfx_t vfx){

  sprite_slice_t* fx = InitSliceByID(vfx.vfx_id, SHEET_VFX);

  uint64_t key = GameObjectMakeUID("VFX", fx->id, spr->gouid);

  fx->gouid = key;
 
  Vector2 r_bounds = RectSize(spr->root->bounds);

  Vector2 v_bounds = RectSize(fx->bounds);

  float x = r_bounds.x / v_bounds.x;
  float y = r_bounds.y / v_bounds.y;

  fx->scale = fminf(x,y);

  fx->scale *= vfx.scale;
  fx->color = vfx.col;

  fx->color.a*=0.67f;

  fx->rot = vfx.rot;
  Vector2 offset = Vector2XY(vfx.offx*r_bounds.x, vfx.offy * r_bounds.y);
  fx->offset = RotateOffset(offset, vfx.rot);
  return fx;
}

sprite_t* InitSpriteByID(int id, SheetID s){
  sprite_t* spr = GameCalloc("InitSprite", 1, sizeof(sprite_t));
  sprite_sheet_data_t* data = &SHEETS[s];

  for (int i = 0; i < data->num_sprites; i++){
    if(data->sprites[i]->id != id)
      continue;

    spr->slice = data->sprites[i];
    spr->root = data->sprites[i];
    spr->sheet = data->sprite_sheet;

    spr->is_visible = true;
    spr->offset = spr->slice->offset;
    if(TEXTURES[s][i].vfx.vfx_id > VFX_NONE){
      spr->fx = InitSpriteVfx(spr, TEXTURES[s][i].vfx);
    }

    return spr;
  }

  return NULL;

}


void SpriteSliceToggle(sprite_slice_t* s){
}

anim_t* InitAnim(int cap, float speed){
  anim_t* a = GameCalloc("InitAnim", 1, sizeof(anim_t));

  a->speed = speed;
  return a;

}

anim_player_t* InitAnimations(int id, int end, SheetID s){
  anim_player_t* a = GameCalloc("InitAnimations", 1, sizeof(anim_player_t));
  a->type = AT_SPRITE;
  sub_texture_t* data = TEXTURES[s];

  for (int i = 0; i < ANIM_SEQ_ALL; i++)
    a->anims[a->num_seq++] = InitAnim(4,1);

  for (int i = 0; i < end; i++){
    int seq_index = 0;
    anim_data_t adat = data[i].adat;
    int seq = adat.anim_seq;

    CharacterSprite cindex = data[i].tag;
    if(cindex == CHAR_NONE)
      continue;
    seq_index = a->anims[seq]->num_seq++;
    a->anims[seq]->duration = adat.anim_dur;
    a->anims[seq]->frames[seq_index] = cindex;
    sprite_slice_t* spr = InitSliceByID(cindex, s);

    a->sequences[seq][seq_index] = spr;
  }

  return a;
}

sprite_t* InitAnimationVec(int id, SheetID s, int dur){

  sprite_t* spr = InitSpriteByID(id, s);
  if(!spr)
    return NULL;

  spr->anim = GameCalloc("InitAnimations", 1, sizeof(anim_player_t));
  anim_player_t* a = spr->anim;
  a->type = AT_VEC;

  for (int i = 0; i < ANIM_SEQ_ALL; i++){
    a->anims[a->num_seq++] = InitAnim(4,1);

    a->anims[i]->duration = dur;

    for (int j = 0; j < 4; j++){
      a->sequences[i][a->anims[i]->num_seq++] = spr->slice;

    }
  }
  return spr;

}

sprite_t* InitAnimationByID(int id, int end, SheetID s){

  sprite_t* spr = InitSpriteByID(id, s);
  if(!spr)
    return NULL;

  spr->anim = InitAnimations(id, end, s);
  return spr;

}

void SpriteSync(ent_t* e, sprite_t *spr){
  if(!spr)
    return;

  if(spr->state == ANIM_SEQ_NONE)
  spr->pos = CellToVector2(e->pos, CELL_WIDTH);

  spr->slice->pos = spr->pos;
  spr->root->pos = spr->pos;
  if(spr->fx)
    spr->fx->pos = spr->pos;
  if(spr->anim){
    sprite_slice_t* next = SpriteAnimate(spr);
    if(!next)
      return;

    e->sprite->slice = next;
  }
}

void SpriteOnStateChange(sprite_t* spr, AnimSequence old, AnimSequence s){
  if(!spr->anim)
    return;
}

bool AnimSetSequence(anim_player_t* a, AnimSequence seq){
  anim_t* cur = a->anims[a->cur_seq];
/*
  if(a->cur_seq != ANIM_SEQ_NONE){
  if(!cur || cur->elapsed < cur->duration)
    return false;

  }
  */
  a->cur_seq = seq;

  cur = a->anims[seq];
  cur->elapsed = 0;
  return true;
}

bool SpriteCanChangeState(sprite_t* spr, AnimSequence old, AnimSequence s){
  if(spr->anim == NULL)
    return false;

  if(old == s)
    return false;

 if(s == ANIM_SEQ_NONE)
  return true;

 return true;
}

bool SpriteSetState(sprite_t* spr, AnimSequence s){
  if(!SpriteCanChangeState(spr, spr->state, s))
    return false;

  AnimSetSequence(spr->anim, s);
  AnimSequence old = spr->state;
  spr->state = s;

  if(spr->anim && spr->anim->type == AT_SPRITE)
  SpriteOnStateChange(spr,old,s);

  return true;
}

sprite_slice_t* SpriteAnimate(sprite_t *spr){
  if(spr->anim == NULL)
    return spr->slice;

  if(spr->state == ANIM_SEQ_NONE)
    return spr->slice;

  int seq = spr->state;
  anim_t* a = spr->anim->anims[seq];
  sprite_slice_t* cur = spr->anim->sequences[seq][a->cur_index];
  a->elapsed++;
  if(a->elapsed >= a->duration){
    a->cur_index++;
    a->elapsed = 0;
    if(a->on_end_seq)
      a->on_end_seq(spr->owner, spr);

    if(a->on_end)
      a->on_end(cur);

    if(a->cur_index >= a->num_seq){
      a->cur_index = 0;
      if(a->on_complete)
      a->on_complete(spr->owner, spr);

      SpriteSetState(spr, ANIM_SEQ_NONE);
    }
    cur = spr->anim->sequences[seq][a->cur_index];
    if(a->on_start)
      a->on_start(cur);
  }

  if(cur)
    return cur;

  return NULL;
}

void SpriteLerp(ent_t* e, sprite_t* s){
  s->pos = Vector2Lerp(s->pos, s->dest, 0.33f);
}


void SpriteSetOwnerGrid(ent_t* e, sprite_t* s){
  e->old_pos = e->pos;
  e->pos = vec_to_cell(s->dest, CELL_WIDTH);

}

bool SpriteCanAnimateTo(sprite_t *spr, Cell from, Cell to){
  Cell dir = cell_card_dir(from, to);

  spr->dest = CellToVector2(to, CELL_WIDTH);
  int grid_index = IntGridIndex(dir.x, dir.y);

  AnimSequence seq = ANIM_SEQ_NONE; 

  switch(grid_index){
    case GRID_U:
      seq = ANIM_SEQ_GO_U;
      break;
    case GRID_D:
      seq = ANIM_SEQ_GO_D;
      break;
    case GRID_R:
      seq = ANIM_SEQ_GO_R;
      break;
    case GRID_L: 
      seq = ANIM_SEQ_GO_L;
      break;
    default:
      break;
  }

  if(seq == ANIM_SEQ_NONE)
    return false;

  return SpriteCanChangeState(spr, spr->state, seq);

}

void SpriteAnimateTo(sprite_t *spr, Cell from, Cell to){
  Cell dir = cell_card_dir(from, to);

  spr->dest = CellToVector2(to, CELL_WIDTH);
  int grid_index = IntGridIndex(dir.x, dir.y);
  
  AnimSequence seq = ANIM_SEQ_NONE;

  switch(grid_index){
    case GRID_U:
      seq = ANIM_SEQ_GO_U;
      break;
    case GRID_D:
      seq = ANIM_SEQ_GO_D;
      break;
    case GRID_R:
      seq = ANIM_SEQ_GO_R;
      break;
    case GRID_L:
      seq = ANIM_SEQ_GO_L;
      break;
    default:
      break;
  }

  if(seq == ANIM_SEQ_NONE)
    return;

  if(!SpriteSetState(spr, seq))
    return;

  spr->anim->anims[seq]->on_end_seq = SpriteLerp;
  spr->anim->anims[seq]->on_complete = SpriteSetOwnerGrid;
}

void DrawTextExOutlined(Font font, const char *text, Vector2 pos, float fontSize, float spacing, Color textColor, Color outlineColor) {
    float outline = 2;

    for (int y = -outline; y <= outline; y++) {
        for (int x = -outline; x <= outline; x++) {
            if (x == 0 && y == 0) continue;
            DrawTextEx(font, text, (Vector2){pos.x + x, pos.y + y}, fontSize, spacing, outlineColor);
        }
    } 
    
    DrawTextEx(font, text, pos, fontSize, spacing, textColor);
}

void DrawSlice(sprite_slice_t *slice, Vector2 position,float rot){
  Rectangle src = slice->bounds;

  position = Vector2Add(position,slice->offset);
  Rectangle dst = {
    position.x,
    position.y,
    slice->bounds.width * slice->scale,
    slice->bounds.height * slice->scale
  };

  Vector2 origin = (Vector2){
    slice->center.x * slice->scale,//offset.x,
      slice->center.y * slice->scale//offset.y
  };

  Texture sheet = *SHEETS[slice->sheet].sprite_sheet;
  Color col = slice->color.a > 0? slice->color: WHITE; 
  DrawTexturePro(sheet,src,dst, origin, rot, col);

  return;
}

bool FreeSprite(sprite_t* s){
  if(!s) return false;

  if(s->anim)
    free(s->anim);

  //for(int i = 0; i<SHADER_DONE;i++)
  //free(s->gls[i]);

  free(s);
  return true;
}

void DrawSpriteAtPos(sprite_t*s , Vector2 pos){
  if(s->is_visible)
    DrawSlice(s->slice, s->pos, s->rot);

}

void DrawSprite(sprite_t* s){
  if(!s->slice)
    return;

  if(s->is_visible)
    DrawSlice(s->slice, s->pos, s->rot);
}

void SpriteLoadSubTextures(sub_texture_t* data, SheetID id, int sheet_cap){
  for(int i = 0; i < sheet_cap;i++){
    sub_texture_t sprData = data[i];

    Vector2 center = Vector2FromXY(sprData.originX,sprData.originY);
    Rectangle bounds = RecFromCoords(sprData.positionX,sprData.positionY,sprData.sourceWidth,sprData.sourceHeight);

    sprite_slice_t *spr = GameMalloc("SpriteLoadSubTextures",sizeof(sprite_slice_t));
    memset(spr,0,sizeof(sprite_slice_t));

    spr->sheet = id;
    spr->scale = sprData.scale == 0? 1: sprData.scale;
    spr->id = sprData.tag;
    spr->center = center;// Vector2Scale(offset,spr->scale);
    spr->offset = VECTOR2_ZERO;//offset;//center;//Vector2Scale(center,spr->scale);
    spr->bounds = bounds;
    SHEETS[id].sprites[SHEETS[id].num_sprites++] = spr;
  }
}

void SpritePreprocessImg(Image *img, Texture2D *out){
  ImageFormat(img, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
  // Access pixels
  Color *pixels = LoadImageColors(*img);

  int pixelCount = img->width * img->height;
  for (int i = 0; i < pixelCount; i++)
  {
    // If pixel is white (tolerate near-white)
    if (pixels[i].r > 240 && pixels[i].g > 240 && pixels[i].b > 240)
    {
      pixels[i].a = 0; // Make transparent
    }
  }

  // Apply modified pixels back to image
  Image newImg = {
    .data = pixels,
    .width = img->width,
    .height = img->height,
    .format = img->format,
    .mipmaps = 1
  };

  // Create a texture from modified image
  *out = LoadTextureFromImage(newImg);
  UnloadImageColors(pixels);
}
