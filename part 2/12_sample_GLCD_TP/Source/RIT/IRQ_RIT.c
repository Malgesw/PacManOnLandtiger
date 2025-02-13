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
#include "CAN/CAN.h"
#include "music/music.h"

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
extern volatile uint16_t lifeCount;
extern volatile unsigned short int timerCount;
extern uint16_t powerPillEaten;

unsigned short int colliding = 0;
unsigned int coinCount = 0;

uint16_t J_up = 0;
uint16_t J_down = 0;
uint16_t J_left = 0;
uint16_t J_right = 0;

volatile int down_0 = 0;
volatile int down_1 = 0;
volatile int down_2 = 0;
volatile unsigned short int togglePause = 1;


NOTE song2[] = {
	
	//GHOST SOUND
	{NOTE_G6, time_128th},
	{NOTE_FS6, time_128th},
	{NOTE_F6, time_128th},
	{NOTE_E6, time_128th},
	{NOTE_DS6, time_128th},
	{NOTE_D6, time_128th},
	{NOTE_CS6, time_128th},
	{NOTE_C6, time_128th},
	{NOTE_B5, time_128th},
	{NOTE_C6, time_128th},
	{NOTE_CS6, time_128th},
	{NOTE_D6, time_128th},
	{NOTE_DS6, time_128th},
	{NOTE_E6, time_128th},
	{NOTE_F6, time_128th},
	{NOTE_FS6, time_128th},
};

NOTE powerUpSong[] = {
	
	// SORT OF SUPER MARIO POWER UP SOUND
	{NOTE_C5, time_128th},
	{NOTE_G4, time_128th},
	{NOTE_C5, time_128th},
	{NOTE_E5, time_128th},
	{NOTE_G5, time_128th},
	{NOTE_C6, time_128th},
	{NOTE_G5, time_128th},
	
};

int currentNote2 = 0;
int currentNote3 = 0;
uint16_t gameStarted = 0;
extern volatile uint16_t introEnded;
extern volatile unsigned short int ghostMode;

void RIT_IRQHandler (void)
{
	if(!isNotePlaying() && gameStarted && introEnded && togglePause != 1 && ghostMode != 2 && !powerPillEaten){
		playNote(song2[currentNote2++]);
		if(currentNote2 == (sizeof(song2) / sizeof(song2[0]))){
			currentNote2 = 0;
		}
	}
	else if(!isNotePlaying() && gameStarted && introEnded && togglePause != 1 && powerPillEaten){
		playNote(powerUpSong[currentNote3++]);
		if(currentNote3 == (sizeof(powerUpSong) / sizeof(powerUpSong[0]))){
			currentNote3 = 0;
			powerPillEaten = 0;
		}
	}
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
						disable_timer(2);
						disable_timer(3);
					}
					else{
						// game unpaused
						if(!gameStarted){
							gameStarted = 1;
							introEnded = 1;
						}
						togglePause = 0;
						clearPauseText();
						reset_RIT();
						enable_timer(0);
						enable_timer(2);
						enable_timer(3);
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
