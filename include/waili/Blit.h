//
//	Low-Level Block Operations
//
//  $Id: Blit.h,v 4.0.4.1 1999/07/20 16:15:43 geert Exp $
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

#ifndef WAILI_BLIT_H
#define WAILI_BLIT_H

#include "Types.h"

#include <string.h>


    //  Copy a Block of Data

template <class Type>
inline void Copy(const Type *src, Type *dst, u_int len)
{
    memmove(dst, src, len*sizeof(Type));
}

template <class Type>
inline void Copy(const Type *src, Type *dst)
{
    memmove(dst, src, sizeof(Type));
}


    //  Fill a Block of Data

template <class Type>
inline void Fill(Type *dst, u_int len, Type value)
{
    for (u_int i = 0; i < len; i++)
	dst[i] = value;
}


    //  Clear a Block of Data

template <class Type>
inline void Clear(Type *dst, u_int len)
{
    memset(dst, 0, len*sizeof(Type));
}

template <class Type>
inline void Clear(Type *dst)
{
    memset(dst, 0, sizeof(Type));
}


    //  Copy a Rectangular Block

template <class Type>
inline void CopyRect(const Type *src, u_int sw, Type *dst, u_int dw,
		     u_int cols, u_int rows)
{
    for (u_int r = 0; r < rows; r++) {
	Copy(src, dst, cols);
	src += sw;
	dst += dw;
    }
}

template <class Type>
inline void CopyRect(const Type *src, u_int sw, u_int sx, u_int sy, Type *dst,
		     u_int dw, u_int dx, u_int dy, u_int cols, u_int rows)
{
    CopyRect(src+sy*sw+sx, sw, dst+dy*dw+dx, dw, cols, rows);
}


    //  Fill a Rectangular Block

template <class Type>
inline void FillRect(Type *dst, u_int dw, u_int cols, u_int rows, Type value)
{
    for (u_int r = 0; r < rows; r++) {
	Fill(dst, cols, value);
	dst += dw;
    }
}

template <class Type>
inline void FillRect(Type *dst, u_int dw, u_int dx, u_int dy, u_int cols,
		     u_int rows, Type value)
{
    FillRect(dst+dy*dw+dx, dw, cols, rows, value);
}


    //  Clear a Rectangular Block

template <class Type>
inline void ClearRect(Type *dst, u_int dw, u_int cols, u_int rows)
{
    for (u_int r = 0; r < rows; r++) {
	Clear(dst, cols);
	dst += dw;
    }
}

template <class Type>
inline void ClearRect(Type *dst, u_int dw, u_int dx, u_int dy, u_int cols,
		      u_int rows)
{
    ClearRect(dst+dy*dw+dx, dw, cols, rows, value);
}

#endif // WAILI_BLIT_H
