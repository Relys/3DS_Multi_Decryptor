#ifndef FS_H
#define FS_H

#include "3dstypes.h"
uint32_t fileOpen(void* fileHandle, wchar_t* fname, uint32_t mode);
void fileRead(void* fileHandle, uint32_t* bytesRead, void* in_buf, uint32_t size);
void fileWrite(void* fileHandle, uint32_t* bytesWritten, void* in_buf, uint32_t size);
void fileClose(void* fileHandle);

#endif
