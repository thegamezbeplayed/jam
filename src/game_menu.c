//#include "game_ui.h"
#include "game_utils.h"
#include "screens.h"
#include "game_tools.h"
#include "game_helpers.h"
#include "game_process.h"
#include "game_ui.h"
#include "game_strings.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

static ui_element_t* active_tooltip = NULL;


ui_element_t* InitElementByName(const char* name, ui_menu_t* m, ui_element_t* o){
  ui_element_d d;

  for (int i = 0; i < ELE_COUNT; i++){
    if(strcmp(name, ELEM_DATA[i].identifier) != 0)
      continue;

    ui_element_d d = ELEM_DATA[i];
    ui_element_t *e = GameCalloc("InitElementByName", 1,sizeof(ui_element_t));

    *e = (ui_element_t){
      .hash     = hash_str_32(d.identifier),
      .type     = d.type,
      .state    = d.state,
      .layout   = d.layout,
      .align    = d.align,
      .set_val  = d.set,
      .get_ctx  = d.context,
      .menu     = m,
      .owner    = o,
      .bounds   = Rect(d.pos.x,d.pos.y,d.size.x,d.size.y),
      .width    = d.size.x,
      .height   = d.size.y,
      .delimiter = d.delimiter
    };

    e->gouid = GameObjectMakeUID(d.identifier, d.type, WorldGetTime());

    e->text = GameCalloc("InitElementByName",1, MAX_LINE_LEN);

    if(d.text[0] == '\0')
      e->text[0] = '\0';
    else
      ElementSetText( e, d.text);
/*

   if(d.texture > 0)
      e->texture = InitScalingElement(d.texture); 
      */
    strcpy(e->name ,d.identifier);
    replace_char(e->name, '_', ' ');
    for (int j = 0; j < UI_POSITIONING; j++)
      e->spacing[j] = d.spacing[j];

    for (int j = 0; j < ELEMENT_DONE; j++){
      if(d.cb[j])
        e->cb[j] = d.cb[j];
    }

    char prev_elem[MAX_NAME_LEN];
    for (int j = 0; j < d.num_children; j++){
      const char* c_name;
      if (d.kids[j][0] =='\0')
        c_name = prev_elem;
      else{
        strcpy(prev_elem, d.kids[j]);
        c_name = d.kids[j];
      }
      ui_element_t* c = InitElementByName(c_name, m, e);
      if(!c)
        continue;

      ElementAddChild(e, c);
      for(int k = 0; k < PARAM_ALL; k++){
        if(d.params[j][k] == PARAM_NONE)
          break;

        c->params[c->num_params++] = d.params[j][k];
      }
    }
    return e;

  }
  TraceLog(LOG_WARNING, "===== ELEMENT %s not found =====", name);
  return NULL;
}

void InitMenuById(MenuId id){
  ui_menu_d d = MENU_DATA[id];

  ui_menu_t m = {0};

  m.id = d.id;
  m.state = d.state;
  m.is_modal = d.is_modal;

  m.element = InitElementByName(d.element, &m, NULL);
  
  for(int i = 0; i < MENU_END; i++){
    if(d.cb[i])
      m.cb[i] = d.cb[i];
    else
      m.cb[i] = MenuInert;

    m.key[i] = d.macro[i];
  }

  ui.menus[id] = m;
}

//void GuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color baseColor) { }
ui_manager_t ui;
void InitUI(void){
  Font font = LoadFontEx("resources/fonts/kenney-pixel-square.ttf", 64, 0, 0);
  SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
  GuiSetFont(font);
  ui.font = GuiGetFont();
#if defined(PLATFORM_WEB)
  GuiSetStyle(DEFAULT,TEXT_SIZE,20);
#elif defined(PLATFORM_ANDROID)
  GuiSetStyle(DEFAULT,TEXT_SIZE,32);
#else
  GuiSetStyle(DEFAULT,TEXT_SIZE,14);
#endif

  Color col = DARKBLUE;
  col.a = 100;
  GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(col));
  col.a = 200;
  GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(col));
  GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(col));

  ui.title_size = 32;
  ui.title_col = BLUE,
  ui.text_size = GuiGetStyle(DEFAULT, TEXT_SIZE);
  ui.text_spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
  SetTextureFilter(font.texture, TEXTURE_FILTER_POINT);
  GuiSetStyle(DEFAULT,TEXT_ALIGNMENT,TEXT_ALIGN_LEFT);
  GuiSetStyle(BUTTON,TEXT_ALIGNMENT,TEXT_ALIGN_MIDDLE);
  GuiSetStyle(DEFAULT,TEXT_ALIGNMENT_VERTICAL,TEXT_ALIGN_TOP);
  GuiSetStyle(LABEL,TEXT_ALIGNMENT,TEXT_ALIGN_LEFT);
  GuiSetStyle(STATUSBAR,TEXT_SIZE,18);
  GuiSetStyle(TEXTBOX,TEXT_SIZE,8);

  GuiSetStyle(DEFAULT, TEXT_LINE_SPACING, 16);
  int pstyle = GuiGetStyle(STATUSBAR, TEXT_PADDING);
  GuiSetStyle(LABEL, TEXT_PADDING, 3);
  GuiSetStyle(STATUSBAR, TEXT_PADDING, 1);
  GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(WHITE));

  InitMenuById(MENU_MAIN);
  InitMenuById(MENU_HUD);
  InitMenuById(MENU_RECAP);
}

ui_menu_t InitMenu(MenuId id,Vector2 pos, Vector2 size, UIAlignment align,UILayout layout, bool modal){
  ui_menu_t m = {0};

  m.is_modal = modal;
  ElementType menutype = UI_PANEL;
  if(m.is_modal){
    m.cb[MENU_CLOSE] = UICloseMenu;
    menutype = UI_MASK;
  }

  m.element = InitElement("UI_MENU_DOM",menutype,pos,size,align,layout);
  for(int i = 0; i < MENU_END; i++)
    m.cb[i] = MenuInert;

  return m;
}

ui_element_t* GetElement(const char* name){
  /*
  uint32_t hash = hash_str_32(name);

  for (int i = 0; i < MAX_ELEMENTS; i++){
    if(ui.elements[i]->hash != hash)
      continue;

    return ui.elements[i];
  }
*/
  return NULL;
}

ui_element_t* InitElement(const char* name, ElementType type, Vector2 pos, Vector2 size, UIAlignment align,UILayout layout){
  ui_element_t* u = GameMalloc("game_menu: InitElement", sizeof(ui_element_t));
  *u = (ui_element_t) {0};

  u->hash = hash_str_32(name);
  u->num_children = 0;
  u->type = type;
  u->state = ELEMENT_IDLE;
  u->sync_val = NULL;//CHAR_DO_NOTHING;
  u->bounds = Rect(pos.x,pos.y,size.x,size.y);
  u->width = size.x;
  u->height = size.y;
  u->text = GameCalloc("InitElement",1, MAX_LINE_LEN);
  u->text[0] = '\0';

  //strcpy(u->text,name);
  for(int i = 0; i < ELEMENT_DONE; i++)
    u->cb[i] = UI_BOOL_DO_NOTHING;

  for (int i = 0; i < UI_POSITIONING; i++)
    u->spacing[i] = 0.0f;

  u->align = align;
  u->layout= layout;
  return u;
}

void ElementAddChild(ui_element_t *o, ui_element_t* c){
  Vector2 pos_adjustment = Vector2FromXY(c->bounds.x,c->bounds.y);
  c->index = o->num_children;
  c->menu = o->menu;
  c->owner = o;
  o->children[o->num_children++] = c;
}

void ElementResize(ui_element_t *e){
 float centerx = VECTOR2_CENTER_SCREEN.x;
  float centery = VECTOR2_CENTER_SCREEN.y;
  float xinc = e->bounds.x;
  float yinc = e->bounds.y;

  e->bounds.x = e->bounds.y = 0;
  float omarginx = e->spacing[UI_MARGIN] + e->spacing[UI_MARGIN_LEFT];
  float omarginy = e->spacing[UI_MARGIN] + e->spacing[UI_MARGIN_TOP];

  float paddingx = 0;
  float paddingb = 0;
  float paddingr = 0;
  float paddingy = 0;
  float cwidths =0, cheights= 0;

  float tallest = e->bounds.height;
  float widest = e->bounds.width;
  for(int i = 0; i<e->num_children; i++){
    if(e->children[i]->align & ALIGN_OVER){
      DO_NOTHING();
    }
    else{
      int cwidth = ElementGetWidthSum(e->children[i]);
      if(cwidth > cwidths)
        cwidths = cwidth;

      int c_hei = ElementGetHeightSum(e->children[i]);
      
      switch(e->layout){
        case LAYOUT_VERTICAL:
          c_hei += e->spacing[UI_PADDING_TOP];
          c_hei += e->spacing[UI_PADDING_BOT];
          cheights+=c_hei;
          break;
        case LAYOUT_HORIZONTAL:
          if(c_hei > cheights)
            cheights = c_hei;
          break;
        case LAYOUT_GRID:
          cheights = +c_hei;
          break;
        case LAYOUT_FREE:
          int widths = ElementGetWidthSum(e->children[i]);
          if (widths > e->bounds.width)
            cwidths = widths;
          break;
        case LAYOUT_STACK:
          if(c_hei > tallest)
            tallest = c_hei;
          if(cwidth > widest)
            widest = cwidth;
         
          cwidths = widest;
          cheights = tallest;
          break;
        default:
          if(e->children[i]->height > cheights)
            cheights = e->children[i]->height;
          if(e->children[i]->width > cwidths)
            cwidths = e->children[i]->width;
          break;
      }
    }
  }

  int t_wid = cwidths + e->spacing[UI_PADDING_RIGHT];
  int t_hei = cheights + e->spacing[UI_PADDING_BOT];
  if( t_wid > e->bounds.width)
    e->bounds.width = t_wid;
  if( t_hei > e->bounds.height)
    e->bounds.height = t_hei;
  
  UIAlignment align = e->align;
  if(e->owner && e->owner->layout != LAYOUT_FREE){
    xinc = e->owner->bounds.x;
    yinc = e->owner->bounds.y;
    centerx = e->owner->bounds.x + e->owner->bounds.width/2;
    centery = e->owner->bounds.y + e->owner->bounds.height/2;
    //align = e->owner->align;
  }

  if(align & ALIGN_CENTER)
    xinc = centerx-e->bounds.width/2;
  if(align & ALIGN_MID)
    yinc = centery- e->bounds.height/2;
  if(align & ALIGN_LEFT)
  e->bounds.x = 0;
  if(align & ALIGN_RIGHT)
    e->bounds.x = e->owner->bounds.x + e->owner->bounds.width - e->bounds.width;
  //xinc += omarginx;
  //yinc += omarginy;

  UILayout layout = e->layout;
  if(e->owner){
    layout = e->owner->layout;
      paddingx = e->owner->spacing[UI_PADDING] + e->owner->spacing[UI_PADDING_LEFT];
      paddingr= e->owner->spacing[UI_PADDING] + e->owner->spacing[UI_PADDING_RIGHT];
    //paddingy = (e->owner->spacing[UI_PADDING] + e->owner->spacing[UI_PADDING_BOT]);
    paddingy += e->owner->spacing[UI_PADDING] + e->owner->spacing[UI_PADDING_TOP];
    paddingb += e->owner->spacing[UI_PADDING] + e->owner->spacing[UI_PADDING_BOT];

  }

  Rectangle prior = RectPos(Vector2XY(xinc,yinc),RECT_ZERO);
  if(e->index > 0){
    prior = e->owner->children[e->index-1]->bounds;
    //prior.height += e->owner->children[e->index-1]->spacing[UI_MARGIN_BOT];
  }
  switch(layout){
    case LAYOUT_VERTICAL:
      xinc += omarginx;
      yinc = omarginy+prior.y + prior.height + paddingy;
      if(e->index > 0)
        yinc += e->owner->spacing[UI_PADDING_BOT];
      break;
    case LAYOUT_HORIZONTAL:
      if(e->index > 0 )
        xinc = paddingx + prior.x + prior.width;
      else if(e->type == UI_TOOL_TIP)
        xinc += e->owner->width + omarginx;
      else
        xinc += omarginx;
      break;
    case LAYOUT_GRID:
      if(!e->owner)
        break;
      //yinc += omarginy;
      int col = e->index%UI_GRID_WIDTH;
      if(col>0){    
        xinc = omarginx+prior.x + prior.width;
        yinc = prior.y;
        xinc += paddingr;    
      }  
      else{
        //yinc = omarginy+prior.y;//height;
        yinc = prior.y + prior.height+omarginy;
        //xinc = prior.x + prior.width+omarginx;
      }
      break;
    default:
      xinc += omarginx + paddingx;
      yinc += omarginy + paddingy;
      if(e->align & ALIGN_RIGHT)
        xinc *= -1;//omarginx;

      break;
  }

  e->bounds = RectInc(e->bounds,xinc,yinc);
 
  for(int i = 0; i < e->num_children; i++)
    ElementResize(e->children[i]);
}

void ElementRender(ui_element_t* e){
  int state = 0;
  int clicked = 0,toggle = 0,focused = 0;

  switch(e->type){
    case UI_BUTTON:
      state = GuiButton(e->bounds,e->text);
      break;
    case UI_PANEL:
  
      GuiPanel(e->bounds, e->text);
      break;
    case UI_LABEL:
      state = GuiLabel(e->bounds,e->text);
      break;
    case UI_ANIMATEXT:
        GuiAnimatext(e->bounds, e->text, e->effect); 
        e->effect->curren_t += 0.016f;
      break;
    case UI_TEXT:
      GuiText(e->bounds,e->text);
      break;
    case UI_HEADER:
       Rectangle header = e->bounds;
       if(e->owner && e->owner->bounds.width > header.width)
         header.width = e->owner->bounds.width;

       GuiHeader(header, e->text);
       break;
    case UI_TITLE:
       GuiTitle(e->bounds, e->text);
       break;
    case UI_TOOL_TIP:
      //active_tooltip = e;
      state = GuiTooltipControl(e->bounds, e->text);
      break;
    case UI_BOX:
      GuiPanel(e->bounds, e->text);
      GuiLabel(e->bounds, e->text);
      break;
    case UI_PROGRESSBAR:
      char* left = TextFormatLineItem(e->value->p->left);
      char* right = TextFormatLineItem(e->value->p->right);
      GuiProgressBar(e->bounds, left, right,
          e->value->p->val, e->value->p->min, e->value->p->max);
      break;
    case UI_ICON:
      DrawRectangleLinesEx(e->bounds, 1.5f,LIGHTGRAY);
      if(e->value && e->value->s != NULL){
        state = GuiLabel(e->bounds, NULL);
        DrawSpriteAtPos(e->value->s, e->value->s->pos);
      }
      break;
    case UI_STATUSBAR:
      GuiStatusBar(e->bounds, e->text);
      break;
    case UI_CHAR_SPR:
      if(!e->value)
        return;
      DrawSprite(e->value->s);
      break;
    case UI_GROUP:
      ui_element_t* ac = e->children[e->active];

      if(ac){
      GuiPanel(e->bounds, ac->text);
      
      }
      break;
    default:
      break;
  }


  switch(state){
    case STATE_FOCUSED:      
      ElementSetState(e,ELEMENT_FOCUSED);
      break;
    case STATE_PRESSED:
        ElementSetState(e,ELEMENT_ACTIVATE);
      break;
    default:
      if(e->type != UI_TOOL_TIP)
      ElementSetState(e,ELEMENT_TOGGLE);
      break;
  }
 
}

void UIRender(void){
  for (int i = 0; i < ui.layer_base; i++){
 
    ElementRender(ui.elements[0][i]);
  }

  for (int i = 0; i < ui.layer_top; i++){
 
    ElementRender(ui.elements[1][i]);
  }

  if(active_tooltip)
    ElementRender(active_tooltip);

}

void UISync(FetchRate poll){
  ui.layer_base = 0;
  ui.layer_top = 0;
  ui.current_frame = WorldGetTime();
  ui.frame_lock = ui.last_press_frame + 15;
  for(int i = 0; i < MENU_DONE; i++){

    ui_menu_t *m = &ui.menus[i];
    if(m->state < MENU_READY)
      continue;

    for (int k = 0; k < MENU_END; k++){
      if(IsKeyPressed(m->key[k]))
        MenuSetState(m, k);

    }
    UISyncMenu(&ui.menus[i], poll);
  }
  //UIRender();
}

void UISyncMenu(ui_menu_t* m, FetchRate poll){
  if(m->state < MENU_READY)
    return;

  if(!m->element->menu )
    m->element->menu = m; 

  UISyncElement(m->element, poll);

  if(IsKeyPressed(KEY_ESCAPE)){
    if(m->cb[MENU_CLOSE](m))
      MenuSetState(m,MENU_CLOSE);
  }
}

void UISyncElement(ui_element_t* e, FetchRate poll){
  if(!e->menu && e->owner)
    e->menu = e->owner->menu;

  if(e->state < ELEMENT_IDLE)
    return;

   if(e->sync_val){
    e->sync_val(e,poll);
    if(e->value){
      switch(e->value->type){
        case VAL_CHAR:
          if(!e->value->c || e->value->c[0] == '\0')
            break;
          ElementSetText(e, e->value->c);
          break;
        case VAL_INT:
          //ElementSetText(e,TextFormat("%i",*e->value->i));
          break;
        case VAL_LN:
          if(e->text)
          PrintElementValue(e->value, e->spacing, e->text);
          break;
        case VAL_ICO:
        default:
          break;
      }
    }
  }

  if((e->align & ALIGN_OVER) == ALIGN_OVER)
   ui.elements[1][ui.layer_top++] = e;
  else 
   ui.elements[0][ui.layer_base++] = e; 

 
  for(int i = 0; i<e->num_children; i++)
    UISyncElement(e->children[i], poll);
}

bool UIFreeElement(ui_element_t* e){
  if(!e)
    return false;

  free(e);

  return true;
}

bool UIHideElement(ui_element_t* e){
  if(ElementSetState(e,ELEMENT_HIDDEN)){
    if(e->type == UI_TOOL_TIP){
        GuiDisableTooltip();
        active_tooltip = NULL;
    }
    return true;
  }

  return false;
}

bool UICloseOwner(ui_element_t* e){
  if(!e->menu)
    return false;

  return UICloseMenu(e->menu);
}

bool UICloseMenu(ui_menu_t* m){
  return MenuSetState(m,MENU_CLOSE);
}

bool UIClearElements(ui_menu_t* m){
  ui_element_t* eo = m->element;
  int num_children = eo->num_children;
  for (int i = 0; i < num_children; i++){
    if(UIFreeElement(eo->children[i]))
      eo->num_children--;

  }
}

bool UISelectOption(ui_element_t* e){
}

bool UITransitionScreen(ui_element_t* e){
  GameTransitionScreen();
  return true; 
}

void ElementSetText(ui_element_t* e, char* str){

  Vector2 size = MeasureTextEx(ui.font, str, ui.text_size, ui.text_spacing);

  if(size.x > e->bounds.width){
    e->bounds.width = size.x;
    if(e->owner)
      ElementResize(e->owner);
    else
      ElementResize(e);
  }

  strcpy(e->text, str);
}

bool ElementSetTooltip(ui_element_t* e){
  GuiEnableTooltip();
  if(active_tooltip)
    ElementSetState(active_tooltip, ELEMENT_HIDDEN);
  active_tooltip = e;
}

bool ElementTabToggle(ui_element_t* e){
  ui_element_t* p = ParamRead(&e->ctx, ui_element_t);

  //return ElementSetState(p, ELEMENT_ACTIVATE);

  switch(p->state){
    case ELEMENT_HIDDEN:
    case ELEMENT_IDLE:
    case ELEMENT_TOGGLE:
      return ElementSetState(p, ELEMENT_ACTIVATE);
      break;
    default:
      return ElementSetState(p, ELEMENT_HIDDEN);
      break;
  }

}

bool ElementToggleTip(ui_element_t* e){
  return true;
}

bool ElementToggle(ui_element_t* e){
  switch(e->prior){
    case ELEMENT_SHOW:
    case ELEMENT_ACTIVATE:
      return ElementSetState(e, ELEMENT_HIDDEN);
      break;
    case ELEMENT_ACTIVATED:
      return ElementSetState(e, ELEMENT_IDLE);
      break;
    default:
      return ElementSetState(e, ELEMENT_ACTIVATE);
      break;
  }
}

bool ElementHideSiblings(ui_element_t* e){
  for(int i = 0; i < e->owner->num_children; i++){
    ui_element_t* c = e->owner->children[i];
    if(c->hash == e->hash && e->index == c->index)
      continue;
    ElementSetState(c, ELEMENT_HIDDEN);
    /*
    if(ElementSetState(c, ELEMENT_HIDDEN))
      e->owner->active = e->index;
      */
  }
}

bool ElementToggleChildren(ui_element_t* e){
  for(int i = 0; i < e->num_children; i++)
    ElementSetState(e->children[i], ELEMENT_TOGGLE);

  return true;
}

bool ElementShowIcon(ui_element_t* e){
  if(!e->value || e->value->s == NULL)
    return false;
        
  e->value->s->pos.x = e->bounds.x + e->spacing[UI_PADDING_LEFT];
  e->value->s->pos.y = e->bounds.y + e->spacing[UI_PADDING_TOP];

  if(e->owner && e->index >= e->owner->num_children - 1)
    ElementResize(e->owner);

  return e->value->s->is_visible = true;
}

bool ElementShowTooltip(ui_element_t* e){
  for(int i = 0; i < e->num_children; i++){
    ui_element_t* c = e->children[i];
    if(c->type != UI_TOOL_TIP)
      continue;

    if(ElementSetState(c, ELEMENT_SHOW))
      ElementSetState(c, ELEMENT_FOCUSED);
  }
}

bool ElementSyncOwnerContext(ui_element_t* e){
  e->ctx = e->owner->ctx;
  e->set_val(e, e->ctx);
}

bool ElementLoadChildren(ui_element_t* e){
  for (int i = 0; i < e->num_children; i++)
    ElementSetState(e->children[i], ELEMENT_LOAD);

  ElementResize(e);
}

bool ElementActivateChildren(ui_element_t* e){
  for (int i = 0; i < e->num_children; i++){
    ElementSetState(e->children[i], ELEMENT_LOAD);
    ElementSetState(e->children[i], ELEMENT_IDLE);
  }
  return ElementSetState(e, ELEMENT_SHOW);
}

bool ElementShowPrimary(ui_element_t* e){
  for (int i = 0; i < e->num_children; i++){
    if(i == 0)
      return ElementSetState(e->children[i], ELEMENT_SHOW);
    else
      return ElementSetState(e->children[i], ELEMENT_HIDDEN);
  }
}

bool ElementShowChildren(ui_element_t* e){
  for (int i = 0; i < e->num_children; i++)
    ElementSetState(e->children[i], ELEMENT_SHOW);
}

bool MenuTransitionScreen(ui_menu_t* m){
  GameTransitionScreen();
}

bool MenuActivateChildren(ui_menu_t* m){
  if(ElementSetState(m->element, ELEMENT_IDLE))
  ElementSetState(m->element, ELEMENT_SHOW);
}

void MenuOnStateChanged(ui_menu_t*m, MenuState old, MenuState s){
  if(m->cb[m->state])
    m->cb[m->state](m);

  switch(old){
    case MENU_INACTIVE:
      //ElementResize(m->element);
      break;
    default:
      break;
  }

  switch(s){
    case MENU_CLOSE:
      MenuSetState(m,MENU_CLOSED);
      break;
    case MENU_LOAD:
      MenuSetState(m, MENU_READY);
      break;
    case MENU_READY:
    case MENU_OPENED:
      GuiSetState(STATE_NORMAL);
      break;
    default:
      break;
  }
}

bool MenuCanChangeState(MenuState old, MenuState s){
  if(old == s)
    return false;

  switch(COMBO_KEY(old,s)){
    case COMBO_KEY(MENU_INACTIVE,MENU_OPENED):
      return false;
      break;
    default:
      break;
  }
  return true;
}

bool MenuSetState(ui_menu_t* m, MenuState s){
  if(!MenuCanChangeState(m->state, s))
    return false;

  MenuState old = m->state;
  m->state = s;

  MenuOnStateChanged(m,old,s);

  return true;
}

bool ElementCanChangeState(ElementState old, ElementState s){
  if(old == s)
    return false;

  state_change_requirement_t *req = &ELEM_STATE_REQ[s];
  return req->can(old, req->required);
}

void ElementStepState(ui_element_t* e, ElementState s){
  switch(s){
    case ELEMENT_ACTIVATE:
      ui.last_press_frame = WorldGetTime();
      ElementSetState(e,ELEMENT_ACTIVATED);
      break;
    case ELEMENT_NONE:
      ElementSetState(e,ELEMENT_IDLE);
    default:
     break;
  } 
}

bool ElementSetState(ui_element_t* e, ElementState s){
  if(!ElementCanChangeState(e->state, s))
    return false;
 
  e->prior = e->state; 
  e->state = s;
  if(e->cb[s] && e->cb[s](e))
    ElementStepState(e,s);

  return true;
}

void ElementSyncVal(ui_element_t* e, FetchRate poll){
  element_value_t* ev = e->value;

  if(e->ctx.type == DATA_NONE || !ev || poll != ev->rate)
    return;

  switch(ev->type){
    case VAL_LN:
      for( int i = 0; i < ev->num_ln; i++)
        PrintSyncLine(ev->l[i], poll);
      break;
    case VAL_ICO:
      ev->get_val(ev, ev->context);
      if(ev->s){
        ev->s->pos.x = e->bounds.x + e->spacing[UI_PADDING_LEFT];
        ev->s->pos.y = e->bounds.y + e->spacing[UI_PADDING_TOP];
      }
      break;
    default:

      if(ev->get_val){
        ev->get_val(ev, ev->context);
        return;
      }
      e->value = e->set_val(e, e->ctx);
      break;
  }
}

bool ElementSyncContext(ui_element_t* e){
  if(e->get_ctx){
    e->ctx = e->get_ctx(e);

    for (int i = 0; i < e->num_children; i++)
      ElementSetState(e->children[i], ELEMENT_HIDDEN);

    return ElementSetState(e, ELEMENT_SHOW);
  }

  return false;
}

bool ElementShow(ui_element_t* e){
 return ElementSetState(e, ELEMENT_SHOW);
}

bool ElementShowContext(ui_element_t* e){
  if(e->value){
    //e->value = e->set_val(e, e->ctx);
    e->sync_val = ElementSyncVal;

    ElementSetState(e, ELEMENT_SHOW);
  }

  return (e->ctx.type == DATA_NONE);

}

ui_effect_t* InitUIEffect(EffectFn fn, Color* col){
  ui_effect_t* e = GameCalloc("InitUIEffect", 1, sizeof(ui_effect_t));

  e->target = col;
  e->fn = fn;
  e->b_egin = 0.0f;
  e->d_ur = 1.0f;
  e->curren_t = 0.0f;
  e->c_hange = 10.0f;

  return e;
}

bool ElementSetFlash(ui_element_t* e){
  e->text_color = BLUE;
  e->effect = InitUIEffect(EaseSineIn, &e->text_color);
  return true;
}

bool ElementSetContext(ui_element_t* e){
  if(e->get_ctx){
    e->ctx = e->get_ctx(e);

    if(e->set_val){
      e->value = e->set_val(e, e->ctx);
      if(!e->sync_val)
      e->sync_val = ElementSyncVal;
    }

  }
  return (e->ctx.type != DATA_NONE);

}
void ElementValueSyncSize(ui_element_t *e, element_value_t* ev){
  bool resize = false;
  ev->text_hei = 0;
  int tsize = ui.text_size;
  if(e->type == UI_TITLE)
    tsize = ui.title_size;
  switch(ev->type){
    case VAL_LN:
      for(int i = 0; i < ev->num_ln; i++){
        ev->text_hei += ev->l[i]->r_hei;
        ev->text_hei += GuiGetStyle(DEFAULT, TEXT_LINE_SPACING)/2;
        if(ev->l[i]->r_len > ev->text_len){
          ev->text_len = ev->l[i]->r_len;
          if(e->bounds.width >= ev->l[i]->r_wid)
            continue;

          e->width = ev->l[i]->r_wid;
          e->bounds.width = ev->l[i]->r_wid;
          resize = true;
        }
      }
      break;
    case VAL_CHAR:
       Vector2 size = MeasureTextEx(ui.font, ev->c, tsize, ui.text_spacing); 
       size.x += GuiGetStyle(LABEL,TEXT_PADDING ) *3;
       ev->text_len = size.x;
       ev->text_hei = size.y;
       if(e->bounds.width < ev->text_len){
         e->bounds.width = ev->text_len;
         e->width = ev->text_len;
         resize = true;
       }
       break;
  }
  if(ev->text_hei > e->bounds.height){
    e->bounds.height = ev->text_hei;
    e->height = ev->text_hei;
    resize = true;
  }

  if(resize && e->owner)
    ElementResize(e->owner);

}

bool ElementSetActiveTab(ui_element_t* e){
  for(int i = 0; i < e->num_children; i++){
    if(!ElementSetState(e->children[i], ELEMENT_LOAD))
      continue;

    if(!ElementSetState(e->children[i], ELEMENT_IDLE))
      continue;

    if(e->children[i]->type != UI_TAB_PANEL)
      continue;

    if(!ElementSetState(e->children[i], ELEMENT_SHOW))
      continue;

    ElementShowChildren(e->children[i]);
  }

  return true;
}

element_value_t* GetElementName(ui_element_t* e, param_t context){
  ui_element_t* other = ParamRead(&context, ui_element_t);

  if(!other || !other->name)
    return NULL;
/*
  if(e->delimiter != '\0')
    ElementSetText(e, StringSplit(other->name, e->delimiter));
  else
  */
  ElementSetText(e, other->name);

  return NULL;
  
}

element_value_t* GetContextParams(ui_element_t* e, param_t context){

}

bool ElementRestartLevel(ui_element_t* self){
  WorldLevelReset(true);

  return true;
}

element_value_t* GetOwnerValue(ui_element_t* e, param_t context){
  if(!e->owner || e->owner->value == NULL)
    return NULL;

  return e->owner->value;

}

element_value_t* GetOwnerTextAt(ui_element_t* e, param_t context){
  if(!e->owner)
    return NULL;

  if(e->owner->text[0] == '\0')
    return NULL;

  element_value_t *ev = GameCalloc("GetOwnerText", 1,sizeof(element_value_t));

  ev->type = VAL_CHAR;
  ev->rate = FETCH_ONCE;

  ev->c = GameMalloc("GetOwnerText", sizeof(char)*MAX_NAME_LEN);

  strcpy(ev->c, SubString(e->owner->text, e->owner->delimiter, e->index));
  ElementValueSyncSize(e, ev);

  return ev;
}

element_value_t* GetOwnerText(ui_element_t* e, param_t context){
  if(!e->owner)
    return NULL;

  if(e->owner->text[0] == '\0')
    return NULL;

  element_value_t *ev = GameCalloc("GetOwnerText", 1,sizeof(element_value_t));

  ev->type = VAL_CHAR;
  ev->rate = FETCH_ONCE;
  ev->c = GameMalloc("GetOwnerText", sizeof(char)*MAX_NAME_LEN);

  strcpy(ev->c, e->owner->text);
  ElementValueSyncSize(e, ev);

  return ev;

}

bool ElementAssignValues(ui_element_t* e){
  if(!e->value)
    return false;

  for (int i = 0; i < e->value->l[0]->num_val; i++){
    element_value_t *val = e->value->l[0]->values[i];
    switch(val->type){
      case VAL_CHAR:
        e->value->c = GameMalloc("ElementAssignValues",sizeof(char)*MAX_NAME_LEN);
        e->value->c = val->c;
        break;
      case VAL_INT:
        e->value->i = GameMalloc("ElementAssignValues",sizeof(int));
        e->value->i = val->i;
        break;
      case VAL_FLOAT:
        e->value->f = GameMalloc("ElementAssignValues",sizeof(float));
        e->value->f = val->f;
        break;
    }
  }
  return true;
}

void ElementContextEmpty(ui_element_t* e, FetchRate poll){
  e->value = NULL;
  
  ElementSetState(e, ELEMENT_IDLE);
}

int ElementSetFirstAvailableChild(ui_element_t* e, param_t p){
  for (int i = 0; i < e->num_children; i++){
    ui_element_t* c = e->children[i];
    if (c->ctx.type != DATA_NONE)
      continue;

    if(c->set_val)
    c->value = c->set_val(c, p);
      
    ElementSetState(c, ELEMENT_SHOW);

    return i;
    
  }

  return -1;
}

bool ElementSetContextParams(ui_element_t* e){
  if(!ElementSetContext(e))
    return false;

  if(e->num_children == 0)
    return false;

  for (int i = 0; i < 5; i++)
    e->children[0]->params[i] = e->params[i];

  return true;
}

param_t ElementOwnerChildren(void* p){
  ui_element_t* e = p;
  param_t others[e->owner->num_children - 1];

  int count = 0;
  for(int i = 0; i < e->owner->num_children; i++){
    ui_element_t* c = e->owner->children[i];
    if(c->hash == e->hash)
      continue;

    others[count++] = ParamMakeObj(DATA_ELEM, c->gouid, c);
  }

  //e->ctx = others;
  for (int i = 0; i < e->num_children; i++)
    e->children[i]->ctx = others[i];
}

param_t ElementGetOwnerContextParams(void* p){
  ui_element_t* c = p; 

  if(!c->owner)
    EMPTY_PARAM;

  c->num_params = c->owner->num_params;
  memcpy(c->params, c->owner->params, sizeof(c->owner->params));

  return c->owner->ctx;
}

param_t ElementGetOwnerContext(void* p){
  ui_element_t* c = p; 

  if(!c->owner)
    return EMPTY_PARAM;

  return c->owner->ctx;
}

int GuiTooltipControl(Rectangle bounds, const char* text){
    int result = 0;
    GuiState state = guiState;

    Color col = GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED)? (int)BASE_COLOR_DISABLED : (int)BACKGROUND_COLOR));
    col.a = 255;
    col = ColorLerp(col, BLACK, 0.33);
    GuiDrawRectangle(bounds, RAYGUI_PANEL_BORDER_WIDTH, GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED)? (int)BORDER_COLOR_DISABLED : (int)LINE_COLOR)), col);
    //--------------------------------------------------------------------


    // Text will be drawn as a header bar (if provided)
    Rectangle statusBar = { bounds.x, bounds.y, bounds.width, (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
    if ((text != NULL) && (bounds.height < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT*2.0f)) bounds.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT*2.0f;

    if (text != NULL)
    {
        // Move panel bounds after the header bar
        bounds.y += (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
        bounds.height -= (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
    }

    // Draw control
    if (text != NULL) GuiStatusBar(statusBar, text);  // Draw panel header as status bar
    return result;

}
void GuiAnimatext(Rectangle bounds, const char *text, ui_effect_t* e){

  e->target->a = e->fn(e->curren_t, e->b_egin, e->c_hange, e->d_ur);
  GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), *e->target);
}

int GuiText(Rectangle bounds, const char *text){

  GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT),ui.title_col );

  return 0;
}


int GuiTitle(Rectangle bounds, const char *text){

  int orig = GuiGetStyle(DEFAULT, TEXT_SIZE);

  GuiSetStyle(DEFAULT, TEXT_SIZE, ui.title_size);
  GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT),ui.title_col );

  GuiSetStyle(DEFAULT, TEXT_SIZE, orig);
  
  return 0;
}

int GuiLabel(Rectangle bounds, const char *text)
{
    GuiState state = guiState;

    Vector2 mousePoint = GetMousePosition();

    // Check button state
    if (CheckCollisionPointRec(mousePoint, bounds)){
      state = STATE_FOCUSED;
    // Handle mouse button down
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        state = STATE_PRESSED;
      }
    }

    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), GetColor(GuiGetStyle(LABEL, TEXT + (state*3))));
    //--------------------------------------------------------------------
    return state;
}

int GuiPanel(Rectangle bounds, const char *text)
{   
    #if !defined(RAYGUI_PANEL_BORDER_WIDTH)
        #define RAYGUI_PANEL_BORDER_WIDTH   1
    #endif

    GuiState state = guiState;

    Vector2 mousePoint = GetMousePosition();

    // Text will be drawn as a header bar (if provided)
    if (CheckCollisionPointRec(mousePoint, bounds)){
      state = STATE_FOCUSED;
      // Handle mouse button down
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        state = STATE_PRESSED;
      } 
    }  
    GuiDrawRectangle(bounds, RAYGUI_PANEL_BORDER_WIDTH, GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED)? (int)BORDER_COLOR_DISABLED : (int)LINE_COLOR)),
                     GetColor(GuiGetStyle(DEFAULT, (state == STATE_DISABLED)? (int)BASE_COLOR_DISABLED : (int)BACKGROUND_COLOR)));
    //--------------------------------------------------------------------
    return state;
}

int GuiHeader(Rectangle bounds, const char *text){
#if !defined(RAYGUI_PANEL_BORDER_WIDTH)
#define RAYGUI_PANEL_BORDER_WIDTH   1
#endif

  int result = 0;
  GuiState state = guiState;

  // Text will be drawn as a header bar (if provided)
  Rectangle statusBar = { bounds.x, bounds.y, bounds.width, (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
  if ((text != NULL) && (bounds.height < RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT*2.0f)) bounds.height = RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT*2.0f;

  if (text != NULL)
  {
    // Move panel bounds after the header bar
    bounds.y += (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
    bounds.height -= (float)RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - 1;
  }

  // Draw control
  //--------------------------------------------------------------------
  if (text != NULL) GuiStatusBar(statusBar, text);  // Draw panel header as status bar 

  return result;
}


param_t ElementIndexContext(void* p){
   ui_element_t* e = p;

   return ParamMake(DATA_INT, sizeof(int), &e->index);

}

param_t ElementLevelContext(void*){
  level_t* l = WorldGetLevel();
  if(!l)
    return EMPTY_PARAM;

  return ParamMakeObj(DATA_LEVEL, l->id, l);
}

param_t ElementPresetContext(void* p){
  ui_element_t* e = p; 

  return e->ctx;
}

param_t ElementOwnerTextAt(void* p){
  ui_element_t* c = p;
  ui_element_t* o = c->owner;
  if (!o || o->text[0] =='\0')
    return EMPTY_PARAM;

  if( strlen( o->text ) < c->index)
    return EMPTY_PARAM;

  ElementSetText(c ,&o->text[c->index]);
  c->text[1] = '\0';
}

param_t ElementNiblings(void *p){
  ui_element_t* e = p; 
  ui_element_t* sib = NULL;
  char cname[MAX_NAME_LEN];
  switch(e->type){
    case UI_TAB_PANEL:
      strcpy(cname, "TAB_BUTTON");
      break;
  }
  for(int i = 0; i < e->owner->num_children; i++){
    if(e->owner->children[i]->hash == e->hash)
      continue;

    sib = e->owner->children[i];
    break; 
  }
  if(!sib)
    return EMPTY_PARAM;

  e->ctx = ParamMakeArray(DATA_ELEM, e->gouid, sib->children, sib->num_children, sizeof(ui_element_t*));

  int cur_child = e->num_children;
  for(int i = cur_child; i < sib->num_children; i++){
    ui_element_t* child = InitElementByName(cname, e->menu, e);
    if(!child)
     continue;
    ElementAddChild(e, child);

    child->ctx = ParamMakeObj(DATA_ELEM, sib->children[i]->gouid, sib->children[i]);
  }
  ElementSetState(e, ELEMENT_IDLE);
}
