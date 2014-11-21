#include "input.h"
#include "3dstypes.h"
#include "libc.h"

#define MAX_CHECKS 100 //Number of button states to check for debouncing
//Really only need so much because we're not calling it regularly from a timer.

uint32_t debouncedButton = 0x0;
uint32_t keyDown = 0x0;
uint32_t keyUp = 0x0;
static uint32_t lastButton = 0x0;
static uint32_t buttonState[MAX_CHECKS] = {0x0};
static uint32_t buttonStateIndex = 0x0;

void updateButtons(void) //based on code from http://www.ganssle.com/debouncing-pt2.htm
{
	uint32_t i, j;
	
	lastButton = debouncedButton;
	
	for(i = 0; i < MAX_CHECKS*10; i++) //Only needed because we're not constantly calling the func with a timer.
	{
		buttonState[buttonStateIndex] = ~*(uint32_t*)0x10146000; //Button state is inverted for some reason. 0=pressed, so we need to invert it.
		
		buttonStateIndex += 1;
		if(buttonStateIndex > MAX_CHECKS)
		{
			buttonStateIndex = 0;
		}
	}
	
	j = 0xFFFF; //Only lower 16 bits are used to hold button state.
	for(i = 0; i < MAX_CHECKS; i++)
	{
		j = j & buttonState[i];
	}
	debouncedButton = j;
	
	keyDown = (lastButton ^ debouncedButton) & debouncedButton;
	keyUp = (lastButton ^ debouncedButton) & lastButton;
}

//This version is meant to be called regularly from a timer
/*void updateButtons(void) //http://www.ganssle.com/debouncing-pt2.htm
{
	uint32_t i, j;
	
	lastButton = debouncedButton;
	buttonState[buttonStateIndex] = ~*(uint32_t*)0x10146000; //Button state is inverted for some reason. 0=pressed, so we need to invert it.
	
	j = 0xFFFF; //Only lower 16 bits are used to hold button state.
	for(i = 0; i < MAX_CHECKS; i++)
	{
		j = j & buttonState[i];
	}
	debouncedButton = j;
	
	buttonStateIndex += 1;
	if(buttonStateIndex > MAX_CHECKS)
	{
		buttonStateIndex = 0;
	}
	keyDown = (lastButton ^ debouncedButton) & debouncedButton;
	keyUp = (lastButton ^ debouncedButton) & lastButton;
}*/
