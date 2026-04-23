#include "game_ui.h"
#include "screens.h"
#define RAYGUI_IMPLEMENTATION

// Title Screen Initialization logic
void InitTitleScreen(void)
{
  MenuSetState(&ui.menus[MENU_MAIN],MENU_READY);
}

// Title Screen Update logic
void UpdateTitleScreen(void)
{
  UISync(FETCH_UPDATE);
}

// Title Screen Draw logic
void DrawTitleScreen(void)
{
  BeginDrawing();
  ClearBackground(RAYWHITE);
  UIRender();
  EndDrawing();
}

// Title Screen Unload logic
void UnloadTitleScreen(void)
{
    // TODO: Unload TITLE screen variables here!
  MenuSetState(&ui.menus[MENU_MAIN],MENU_INACTIVE);
}
