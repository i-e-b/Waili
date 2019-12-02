//
//	Range checking for wavelet coefficients
//
//  $Id: Rangecheck.C,v 4.2.4.1 1999/07/20 16:16:19 geert Exp $
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

#ifndef NULL
#define NULL	0
#endif

#include <stdio.h>
#include <stdlib.h>

#include <waili/Image.h>
#include <waili/Timer.h>


#define PIXTYPE_MINBITS		(64)
#define PIXTYPE_PROBE_MINVAL	(-36028797018963968LL)
#define PIXTYPE_PROBE_MAXVAL	(36028797018963967LL)


const char *ProgramName;

int Verbose = 0;


static void
#ifdef __GNUG__
__attribute__ ((noreturn))
#endif // __GNUG__
Usage(void)
{
    Die("\nUsage: %s [options] filename\n\n"
	"Valid options are:\n"
	"    -h, --help            display this usage information\n"
	"    -v, --verbose         verbose mode\n"
	"    -p, --primal number   number of primal vanishing moments (default: 2)\n"
	"    -d, --dual number     number of dual vanishing moments (default: 2)\n"
	"    -l, --levels          number of transform levels (default: 1)\n"
	"    --min number          minimum pixel value\n"
	"    --max number          maximum pixel value\n"
	"\n",
	ProgramName);
}


Channel *DoFwt(Channel *test, const Wavelet *wavelet, u_int numlevels)
{
    for (u_int j = 0; j < numlevels; j++) {
	Channel *ch = test->PushFwtStepR(*wavelet);
	if (!ch)
	    Die("PushFwtStepR() returned NULL (j = %d)\n", j);
	if (ch != test) {
	    delete test;
	    test = ch;
	}
    }
    return(test);
}

void DumpScale(const NTChannel &ch)
{
    for (u_int i = 0; i < ch.GetCols(); i++)
	putc(i%10 == 0 ? '0'+((i/10)%10) : ' ', stdout);
    puts("");
    for (u_int i = 0; i < ch.GetCols(); i++)
	putc('0'+(i%10), stdout);
    puts("");
}

void DumpMarks(const NTChannel &ch)
{
    for (u_int i = 0; i < ch.GetCols(); i++) {
	PixType val = ch(i, 0);
	char mark;
	if (val < 0)
	    mark = '-';
	else if (val > 0)
	    mark = '+';
	else
	    mark = '0';
	putc(mark, stdout);
    }
    puts("");
}

void TestData(const NTChannel &ch)
{
    int state = 0;

    for (u_int i = 0; i < ch.GetCols(); i++)
	switch (state) {
	    case 0:	/* initial */
		if (ch(i, 0))
		    goto bad;
		state = 1;
		break;
	    case 1:	/* leading zeroes */
		if (ch(i, 0))
		    state = 2;
		break;
	    case 2:	/* real data */
		if (!ch(i, 0))
		    state = 3;
		break;
	    case 3:	/* trailing zeroes */
		if (ch(i, 0))
		    goto bad;
		break;
	}
    if (state == 3)
	return;
bad:
    DumpMarks(ch);
    Die("bad result sequence\n");
}

PixType DoEval(const NTChannel &ch, const Wavelet *wavelet, u_int len,
	       u_int numlevels, u_int probe)
{
    Channel *test = new NTChannel(len, 1);
    test->Clear();
    TestData(ch);
    if (Verbose)
	DumpMarks(ch);
    for (u_int i = 0; i < ch.GetCols(); i++)
	(*test)(i, 0) = ch(i, 0);
    test = DoFwt(test, wavelet, numlevels);
    PixType val = (*test)(probe, 0);
    delete test;
    return(val);
}


int main(int argc, char *argv[])
{
    if (sizeof(PixType) < PIXTYPE_MINBITS/8)
	Die("PixType must contain %d bits (now: %d bits)\n", PIXTYPE_MINBITS,
	    8*sizeof(PixType));

    Wavelet *wavelet = NULL;

    u_int np = 2, nd = 2;
    u_int numlevels = 1;
    PixType minval = -128;
    PixType maxval = 127;

    ProgramName = argv[0];
    while (--argc > 0) {
	argv++;
	if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help"))
	    Usage();
	else if (!strcmp(argv[0], "-v") || !strcmp(argv[0], "--verbose"))
	    Verbose = 1;
	else if (!strcmp(argv[0], "-p") || !strcmp(argv[0], "--primal"))
	    if (argc > 1) {
		np = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "-d") || !strcmp(argv[0], "--dual"))
	    if (argc > 1) {
		nd = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "-l") || !strcmp(argv[0], "--levels"))
	    if (argc > 1) {
		numlevels = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "--min"))
	    if (argc > 1) {
		minval = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "--max"))
	    if (argc > 1) {
		maxval = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else
	    Usage();
    }
    if (minval > maxval)
	Die("The minimum value must not be larger than the maximum value\n");
    if (numlevels < 1)
	Die("The number of levels must be equal to or greather than 1\n");

    wavelet = Wavelet::CreateCDF(np, nd);
    u_int fstart = wavelet->GetGStart() <? wavelet->GetHStart();
    u_int fend = wavelet->GetGEnd() >? wavelet->GetHEnd();
    u_int limit = (-fstart >? fend)+1;
#if 0
    u_int start = fstart, end = fend;
    for (u_int i = 1; i < numlevels; i++) {
	start = 2*start+fstart;
	end = 2*end+fend;
    }
    if (Verbose)
	printf("start = %d, end = %d (real)\n", start, end);
    start -= 2;
    end += 2;
    u_int len = end-start+1;
    u_int len2 = 1;
    while (len2 < len)
	len2 <<= 1;
    u_int lprobe = (-start)>>numlevels;
    u_int hprobe = lprobe+(len2>>numlevels);

    if (Verbose)
	printf("start = %d, end = %d, len = %d, len2 = %d, lprobe = %d, "
	       "hprobe = %d\n",
	       start, end, len, len2, lprobe, hprobe);
#else
    u_int len = 2*limit;
    for (u_int i = 1; i < numlevels; i++)
	len = 2*(len+limit);
    len += 5;
    u_int len2 = 1;
    while (len2 < len)
	len2 <<= 1;
    u_int lprobe = (len>>(numlevels+1));
    u_int hprobe = lprobe+(len2>>numlevels);

    if (Verbose)
	printf("fstart = %d, fend = %d, limit = %d, len = %d, len2 = %d, "
	       "lprobe = %d, hprobe = %d\n",
	       fstart, fend, limit, len, len2, lprobe, hprobe);
#endif

    printf("Wavelet CDF (%d, %d), vector of %d elements, %d levels\n", np, nd,
	   len2, numlevels);

    Channel *test;
    NTChannel minlchan(len, 1);
    NTChannel maxlchan(len, 1);
    NTChannel minhchan(len, 1);
    NTChannel maxhchan(len, 1);

    Timer timer;

    timer.Tic();

    for (u_int i = 0; i < len; i++) {
	test = new NTChannel(len2, 1);
	test->Clear();
	(*test)(i, 0) = PIXTYPE_PROBE_MINVAL;
	test = DoFwt(test, wavelet, numlevels);
	PixType minltest = (*test)(lprobe, 0);
	PixType minhtest = (*test)(hprobe, 0);

	delete test;
	test = new NTChannel(len2, 1);
	test->Clear();
	(*test)(i, 0) = PIXTYPE_PROBE_MAXVAL;
	test = DoFwt(test, wavelet, numlevels);
	PixType maxltest = (*test)(lprobe, 0);
	PixType maxhtest = (*test)(hprobe, 0);
	delete test;

	if (minltest == 0 && maxltest == 0) {
	    minlchan(i, 0) = 0;
	    maxlchan(i, 0) = 0;
	} else if (minltest < maxltest) {
	    minlchan(i, 0) = minval;
	    maxlchan(i, 0) = maxval;
	} else {
	    minlchan(i, 0) = maxval;
	    maxlchan(i, 0) = minval;
	}

	if (minhtest == 0 && maxhtest == 0) {
	    minhchan(i, 0) = 0;
	    maxhchan(i, 0) = 0;
	} else if (minhtest < maxhtest) {
	    minhchan(i, 0) = minval;
	    maxhchan(i, 0) = maxval;
	} else {
	    minhchan(i, 0) = maxval;
	    maxhchan(i, 0) = minval;
	}
    }
    PixType minlpval = DoEval(minlchan, wavelet, len2, numlevels, lprobe);
    PixType maxlpval = DoEval(maxlchan, wavelet, len2, numlevels, lprobe);
    if (Verbose)
	DumpScale(minlchan);
    PixType minhpval = DoEval(minhchan, wavelet, len2, numlevels, hprobe);
    PixType maxhpval = DoEval(maxhchan, wavelet, len2, numlevels, hprobe);

    timer.Toc();

#ifdef __linux
    printf("Low-Pass:  minimum = %Ld, maximum = %Ld\n", minlpval, maxlpval);
    printf("High-Pass: minimum = %Ld, maximum = %Ld\n", minhpval, maxhpval);
#else /* no %Ld */
    printf("Low-Pass:  minimum = %ld, maximum = %ld\n", (long)minlpval,
	   (long)maxlpval);
    printf("High-Pass: minimum = %ld, maximum = %ld\n", (long)minhpval,
	   (long)maxhpval);
#endif /* no %Ld */
    exit(0);
}
