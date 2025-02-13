#include <stdlib.h>
#include "GLCD/GLCD.h"
#include "map.h"
#include <stdio.h>
#include <stdlib.h>

// map is 240 x 320 (w x h), grid is 31 x 28

// 0 -> empty tile with "coin"
// 1 -> wall
// 2 -> empty tile
// 3 -> teleport tile

extern volatile unsigned short int timerCount;
extern int pacManPosX;
extern int pacManPosY;
extern unsigned short int colliding;
extern volatile unsigned short int togglePause;
extern uint16_t pausePosX;
extern uint16_t pausePosY;

int cell_width = (int)(MAX_X / COLUMNS);
int cell_height = (int)(MAX_Y / ROWS);
unsigned short int count = 0;
uint16_t lifeCount = 1;

unsigned short int mapGrid[ROWS][COLUMNS] = {
	
	{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
	{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
	{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
	{2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 2, 2, 2, 2, 2},
	{2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{2, 3, 2, 2, 2, 2, 2, 0, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1, 2, 2, 2, 0, 2, 2, 2, 2, 2, 3},
	{2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 1, 0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 1, 2, 2, 2, 2, 2},
	{2, 1, 1, 1, 1, 1, 1, 0, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 0, 1, 1, 1, 1, 1, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
	{2, 1, 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1},
	{2, 1, 1, 0, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
	{2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
		
};

void drawRectangle(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height, uint16_t color){
	
	LCD_DrawLine(x0, y0, x0+width, y0, color);
	LCD_DrawLine(x0, y0, x0, y0+height, color);
	LCD_DrawLine(x0+width, y0, x0+width, y0+height, color);
	LCD_DrawLine(x0, y0+height, x0+width, y0+height, color);
	
}

void drawCell(uint16_t x0, uint16_t y0, uint16_t color){
	int i, j;
	for(i = x0; i < x0 + cell_width; i++){
		for(j = y0; j < y0 + cell_height; j++){
			LCD_SetPoint(i, j, color);
		}
	}
}

void drawBigCoin(uint16_t cX, uint16_t cY){
	LCD_SetPoint(cX, cY, Yellow);
	LCD_SetPoint(cX, cY+1, Yellow);
	LCD_SetPoint(cX+1, cY, Yellow);
	LCD_SetPoint(cX+1, cY+1, Yellow);
	LCD_SetPoint(cX, cY-1, Yellow);
	LCD_SetPoint(cX-1, cY, Yellow);
	LCD_SetPoint(cX-1, cY-1, Yellow);
	LCD_SetPoint(cX+1, cY-1, Yellow);
	LCD_SetPoint(cX-1, cY+1, Yellow);
}

void drawPacMan(uint16_t x0, uint16_t y0, uint16_t width, uint16_t height){
	
	int i, j;
	uint16_t cX = x0 + width/2;
	uint16_t cY = y0 + height/2;
	int r = (int)(width/2);
	for(i = x0; i < x0 + width; i++){
		for(j = y0; j < y0 + height; j++){
			if((i-cX)*(i-cX) + (j-cY)*(j-cY) <= r*r){
				// (i, j) is inside the circle
				LCD_SetPoint(i, j, Yellow);
			}
			else{
				LCD_SetPoint(i, j, Black);
			}
		}
	}
}

void clearOldPacManPos(){
	
	int oldX0 = pacManPosX;
	int oldY0 = pacManPosY;
	if(mapGrid[oldY0][oldX0] == 1){
		drawCell(oldX0*cell_width, oldY0*cell_height, Blue);
	}
	else{
		drawCell(oldX0*cell_width, oldY0*cell_height, Black);
	}
}

void spawnBigCoin(){
	int i,j;
	srand(LPC_TIM1->TC);
	do{
		i = rand() % COLUMNS;
		j = rand() % ROWS;
	}while(mapGrid[j][i] != 0); // repeat until a valid cell is selected
	drawBigCoin(i*cell_width + (int)(cell_width/2), j*cell_height + (int)(cell_height/2));
	mapGrid[j][i] = 5; // tile number to identify big coins
}

void drawLife(){
	drawPacMan((2 + 2*(lifeCount-1))*cell_width, 30.5*cell_height, cell_width, cell_height);
	lifeCount++;
}

void clearPauseText(){
	int i, j;
	for(i = pausePosX; i < pausePosX+9; i++){
		for(j = pausePosY; j < pausePosY+2; j++){
			if(mapGrid[j][i] == 1){
				drawCell(i*cell_width, j*cell_height, Blue);
			}
			else{
				drawCell(i*cell_width, j*cell_height, Black);
			}
		}
	}
}

void initMap(){
	
	int i, j;
	for(i = 0; i < ROWS; i++){
		for(j = 0; j < COLUMNS; j++){
			switch(mapGrid[i][j]){
				case 0:
					// empty tile with coin
					drawCell(j*cell_width, i*cell_height, Black);
					LCD_SetPoint(j*cell_width + (int)(cell_width/2), i*cell_height + (int)(cell_height/2), Yellow);
					count++;
					break;
				case 1:
					// wall
					drawCell(j*cell_width, i*cell_height, Blue);
					break;
				case 2:
					// empty tile
					drawCell(j*cell_width, i*cell_height, Black);
					break;
				case 3:
					// teleport tile
					drawCell(j*cell_width, i*cell_height, Black);
					break;
				default:
					drawCell(j*cell_width, i*cell_height, Black);
					break;
			}
		}
	}
	
	// "Game over in:" position: (0, 0)
	// "Score:" position: (20, 0)
	GUI_Text(0*cell_width, 0*cell_height, (uint8_t *)" Game over in : ", White, Black);
	GUI_Text(20*cell_width, 0*cell_height, (uint8_t *)" Score : ", White, Black);
	drawPacMan(pacManPosX*cell_width, pacManPosY*cell_height, cell_width, cell_height);
	drawLife();
	
	if(togglePause){
		GUI_Text(pausePosX*cell_width, pausePosY*cell_height, (uint8_t *)" PAUSE ", White, Black);
	}
	
	char str[20];
	sprintf(str, "%d", timerCount);
	GUI_Text(6*cell_width, (1+0.2)*cell_height, (uint8_t *)str, White, Black);
	
}
