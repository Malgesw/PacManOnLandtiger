/****************************************Copyright (c)****************************************************
**                                      
**                                 http://www.powermcu.com
**
**--------------File Info---------------------------------------------------------------------------------
** File name:               main.c
** Descriptions:            The GLCD application function
**
**--------------------------------------------------------------------------------------------------------
** Created by:              AVRman
** Created date:            2010-11-7
** Version:                 v1.0
** Descriptions:            The original version
**
**--------------------------------------------------------------------------------------------------------
** Modified by:             Paolo Bernardi
** Modified date:           03/01/2020
** Version:                 v2.0
** Descriptions:            basic program for LCD and Touch Panel teaching
**
*********************************************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "LPC17xx.h"
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "joystick/joystick.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "map/map.h"
#include <stdlib.h>

extern volatile int count;
volatile unsigned short int timerCount = 60;
volatile unsigned short int spawnedBigCoins = 0;

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif

int main(void)
{
  SystemInit();  												/* System Initialization (i.e., PLL)  */
	BUTTON_init();												/* BUTTON Initialization              */
	joystick_init();											/* Joystick Initialization            */
  LCD_Initialization();
  TP_Init();
	//TouchPanel_Calibrate();
	
	//init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
	init_RIT(0x00989680); // use this when using the emulator
	enable_RIT();													/* RIT enabled												*/
	
	LPC_SC -> PCONP |= (1 << 22);  // TURN ON TIMER 2
	//LPC_SC -> PCONP |= (1 << 23);  // TURN ON TIMER 3
	
	LCD_Clear(Black);
	
	// game over timer (1s clock period) 0x17D7840
	init_timer(0, 0, 0, 1, 0x17D7840);
	// main loop timer
	init_timer(1, 0, 0, 3, 0x2625A0); //(10fps -> (1/10)s clock period)
	
	srand(LPC_TIM0->TC);
	
	uint32_t bigCoinsTimerCount = (uint32_t)(2 + rand() % 9);	// big coins will spawn at random time between 2 to 10s
	bigCoinsTimerCount = bigCoinsTimerCount*25*1000000;
	init_timer(2, 0, 0, 3, bigCoinsTimerCount);
	
	initMap();
	NVIC_SetPriority(TIMER0_IRQn, 2);
	NVIC_SetPriority(TIMER1_IRQn, 1);
	NVIC_SetPriority(TIMER2_IRQn, 1);
	NVIC_SetPriority(RIT_IRQn, 2);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
	
  while (1)	
  {
		__ASM("wfi");
  }
}

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/
