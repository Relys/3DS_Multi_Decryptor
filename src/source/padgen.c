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
	draw_fillrect(0, 50, 400, 180, BLACK);
	
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
	draw_fillrect(0, 50, 400, 180, BLACK);
	
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

static const uint8_t zero_buf[16] __attribute__((aligned(16))) = {0};

uint32_t createPad(struct pad_info *info)
{
	#define BUFFER_ADDR ((volatile uint8_t*)0x21000000)
	#define BLOCK_SIZE  (1*1024*1024)
	
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
	
	uint8_t ctr[16] __attribute__((aligned(32)));
	memcpy(ctr, info->CTR, 16);
	
	uint32_t size_bytes = info->size_mb*1024*1024;
	uint32_t size_100 = size_bytes/100;
	uint32_t i, j;
	for (i = 0; i < size_bytes; i += BLOCK_SIZE) {
		for (j = 0; (j < BLOCK_SIZE) && (i+j < size_bytes); j+= 16) {
			set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
			aes_decrypt((void*)zero_buf, (void*)BUFFER_ADDR+j, ctr, 1, AES_CTR_MODE);
			add_ctr(ctr, 1);
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
