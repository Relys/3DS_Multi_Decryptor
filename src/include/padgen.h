#ifndef _PADGEN_H
#define _PADGEN_H

#include "3dstypes.h"

#define MAXENTRIES 1024

struct sd_info_entry {
	uint8_t CTR[16];
	uint32_t size_mb;
	wchar_t filename[90];
} __attribute__((packed));

struct sd_info {
	uint32_t n_entries;
	struct sd_info_entry entries[MAXENTRIES];
} __attribute__((packed, aligned(16)));


struct ncch_info_entry {
	uint8_t  CTR[16];
	uint8_t  keyY[16];
	uint32_t size_mb;
	uint8_t  reserved[8];
	uint32_t uses7xCrypto;
	wchar_t  filename[56]; 
} __attribute__((packed));

struct ncch_info {
	uint32_t padding;
	uint32_t ncch_info_version;
	uint32_t n_entries;
	uint8_t  reserved[4];
	struct ncch_info_entry entries[MAXENTRIES];
} __attribute__((packed, aligned(16)));


struct pad_info {
	uint32_t keyslot;
	uint32_t setKeyY;
	uint8_t CTR[16];
	uint8_t  keyY[16];
	uint32_t size_mb;
	wchar_t filename[90];
} __attribute__((packed, aligned(16)));


uint32_t ncchPadgen(void);
uint32_t sdPadgen(void);
uint32_t nandPadgen(void);

uint32_t createPad(struct pad_info *info);

#endif
