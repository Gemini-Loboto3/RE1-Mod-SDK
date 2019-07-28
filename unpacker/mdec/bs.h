/* $Id: bs.h,v 1.5 2004/11/27 21:48:07 pixel Exp $ */

/*
	libbs - library for the bitstream image format

	Copyright (C) 1999, 2000 by these people, who contributed to this project

	  bero@geocities.co.jp
	  Daniel Balster <dbalster@psxdev.de>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*
	DCT code is based on Independent JPEG Group's sotfware
*/

#ifndef __LIB_BS_H
#define __LIB_BS_H

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/types.h>
#include <stdarg.h>
#include "generic.h"

typedef struct bs_input_image_t
{
	int width,height;
	int bit;
	int nextline;
	unsigned char *top,*lpbits;
} bs_input_image_t;

#define BS_MAGIC 0x3800
#define BS_TYPE 2

typedef struct bs_header_t
{
	Uint16 length;
	Uint16 magic;
	Uint16 q_scale;
	Uint16 type;
} bs_header_t;

typedef struct YUV_RGB
{
	BYTE b;
	BYTE g;
	BYTE r;
} YUV_RGB;

/* prototypes */

#ifdef __cplusplus
extern "C" {
#endif

void bs_init (void);

int bs_encode (				/* returns BS image size in bytes */
	bs_header_t *outbuf,	/* output BS image */
	bs_input_image_t *img,	/* input image descriptor */
	int type,				/* image type (use BS_TYPE) */
	int q_scale,			/* Q scaling factor (1=best,>= lower quality) */
	const unsigned char *myiqtab	/* provide own iqtab (NULL == default) */
	);

void bs_decode_rgb24 (
	unsigned char *outbuf,	/* output RGB bytes (width*height*3) */
	bs_header_t *img,		/* input BS image */
	int width, int height,	/* dimension of BS image */
	const unsigned char *myiqtab
	);

void bs_decode_rgb15 (
	unsigned short *outbuf,	/* output RGB bytes (width*height*2) */
	bs_header_t *img,		/* input BS image */
	int width, int height,	/* dimension of BS image */
	const unsigned char *myiqtab
	);

const unsigned char *bs_iqtab (void);

#ifdef __cplusplus
}
#endif

#endif /* __LIB_BS_H */
