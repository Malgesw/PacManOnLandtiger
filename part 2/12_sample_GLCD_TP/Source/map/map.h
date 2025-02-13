#define ROWS 31
#define COLUMNS 28

void initMap();
void drawRectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
void drawBigCoin(uint16_t cX, uint16_t cY);
void drawPacMan(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color);
void clearOldPacManPos();
void clearOldGhostPos();
void spawnBigCoin();
void drawLife();
void removeLife();
void clearPauseText();