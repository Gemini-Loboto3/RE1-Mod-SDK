// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <stdio.h>

typedef unsigned char    u8;
typedef unsigned short   u16;
typedef unsigned int     u32;
typedef unsigned __int64 u64;
typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;
typedef signed __int64   s64;

#define MAGIC(a,b,c,d)		(a | (b<<8) | (c<<16) | (d<<24))

// modsdk functions
EXTERN_C __declspec(dllexport) void Modsdk_post_init();
EXTERN_C __declspec(dllexport) void Modsdk_load(u8 *src, size_t pos, size_t size);
EXTERN_C __declspec(dllexport) void Modsdk_save(u8* &dst, size_t &size);
