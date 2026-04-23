//////////////////////////////////////////////////////////////////////////////////
//                                                                              //
// rTexpacker v5.0 Atlas Descriptor Code exporter v5.0                          //
//                                                                              //
// more info and bugs-report:  github.com/raylibtech/rtools                     //
// feedback and support:       ray[at]raylibtech.com                            //
//                                                                              //
// Copyright (c) 2019-2025 raylib technologies (@raylibtech)                    //
//                                                                              //
//////////////////////////////////////////////////////////////////////////////////

#define SHAPES_IMAGE_PATH      "asset_shapes.png"

// Atlas sprites array
static sub_texture_t SHAPE_SPRITES[SHAPE_COUNT] = {
    {  SHAPE_GREEN_SQUARE, 64, 64, 0, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_DIAMOND, 64, 64, 128, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_PENTAGON, 64, 64, 256, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_STUD, 64, 64, 384, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_TRIANGLE, 64, 64, 512, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_COMPASS, 64, 64, 640, 0, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_STAR, 64, 60, 641, 384, 129, 120, 0, false, 0, 0, 129, 120, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_GEM, 64, 52, 778, 512, 129, 104, 0, false, 0, 0, 129, 104, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_CIRCLE, 64, 64, 768, 0, 129, 128, 0, false, 0, 0, 129, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GREEN_HEART, 65, 59, 518, 512, 130, 118, 0, false, 0, 0, 130, 118, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_SQUARE, 64, 64, 0, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_DIAMOND, 64, 64, 128, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_PENTAGON, 64, 64, 256, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_STUD, 64, 64, 384, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_TRIANGLE, 64, 64, 512, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_COMPASS, 64, 64, 640, 128, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_STAR, 64, 60, 129, 512, 129, 120, 0, false, 0, 0, 129, 120, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_GEM, 64, 52, 258, 632, 129, 104, 0, false, 0, 0, 129, 104, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_CIRCLE, 64, 64, 768, 128, 129, 128, 0, false, 0, 0, 129, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_GRAY_HEART, 65, 59, 388, 512, 130, 118, 0, false, 0, 0, 130, 118, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_SQUARE, 64, 64, 0, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_DIAMOND, 64, 64, 128, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_PENTAGON, 64, 64, 256, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_STUD, 64, 64, 384, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_TRIANGLE, 64, 64, 512, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_COMPASS, 64, 64, 640, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_STAR, 64, 60, 770, 384, 129, 120, 0, false, 0, 0, 129, 120, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_GEM, 64, 52, 129, 632, 129, 104, 0, false, 0, 0, 129, 104, 0, 0, 0, 0, 0 },
    {  SHAPE_BLUE_HEART, 65, 59, 258, 512, 130, 118, 0, false, 0, 0, 130, 118, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_SQUARE, 64, 64, 768, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_DIAMOND, 64, 64, 896, 256, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_PENTAGON, 64, 64, 0, 384, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_STUD, 64, 64, 128, 384, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_TRIANGLE, 64, 64, 256, 384, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_COMPASS, 64, 64, 384, 384, 128, 128, 0, false, 0, 0, 128, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_STAR, 64, 60, 0, 512, 129, 120, 0, false, 0, 0, 129, 120, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_GEM, 64, 52, 0, 632, 129, 104, 0, false, 0, 0, 129, 104, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_CIRCLE, 64, 64, 512, 384, 129, 128, 0, false, 0, 0, 129, 128, 0, 0, 0, 0, 0 },
    {  SHAPE_RED_HEART, 65, 59, 648, 512, 130, 118, 0, false, 0, 0, 130, 118, 0, 0, 0, 0, 0 },
};
