#include "raylib.h"
#include "game_tools.h"
#include "game_process.h"
#if defined(PLATFORM_ANDROID)
#include <jni.h>
#endif
camera_t* cam;

void InitCamera(float zoom, float rot, Vector2 offset, Vector2 target){
  cam = GameCalloc("InitCamera", 1,sizeof(camera_t));
  Camera2D* raycam = GameCalloc("InitCamera", 1,sizeof(Camera2D));

  raycam->offset = offset;
  raycam->offset.y*=2;
  raycam->rotation = rot;
  raycam->zoom = zoom;

  raycam->target = target;

  float hei_disp = 128;
  float wid_disp = 176;
  cam->bounds = Rect(wid_disp, hei_disp, ROOM_WIDTH, ROOM_HEIGHT);
  cam->size = vec_to_cell(RectSize(cam->bounds), CELL_WIDTH);
  cam->view = Rect(0, 0, ROOM_WIDTH, -ROOM_HEIGHT);
  cam->render = LoadRenderTexture(cam->bounds.width, cam->bounds.height);
  cam->target = CELL_UNSET;
  cam->camera = raycam;
  cam->play_area = RECT(0, 0,15,10);
  cam->show_area = RECT(0,0,10,10);
  cam->offset = CELL_NEW(5,5);
}

void ScreenCameraToggle(void){
  cam->mode = !cam->mode;

  if(cam->mode){
    BeginTextureMode(cam->render);
    ClearBackground(BLACK);
    BeginMode2D(*cam->camera);

  }
  else{
    EndMode2D();
    EndTextureMode();
    DrawTexturePro(cam->render.texture, cam->view, cam->bounds, VECTOR2_ZERO, 0, WHITE);
  }
}

void ScreenCameraSync(Cell target){

  Rectangle new_show = {
    target.x - cam->offset.x,
    target.y + 4 * cam->offset.y,
    cam->show_area.width,
    cam->show_area.height
  };

  cam->show_area = clamp_rect_to_bounds(new_show, cam->play_area);

  cam->target = target;

  Vector2 vpos =  CellToVector2(rect_center(cam->show_area),CELL_WIDTH);
  cam->camera->target = Vector2Lerp(cam->camera->target,vpos,0.1);
  cam->pos.x = target.x - cam->size.x/2;
  cam->pos.y = target.y - cam->size.y/2;

}

void ScreenRender(Color c){
  DrawRectangleLinesEx(cam->bounds, 1.5f, c);
 
  Rectangle show = RectScale(cam->show_area, 60); 
  Rectangle play = RectScale(cam->play_area, 60); 
  DrawRectangleLinesEx(show, 1.5f, BLUE);
  DrawRectangleLinesEx(play, 1.5f, GREEN);
}


