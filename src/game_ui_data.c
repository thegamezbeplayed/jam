#include "game_ui.h"
#include "game_process.h"

ui_menu_d MENU_DATA[MENU_DONE] = {
  [MENU_MAIN] = {MENU_MAIN, "TITLE_MENU_DOM", MENU_INACTIVE, false,
    {
      [MENU_READY]    = MenuActivateChildren,
      [MENU_ACTIVE] = MenuTransitionScreen
    },
    { [MENU_ACTIVE] = KEY_ENTER }
  },
  [MENU_HUD] = {MENU_HUD, "HUD_MENU_DOM", MENU_INACTIVE, false,
    {[MENU_READY] = MenuActivateChildren,
    }
  },
  [MENU_RECAP] = {MENU_RECAP, "EXIT_MENU_DOM", MENU_INACTIVE, false,
    {[MENU_READY] = MenuActivateChildren,
      [MENU_ACTIVE] = MenuTransitionScreen
    },
    { [MENU_ACTIVE] = KEY_ENTER }
  },

};

ui_element_d ELEM_DATA[ELE_COUNT] = {
  {"EXIT_MENU_DOM", VECTOR2_ZERO, FIXED_MENU_FULL, UI_CONTAINER,
    ELEMENT_NONE, LAYOUT_STACK, ALIGN_MID | ALIGN_CENTER,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    {
      [UI_MARGIN_TOP] = 8, [UI_MARGIN_LEFT] = 6,
      [UI_PADDING_TOP] = 24, [UI_PADDING_LEFT] = 8 
    },
    1, {
    //  "EXIT_BACKGROUND"
      "EXIT_PANEL",
    },
    .text = "Signals Crossed"
  },

  {"TITLE_MENU_DOM", VECTOR2_ZERO, FIXED_MENU_FULL, UI_CONTAINER,
    ELEMENT_NONE, LAYOUT_STACK, ALIGN_MID | ALIGN_CENTER,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    {
      [UI_MARGIN_TOP] = 8, [UI_MARGIN_LEFT] = 6,
      [UI_PADDING_TOP] = 24, [UI_PADDING_LEFT] = 8 
    },
    1, {
    //  "TITLE_BACKGROUND"
      "TITLE_PANEL",
    },
    .text = "signals crossed"
  },
  {"EXIT_PANEL", VECTOR2_ZERO, VECTOR2_ZERO, UI_GROUP, ELEMENT_NONE,
    LAYOUT_VERTICAL, ALIGN_MID | ALIGN_CENTER,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    {
      [UI_MARGIN_TOP] = -36, [UI_PADDING_BOT] = 36, [UI_PADDING_TOP] = -24
    },
    .num_children = 5,{
      "TITLE_HEADER",
      "TEXT_HEADER",
      "TEXT_HEADER",
      "TEXT_HEADER",
      "TEXT_HEADER",
      "FLASHY_TEXT"
    },
    .text = "SIGNALS CROSSED,Thank you for playing,  Sorry it was so short getting sick and a massive emscripten bug really killed progress,If you want to follow my progress as I hope to release this as a full game head over to Thegamezbeplayed.com!,Press enter to keep Signalling!",
    .delimiter = ','
  },
  {"TITLE_PANEL", VECTOR2_ZERO, VECTOR2_ZERO, UI_GROUP, ELEMENT_NONE,
    LAYOUT_VERTICAL, ALIGN_MID | ALIGN_CENTER,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    {
      [UI_MARGIN_TOP] = -36, [UI_PADDING_BOT] = 36, [UI_PADDING_TOP] = -24
    },
    5,{
      "TITLE_HEADER",
      "TEXT_HEADER",
      "TEXT_HEADER",
      "TEXT_HEADER",
      "FLASHY_TEXT"
    },
    .text = "SIGNALS CROSSED, A puzzle and memory game made in 72 hours, Made by August Karbowski, Music by Danego, Press Enter to Jam!",
    .delimiter = ','
  },
  {"TITLE_HEADER", VECTOR2_ZERO, VECTOR2_ZERO, UI_TITLE, ELEMENT_NONE,
    LAYOUT_HORIZONTAL, ALIGN_MID | ALIGN_CENTER,
    GetOwnerTextAt, ElementPresetContext,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,
    }
  },
  {"TEXT_HEADER", VECTOR2_ZERO, VECTOR2_ZERO, UI_TEXT, ELEMENT_NONE,
    LAYOUT_HORIZONTAL, ALIGN_MID | ALIGN_CENTER,
    GetOwnerTextAt, ElementPresetContext,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,
    }
  },
  {"FLASHY_TEXT", VECTOR2_ZERO, VECTOR2_ZERO, UI_TEXT, ELEMENT_NONE,
    LAYOUT_HORIZONTAL, ALIGN_MID | ALIGN_CENTER,
    GetOwnerTextAt, ElementPresetContext,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,
    },
    {
      [UI_MARGIN_BOT] = 16, [UI_MARGIN_TOP] = 32
    }
  },
  {"HUD_MENU_DOM", VECTOR2_ZERO, FIXED_SCREEN_SIZE, UI_CONTAINER,
    ELEMENT_NONE,  LAYOUT_STACK, ALIGN_CENTER | ALIGN_TOP,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    .num_children = 1, .kids ={"TOP_HUD_PANE"}
  },
  {"TOP_HUD_PANE", VECTOR2_ZERO, FIXED_SCREEN_SIZE, UI_PANEL,
    ELEMENT_NONE, LAYOUT_HORIZONTAL, ALIGN_MID | ALIGN_CENTER,
    .cb = {
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren,
    },
    {
      [UI_PADDING_LEFT] = 320
    },
    .num_children = 1, .kids = {
      "BUTTON_RESTART",
    },
    .params = {
      {PARAM_LEVEL},
      {PARAM_TURN},
      {PARAM_SCORE},
    },
    .text = "LEVEL:,TURN:, SCORE:",
    .delimiter = ','
  },
  {"PARAM_LABEL", VECTOR2_ZERO, FIXED_BUTTON_SIZE, UI_LABEL,
    ELEMENT_NONE, LAYOUT_HORIZONTAL, ALIGN_CENTER | ALIGN_MID,
    GetOwnerTextAt, ElementLevelContext,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,
      [ELEMENT_IDLE] = ElementActivateChildren,
      [ELEMENT_SHOW] = ElementShowChildren
    },
    {
    },
    .num_children = 2, .kids = {"NAME_TEXT","VALUE_TEXT"}
  },
  {"NAME_TEXT", VECTOR2_ZERO, VECTOR2_ZERO, UI_TEXT,
    ELEMENT_NONE, LAYOUT_HORIZONTAL, ALIGN_LEFT,
    GetOwnerText, ElementPresetContext,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,
    }
  },
  {"VALUE_TEXT", VECTOR2_ZERO, VECTOR2_ZERO, UI_TEXT,
    ELEMENT_NONE, LAYOUT_HORIZONTAL, ALIGN_LEFT,
    GetContextParams, ElementGetOwnerContextParams,
    .cb = {
      [ELEMENT_LOAD] = ElementSetContext,

    }
  },
  {"BUTTON_RESTART", VECTOR2_ZERO, FIXED_BUTTON_SIZE, UI_BUTTON,
    ELEMENT_NONE, LAYOUT_HORIZONTAL, ALIGN_MID | ALIGN_CENTER,
    NULL, NULL,
    .cb = {
      [ELEMENT_ACTIVATE] = ElementRestartLevel

    },
    .text = "RESTART"
  }
};

