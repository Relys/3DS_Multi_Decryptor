#include "dumpNand.h"
#include "3dstypes.h"
#include "libc.h"
#include "FS.h"
#include "draw.h"

uint32_t dumpNand(void)
{
	#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
	#define BLOCK_SIZE  (2*1024*1024)
	
	uint8_t fileHandle[32] = {0x00};
	uint8_t nandHandle[32] = {0x00};
	uint32_t bytesWritten;
	uint32_t bytesRead;
	uint32_t result;
	
	memset(&fileHandle, 0, 32);
	memset(&nandHandle, 0, 32);
	
	DEBUG("Opening sdmc:/nand_dump.bin ...");
	result = fileOpen(&fileHandle, L"sdmc:/nand_dump.bin", 6);
	if(result != 0)
		return 1;
	
	DEBUG("Opening wnand:/ ...");
	result = fileOpen(&nandHandle, L"wnand:/", 1);
	if(result != 0)
	{
		fileClose(&fileHandle);
		return 1;
	}
	
	DEBUG("Dumping NAND...");
	
	uint32_t i = 0;
	do
	{
		fileRead(&nandHandle, &bytesRead, (void*)BUFFER_ADDR, BLOCK_SIZE);
		fileWrite(&fileHandle, &bytesWritten, (void*)BUFFER_ADDR, bytesRead);
		if(bytesWritten != bytesRead)
		{
			DEBUG("ERROR, SD card may be full.");
			fileClose(&nandHandle);
			fileClose(&fileHandle);
			return 1;
		}
		i += bytesRead;
		draw_fillrect(SCREEN_TOP_W-33, 1, 32, 8, BLACK);
		font_draw_stringf(SCREEN_TOP_W-33, 1, CYAN, "%i%%", i/(988807168/100)); //988,807,168 bytes is how big my NAND is. Just used for the percentage indicator.
	} while(bytesRead == BLOCK_SIZE);
	
	draw_fillrect(SCREEN_TOP_W-33, 1, 32, 8, BLACK);
	DEBUG("Done!");
	
	fileClose(&nandHandle);
	fileClose(&fileHandle);
	return 0;
}
