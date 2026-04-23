#include "game_ui.h"

float ElementGetWidthSum(ui_element_t *e){
  if(e->state < ELEMENT_IDLE)
    return 0;

  float width = e->bounds.width;// + e->spacing[UI_MARGIN] + e->spacing[UI_MARGIN_LEFT];
/*
  if(e->owner)
  width += e->owner->spacing[UI_PADDING_RIGHT];
*/
  int padding = e->spacing[UI_PADDING_RIGHT];
  float cwidth = 0;
  if (e->layout == LAYOUT_GRID){
    if(e->num_children == 0)
      return width;

    int gwidth = (e->num_children > UI_GRID_WIDTH)? UI_GRID_WIDTH : e->num_children;
    cwidth = (e->children[0]->bounds.width + padding) * gwidth;
    return width+cwidth;
  }
    

  for(int i = 0; i < e->num_children; i++){
    if(e->children[i]->align & ALIGN_OVER)
      continue;

  int wid = ElementGetWidthSum(e->children[i]);
    switch(e->layout){
      case LAYOUT_HORIZONTAL:
        break;
      case LAYOUT_GRID:
        cwidth += ElementGetWidthSum(e->children[i]);
        break;
      default:
        width = (width<wid)?wid:width;
        break;
    }
  }

  return width+cwidth;
}

float ElementGetHeightSum(ui_element_t *e){
  if(e->state < ELEMENT_LOAD)
    return 0;

  float height = e->bounds.height;
  int padding = 0;
  if(e->owner)
    padding = e->owner->spacing[UI_PADDING_TOP];
  
  if(e->index > 0)
   padding += e->owner->spacing[UI_PADDING_BOT]; 
  float cheight = 0;
  if (e->layout == LAYOUT_GRID){
    if(e->num_children == 0)
      return height;

    height += (padding + e->children[0]->bounds.height) * e->num_children / UI_GRID_WIDTH - padding;
    return height;
  }

  for(int i = 0; i < e->num_children; i++){
    if(e->children[i]->align & ALIGN_OVER)
      continue;

    int hei = ElementGetHeightSum(e->children[i]);

    switch(e->layout){
      case LAYOUT_VERTICAL:
      case LAYOUT_GRID:
        height = e->spacing[UI_MARGIN_TOP] + e->spacing[UI_MARGIN_BOT];
        cheight += hei+padding;
        break;
      default:
        height = (height < hei)?hei:height;
        break;
    }
  }
  return height+cheight;
}
    
