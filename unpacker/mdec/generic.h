/*
 *  PSX-Tools Bundle Pack
 *  Copyright (C) 2002 Nicolas "Pixel" Noble
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma pack(1)

#ifndef __GENERIC_H__
#define __GENERIC_H__
/*#ifdef __cplusplus
#include "String.h"
#endif*/

#define M_BARE   -1
#define M_ERROR   0
#define M_STATUS  1
#define M_WARNING 2
#define M_INFO    3

#ifndef bcopy
#define bcopy(x,y,z) memcpy((y),(x),(z))
#endif

#ifndef MAX
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

#ifndef MAX
#define MAX(a,b) ((a)<(b)?(b):(a)
#endif

#ifndef SDL_VERSIONNUM
typedef unsigned long int Uint32;
#endif

#ifndef int32
typedef signed long int int32;
#endif

#ifndef Uint16
typedef unsigned short int Uint16;
#endif

#ifndef int16
typedef signed short int int16;
#endif

#ifndef Uint8
typedef unsigned char Uint8;
#endif

#ifndef int8
typedef signed char int8;
#endif

#ifndef Byte
typedef Uint8 Byte;
#endif

#ifndef Word
typedef Uint16 Word;
#endif

#ifndef DWord
typedef Uint32 DWord;
#endif

#if defined __linux__ || defined __CYGWIN32__
#define PACKED __attribute__((packed))
#else
#ifndef PACKED
#define PACKED
#endif
#endif

extern char verbosity;
char ** split(char * s, char t);

#ifdef __cplusplus
//void printm(int level, String fmt, ...);

#ifndef MAX
template<class T>
inline T MAX(T a, T b) {
    return a < b ? b : a;
}
#endif

#ifndef MIN
template<class T>
inline T MIN(T a, T b) {
    return a > b ? b : a;
}
#endif

#else
#ifndef MAX
#define MAX(__a,__b) ((__a)<(__b)?(__b):(__a))
#endif

#ifndef MIN
#define MIN(__a,__b) ((__a)>(__b)?(__b):(__a))
#endif

#endif

#include <sys/types.h>
#include <sys/stat.h>

#if defined __linux__ || defined __CYGWIN32__
#define MKDIR(name) mkdir(name, 0777)
#elif defined __WIN32__
#define MKDIR mkdir
#elif defined WIN32
#else
#error Unknow compiler/platform
#endif

#endif
