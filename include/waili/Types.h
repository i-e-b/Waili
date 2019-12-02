//
//	Data Types
//
//  $Id: Types.h,v 4.0.4.1 1999/07/20 16:15:45 geert Exp $
//
//  Copyright (C) 1996-1999 Department of Computer Science, K.U.Leuven, Belgium
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#ifndef WAILI_TYPES_H
#define WAILI_TYPES_H

#include "Compiler.h"

#include <sys/types.h>

//
//  These type definitions should work fine for at least the following
//  architectures:
//
//	DEC Alpha (OSF/1)
//	HP PA-RISC (HP-UX)
//	IBM RS/6000 (AIX)
//	Intel 386 (Linux)
//	MIPS R2000, R3000 (ULTRIX)
//	Motorola 68k (SunOS, Linux)
//	Sun SPARC (SunOS, Solaris)
//

typedef unsigned char		u8;	/* 8 bit unsigned integer */
typedef unsigned short		u16;	/* 16 bit unsigned integer */
typedef unsigned int		u32;	/* 32 bit unsigned integer */
typedef unsigned long long	u64;	/* 64 bit unsigned integer */

typedef signed char		s8;	/* 8 bit signed integer */
typedef signed short		s16;	/* 16 bit signed integer */
typedef signed int		s32;	/* 32 bit signed integer */
typedef signed long long	s64;	/* 64 bit signed integer */

typedef float			f32;	/* 32 bit floating point */
typedef double			f64;	/* 64 bit floating point */


//  Pixel type

typedef s16			PixType;


#endif // WAILI_TYPES_H
