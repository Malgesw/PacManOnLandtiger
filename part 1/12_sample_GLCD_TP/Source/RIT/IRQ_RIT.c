/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "RIT.h"
#include "map/map.h"
#include "timer/timer.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern int pacManPosX;
extern int pacManPosY;
extern int cell_width;
extern int cell_height;
extern unsigned short int mapGrid[ROWS][COLUMNS];
extern uint16_t pausePosX;
extern uint16_t pausePosY;
extern volatile unsigned short int spawnedBigCoins;

unsigned short int colliding = 0;
unsigned int coinCount = 0;

int J_up = 0;
int J_down = 0;
int J_left = 0;
int J_right = 0;

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
volatile unsigned short int togglePause = 1;

void RIT_IRQHandler (void)
{					
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && !togglePause){	
		/* Joytick UP pressed */
		J_down = 0;
		J_up = 1;
		J_right = 0;
		J_left = 0;
	}
	//else{
			//J_up=0;
	//}
	
	if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && !togglePause){	
		/* Joytick DOWN pressed */
		J_down = 1;
		J_up = 0;
		J_right = 0;
		J_left = 0;
	}
	//else{
			//J_down=0;
	//}
	
	if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && !togglePause){	
		/* Joytick LEFT pressed */
		J_down = 0;
		J_up = 0;
		J_right = 0;
		J_left = 1;
	}
	//else{
			//J_left=0;
	//}
	
	if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && !togglePause){	
		/* Joytick RIGHT pressed */
		J_down = 0;
		J_up = 0;
		J_right = 1;
		J_left = 0;
	}
	//else{
			//J_right=0;
	//}
	
	/* button management */
	/*************************INT0***************************/
	if(down_0 !=0){
		down_0++;
		if((LPC_GPIO2->FIOPIN & (1<<10)) == 0){
			switch(down_0){
				case 2:
					if(!togglePause){
						// game paused
						togglePause = 1;
						GUI_Text(pausePosX*cell_width, pausePosY*cell_height, (uint8_t *)" PAUSE ", White, Black);
						reset_RIT();
						disable_timer(0);
						disable_timer(1);
						disable_timer(2);
					}
					else{
						// game unpaused
						togglePause = 0;
						clearPauseText();
						reset_RIT();
						enable_timer(0);
						enable_timer(1);
						enable_timer(2);
					}
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_0=0;			
			NVIC_EnableIRQ(EINT0_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 20);     /* External interrupt 0 pin selection */
		}
	} // end INT0

	/*************************KEY1***************************/
	if(down_1 !=0){
		down_1++;
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){
			switch(down_1){
				case 2:
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	} // end KEY1

	/*************************KEY2***************************/
	if(down_2 !=0){
		down_2++;
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){
			switch(down_2){
				case 2:
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;		
			NVIC_EnableIRQ(EINT2_IRQn);							 /* disable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	} // end KEY2
	
	reset_RIT();
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
