//
//	Data Storage Class
//
//  $Id: Storage.h,v 4.0.2.2.2.1 1999/07/20 16:15:45 geert Exp $
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

#ifndef WAILI_STORAGE_H
#define WAILI_STORAGE_H

#include "Types.h"

#include <stdio.h>
#include <string.h>


    //  Normal or Compressed Stream Class

class Stream {
    public:
	Stream();
	Stream(const char *name, const char *mode = "r");
	virtual ~Stream();

	virtual void Open(const char *name, const char *mode = "r");
	virtual void Close(void);

	virtual void RawRead(void *data, int size);
	virtual void RawWrite(const void *data, int size);

	void Read(u8 *x, u_int cnt = 1);
	void Read(u16 *x, u_int cnt = 1);
	void Read(u32 *x, u_int cnt = 1);
	void Read(u64 *x, u_int cnt = 1);
	void Read(s8 *x, u_int cnt = 1);
	void Read(s16 *x, u_int cnt = 1);
	void Read(s32 *x, u_int cnt = 1);
	void Read(s64 *x, u_int cnt = 1);
	void Read(f32 *x, u_int cnt = 1);
	void Read(f64 *x, u_int cnt = 1);

	void Write(const u8 *x, u_int cnt = 1);
	void Write(const u16 *x, u_int cnt = 1);
	void Write(const u32 *x, u_int cnt = 1);
	void Write(const u64 *x, u_int cnt = 1);
	void Write(const s8 *x, u_int cnt = 1);
	void Write(const s16 *x, u_int cnt = 1);
	void Write(const s32 *x, u_int cnt = 1);
	void Write(const s64 *x, u_int cnt = 1);
	void Write(const f32 *x, u_int cnt = 1);
	void Write(const f64 *x, u_int cnt = 1);

	void Read(u8 &x);
	void Read(u16 &x);
	void Read(u32 &x);
	void Read(u64 &x);
	void Read(s8 &x);
	void Read(s16 &x);
	void Read(s32 &x);
	void Read(s64 &x);
	void Read(f32 &x);
	void Read(f64 &x);

	void Write(const u8 &x);
	void Write(const u16 &x);
	void Write(const u32 &x);
	void Write(const u64 &x);
	void Write(const s8 &x);
	void Write(const s16 &x);
	void Write(const s32 &x);
	void Write(const s64 &x);
	void Write(const f32 &x);
	void Write(const f64 &x);

	void Puts(const char *s);
	void Printf(const char *fmt, ...)
#ifdef __GNUG__
	__attribute__ ((format (printf, 2, 3)))
#endif // __GNUG__
;

    protected:
	FILE *File;
	char *Name;
	char *Mode;
	u_int Compressed;

    private:
	static const char *rcsid;
};


// ----------------------------------------------------------------------------


    //  Inline Class Member Functions

inline Stream::Stream()
    : File(NULL)
{}

inline void Stream::Read(u8 *x, u_int cnt)
{
    RawRead(x, cnt*sizeof(*x));
}

inline void Stream::Read(s8 *x, u_int cnt)
{
    Read((u8 *)x, cnt);
}

inline void Stream::Read(s16 *x, u_int cnt)
{
    Read((u16 *)x, cnt);
}

inline void Stream::Read(s32 *x, u_int cnt)
{
    Read((u32 *)x, cnt);
}

inline void Stream::Read(s64 *x, u_int cnt)
{
    Read((u64 *)x, cnt);
}

inline void Stream::Read(f32 *x, u_int cnt)
{
    Read((u32 *)x, cnt);
}

inline void Stream::Read(f64 *x, u_int cnt)
{
    Read((u64 *)x, cnt);
}

inline void Stream::Write(const u8 *x, u_int cnt)
{
    RawWrite(x, cnt*sizeof(*x));
}

inline void Stream::Write(const s8 *x, u_int cnt)
{
    Write((const u8 *)x, cnt);
}

inline void Stream::Write(const s16 *x, u_int cnt)
{
    Write((u16 *)x, cnt);
}

inline void Stream::Write(const s32 *x, u_int cnt)
{
    Write((u32 *)x, cnt);
}

inline void Stream::Write(const s64 *x, u_int cnt)
{
    Write((u64 *)x, cnt);
}

inline void Stream::Write(const f32 *x, u_int cnt)
{
    Write((u32 *)x, cnt);
}

inline void Stream::Write(const f64 *x, u_int cnt)
{
    Write((u64 *)x, cnt);
}

inline void Stream::Read(u8 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(u16 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(u32 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(u64 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(s8 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(s16 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(s32 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(s64 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(f32 &x)
{
    Read(&x, 1);
}

inline void Stream::Read(f64 &x)
{
    Read(&x, 1);
}

inline void Stream::Write(const u8 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const u16 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const u32 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const u64 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const s8 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const s16 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const s32 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const s64 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const f32 &x)
{
    Write(&x, 1);
}

inline void Stream::Write(const f64 &x)
{
    Write(&x, 1);
}

inline void Stream::Puts(const char *s)
{
    RawWrite(s, strlen(s));
}

#endif // WAILI_STORAGE_H
