//
//	Utility Routines
//
//  $Id: Util.h,v 4.0.4.1 1999/07/20 16:15:45 geert Exp $
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

#ifndef WAILI_UTIL_H
#define WAILI_UTIL_H

#include "Types.h"

#include <stddef.h>
#include <math.h>


#define EPS	(1E-14)


    //  Program Failure

void Die(const char *fmt, ...)
#ifdef __GNUG__
__attribute__ ((noreturn, format (printf, 1, 2)))
#endif // __GNUG__
;


    //  Unimplemented Functions

#define NotYetImplemented				\
    Die("%s: Not yet implemented (%s:%d)\n", __FUNCTION__, __FILE__, __LINE__)


    //	Memory Tracking

#ifdef TRACK_MEMORY
void *operator new(size_t);
void *operator new[](size_t);
void operator delete(void *);
void operator delete[](void *);
void DumpMemoryStatus(void);
#endif /* TRACK_MEMORY */


    //	Min/Max Functions

template <class Type>
inline Type Min(Type x, Type y)
{
#ifdef __GNUG__
    return(x <? y);
#else /* !__GNUG__ */
    return(x < y ? x : y);
#endif /* !__GNUG__ */
}

template <class Type>
inline Type Max(Type x, Type y)
{
#ifdef __GNUG__
    return(x >? y);
#else /* !__GNUG__ */
    return(x > y ? x : y);
#endif /* !__GNUG__ */
}

inline int Odd(int x)
{
    return(x%2);
}

inline int Even(int x)
{
    return(!Odd(x));
}

template <class Type>
inline Type Abs(Type x)
{
    return(x < 0 ? -x : x);
}

inline f32 Abs(f32 x)
{
    return(fabs(x));
}

inline f64 Abs(f64 x)
{
    return(fabs(x));
}

#endif // WAILI_UTIL_H
