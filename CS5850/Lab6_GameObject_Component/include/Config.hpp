#ifndef CONFIG_H
#define CONFIG_H
// ==================== Libraries ==================
// Depending on the operating system we use
// The paths to SDL are actually different.
// The #define statement should be passed in
// when compiling using the -D argument.
// This gives an example of how a programmer
// may support multiple platforms with different
// dependencies.
//
// Note that your path may be different depending on where you intalled things

#if defined(LINUX) || defined(MINGW)
    #include <SDL2/SDL.h>
#else // This works for Mac
    #include <SDL.h>
#endif
// ===============application============//
extern bool quitApp;
// =========== component system =========//
const int TRANSFORM_ID {1};
const int CONTROLLER_ID {2};
const int SPRITE_ID {3};
// ============== constants =============
const int WINDOW_WIDTH {1280};
const int WINDOW_HEIGHT {720};
const int TILEMAP_COL {30};
const int TILEMAP_ROW {20};
const int TILE_WIDTH {64};
const int TILE_HEIGHT {64};
const int TILE_FILE_COL {8};
const int TILE_FILE_ROW {8};

const int TILE_ON_WINDOW_W = WINDOW_WIDTH / TILE_WIDTH;
const int TILE_ON_WINDOW_H = WINDOW_HEIGHT / TILE_HEIGHT;

const int LEVEL_WIDTH = TILEMAP_COL * TILE_WIDTH;
const int LEVEL_HEIGHT = TILEMAP_ROW * TILE_HEIGHT;

const int WINDOW_W_HALF = WINDOW_WIDTH / 2;
const int WINDOW_H_HALF = WINDOW_HEIGHT / 2;
// ============== files ===================//
const char* const SPRITE_FILE = "./images/sprite.bmp";
const char* const TILE_FILE = "./images/Tiles1.bmp";
const char* const LEVEL_FILE = "./levels/level1";
// data specifically for sprite.bmp
const int BMP_ROW    {3};
const int BMP_COLOMN {6};
const int BMP_IMG_W  {75};
const int BMP_IMG_H  {87};
const int BMP_BLK_X  {6};
const int BMP_BLK_Y  {3};
// =============== frames =================//
//const int frame_rate {30};
// I used microseconds instead of miliseconds for better precision
// using miliseconds (larger gap) would sometimes ignore time elapsed if
// the last update/render loop runs too fast
const int MCS_PER_SECOND {1000000};
const bool STABLE_FRAME {true};
// this prevent the sprites with less grid to move too fast
const double SPRITE_STABLIZER {1.1};

//typedef std::pair<int, int> position;
// =============== rules ==================//
const int PLAYER_SPEED {10};
#endif