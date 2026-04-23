#include "raylib.h"
#include "game_process.h"
#include "game_ui.h"
#include "screens.h"


Camera2D camera = { 0 };
//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void){
  MenuSetState(&ui.menus[MENU_HUD],MENU_READY);
  //camera.target = player.position;
  InitCamera(2.0f,0.0f,Vector2Scale(ROOM_SIZE,0.5f),
      Vector2Scale(ROOM_SIZE, 0.25f));

  InitAssetManager(MAX_LAYER_SPRITES);
  InitGameEvents();
}

void PreUpdate(void){
  GameProcessStep();
  WorldPreUpdate();
}

void FixedUpdate(void){
  AudioStep();
  WorldFixedUpdate();
}

void PostUpdate(void){
  WorldPostUpdate();
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
  UISync(FETCH_UPDATE);
  if(player)
    ScreenCameraSync( WorldPlayer()->pos );
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
  BeginDrawing();
  ClearBackground(BLACK);

  ScreenCameraToggle();

  WorldRender();

  ScreenCameraToggle();
  //MapGenRender();
  UIRender();

  EndDrawing();


}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
  MenuSetState(&ui.menus[MENU_HUD],MENU_INACTIVE);
  //MenuSetState(&ui.menus[MENU_PLAY_AREA],MENU_INACTIVE);

  GameProcessEnd();
  // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
  return 0;
//  return finishScreen;
}
