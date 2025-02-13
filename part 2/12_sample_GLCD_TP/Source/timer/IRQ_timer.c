/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer.h"
#include "RIT/RIT.h"
#include "map/map.h"
#include "CAN/CAN.h"
#include "pathfinder/pathfinder.h"
#include "music/music.h"
#include <stdio.h>

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern volatile unsigned short int timerCount;
extern unsigned short int mapGrid[ROWS][COLUMNS];
extern int coinCount;
extern uint16_t J_up;
extern uint16_t J_down;
extern uint16_t J_left;
extern uint16_t J_right;
extern volatile unsigned short int togglePause;
extern volatile unsigned short int count;
extern volatile unsigned short int spawnedBigCoins;
extern volatile unsigned short int ticks;
extern volatile uint16_t powerPillSpawnTime;
extern volatile uint16_t maxTicks;
extern volatile uint16_t lifeCount;
extern volatile unsigned short int ghostMode;
extern int cell_width;
extern int cell_height;

int pacManPosX = 3;
int pacManPosY = 4;
unsigned short int bigCoinsNumber = 6;
unsigned short int isStart = 1;

uint16_t pausePosX = 11;
uint16_t pausePosY = 15;
uint16_t ghostPosX = 14;
uint16_t ghostPosY = 15;
uint16_t c = 1;
uint16_t elapsedTime = 0;
uint16_t respawnTicks = 0;
uint16_t maxRespawnTicks = 3;
uint16_t frightenedTicks = 0;
uint16_t maxFrightenedTicks = 10;
uint16_t isDead = 0;
uint16_t ghostFreq = 2;
uint16_t respawnPacman = 0;
uint16_t powerPillEaten = 0;
uint16_t SinTable[45] =
{
    410, 467, 523, 576, 627, 673, 714, 749, 778,
    799, 813, 819, 817, 807, 789, 764, 732, 694, 
    650, 602, 550, 495, 438, 381, 324, 270, 217,
    169, 125, 87 , 55 , 30 , 12 , 2  , 0  , 6  ,   
    20 , 41 , 70 , 105, 146, 193, 243, 297, 353
};

NOTE song[] = {
	// PACMAN INTRO MUSIC
	{NOTE_C3, time_64th},
	{NOTE_C4, time_64th},
	{NOTE_G3, time_64th},
	{NOTE_E3, time_64th},
	{NOTE_C4, time_128th},
	{NOTE_G3, time_64th},
	{NOTE_E3, time_64th*2},
	{REST, time_128th},
	{NOTE_CS3, time_64th},
	{NOTE_CS4, time_64th},
	{NOTE_GS3, time_64th},
	{NOTE_F3, time_64th},
	{NOTE_CS4, time_128th},
	{NOTE_GS3, time_64th},
	{NOTE_F3, time_64th*2},
	{REST, time_128th},
	{NOTE_C3, time_64th},
	{NOTE_C4, time_64th},
	{NOTE_G3, time_64th},
	{NOTE_E3, time_64th},
	{NOTE_C4, time_128th},
	{NOTE_G3, time_64th},
	{NOTE_E3, time_64th*2},
	{REST, time_128th},
	{NOTE_E3, time_128th},
	{NOTE_F3, time_128th},
	{NOTE_FS3, time_128th},
	{REST, time_64th},
	{NOTE_FS3, time_128th},
	{NOTE_G3, time_128th},
	{NOTE_GS3, time_128th},
	{REST, time_64th},
	{NOTE_GS3, time_128th},
	{NOTE_A3, time_128th},
	{NOTE_AS3, time_128th},
	{NOTE_C4, time_64th*2}

};

int currentNote = 0;
uint16_t introEnded = 0;
extern uint16_t gameStarted;

void sendStats(){
	// from CAN1 to CAN2
	//CAN_TxMsg.data[0] = (lifeCount >> 8) & 0xFF;
	CAN_TxMsg.data[0] = timerCount & 0xFF;
	CAN_TxMsg.data[1] = lifeCount & 0xFF;
	CAN_TxMsg.data[2] = (coinCount >> 8) & 0xFF;
	CAN_TxMsg.data[3] = coinCount & 0xFF;
	CAN_TxMsg.len = 4;
	CAN_TxMsg.id = 2;
	CAN_TxMsg.format = STANDARD_FORMAT;
	CAN_TxMsg.type = DATA_FRAME;
	CAN_wrMsg (1, &CAN_TxMsg); 
}

void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		if(!togglePause){
			disable_timer(0);
			timerCount--;
			elapsedTime++;
			if(timerCount != 60 - elapsedTime){
				timerCount = 60 - elapsedTime;
			}
			sendStats();
			reset_RIT();
			reset_timer(0);
			enable_timer(0);
		}
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
	if(LPC_TIM1->IR & 1) // MR0
	{ 
		// main loop
		if(!isNotePlaying() && !gameStarted && !introEnded){
		playNote(song[currentNote++]);
		if(currentNote == (sizeof(song) / sizeof(song[0]))){
				introEnded = 1;
			}
	  }	
		if(timerCount > 0 && lifeCount != 0 && !isDead){
			if(count > 0){
				if(!togglePause){
					
					ticks++;
					if(ghostMode == 2){
						respawnTicks++;
					}
					else if(ghostMode == 1){
						frightenedTicks++;
					}
					if(!respawnPacman && J_up && mapGrid[pacManPosY - 1][pacManPosX] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY - 1][pacManPosX] == 0){
							mapGrid[pacManPosY - 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY - 1][pacManPosX] == 5){
							mapGrid[pacManPosY - 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							powerPillEaten = 1;
							count--;
							if(ghostMode != 2){
								ghostMode = 1;
							}
						}
						pacManPosY--;
					}
					if(!respawnPacman && J_down && mapGrid[pacManPosY + 1][pacManPosX] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY + 1][pacManPosX] == 0){
							mapGrid[pacManPosY + 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY + 1][pacManPosX] == 5){
							mapGrid[pacManPosY + 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							powerPillEaten = 1;
							count--;
							if(ghostMode != 2){
								ghostMode = 1;
							}
						}
						pacManPosY++;
					}
					if(!respawnPacman && J_left && mapGrid[pacManPosY][pacManPosX - 1] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY][pacManPosX - 1] == 0){
							mapGrid[pacManPosY][pacManPosX - 1] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY][pacManPosX - 1] == 5){
							mapGrid[pacManPosY][pacManPosX - 1] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							powerPillEaten = 1;
							count--;
							if(ghostMode != 2){
								ghostMode = 1;
							}
						}
						pacManPosX--;
						if(mapGrid[pacManPosY][pacManPosX] == 3 && pacManPosX != 27){
							pacManPosX = 27;
						}
					}
					if(!respawnPacman && J_right && mapGrid[pacManPosY][pacManPosX + 1] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY][pacManPosX + 1] == 0){
							mapGrid[pacManPosY][pacManPosX + 1] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY][pacManPosX + 1] == 5){
							mapGrid[pacManPosY][pacManPosX + 1] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							powerPillEaten = 1;
							count--;
							if(ghostMode != 2){
								ghostMode = 1;
							}
						}
						pacManPosX++;
						if(mapGrid[pacManPosY][pacManPosX] == 3 && pacManPosX != 1){
							pacManPosX = 1;
						}
					}
					if(!respawnPacman){
						drawPacMan(pacManPosX*cell_width, pacManPosY*cell_height, cell_width, cell_height, Yellow);
					}
					
					if(ghostPosX == pacManPosX && ghostPosY == pacManPosY && ghostMode != 2){
						if(ghostMode == 0){
							removeLife();
							if(lifeCount == 0 && !isDead){
								isDead = 1;
							}
							else if(lifeCount > 0 && !isDead && ghostMode != 2){
								ghostMode = 2;
								respawnTicks = 0;
								respawnPacman = 1;
								drawCell(ghostPosX*cell_width, ghostPosY*cell_height, Black);
								disable_timer(0);
								disable_timer(2);
								disable_timer(3);
								disable_RIT();
							}
						}
						else{
							ghostMode = 2;
							coinCount+=100;
							drawCell(ghostPosX*cell_width, ghostPosY*cell_height, Black);
							ghostPosX = 14;
							ghostPosY = 15;
						}
					}
					
					if((ticks % ghostFreq) == 0){ // change to (ticks % 2) == 0 if other timers are integrated
						//update ghost's position
						Point p = {ghostPosX, ghostPosY};
						if(ghostMode == 0){
							Point dest = {pacManPosX, pacManPosY};
							Point nextGhostDir = getNextDir(p, dest);
							clearOldGhostPos();
							Point nextGhostPos = {ghostPosX + nextGhostDir.posX, ghostPosY + nextGhostDir.posY};
							ghostPosX = nextGhostPos.posX;
							ghostPosY = nextGhostPos.posY;
							if(nextGhostDir.posX == 1 && mapGrid[ghostPosY][ghostPosX] == 3 && ghostPosX != 1){
								ghostPosX = 1;
							}
							else if(nextGhostDir.posX == -1 && mapGrid[ghostPosY][ghostPosX] == 3 && ghostPosX != 27){
								ghostPosX = 27;
							}
							drawPacMan(ghostPosX*cell_width, ghostPosY*cell_height, cell_width, cell_height, Red);
						}
						else if(ghostMode == 1){
							//Point nextGhostDir = getNextRandomDir(p);
							Point dest = {pacManPosX, pacManPosY};
							Point nextGhostDir = getNextDir(p, dest);
							clearOldGhostPos();
							Point nextGhostPos = {ghostPosX + nextGhostDir.posX, ghostPosY + nextGhostDir.posY};
							ghostPosX = nextGhostPos.posX;
							ghostPosY = nextGhostPos.posY;
							if(nextGhostDir.posX == 1 && mapGrid[ghostPosY][ghostPosX] == 3 && ghostPosX != 1){
								ghostPosX = 1;
							}
							else if(nextGhostDir.posX == -1 && mapGrid[ghostPosY][ghostPosX] == 3 && ghostPosX != 27){
								ghostPosX = 27;
							}
							drawPacMan(ghostPosX*cell_width, ghostPosY*cell_height, cell_width, cell_height, Blue);
						}
					}
					
					if(ghostPosX == pacManPosX && ghostPosY == pacManPosY && ghostMode != 2){
						if(ghostMode == 0){
							removeLife();
							if(lifeCount == 0 && !isDead){
								isDead = 1;
								drawCell(ghostPosX*cell_width, ghostPosY*cell_height, Black);
							}
							else if(lifeCount > 0 && !isDead && ghostMode != 2){
								ghostMode = 2;
								respawnTicks = 0;
								respawnPacman = 1;
								drawCell(ghostPosX*cell_width, ghostPosY*cell_height, Black);
								disable_timer(0);
								disable_timer(2);
								disable_timer(3);
								disable_RIT();
							}
						}
						else{
							ghostMode = 2;
							coinCount+=100;
							drawCell(ghostPosX*cell_width, ghostPosY*cell_height, Black);
							ghostPosX = 14;
							ghostPosY = 15;
						}
					}
					
					if(ticks == 10*powerPillSpawnTime && spawnedBigCoins < bigCoinsNumber && !respawnPacman){
						spawnBigCoin();
						spawnedBigCoins++;
						if(spawnedBigCoins == bigCoinsNumber){
							maxTicks = 2;
						}
					}
					
					if(respawnTicks == 10*maxRespawnTicks){
						ghostMode = 0;
						respawnTicks = 0;
						frightenedTicks = 0;
						ghostPosX = 14;
						ghostPosY = 15;
						if(respawnPacman){
							enable_timer(0);
							enable_timer(2);
							enable_timer(3);
							enable_RIT();
							respawnPacman = 0;
							pacManPosX = 3;
							pacManPosY = 4;
							J_up = 0;
							J_down = 0;
							J_right = 0;
							J_left = 0;
						}
					}
					
					if(frightenedTicks == 10*maxFrightenedTicks){
						ghostMode = 0;
						frightenedTicks = 0;
					}
					
					if(ticks == maxTicks){
						ticks = 0;
					}
					
					sendStats();
					
					if(coinCount >= 1000*c){
						c++;
						drawLife();
					}
					
					if(count < 60){
						ghostFreq = 1;
					}
				}
			}
			else{
				// player wins
				GUI_Text(9*cell_width, 14*cell_height, (uint8_t *)" Victory! ", White, Black);
				disable_timer(0);
				disable_timer(1);
				disable_timer(2);
				disable_timer(3);
				disable_RIT();
			}
		}
		else{
			GUI_Text(9*cell_width, 14*cell_height, (uint8_t *)" Game over! ", White, Black);
			timerCount = 0;
			disable_timer(0);
			disable_timer(1);
			disable_timer(2);
			disable_timer(3);
			disable_RIT();
		}
		
		LPC_TIM1->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM1->IR & 2){ // MR1
		// your code
		LPC_TIM1->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 4){ // MR2
		// your code	
		LPC_TIM1->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM1->IR & 8){ // MR3
		// your code	
		LPC_TIM1->IR = 8;			// clear interrupt flag 
	} 

	return;
}

/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER2_IRQHandler (void)
{
	if(LPC_TIM2->IR & 1) // MR0
	{ 
		// your code
		static int sineticks=0;
		/* DAC management */	
		static int currentValue; 
		currentValue = SinTable[sineticks];
		currentValue -= 410;
		currentValue /= 1;
		currentValue += 410;
		LPC_DAC->DACR = currentValue <<6;
		sineticks++;
		if(sineticks==45){
			sineticks=0;
		}
		LPC_TIM2->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM2->IR & 2){ // MR1
		// your code
		LPC_TIM2->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 4){ // MR2
		// your code	
		LPC_TIM2->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM2->IR & 8){ // MR3
		// your code	
		LPC_TIM2->IR = 8;			// clear interrupt flag 
	} 
  return;
}


/******************************************************************************
** Function name:		Timer2_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER3_IRQHandler (void)
{
  if(LPC_TIM3->IR & 1) // MR0
	{
		// your code
		disable_timer(2);
		LPC_TIM3->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM3->IR & 2){ // MR1
		// your code	
		LPC_TIM3->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 4){ // MR2
		// your code	
		LPC_TIM3->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM3->IR & 8){ // MR3
		// your code	
		LPC_TIM3->IR = 8;			// clear interrupt flag 
	}
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/

/*
void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		LPC_TIM0->IR = 1;			//clear interrupt flag
	}
	else if(LPC_TIM0->IR & 2){ // MR1
		// your code	
		LPC_TIM0->IR = 2;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 4){ // MR2
		// your code	
		LPC_TIM0->IR = 4;			// clear interrupt flag 
	}
	else if(LPC_TIM0->IR & 8){ // MR3
		// your code	
		LPC_TIM0->IR = 8;			// clear interrupt flag 
	}
  return;
}*/
