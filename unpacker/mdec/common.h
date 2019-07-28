/* $Id: common.h,v 1.4 2004/11/27 21:48:07 pixel Exp $ */

/*
	common stuff

	Copyright (C) 1997, 1998, 1999, 2000 by these people, who contributed to this project

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

#ifndef __COMMON_H
#define __COMMON_H

#define _GNU_SOURCE
#define _USE_GNU

#include "generic.h"

#include <sys/types.h>

#if ENABLE_NLS
#if HAVE_LOCALE_H
#include <locale.h>
#endif
#if HAVE_LIBINTL_H
#include <libintl.h>
#endif
#define _(string) gettext(string)
#define N_(string) (string)
#else
#define _(string) (string)
#define N_(string) (string)
#endif

#endif
