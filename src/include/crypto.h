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

void add_ctr(void* ctr, u32 carry);

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
