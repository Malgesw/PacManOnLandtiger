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
#include "map/map.h"
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
extern int J_up;
extern int J_down;
extern int J_left;
extern int J_right;
extern volatile unsigned short int togglePause;
extern volatile unsigned short int count;
extern volatile unsigned short int spawnedBigCoins;
extern int cell_width;
extern int cell_height;

int pacManPosX = 3;
int pacManPosY = 4;
unsigned short int bigCoinsNumber = 6;
uint16_t pausePosX = 11;
uint16_t pausePosY = 15;
uint16_t c = 1;

void TIMER0_IRQHandler (void)
{
	if(LPC_TIM0->IR & 1) // MR0
	{ 
		// your code
		if(!togglePause){
			disable_timer(0);
			timerCount--;
			char str[20];
			sprintf(str, "%02d", timerCount);
			GUI_Text(6*cell_width, (1+0.2)*cell_height, (uint8_t *)str, White, Black);
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
		if(timerCount > 0){
			if(count > 0){
				if(!togglePause){
		
					if(J_up && mapGrid[pacManPosY - 1][pacManPosX] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY - 1][pacManPosX] == 0){
							mapGrid[pacManPosY - 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY - 1][pacManPosX] == 5){
							mapGrid[pacManPosY - 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							count--;
						}
						pacManPosY--;
					}
					if(J_down && mapGrid[pacManPosY + 1][pacManPosX] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY + 1][pacManPosX] == 0){
							mapGrid[pacManPosY + 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY + 1][pacManPosX] == 5){
							mapGrid[pacManPosY + 1][pacManPosX] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							count--;
						}
						pacManPosY++;
					}
					if(J_left && mapGrid[pacManPosY][pacManPosX - 1] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY][pacManPosX - 1] == 0){
							mapGrid[pacManPosY][pacManPosX - 1] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY][pacManPosX - 1] == 5){
							mapGrid[pacManPosY][pacManPosX - 1] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							count--;
						}
						pacManPosX--;
						if(mapGrid[pacManPosY][pacManPosX] == 3 && pacManPosX != 27){
							pacManPosX = 27;
						}
					}
					if(J_right && mapGrid[pacManPosY][pacManPosX + 1] != 1){
						clearOldPacManPos();
						if(mapGrid[pacManPosY][pacManPosX + 1] == 0){
							mapGrid[pacManPosY][pacManPosX + 1] = 4; // special type for empty cells that had a coin before
							coinCount+=10;
							count--;
						}
						else if(mapGrid[pacManPosY][pacManPosX + 1] == 5){
							mapGrid[pacManPosY][pacManPosX + 1] = 4; // special type for empty cells that had a coin before
							coinCount+=50; // big coin
							count--;
						}
						pacManPosX++;
						if(mapGrid[pacManPosY][pacManPosX] == 3 && pacManPosX != 1){
							pacManPosX = 1;
						}
					}
					drawPacMan(pacManPosX*cell_width, pacManPosY*cell_height, cell_width, cell_height);
					
					char str[20];
					sprintf(str, "%d", coinCount);
					GUI_Text(24*cell_width, (1+0.2)*cell_height, (uint8_t *)str, White, Black);
					
					if(coinCount >= 1000*c){
						c++;
						drawLife();
					}
				}
			}
			else{
				// player wins
				GUI_Text(9.5*cell_width, 14*cell_height, (uint8_t *)" Victory! ", White, Black);
				disable_timer(0);
				disable_timer(1);
				disable_timer(2);
			}
		}
		else{
			GUI_Text(9*cell_width, 14*cell_height, (uint8_t *)" Game over! ", White, Black);
			timerCount = 0;
			disable_timer(0);
			disable_timer(1);
			disable_timer(2);
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
		if (spawnedBigCoins < bigCoinsNumber){
			spawnBigCoin();
			spawnedBigCoins++;
		}
		else{
			disable_timer(2);
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
