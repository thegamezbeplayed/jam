#include "game_ui.h"
#include "game_common.h"
#include "game_types.h"
//#include "game_systems.h"
#include "game_strings.h"
#include "game_helpers.h"
#include "game_process.h"

#include <stdio.h>

line_item_t* InitLineItem(element_value_t *val, const char* format){
  line_item_t* ln = GameCalloc("InitLineItem", 1,sizeof(line_item_t));

  ln->values[ln->num_val++]=val;

  ln->text_format = strdup(format);

  return ln;
}

line_item_t* InitLineItems(element_value_t **val, int num_val, const char* format){
  line_item_t* ln = GameCalloc("InitLineItems", 1,sizeof(line_item_t));

  for (int i = 0; i < num_val; i++)
    ln->values[ln->num_val++]=val[i];

  ln->text_format = strdup(format);

  return ln;
}
void PrintSyncLine(line_item_t* ln, FetchRate poll){
    for(int i = 0; i < ln->num_val; i++){
      if(!ln->values[i]->get_val)
        continue;

      if(ln->values[i]->rate <= poll)
        ln->values[i]->get_val(ln->values[i],ln->values[i]->context);
    }
}

char *TextFormatLineItem(line_item_t *item) {
    char *buffer = GameMalloc("TextFormatLineItem", 2048 * sizeof(char));
    char temp[256];

    buffer[0] = 0;
    const char *fmt = item->text_format;

    item->r_len = 0;
    int arg_index = 0;

    for (int i = 0; fmt[i] != 0; i++) {
      // Check for '%' format token
      if (fmt[i] == '%' && fmt[i+1] != 0 && arg_index < item->num_val) {
        element_value_t *val = item->values[arg_index];
        size_t v_len = 0;
        size_t p_len = 0;
        size_t before = strlen(buffer);
        switch (fmt[i+1]) {
          case 's':
            if (val->type == VAL_CHAR)
              strcat(buffer, val->c);
            else
              strcat(buffer, "<BAD%S>");
            break;

          case 'i':
            if (val->type == VAL_INT) {
              sprintf(temp, "%i", *val->i);
              strcat(buffer, temp);
            } else {
              strcat(buffer, "<BAD%I>");
            }
            break;
          case 'f':
            if (val->type == VAL_FLOAT) {
              sprintf(temp, "%.2f", *val->f);
              strcat(buffer, temp);
            } else {
              strcat(buffer, "<BAD%F>");
            }
            break;
          case 'l':
            if(val->type == VAL_LN)
              strcat(buffer, TextFormatLineItem(val->l[0]));
            else
              strcat(buffer, "<BAD LINE>");
            break;
          default:
            // Copy unknown sequence literally
            strncat(buffer, &fmt[i], 2);
            break;
        }
        size_t after = strlen(buffer);
        item->r_len += after - before;
        arg_index++;
        i++; // Skip format character
      } else {
        // Regular character
        int len = strlen(buffer);
        buffer[len] = fmt[i];
        buffer[len+1] = 0;
      }
    }

    return buffer;
}



char* PrintElementValue(element_value_t* ev, int spacing[UI_POSITIONING], char* out){
  //char* out = GameCalloc("PrintElementValue", 1, 1024);

  out[0] = '\0';
  int p_top = imax(spacing[UI_PADDING], spacing[UI_PADDING_TOP]);
  int p_bot = 1 + imax(spacing[UI_PADDING], spacing[UI_PADDING_BOT]);

  int len = 0;
  for( int n = 0; n < ev->num_ln; n++){

    len += p_top + p_bot;
    int i = ev->reverse
      ? (ev->num_ln - 1 - n)
      : n;

    const char* ln = TextFormatLineItem(ev->l[i]);
    len+=ev->l[i]->r_len;
    strcat(out,ln);
  }

  return out;
  
}
