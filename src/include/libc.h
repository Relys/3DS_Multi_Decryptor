/* Grabbed from HBL (https://valentine-hbl.googlecode.com)*/


#ifndef _LIBC_H_
#define _LIBC_H_

#include "3dstypes.h"

void *memset(void * ptr, int value, size_t num);
void *memcpy(void *destination, const void *source, size_t num);
//Returns number of digits
//int u32tostr(u32 n, char *out);
void wstr_to_str(const wchar_t *in, char *out);

#include <stdarg.h>

void putcp(void *p, char c);
void tfp_format(void *putp, void (*putf) (void *, char), char *fmt, va_list va);


//void  numu64tostr(uint64_t n, char *out);
//int   strlen(const char *text);
//char *strcpy(char *dest, const char *src);
//void  wstr_to_str(const wchar_t *in, char *out);
//void  wcstrcpy(wchar_t *destination, const wchar_t *source);
//wchar_t *wcstrcat(wchar_t *destination, const wchar_t *source);
//int  n_digits(int n);
//void int_to_wstr(int n, wchar_t *dest);

#endif
