#include "padgen.h"
#include "3dstypes.h"
#include "FS.h"
#include "libc.h"
#include "draw.h"
#include "crypto.h"

uint32_t ncchPadgen()
{
	uint8_t fileHandle[32] = {0x0};
	uint32_t bytesRead;
	uint32_t result;
	
	struct ncch_info *info;
	info = (struct ncch_info *)((void *)0x20316000);
	
	uint8_t slot0x25KeyX[16] = {0x00};
	
	memset(&fileHandle, 0, 32);
	result = fileOpen(&fileHandle, L"sdmc:/slot0x25KeyX.bin", 1);
	if(result != 0)
	{
		DEBUG("Could not open sdmc:/slot0x25KeyX.bin!");
		return 1;
	}
	fileRead(&fileHandle, &bytesRead, &slot0x25KeyX, 16);
	fileClose(&fileHandle);
	if(bytesRead != 16)
	{
		DEBUG("slot0x25KeyX.bin is too small!");
		return 1;
	}
	setup_aeskeyX(0x25, slot0x25KeyX);
	
	memset(&fileHandle, 0, 32);
	DEBUG("Opening sdmc:/ncchinfo.bin ...");
	result = fileOpen(&fileHandle, L"sdmc:/ncchinfo.bin", 1);
	if(result != 0)
	{
		DEBUG("Could not open sdmc:/ncchinfo.bin!");
		return 1;
	}
	fileRead(&fileHandle, &bytesRead, info, 16);
	
	if (!info->n_entries || info->n_entries > MAXENTRIES || (info->ncch_info_version != 0xF0000003)) {
		DEBUG("Too many/few entries, or wrong version ncchinfo.bin");
		return 0;
	}
	fileRead(&fileHandle, &bytesRead, info->entries, info->n_entries*sizeof(struct ncch_info_entry));
	fileClose(&fileHandle);
	
	DEBUG("Number of entries: %i", info->n_entries);
	
	uint32_t i;
	for(i = 0; i < info->n_entries; i++) {
		DEBUG("Creating pad number: %i  size (MB): %i", i+1, info->entries[i].size_mb);
		
		struct pad_info padInfo = {.setKeyY = 1, .size_mb = info->entries[i].size_mb};
		memcpy(padInfo.CTR, info->entries[i].CTR, 16);
		memcpy(padInfo.keyY, info->entries[i].keyY, 16);
		memcpy(padInfo.filename, info->entries[i].filename, 112);
		
		if(info->entries[i].uses7xCrypto)
			padInfo.keyslot = 0x25;
		else
			padInfo.keyslot = 0x2C;
		
		result = createPad(&padInfo);
		if (!result)
			DEBUG("\tDone!");
		else
			return 1;
	}
	draw_fillrect(0, 50, SCREEN_TOP_W, (SCREEN_TOP_H-10-50), BLACK);
	
	return 0;
}

uint32_t sdPadgen()
{
	uint8_t fileHandle[32] = {0x0};
	uint32_t bytesRead;
	uint32_t result;
	
	struct sd_info *info;
	info = (struct sd_info *)((void *)0x20316000);
	
	memset(&fileHandle, 0, 32);
	DEBUG("Opening sdmc:/SDinfo.bin ...");
	result = fileOpen(&fileHandle, L"sdmc:/SDinfo.bin", 1);
	if(result != 0)
	{
		DEBUG("Could not open sdmc:/SDinfo.bin!");
		return 1;
	}
	fileRead(&fileHandle, &bytesRead, info, 4);
	
	if (!info->n_entries || info->n_entries > MAXENTRIES) {
		DEBUG("Too many/few entries!");
		fileClose(&fileHandle);
		return 1;
	}
	
	DEBUG("Number of entries: %i", info->n_entries);
	
	fileRead(&fileHandle, &bytesRead, info->entries, info->n_entries*sizeof(struct sd_info_entry));
	fileClose(&fileHandle);
	
	uint32_t i;
	for(i = 0; i < info->n_entries; i++) {
		DEBUG("Creating pad number: %i size (MB): %i", i+1, info->entries[i].size_mb);
		
		struct pad_info padInfo = {.keyslot = 0x34, .setKeyY = 0, .size_mb = info->entries[i].size_mb};
		memcpy(padInfo.CTR, info->entries[i].CTR, 16);
		memcpy(padInfo.filename, info->entries[i].filename, 180);
		
		result = createPad(&padInfo);
		if (!result)
			DEBUG("\tDone!");
		else
			return 1;
	}
	draw_fillrect(0, 50, SCREEN_TOP_W, (SCREEN_TOP_H-10-50), BLACK);
	
	return 0;
}

uint32_t nandPadgen()
{
	//Just trying to make sure the data we're looking for is where we think it is.
	//Should use some sort of memory scanning for it instead of hardcoding the address, though.
	if (*(uint32_t*)0x080D7CAC != 0x5C980)
		return 1;
	
	uint8_t ctr[16] = {0x0};
	uint32_t i = 0;
	for(i = 0; i < 16; i++) {
		ctr[i] = *((uint8_t*)(0x080D7CDC+(15-i))); //The CTR is stored backwards in memory.
	}
	add_ctr(ctr, 0xB93000); //The CTR stored in memory would theoretically be for NAND block 0, so we need to increment it some.
	
	DEBUG("Creating NAND FAT16 xorpad.  size (MB): 760");
	DEBUG("\tFilename: sdmc:/nand.fat16.xorpad");
	
	struct pad_info padInfo = {.keyslot = 0x4, .setKeyY = 0, .size_mb = 760, .filename = L"sdmc:/nand.fat16.xorpad"};
	//It's actually around 758MB in size. But meh, I'll just round up a bit.
	memcpy(padInfo.CTR, ctr, 16);
	
	uint32_t result = createPad(&padInfo);
	if(result == 0)
	{
		DEBUG("\tDone!");
		return 0;
	}
	else
	{
		return 1;
	}
}

inline uint32_t swap_uint32(uint32_t val)
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF ); 
    return (val << 16) | (val >> 16);
}

//Update 11/25/14
//Since this function just generates XORPADs, it's pointless to do CTR mode with zero'd input. Just do ECB with the "CTR" as input.
//Changed input endianness to little endian. Makes updating the "CTR" much easier.
//Inlined and changed some stuff.
//Gets ~3.15MB/s, up from ~1.8MB/s before

//Update 11/26/14
//Switched to assembly for inner loop.
//BLOCK_SIZE raised to 4MB.
//Gets ~3.55MB/s
uint32_t createPad(struct pad_info *info)
{
	#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
	#define BLOCK_SIZE  (4*1024*1024)
	
	uint8_t fileHandle[32] = {0x0};
	uint32_t bytesWritten;
	uint32_t result;
	
	memset(&fileHandle, 0, 32);
	
	result = fileOpen(&fileHandle, info->filename, 6);
	if(result != 0)
		return 1;
	
	if(info->setKeyY)
		setup_aeskey(info->keyslot, AES_BIG_INPUT|AES_NORMAL_INPUT, info->keyY);
	use_aeskey(info->keyslot);
	
	uint32_t ctr[4] __attribute__((aligned(32)));
	memcpy(ctr, info->CTR, 16);
	//CTR we get is big endian, swap it.
	ctr[0] = swap_uint32(ctr[0]);
	ctr[1] = swap_uint32(ctr[1]);
	ctr[2] = swap_uint32(ctr[2]);
	ctr[3] = swap_uint32(ctr[3]);
	
	uint32_t size_bytes = info->size_mb*1024*1024;
	uint32_t size_100 = size_bytes/100;
	uint32_t i, j;
	uint32_t buffAddr, buffEnd;
	
	for (i = 0; i < size_bytes; i += BLOCK_SIZE) {
		buffAddr = (uint32_t)BUFFER_ADDR;
		
		for(j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j += (512*1024))
		{
			*REG_AESCNT = 0;
			*REG_AESBLKCNT = ((512*1024) / AES_BLOCK_SIZE) << 16;
			*REG_AESCNT = AES_ECB_ENCRYPT_MODE |
				AES_CNT_START |
				AES_CNT_INPUT_ORDER |
				AES_CNT_OUTPUT_ORDER |
				//AES_CNT_INPUT_ENDIAN |
				AES_CNT_OUTPUT_ENDIAN |
				AES_CNT_FLUSH_READ |
				AES_CNT_FLUSH_WRITE;
			
			buffEnd = buffAddr + (512*1024);
			
			//This is my first time using inline assembly, and it probably shows.
			asm volatile(
				"LDR     R1, =0x10009000\n\t" /* Base AES IO Register */
				
				"wrFifoWait:\n\t"
					"LDR     R0, [R1]\n\t"
					"AND     R0, R0, #0x1F\n\t"
					"CMP     R0, #0xF\n\t"
					"BHI     wrFifoWait\n\t"
				
				"STR     %[ctr0], [R1,#8]\n\t" /* Write input into AES WR FIFO */
				"STR     %[ctr1], [R1,#8]\n\t"
				"STR     %[ctr2], [R1,#8]\n\t"
				"STR     %[ctr3], [R1,#8]\n\t"
				
				"rdFifoWait:\n\t"
					"LDR     R0, [R1]\n\t"
					"MOV     R0, R0,LSR#5\n\t"
					"AND     R0, R0, #0x1F\n\t"
					"CMP     R0, #3\n\t"
					"BLS     rdFifoWait\n\t"
				
				"LDR     R5, [R1,#0xC]\n\t" /* Read output from AES RD FIFO */
				"LDR     R6, [R1,#0xC]\n\t"
				"LDR     R7, [R1,#0xC]\n\t"
				"LDR     R8, [R1,#0xC]\n\t"
				"STMIA   %[buffAddr]!, {R5-R8}\n\t"
				
				"ADDS    %[ctr3], %[ctr3], #1\n\t" /* Increment the "CTR" */
				"ADCS    %[ctr2], %[ctr2], #0\n\t"
				"ADCS    %[ctr1], %[ctr1], #0\n\t"
				"ADC     %[ctr0], %[ctr0], #0\n\t"
				
				"CMP     %[buffAddr], %[buffEnd]\n\t" /* Did we finish our 512KB block? */
				"BNE     wrFifoWait"
				: [ctr0]"+r" (ctr[0]), [ctr1]"+r" (ctr[1]), [ctr2]"+r" (ctr[2]), [ctr3]"+r" (ctr[3]), [buffAddr]"+r" (buffAddr) /* Output */
				: [buffEnd]"r" (buffEnd) /* Input */
				: "r0", "r1", "r5", "r6", "r7", "r8" /* Clobbered */
			);
		}
		
		draw_fillrect(SCREEN_TOP_W-33, 1, 32, 8, BLACK);
		font_draw_stringf(SCREEN_TOP_W-33, 1, CYAN, "%i%%", (i+j)/size_100);
		
		fileWrite(&fileHandle, &bytesWritten, (void*)BUFFER_ADDR, j);
		if(bytesWritten != j)
		{
			draw_fillrect(0, 50, 400, 180, BLACK);
			console_y = 50;
			DEBUG("ERROR, SD card may be full.");
			fileClose(&fileHandle);
			return 1;
		}
	}
	
	draw_fillrect(SCREEN_TOP_W-33, 1, 32, 8, BLACK);
	fileClose(&fileHandle);
	return 0;
}
