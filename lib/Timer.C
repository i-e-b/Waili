//
//	Timer Class
//
//  $Id: Timer.C,v 4.0.4.1 1999/07/20 16:15:52 geert Exp $
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

#include <stdio.h>
#include <sys/times.h>
#include <assert.h>

#include <waili/Timer.h>


#if defined(sun) && !defined(CLK_TCK)	// SunOS
#define CLK_TCK	60
#endif


const char *Timer::rcsid = "$Id: Timer.C,v 4.0.4.1 1999/07/20 16:15:52 geert Exp $";


static inline void CurrentTime(time_t &real, time_t &user, time_t &system)
{
    struct tms tms;

    real = times(&tms);
    user = tms.tms_utime;
    system = tms.tms_stime;
}


    //  Start the Timer

void Timer::Start(void)
{
    time_t real, user, system;

    if (!Running) {
	CurrentTime(real, user, system);
	Running = 1;
	Real = real-Real;
	User = user-User;
	System = system-System;
    }
}


    //  Stop the Timer

void Timer::Stop(void)
{
    time_t real, user, system;

    if (Running) {
	CurrentTime(real, user, system);
	Running = 0;
	Real = real-Real;
	User = user-User;
	System = system-System;
    }
}


    //  Reset the Timer

void Timer::Reset(void)
{
    Running = 0;
    Real = 0;
    User = 0;
    System = 0;
}


    //  Get the Real part of the Timer

f32 Timer::GetReal(void) const
{
    time_t real, user, system;

    if (Running) {
	CurrentTime(real, user, system);
	real -= Real;
    } else
	real = Real;

    return((f32)real/CLK_TCK);
}


    //  Get the User part of the Timer

f32 Timer::GetUser(void) const
{
    time_t real, user, system;

    if (Running) {
	CurrentTime(real, user, system);
	user -= User;
    } else
	user = User;

    return((f32)user/CLK_TCK);
}


    //  Get the System part of the Timer

f32 Timer::GetSystem(void) const
{
    time_t real, user, system;

    if (Running) {
	CurrentTime(real, user, system);
	system -= System;
    } else
	system = System;

    return((f32)system/CLK_TCK);
}


    //  Get a Time Stamp copy of the Timer

Timer Timer::GetStamp(void) const
{
    Timer t;
    time_t real, user, system;

    if (Running) {
	CurrentTime(real, user, system);
	t.Real = real-Real;
	t.User = user-User;
	t.System = system-System;
    } else {
	t.Real = Real;
	t.User = User;
	t.System = System;
    }
    return(t);
}


    //  Reset and Start the Timer

void Timer::Tic(void)
{
    CurrentTime(Real, User, System);
    Running = 1;
}


    //  Dump the current Timer values

void Timer::Toc(void)
{
    Timer t;

    t = GetStamp();

    fprintf(stderr, "Real = %10.3f   User = %10.3f   System = %10.3f\n",
	      (f32)t.Real/CLK_TCK, (f32)t.User/CLK_TCK,
	      (f32)t.System/CLK_TCK);
}


    //  Timer math

Timer Timer::operator+(const Timer &t)
{
    Timer res;

    assert(!Running && !t.Running);

    res.Real = Real+t.Real;
    res.User = User+t.User;
    res.System = System+t.System;

    return(res);
}

Timer Timer::operator-(const Timer &t)
{
    Timer res;

    assert(!Running && !t.Running);

    res.Real = Real-t.Real;
    res.User = User-t.User;
    res.System = System-t.System;

    return(res);
}

void Timer::operator+=(const Timer &t)
{
    assert(!t.Running);

    if (Running) {
	Real -= t.Real;
	User -= t.User;
	System -= t.System;
    } else {
	Real += t.Real;
	User += t.User;
	System += t.System;
    }
}

void Timer::operator-=(const Timer &t)
{
    assert(!t.Running);

    if (Running) {
	Real += t.Real;
	User += t.User;
	System += t.System;
    } else {
	Real -= t.Real;
	User -= t.User;
	System -= t.System;
    }
}
