#define CHAR_IMAGE_PATH      "character_asset.png"

// Atlas sprites array
static sub_texture_t CHAR_SPRITES[CHAR_DONE] = {
 { CHAR_DOWN_0,  8, 8, 0, 0, 16, 16,	
   {ANIM_SEQ_NONE, 0,	4},
   {VFX_SHADOW_OBLONG, VFX_SHAD, .125f, 0.1f,	DARKGRAY, 1.25f, 200}
 },
 { CHAR_DOWN_0,  8, 8, 0, 0, 16, 16,
   {ANIM_SEQ_GO_D, 0,	4}
 },
 { CHAR_DOWN_1,  8, 8, 16, 0, 16, 16,	
   {ANIM_SEQ_GO_D, 1,	4}
 },
 { CHAR_DOWN_2,  8, 8, 32, 0, 16, 16,	
   {ANIM_SEQ_GO_D, 2,	4}
 },
 { CHAR_LEFT_0,  8, 8, 48, 0, 16, 16, 
   {ANIM_SEQ_GO_L,	0,	4}
 },
 { CHAR_LEFT_1,  8, 8, 64, 0, 16, 16,	
   {ANIM_SEQ_GO_L,	1,	4}
 },
 { CHAR_LEFT_2,  8, 8, 80, 0, 16, 16,	
   {ANIM_SEQ_GO_L,	2,	4}
 },
 { CHAR_RIGHT_0,  8, 8, 96, 0, 16, 16,	
   {ANIM_SEQ_GO_R,	0,	4}
 },
 { CHAR_RIGHT_1,  8, 8, 112, 0, 16, 16,	
   {ANIM_SEQ_GO_R,	1,	4}
 }, 
 { CHAR_RIGHT_2,  8, 8, 0, 16, 16,	16, 
   {ANIM_SEQ_GO_R,	2,	4}
 },
 { CHAR_UP_0,  8, 8, 16, 16, 16, 16,	
   {ANIM_SEQ_GO_U,	0,	4}
 },
 { CHAR_UP_1,  8, 8, 32, 16, 16, 16,	
   {ANIM_SEQ_GO_U,	1,	4}
 },
 { CHAR_UP_2,  8, 8, 48, 16, 16, 16,	
   {ANIM_SEQ_GO_U,	2,	4}
 },
};

