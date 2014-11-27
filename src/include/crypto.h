#ifndef CRYPTO_H
#define CRYPTO_H

#include "3dstypes.h"
typedef unsigned int size_t;

#define AES_BIG_INPUT      1
#define AES_LITTLE_INPUT   0
#define AES_NORMAL_INPUT   4
#define AES_REVERSED_INPUT 0
#define AES_BLOCK_SIZE 0x10

#define AES_CCM_DECRYPT_MODE (0 << 27)
#define AES_CCM_ENCRYPT_MODE (1 << 27)
#define AES_CTR_MODE         (2 << 27)
#define AES_CTR_MODE         (2 << 27)
#define AES_CBC_DECRYPT_MODE (4 << 27)
#define AES_CBC_ENCRYPT_MODE (5 << 27)
#define AES_ECB_DECRYPT_MODE (6 << 27)
#define AES_ECB_ENCRYPT_MODE (7 << 27)

#define REG_AESCNT     ((volatile uint32_t*)0x10009000)
#define REG_AESBLKCNT  ((volatile uint32_t*)0x10009004)
#define REG_AESWRFIFO  ((volatile uint32_t*)0x10009008)
#define REG_AESRDFIFO  ((volatile uint32_t*)0x1000900C)
#define REG_AESKEYSEL  ((volatile uint8_t *)0x10009010)
#define REG_AESKEYCNT  ((volatile uint8_t *)0x10009011)
#define REG_AESCTR     ((volatile uint32_t*)0x10009020)
#define REG_AESKEYFIFO ((volatile uint32_t*)0x10009108)
#define REG_AESKEYXFIFO ((volatile uint32_t*)0x10009104)

#define AES_CNT_START         0x80000000
#define AES_CNT_INPUT_ORDER   0x02000000
#define AES_CNT_OUTPUT_ORDER  0x01000000
#define AES_CNT_INPUT_ENDIAN  0x00800000
#define AES_CNT_OUTPUT_ENDIAN 0x00400000
#define AES_CNT_FLUSH_READ    0x00000800
#define AES_CNT_FLUSH_WRITE   0x00000400

void add_ctr(void* ctr, uint32_t carry);

void setup_aeskeyX(u8 keyslot, void* keyx);
void decrypt(void* key, void* iv, void* inbuf, void* outbuf, size_t size);
void setup_aeskey(uint32_t keyno, int value, void* key);
void use_aeskey(uint32_t keyno);
void set_ctr(int mode, void* iv);
void aes_decrypt(void* inbuf, void* outbuf, void* iv, size_t size, uint32_t mode);
void _decrypt(uint32_t value, void* inbuf, void* outbuf, size_t blocks);
void aes_fifos(void* inbuf, void* outbuf, size_t blocks);
void set_aeswrfifo(uint32_t value);
uint32_t read_aesrdfifo(void);
uint32_t aes_getwritecount();
uint32_t aes_getreadcount();
uint32_t aescnt_checkwrite();
uint32_t aescnt_checkread();

#endif /* end of include guard: CRYPTO_H */
