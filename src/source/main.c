#include "3dstypes.h"
#include "3ds.h"
#include "draw.h"
#include "libc.h"
#include "crypto.h"
#include "FS.h"
#include "input.h"
#include "padgen.h"
#include "dumpNand.h"
#include "titlekeyDecrypt.h"

extern void ExitThread(void);
extern uint32_t debouncedButton;
extern uint32_t keyDown;
extern uint32_t keyUp;

#define MENU_Y 30

static const char* choices[] = {"NCCH padgen", "SD padgen", "Title Key Decrypter", "NAND FAT16 padgen", "NAND dumper"};
static const uint32_t numChoices = sizeof(choices)/sizeof(choices[0]);

void menuSelection(uint32_t selection)
{
	ClearScreen();
	console_y = 10;
	
	DEBUG("%s", choices[selection]);
	DEBUG("");
	
	uint32_t result = 0;
	switch(selection)
	{
		case 0:
			result = ncchPadgen();
			break;
		case 1:
			result = sdPadgen();
			break;
		case 2:
			result = decryptTitlekeys();
			break;
		case 3:
			result = nandPadgen();
			break;
		case 4:
			result = dumpNand();
			break;
		default:
			result = 1;
			break;
	}
	if(result != 0)
		DEBUG("AN ERROR OCCURED");
	
	draw_fillrect(0, 230, 400, 10, BLACK);
	font_draw_string(0, 230, WHITE, "Finished. Press B to return to the menu");
	
	while(1)
	{
		updateButtons();
		if(keyDown & BUTTON_B)
			return;
	}
}

//Beginning of ugly freezing workaround
static volatile uint32_t blockThreads = 0; //Requires '-fno-zero-initialized-in-bss' due to our build setup.
static uint32_t checkThread = 1;
int main()
{
	uint8_t fileHandle[32] = {0x00};
	uint32_t bytesWritten;
	
	while(blockThreads){}; //Just in case a third thread tries to run out code. Shouldn't happen.
	
	if (checkThread) {
		checkThread = 0;
		fileOpen(&fileHandle, L"sdmc:/DeleteMe.bin", 6);
		fileWrite(&fileHandle, &bytesWritten, &checkThread, 4);
		fileClose(&fileHandle);
		uint32_t x;
		for(x = 0; x < 1000; x++) {
			if (blockThreads) {
				ExitThread();
				while(1){}; //Just in case ExitThread doesn't work or something, I don't know.
			}
		}
	}
	blockThreads = 1;
	//Explanation: Sometimes, using the FS functions just works.
	//Sometimes it causes another thread to start executing our code.
	//If that happens, we want to run on that second thread.
	//This only happens the first time they're used, so we just check at the beginning.
//End of ugly freezing workaround
	
	uint32_t cursorPos = 0;
	uint32_t i;
	uint32_t exitLoop = 0;
	
	while(1)
	{
		ClearScreen();
		
		draw_fillrect(0, 1, 400, 10, BLACK);
		font_draw_string(0, 1, RED, "3DS multitool thing *insert better name here*");
		
		font_draw_string(0, 210, WHITE, "DPAD UP/DOWN: Change selection");
		font_draw_string(0, 220, WHITE, "           A: Select item");
		
		for(i = 0; i < numChoices; i++)
		{
			font_draw_string(40, MENU_Y + (i * 10), WHITE, choices[i]);
		}
		font_draw_string(0, MENU_Y + (cursorPos * 10), YELLOW, "-->");
		
		updateButtons();
		exitLoop = 0;
		while(!exitLoop)
		{
			updateButtons();
			
			if(keyDown & BUTTON_A)
			{
				//draw_fillrect(0, 0, 400, 10, BLACK);
				//font_draw_stringf(0, 0, RED, "You selected \"%s\"", choices[cursorPos]);
				//Do something.
				if(cursorPos > numChoices)
				{
					draw_fillrect(0, 230, 400, 10, BLACK);
					font_draw_string(0, 230, RED, "INVALID SELECTION");
					return 0; //Something went wrong, let's just return.
				}
				else
				{
					menuSelection(cursorPos);
				}
				exitLoop = 1;
			}
			
			if(keyDown & BUTTON_DD)
			{
				draw_fillrect(0, MENU_Y + (cursorPos * 10), 30, 10, BLACK);
				cursorPos += 1;
				if (cursorPos > (numChoices - 1))
					cursorPos = 0;
				font_draw_string(0, MENU_Y + (cursorPos * 10), YELLOW, "-->");
			}
			
			if(keyDown & BUTTON_DU)
			{
				draw_fillrect(0, MENU_Y + (cursorPos * 10), 30, 10, BLACK);
				if (cursorPos == 0) {
					cursorPos = (numChoices - 1);
				} else {
					cursorPos -= 1;
				}
				font_draw_string(0, MENU_Y + (cursorPos * 10), YELLOW, "-->");
			}
		}
	}
	return 0;
}
