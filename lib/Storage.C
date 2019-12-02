//
//	Data Storage Class
//
//  $Id: Storage.C,v 4.0.2.2.2.1 1999/07/20 16:15:52 geert Exp $
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

#include <stdarg.h>
#include <errno.h>
#include <netinet/in.h>

#include <waili/Util.h>
#include <waili/Storage.h>


const char *Stream::rcsid = "$Id: Storage.C,v 4.0.2.2.2.1 1999/07/20 16:15:52 geert Exp $";


    //  Create a Stream

Stream::Stream(const char *name, const char *mode)
    : File(NULL)
{
    Open(name, mode);
}


    //  Delete a Stream

Stream::~Stream()
{
    if (File)
	Close();
}


    //  Open a Stream

void Stream::Open(const char *name, const char *mode)
{
    if (File)
	Die("%s: Stream `%s' already opened\n", __FUNCTION__, Name);

    u_int nlen = strlen(name);

    if ((nlen > 3) && !strncmp(&name[nlen-3], ".gz", 3)) {
	Compressed = 1;
	char *cmd;
	char *pmode;
	if (!strcmp(mode, "r")) {
	    cmd = new char[nlen+6];
	    pmode = "r";
	    sprintf(cmd, "zcat %s", name);
	} else if (!strcmp(mode, "w")) {
	    cmd = new char[nlen+8];
	    pmode = "w";
	    sprintf(cmd, "gzip > %s", name);
	} else if (!strcmp(mode, "a")) {
	    cmd = new char[nlen+8];
	    pmode = "w";
	    sprintf(cmd, "gzip >> %s", name);
	} else
	    Die("%s: Invalid mode `%s' for a compressed stream\n",
		__FUNCTION__, mode);
	if (!(File = popen(cmd, pmode)))
	    Die("%s: popen() failed for `%s': %s\n", __FUNCTION__, name,
		strerror(errno));
	delete[] cmd;
    } else {
	Compressed = 0;
	if (!(File = fopen(name, mode)))
	    Die("%s: fopen() failed for `%s': %s\n", __FUNCTION__, name,
		strerror(errno));
    }
    Name = new char[nlen+1];
    strcpy(Name, name);
    Mode = new char[strlen(mode)+1];
    strcpy(Mode, mode);
}


    //  Close a Stream

void Stream::Close(void)
{
    if (!File)
	Die("%s: Stream not opened yet\n", __FUNCTION__);
    if (Compressed)
	pclose(File);
    else
	fclose(File);
    File = NULL;
    delete[] Name;
    delete[] Mode;
}


    //  Read Raw Data from a Stream

void Stream::RawRead(void *data, int size)
{
    if (fread((char *)data, size, 1, File) != 1)
	Die("%s: fread() on `%s' failed: %s\n", __FUNCTION__, Name,
	    strerror(errno));
}


    //  Write Raw Data to a Stream

void Stream::RawWrite(const void *data, int size)
{
    if (fwrite((char *)data, size, 1, File) != 1)
	Die("%s: fwrite() on `%s' failed: %s\n", __FUNCTION__, Name,
	    strerror(errno));
}


    //  Format and Write a String to a Stream

void Stream::Printf(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(File, fmt, args);
    va_end(args);
}


    //  Endianness-aware Routines

#if defined(BYTE_ORDER) && defined(BIG_ENDIAN) && defined(LITTLE_ENDIAN)
    #if BYTE_ORDER == BIG_ENDIAN
	#define WAILI_BIG_ENDIAN
    #elif BYTE_ORDER == LITTLE_ENDIAN
	#define WAILI_LITTLE_ENDIAN
    #endif
#elif defined(__BIG_ENDIAN) || defined(_BIG_ENDIAN)
    #define WAILI_BIG_ENDIAN
#elif defined(__LITTLE_ENDIAN) || defined(_LITTLE_ENDIAN)
    #define WAILI_LITTLE_ENDIAN
#endif
#if !defined(WAILI_BIG_ENDIAN) && !defined(WAILI_LITTLE_ENDIAN)
#error Please fix me
#endif

static inline u64 ntohll(u64 x)
{
#ifdef WAILI_BIG_ENDIAN
    return x;
#else
    u64 hi = ntohl(x & 0xffffffff);
    u64 lo = ntohl(x >> 32);
    return hi<<32 | lo;
#endif
}

static inline u64 htonll(u64 x)
{
#ifdef WAILI_BIG_ENDIAN
    return x;
#else
    u64 hi = htonl(x & 0xffffffff);
    u64 lo = htonl(x >> 32);
    return hi<<32 | lo;
#endif
}


void Stream::Read(u16 *x, u_int cnt)
{
    RawRead(x, cnt*sizeof(*x));
    for (u_int i = 0; i < cnt; i++)
	x[i] = ntohs(x[i]);
}

void Stream::Read(u32 *x, u_int cnt)
{
    RawRead(x, cnt*sizeof(*x));
    for (u_int i = 0; i < cnt; i++)
	x[i] = ntohl(x[i]);
}

void Stream::Read(u64 *x, u_int cnt)
{
    RawRead(x, cnt*sizeof(*x));
    for (u_int i = 0; i < cnt; i++)
	x[i] = ntohll(x[i]);
}

#define TMP_CONVERT_LEN_S	512
#define TMP_CONVERT_LEN_L	256
#define TMP_CONVERT_LEN_LL	128

static union {
    u16 s[TMP_CONVERT_LEN_S];
    u32 l[TMP_CONVERT_LEN_L];
    u64 ll[TMP_CONVERT_LEN_LL];
} tmp_convert;

void Stream::Write(const u16 *x, u_int cnt)
{
    while (cnt > 0) {
	u_int cnt2 = Min(cnt, (u_int)TMP_CONVERT_LEN_S);
	for (u_int i = 0; i < cnt2; i++)
	    tmp_convert.s[i] = htons(x[i]);
	RawWrite(tmp_convert.s, cnt2*sizeof(*x));
	x += cnt2;
	cnt -= cnt2;
    }
}

void Stream::Write(const u32 *x, u_int cnt)
{
    while (cnt > 0) {
	u_int cnt2 = Min(cnt, (u_int)TMP_CONVERT_LEN_L);
	for (u_int i = 0; i < cnt2; i++)
	    tmp_convert.l[i] = htonl(x[i]);
	RawWrite(tmp_convert.l, cnt2*sizeof(*x));
	x += cnt2;
	cnt -= cnt2;
    }
}

void Stream::Write(const u64 *x, u_int cnt)
{
    while (cnt > 0) {
	u_int cnt2 = Min(cnt, (u_int)TMP_CONVERT_LEN_LL);
	for (u_int i = 0; i < cnt2; i++)
	    tmp_convert.ll[i] = htonll(x[i]);
	RawWrite(tmp_convert.ll, cnt2*sizeof(*x));
	x += cnt2;
	cnt -= cnt2;
    }
}
