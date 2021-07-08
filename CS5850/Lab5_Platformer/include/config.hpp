#ifndef CONFIG_H
#define CONFIG_H
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

const char* const SPRITE_FILE = "./images/sprite.bmp";
const char* const TILE_FILE = "./images/Tiles1.bmp";
const char* const LEVEL_FILE = "./levels/level1";
// data specifically for sprite.bmp
const int BMP_ROW    = {6};
const int BMP_COLOMN = {3};
const int BMP_IMG_W  = {75};
const int BMP_IMG_H  = {87};
const int BMP_BLK_X  = {6};
const int BMP_BLK_Y  = {3};

typedef std::pair<int, int> position;

#endif