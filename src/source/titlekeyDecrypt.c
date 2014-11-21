#include "titlekeyDecrypt.h"
#include "3dstypes.h"
#include "libc.h"
#include "FS.h"
#include "draw.h"
#include "crypto.h"

uint32_t decryptTitlekeys(void)
{
	uint8_t encHandle[32] = {0x00};
	uint8_t decHandle[32] = {0x00};
	uint32_t bytesWritten;
	uint32_t bytesRead;
	uint32_t result;
	
	memset(&encHandle, 0, 32);
	memset(&decHandle, 0, 32);
	
	struct enckeys_info *info;
	info = (struct enckeys_info *)((void *)0x20316000);
	
	DEBUG("Opening sdmc:/encTitleKeys.bin ...");
	result = fileOpen(&encHandle, L"sdmc:/encTitleKeys.bin", 1);
	if(result != 0)
	{
		DEBUG("Could not open sdmc:/encTitleKeys.bin!");
		return 1;
	}
	DEBUG("Opened! Reading info...");
	fileRead(&encHandle, &bytesRead, info, 16);
	
	if (!info->n_entries || info->n_entries > MAXENTRIES) {
		ERROR("Too many/few entries specified: %i", info->n_entries);
		fileClose(&encHandle);
		return 1;
	}
	
	DEBUG("Number of entries: %i", info->n_entries);
	
	fileRead(&encHandle, &bytesRead, info->entries, info->n_entries*sizeof(struct title_key_entry));
	fileClose(&encHandle);
	
	DEBUG("Decrypting Title Keys...");
	
	uint8_t ctr[16] __attribute__((aligned(32)));
	uint8_t keyY[16] __attribute__((aligned(32)));
	uint32_t i;
	for(i = 0; i < info->n_entries; i++) {
		memset(ctr, 0, 16);
		memcpy(ctr, info->entries[i].titleId, 8);
		set_ctr(AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
		memcpy(keyY, (void *) (0x08097A8C + ((20 * info->entries[i].commonKeyIndex) + 5)), 16);
		setup_aeskey(0x3D, AES_BIG_INPUT|AES_NORMAL_INPUT, keyY);
		use_aeskey(0x3D);
		aes_decrypt(info->entries[i].encryptedTitleKey, info->entries[i].encryptedTitleKey, ctr, 1, AES_CBC_DECRYPT_MODE);
	}
	
	result = fileOpen(&decHandle, L"sdmc:/decTitleKeys.bin", 6);
	if(result != 0)
		return 1;
	fileWrite(&decHandle, &bytesWritten, info, info->n_entries*sizeof(struct title_key_entry)+16);
	fileClose(&decHandle);
	
	DEBUG("Done!");
	
	return 0;
}
