#include "game_assets.h"
#include "game_utils.h"
#include "game_math.h"
#include "game_tools.h"
#include "screens.h"
#include "character_asset.h"
#include "tile_assets.h"

asset_manager_t AssMan;

void InitAssetManager(int cap){

  AssMan.layers = GameCalloc("InitAssMan", 1, sizeof(layer_renderer_t));

  AssMan.tile_cap = MAX_SPRITES;
  AssMan.tiles = GameCalloc("InitAssMan", MAX_SPRITES, sizeof(render_asset_t));

  for(int i = 0; i < LAYER_DONE; i++)
    HashInit(&AssMan.layers->sprites[i], next_pow2_int(cap*2));
}

void AssetAdd(sprite_t* s, RenderLayer l){
  AssMan.counts[l]++;
  HashPut(&AssMan.layers->sprites[l], s->gouid, s);

}

void AssetReset(void){
  for(int i = 0; i < LAYER_DONE; i++){
    HashClear(&AssMan.layers->sprites[i]);
    HashInit(&AssMan.layers->sprites[i], AssMan.layers->sprites[i].cap);
    AssMan.counts[i] = 0;
  }
}

void AssetAddTile(sprite_t* s){
  render_asset_t* ass = &AssMan.tiles[AssMan.num_tiles++];

  ass->s = s;
}

void AssetRender(void){
  for(int i = 0; i < AssMan.num_tiles; i++){
    render_asset_t* ass = &AssMan.tiles[AssMan.num_tiles];

    DrawSprite(ass->s);

  }
  hash_iter_t iter;  
  for(int i = 0; i < LAYER_DONE; i++){
    HashStart(&AssMan.layers->sprites[i], &iter);

    hash_slot_t* s;
    while ((s = HashNext(&iter))){
      sprite_t* spr = s->value;
      DrawSprite(spr);
    }
  }
}

void InitResources(){
  Image spritesImg = LoadImage(TextFormat("resources/%s",CHAR_IMAGE_PATH)); 
  SHEETS[SHEET_CHAR].sprite_sheet = GameMalloc("",sizeof(Texture2D));
  SpriteLoadSubTextures(CHAR_SPRITES,SHEET_CHAR,CHAR_DONE);
  *SHEETS[SHEET_CHAR].sprite_sheet = LoadTextureFromImage(spritesImg);


  Image tilesImg = LoadImage(TextFormat("resources/%s",TILE_IMAGE_PATH)); 
  SHEETS[SHEET_TILE].sprite_sheet = GameMalloc("",sizeof(Texture2D));
  SpriteLoadSubTextures(TILE_SPRITES,SHEET_TILE,TILE_DONE);
  *SHEETS[SHEET_TILE].sprite_sheet = LoadTextureFromImage(tilesImg);


}

sprite_t* InitSpriteByID(int id, SheetID s){
  sprite_t* spr = GameCalloc("InitSprite", 1, sizeof(sprite_t));
  sprite_sheet_data_t* data = &SHEETS[s];

  for (int i = 0; i < data->num_sprites; i++){
    if(data->sprites[i]->id != id)
      continue;

    spr->slice = data->sprites[i];
    spr->sheet = data->sprite_sheet;

    spr->is_visible = true;
    spr->offset = spr->slice->offset;
  }

  return spr;

}

anim_t* InitAnim(int cap, float speed){
  anim_t* a = GameCalloc("InitAnim", 1, sizeof(anim_t));

  a->speed = speed;

  return a;

}
anim_player_t* InitAnimations(CharacterSprite set[12], SheetID s, int dur){
  anim_player_t* a = GameCalloc("InitAnimations", 1, sizeof(anim_player_t));

  for (int i = 0; i < ANIM_SEQ_ALL; i++){
    if(DEF_ANIM[i].seq > ANIM_SEQ_NONE)
      a->num_seq++;
    a->anims[i] = InitAnim(4,1);
    for(int j = 0; j < 4; j++){

      CharacterSprite cindex = DEF_ANIM[i].indexes[j];
      if(cindex == CHAR_NONE)
        break;
      a->anims[i]->frames[j] = cindex;
      a->anims[i]->num_seq++;
      sprite_t* spr = InitSpriteByID(cindex, s);

      a->sequences[i][j] = spr;
    }
  }

  int sub = dur / a->num_seq;

  for(int i = 0; i < a->num_seq; i++)
    a->anims[i]->duration = sub;

  return a;
}

sprite_t* InitAnimationByID(CharacterSprite set[12], int base, SheetID s){

  sprite_t* spr = InitSpriteByID(base, s);
  if(!spr)
    return NULL;

  spr->anim = InitAnimations(set, s, 12);
  return spr;

}

void SpriteSync(ent_t* e, sprite_t *spr){
  if(!spr)
    return;

  if(spr->state == ANIM_SEQ_NONE)
  spr->pos = CellToVector2(e->pos, CELL_WIDTH);

  if(spr->anim){
    sprite_t* next = SpriteAnimate(spr);
    if(!next)
      return;

    next->pos = spr->pos;
    next->is_visible = true;

    e->sprite = next;
  }
}

void SpriteOnStateChange(sprite_t* spr, AnimSequence old, AnimSequence s){
}

bool AnimSetSequence(anim_player_t* a, AnimSequence seq){
  anim_t* cur = a->anims[a->cur_seq];

  if(a->cur_seq != ANIM_SEQ_NONE){
  if(!cur || cur->elapsed < cur->duration)
    return false;

  }
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

 return AnimSetSequence(spr->anim, s);
}

bool SpriteSetState(sprite_t* spr, AnimSequence s){
  if(!SpriteCanChangeState(spr, spr->state, s))
    return false;

  AnimSequence old = spr->state;
  spr->state = s;

  SpriteOnStateChange(spr,old,s);

  return true;
}

sprite_t* SpriteAnimate(sprite_t *spr){
  if(spr->anim == NULL)
    return spr;

  if(spr->state == ANIM_SEQ_NONE)
    return spr;

  anim_t* a = spr->anim->anims[spr->state];
  a->elapsed++;
  if(a->elapsed >= a->duration){
    a->cur_index++;
    if(a->on_end_seq)
      a->on_end_seq(spr->owner, spr);

    a->elapsed = 0;
    if(a->cur_index >= a->num_seq){
      a->cur_index = 0;
      if(a->on_complete)
      a->on_complete(spr->owner, spr);


      if(SpriteSetState(spr, ANIM_SEQ_NONE))
        return spr;
    }
  }
  sprite_t* cur = spr->anim->sequences[spr->state][a->cur_index];

  if(cur)
    return cur;

  return spr;
}

void SpriteLerp(ent_t* e, sprite_t* s){
  s->pos = Vector2Lerp(s->pos, s->dest, 0.33f);
}


void SpriteSetOwnerGrid(ent_t* e, sprite_t* s){
  e->old_pos = e->pos;
  e->pos = vec_to_cell(s->dest, CELL_WIDTH);

}

void SpriteAnimateTo(sprite_t *spr, Cell from, Cell to){
  Cell dir = cell_card_dir(from, to);

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

  spr->dest = CellToVector2(to, CELL_WIDTH);
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


void DrawSlice(sprite_t *spr, Vector2 position,float rot){
  sprite_slice_t* slice = spr->slice; 
  Rectangle src = slice->bounds;

  position = Vector2Add(position,spr->offset);
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

  DrawTexturePro(*spr->sheet,src,dst, origin, rot, WHITE);

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
    DrawSlice(s, pos, s->rot);

}
void DrawSprite(sprite_t* s){
  if(!s->slice)
    return;

  if(s->is_visible)
    DrawSlice(s, s->pos,s->rot);
}

void SpriteLoadSubTextures(sub_texture_t* data, SheetID id, int sheet_cap){
  for(int i = 0; i < sheet_cap;i++){
    sub_texture_t sprData = data[i];

    Vector2 center = Vector2FromXY(sprData.originX,sprData.originY);
    Rectangle bounds = RecFromCoords(sprData.positionX,sprData.positionY,sprData.sourceWidth,sprData.sourceHeight);

    sprite_slice_t *spr = GameMalloc("SpriteLoadSubTextures",sizeof(sprite_slice_t));
    memset(spr,0,sizeof(sprite_slice_t));

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
