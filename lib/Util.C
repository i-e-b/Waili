//
//	Utility Routines
//
//  $Id: Util.C,v 4.0.2.2.2.1 1999/07/20 16:15:52 geert Exp $
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
#include <stdio.h>
#include <stdlib.h>

#include <waili/Types.h>
#include <waili/Util.h>


    //  Program Failure

void Die(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(1);
}


// ----------------------------------------------------------------------------

#ifdef TRACK_MEMORY

    //  Memory Tracking

#define MAX_HASH	(31)
#define MEMTRACK_MAGIC	(0x4d74724d)	// MtrM

struct MemChunk {
    u32 Magic;
    struct MemChunk *Next;
    size_t Size;
    bool IsArray;
    // sizeof(struct MemChunk) == 16
    u_char Address[0];
};

static struct MemChunk *MemChunks[MAX_HASH] = { 0, };
static size_t AllocatedMemory = 0;


static void AddMemChunk(struct MemChunk *chunk)
{
    u_int hash = (u_int)chunk % MAX_HASH;

    chunk->Magic = MEMTRACK_MAGIC;
    chunk->Next = MemChunks[hash];
    MemChunks[hash] = chunk;
    AllocatedMemory += chunk->Size;
}

static void RemMemChunk(struct MemChunk *chunk)
{
    u_int hash = (u_int)chunk % MAX_HASH;
    struct MemChunk **p = &MemChunks[hash];
    const char *msg;

    switch (chunk->Magic) {
    	case MEMTRACK_MAGIC:
	    for (p = &MemChunks[hash]; *p; p = &(*p)->Next)
		if (*p == chunk) {
		    *p = chunk->Next;
		    chunk->Magic = ~MEMTRACK_MAGIC;
		    AllocatedMemory -= chunk->Size;
		    return;
		}
	    msg = "Not on list";
	    break;
	case ~MEMTRACK_MAGIC:
	    msg = "Freed twice";
	    break;
	default:
	    msg = "No magic";
	    break;
    }
    fprintf(stderr, "Warning: trying to delete unallocated block of size %d at"
		    " %p (%s)\n", chunk->Size, chunk->Address, msg);
}


static void *TrackedMalloc(size_t size, bool is_array)
{
    struct MemChunk *chunk;

    if (!(chunk = (struct MemChunk *)malloc(sizeof(struct MemChunk)+size)))
	Die("Fatal Error: Can't allocate %d bytes\n", size);
    else {
	chunk->Size = size;
	chunk->IsArray = is_array;
	AddMemChunk(chunk);
	return(chunk->Address);
    }
}

static void TrackedFree(void *p, bool is_array)
{
    struct MemChunk *chunk;

    if (p) {
	chunk = (struct MemChunk *)(p-sizeof(struct MemChunk));
	if (chunk->IsArray != is_array)
	    fprintf(stderr, "Warning: using delete%s on a block allocated with"
			    " new%s (%d bytes at %p)\n",
			    is_array ? " []" : "", is_array ? "" : " []",
			    chunk->Size, chunk->Address);
	RemMemChunk(chunk);
	free(chunk);
    }
}

void *operator new(size_t size)
{
    return(TrackedMalloc(size, 0));
}

void *operator new[](size_t size)
{
    return(TrackedMalloc(size, 1));
}

void operator delete(void *p)
{
    TrackedFree(p, 0);
}

void operator delete[](void *p)
{
    TrackedFree(p, 1);
}


void DumpMemoryStatus(void)
{
    struct MemChunk *chunk;

    if (AllocatedMemory) {
	fprintf(stderr, "%d bytes allocated:\n", AllocatedMemory);
	for (u_int i = 0; i < MAX_HASH; i++)
	    for (chunk = MemChunks[i]; chunk; chunk = chunk->Next)
		fprintf(stderr, "    %p: %d bytes%s\n", chunk->Address,
			chunk->Size, chunk->IsArray ? " []" : "");
    } else
	fputs("No memory allocated\n", stderr);
}

#endif /* TRACK_MEMORY */
