#ifndef __GAME_UI__
#define __GAME_UI__
#include <stdint.h>
#include <stdbool.h>
#include "raylib.h"
#include "game_assets.h"
#include "game_utils.h"

#define MAX_LINE_ITEMS 12
#define MAX_LINE_VAL 4
#define MAX_LINE_LEN 320
#define MAX_SUB_ELE 16
#define MAX_ELEMENTS 128

#define UI_GRID_WIDTH 6
#define UI_GRID_HEIGHT 3
#define ELE_COUNT 43
#if defined(PLATFORM_ANDROID)
#define DEFAULT_MENU_SIZE (Vector2){GetScreenWidth()/2, GetScreenHeight()/2}
#define DEFAULT_MENU_THIN_SIZE (Vector2){GetScreenWidth(), 64*UI_SCALE}
#define DEFAULT_BUTTON_SIZE (Vector2){160*UI_SCALE, 96*UI_SCALE}
#define XS_PANEL_SIZE (Vector2){264*UI_SCALE, 112*UI_SCALE}
#define XS_PANEL_THIN_SIZE (Vector2){264*UI_SCALE, 80*UI_SCALE}
#else
#define DEFAULT_MENU_SIZE (Vector2){GetScreenWidth()/2, GetScreenHeight()*.75f}
#define FIXED_MENU_FULL (Vector2){1600, 1080}
#define DEFAULT_MENU_SIZE (Vector2){GetScreenWidth()/2, GetScreenHeight()*.75f}
#define DEFAULT_MENU_THIN_SIZE (Vector2){GetScreenWidth()/2, 64*UI_SCALE}
#define DEFAULT_BUTTON_SIZE (Vector2){120*UI_SCALE, 48*UI_SCALE}
#define XS_PANEL_SIZE (Vector2){108*UI_SCALE, 64*UI_SCALE}
#define XS_PANEL_THIN_SIZE (Vector2){108*UI_SCALE, 32*UI_SCALE}
#endif
#define LARGE_BUTTON_SIZE     (Vector2){164*UI_SCALE, 32*UI_SCALE}
#define SQUARE_PANEL          (Vector2){96,96}
#define DEFAULT_BUTTON_WIDE   (Vector2){172*UI_SCALE, 48*UI_SCALE}
#define DEFAULT_PANEL_SIZE    (Vector2){GetScreenWidth()*UI_SCALE, 64*UI_SCALE}
#define FIXED_PANEL_HOR       (Vector2){910, 64}
#define FIXED_PANEL_VER       (Vector2){96, 480}
#define STAT_SHEET_PANEL_VER  (Vector2){64, 416}
#define SUB_PANEL_VER  (Vector2){64, 192}
#define DEFAULT_PANEL_THIN_SIZE (Vector2){224*UI_SCALE, 32*UI_SCALE}
#define LARGE_PANEL_THIN_SIZE (Vector2){GetScreenWidth()*UI_SCALE, 32*UI_SCALE}
#define SMALL_PANEL_SIZE (Vector2){192*UI_SCALE, 64*UI_SCALE}
#define SMALL_PANEL_THIN_SIZE (Vector2){184*UI_SCALE, 32*UI_SCALE}
#define DEFAULT_LINE_SIZE (Vector2){2 *UI_SCALE, 64*UI_SCALE}
#define FIXED_GRID_PANEL      (Vector2){124, 64}
#define FIXED_CONTROL         (Vector2){160, 36}
#define FIXED_BUTTON_SIZE     (Vector2){96, 48}
#define FIXED_LABEL_SIZE      (Vector2){128, 48}
#define FIXED_VAL_LABEL_SIZE  (Vector2){80, 18}
#define FIXED_HEADER_SIZE     (Vector2){144, 18}
#define FIXED_BOX_SIZE        (Vector2){36, 36}
#define FIXED_TOOL_TIP        (Vector2){96, 20}
#define FIXED_TITLE_CHAR      (Vector2){32, 48}
#define FIXED_SCREEN_SIZE     (Vector2){450, 64}
#define FIXED_DETAILS_BOX     (Vector2){264, 48}

#define UI_PANEL_RIGHT (Vector2){1472, 0}
#define UI_PANEL_BOT (Vector2){316, 688}
#define UI_PANEL_TOP (Vector2){316, 0}

#define UI_LOG_HOR (Vector2){640, 220}
#define LABEL_LOG (Vector2){608, 18}
#define LIST_LEFT_HAND_PAD 18
#define LIST_RIGHT_HAND_PAD 8

typedef struct element_value_s element_value_t;

typedef enum{
  MENU_INACTIVE,
  MENU_LOAD,
  MENU_READY,
  MENU_CLOSED,
  MENU_ACTIVE,
  MENU_OPENED,
  MENU_FOCUSED,
  MENU_CLOSE,
  MENU_END
}MenuState;

typedef enum{
  ELEMENT_NONE,
  ELEMENT_LOAD,
  ELEMENT_HIDDEN,
  ELEMENT_IDLE,
  ELEMENT_SHOW,
  ELEMENT_FOCUSED,
  ELEMENT_ACTIVATE,
  ELEMENT_TOGGLE,
  ELEMENT_ACTIVATED,
  ELEMENT_DONE
}ElementState;

typedef enum{
  UI_MASK,
  UI_BUTTON,
  UI_LABEL,
  UI_HEADER,
  UI_STATUSBAR,
  UI_PROGRESSBAR,
  UI_PANEL,
  UI_GROUP,
  UI_CONTAINER,
  UI_TAB_PANEL,
  UI_BOX,
  UI_ICON,
  UI_LINE,
  UI_TEXT,
  UI_ANIMATEXT,
  UI_TITLE,
  UI_GAME,
  UI_TOOL_TIP,
  UI_CHAR_SPR,
  UI_BLANK
}ElementType;

typedef enum{
  LAYOUT_FREE,
  LAYOUT_VERTICAL,
  LAYOUT_HORIZONTAL,
  LAYOUT_STACK,
  LAYOUT_GRID,
}UILayout;

typedef enum{
  ALIGN_NONE = 0,
  ALIGN_CENTER  = 0x01,
  ALIGN_LEFT    = 0x02,
  ALIGN_RIGHT   = 0x04,
  ALIGN_TOP     = 0x10,
  ALIGN_MID     = 0x20,
  ALIGN_BOT     = 0x40,
  ALIGN_UNIFORM = 0x200,
  ALIGN_OVER    = 0x100,
}UIAlignment;

typedef enum{
  UI_PADDING,
  UI_PADDING_TOP,
  UI_PADDING_BOT,
  UI_PADDING_LEFT,
  UI_PADDING_RIGHT,
  UI_MARGIN,
  UI_MARGIN_TOP,
  UI_MARGIN_BOT,
  UI_MARGIN_LEFT,
  UI_MARGIN_RIGHT,
  UI_POSITIONING
}UIPosition;

typedef enum{
  MENU_NONE,
  MENU_MAIN,
  MENU_OPTIONS,
  MENU_PAUSE,
  MENU_RECAP,
  MENU_HUD,
  MENU_PLAY_AREA,
  MENU_EXIT,
  MENU_DEBUG,
  MENU_DONE
}MenuId;

typedef enum{
  FETCH_NONE,
  FETCH_UPDATE,
  FETCH_EVENT,
  FETCH_TURN,
  FETCH_ONCE,
  FETCH_ACTIVE,
  FETCH_DONE,
}FetchRate;


typedef struct{
  int               num_val, r_len, r_wid, r_hei, padd_r, padd_l, des_len;
  element_value_t   *values[4];
  const char*       text_format;
}line_item_t;

typedef struct{
  float       min, max, *val;
  line_item_t *left, *right;
}progress_item_t;

line_item_t* InitLineItems(element_value_t **val, int num_val, const char* format);
line_item_t* InitLineItem(element_value_t *val, const char* format);
const char* PrintLine(line_item_t* ln);
char *TextFormatLineItem(line_item_t *item);
void PrintSyncLine(line_item_t* ln, FetchRate poll);
char* PrintElementValue(element_value_t* ev, int spacing[UI_POSITIONING], char* out);

int GuiTooltipControl(Rectangle bounds, const char* text);

typedef struct ui_element_s ui_element_t;
typedef bool (*ElementCallback)( ui_element_t* self);
bool ElementRestartLevel(ui_element_t* self);

typedef enum {
  VAL_ICO,
  VAL_INT,
  VAL_FLOAT,
  VAL_CHAR,
  VAL_LN,
  VAL_PROG,
} ValueType;

typedef element_value_t* (*ElementFetchValue)(element_value_t* e, param_t);
typedef element_value_t* (*ElementSetValue)(ui_element_t* e, param_t);

struct element_value_s{
  FetchRate   rate;
  ValueType type;
  union {
    int             *i;
    float           *f;
    char            *c;
    line_item_t     *l[MAX_LINE_ITEMS];
    sprite_t        *s;
    progress_item_t *p;
  };
  size_t            char_len, num_ln, text_len, text_hei;
  param_t           context;
  int               index;
  bool              reverse;
  ElementFetchValue get_val;
};

typedef void (*ElementValueSync)(ui_element_t* e, FetchRate poll);
void ElementSetText(ui_element_t* e, char* str);

void ElementDynamicValue(ui_element_t* e, FetchRate poll);

typedef param_t (*ElementDataContext)(void*);
param_t ElementGetOwnerContext(void*);
param_t ElementGetOwnerContextParams(void*);
param_t ElementOwnerItemContext(void*);
param_t ElementOwnerChildren(void*);
param_t ElementNiblings(void *);
param_t ElementOwnerTextAt(void*);
param_t ElementPresetContext(void*);
param_t ElementLevelContext(void*);
param_t ElementIndexContext(void* p);

typedef float (*EffectFn)(float t, float b, float c, float d);
typedef struct{
  float       curren_t, b_egin, c_hange, d_ur;
  Color       *target;
  EffectFn    fn;
}ui_effect_t;

struct ui_element_s{
  uint32_t            hash;
  game_object_uid_i   gouid;
  char                name[MAX_NAME_LEN];
  int                 index,active;
  struct ui_menu_s    *menu;
  struct ui_element_s *owner;
  ElementType         type;
  ElementState        state, prior;
  ElementCallback     cb[ELEMENT_DONE];
  Rectangle           bounds;
  float               width,height;
  UILayout            layout;
  UIAlignment         align;
  int                 spacing[UI_POSITIONING];
  char*               text;
  ElementSetValue     set_val;
  ElementValueSync    sync_val;
  element_value_t     *value;
  int                 num_children, num_params;
  ui_element_t*       children[MAX_SUB_ELE];
  ElementDataContext  get_ctx;
  GameObjectParam     params[4];
  param_t             ctx;
  char                delimiter[1];
  ui_effect_t         *effect;
  Color               text_color;
};

typedef struct{
  const char          identifier[MAX_NAME_LEN];
  Vector2             pos, size;
  ElementType         type;
  ElementState        state;
  UILayout            layout;
  UIAlignment         align;
  ElementSetValue     set;
  ElementDataContext  context;
  ElementCallback     cb[ELEMENT_DONE];
  int                 spacing[UI_POSITIONING];
  //ElementValueSync  sync;
  int                 num_children;
  const char          kids[MAX_SUB_ELE][MAX_NAME_LEN];
  GameObjectParam     params[MAX_SUB_ELE][PARAM_ALL];
  char                text[MAX_LINE_LEN];
  const char          format;
  const char          delimiter; 
}ui_element_d;
extern ui_element_d ELEM_DATA[ELE_COUNT];

ui_element_t* InitElement(const char* name, ElementType type, Vector2 pos, Vector2 size, UIAlignment align,UILayout layout);
ui_element_t* GetElement(const char* name);
void ElementStepState(ui_element_t* e, ElementState s);
bool ElementSetState(ui_element_t* e, ElementState s);
void ElementAddChild(ui_element_t *o, ui_element_t* c);
void UISyncElement(ui_element_t* e, FetchRate poll);
bool UICloseOwner(ui_element_t* e);        
bool UIFreeElement(ui_element_t* e);
bool UIHideElement(ui_element_t* e);
bool ElementActivateChildren(ui_element_t*);
bool ElementLoadChildren(ui_element_t*);
bool ElementShow(ui_element_t* e);
bool ElementShowIcon(ui_element_t* e);
bool ElementShowChildren(ui_element_t*);
bool ElementShowPrimary(ui_element_t*);
bool ElementTabToggle(ui_element_t* e);
bool ElementAssignValues(ui_element_t* e);
bool ElementHideSiblings(ui_element_t* e);
bool ElementSyncContext(ui_element_t* e);
bool ElementShowContext(ui_element_t* e);
bool ElementSyncOwnerContext(ui_element_t* e);
bool ElementToggleTooltip(ui_element_t* e);
bool ElementToggle(ui_element_t* e);
bool ElementToggleChildren(ui_element_t* e);
bool ElementDynamicChildren(ui_element_t* e);
bool ElementShowTooltip(ui_element_t* e);
bool ElementSetTooltip(ui_element_t* e);
bool ElementSetActiveTab(ui_element_t* e);
bool ElementSetContext(ui_element_t* e);
bool ElementSetFlash(ui_element_t* e);

struct ui_menu_s;
typedef bool (*MenuCallback)(struct ui_menu_s* self);

element_value_t* GetOwnerValue(ui_element_t* e, param_t context);
element_value_t* GetContextParams(ui_element_t* e, param_t context);
element_value_t* GetOwnerText(ui_element_t* e, param_t context);
element_value_t* GetOwnerTextAt(ui_element_t* e, param_t context);
element_value_t* GetElementName(ui_element_t* e, param_t context);
element_value_t* GetTitleScreen(ui_element_t* e, param_t context);

typedef struct ui_menu_s{
  MenuId        id;
  int           process_id;
  ui_element_t  *element;
  MenuCallback  cb[MENU_END];
  KeyboardKey   key[MENU_END];
  MenuState     state;
  bool          is_modal;
}ui_menu_t;

typedef struct{
  MenuId        id;
  char          element[MAX_NAME_LEN];
  MenuState     state;
  bool          is_modal;
  MenuCallback  cb[MENU_END];
  KeyboardKey   macro[MENU_END];
}ui_menu_d;
extern ui_menu_d MENU_DATA[MENU_DONE];

ui_menu_t InitMenu(MenuId id,Vector2 pos, Vector2 size, UIAlignment align,UILayout layout, bool modal);

bool UIClearElements(ui_menu_t* m);
void UISyncMenu(ui_menu_t* m, FetchRate poll);
bool UICloseMenu(ui_menu_t* m);
bool UISelectOption(ui_element_t* e);
bool UITransitionScreen(ui_element_t* e);
void DrawMenu(ui_menu_t* m);
bool MenuCanChangeState(MenuState old, MenuState s);
bool MenuSetState(ui_menu_t* m, MenuState s);
void MenuOnStateChanged(ui_menu_t* m, MenuState old, MenuState s);
static bool MenuInert(ui_menu_t* self){
  return false;
}
bool MenuTransitionScreen(ui_menu_t* m);
bool MenuActivateChildren(ui_menu_t*);
bool MenuProcessReady(ui_menu_t*);
typedef struct{
  Font         font;
  float        text_size, text_spacing;
  float        title_size, title_spacing;
  Color        title_col;
  //MenuId      open_menu;
  ui_menu_t    menus[MENU_DONE];
  int          layer_base, layer_top;
  int          last_press_frame, current_frame, frame_lock;
  ui_element_t *elements[2][MAX_ELEMENTS];
  RenderTexture2D base,top;
}ui_manager_t;

extern ui_manager_t ui;

void InitUI(void);

void UISync(FetchRate poll);
void UIRender(void);
bool TogglePause(ui_menu_t* m);
static inline bool UI_BOOL_DO_NOTHING(ui_element_t* self){return false;}

float ElementGetWidthSum(ui_element_t *e);
float ElementGetHeightSum(ui_element_t *e);

static state_change_requirement_t ELEM_STATE_REQ[ELEMENT_DONE] = {
  {ELEMENT_NONE, NEVER, ELEMENT_NONE},
  {ELEMENT_LOAD, EQUAL_TO, ELEMENT_NONE},
  {ELEMENT_HIDDEN, ALWAYS, ELEMENT_NONE},
  {ELEMENT_IDLE, ALWAYS, ELEMENT_HIDDEN},
  {ELEMENT_SHOW, ALWAYS, ELEMENT_NONE},
  {ELEMENT_FOCUSED, NOT_EQUAL_TO, ELEMENT_HIDDEN},
  {ELEMENT_ACTIVATE, ALWAYS, ELEMENT_NONE},
  {ELEMENT_TOGGLE, GREATER_THAN, ELEMENT_IDLE},
  {ELEMENT_ACTIVATED, NOT_EQUAL_TO, ELEMENT_HIDDEN}
};
int GuiTitle(Rectangle bounds, const char *text);
int GuiText(Rectangle bounds, const char *text);
int GuiHeader(Rectangle bounds, const char *text);
void GuiAnimatext(Rectangle bounds, const char *text, ui_effect_t*);

#endif
