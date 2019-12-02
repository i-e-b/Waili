//
//	Timer Class
//
//  $Id: Timer.h,v 4.0.4.1 1999/07/20 16:15:45 geert Exp $
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

#ifndef WAILI_TIMER_H
#define WAILI_TIMER_H

#include "Types.h"

#ifdef __ultrix
#define _POSIX_SOURCE
#endif // __ultrix
#include <time.h>
#ifdef __ultrix
#undef _POSIX_SOURCE
#endif // __ultrix


class Timer {
    public:
	Timer();
	Timer(const Timer &t);

	void Start(void);
	void Stop(void);
	void Reset(void);
	f32 GetReal(void) const;
	f32 GetUser(void) const;
	f32 GetSystem(void) const;
	Timer GetStamp(void) const;
	int IsRunning(void) const;
	void Tic(void);
	void Toc(void);

	Timer operator+(const Timer &t);
	Timer operator-(const Timer &t);
	void operator+=(const Timer &t);
	void operator-=(const Timer &t);

    private:
	int Running;
	time_t Real;
	time_t User;
	time_t System;

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Inline Class Member Functions

inline Timer::Timer()
    : Running(0), Real(0), User(0), System(0)
{}

inline Timer::Timer(const Timer &t)
    : Running(t.Running), Real(t.Real), User(t.User), System(t.System)
{}

inline int Timer::IsRunning(void) const
{
    return(Running);
}

#endif // WAILI_TIMER_H
