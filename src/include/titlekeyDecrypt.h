#ifndef _TITLEKEYDECRYPT_H
#define _TITLEKEYDECRYPT_H

#include "3dstypes.h"

#define MAXENTRIES 1024
struct title_key_entry {
	uint32_t commonKeyIndex;
	uint8_t  reserved[4];
	uint8_t  titleId[8];
	uint8_t  encryptedTitleKey[16];
} __attribute__((packed));

struct enckeys_info {
	uint32_t n_entries;
	uint8_t  reserved[12];
	struct title_key_entry entries[MAXENTRIES];
} __attribute__((packed, aligned(16)));

uint32_t decryptTitlekeys(void);

#endif
