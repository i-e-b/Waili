//
//	Miscellaneous tests
//
//  $Id: Test.C,v 4.1.2.1.2.1 1999/07/20 16:16:19 geert Exp $
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


const char *ProgramName;


static void
#ifdef __GNUG__
__attribute__ ((noreturn))
#endif // __GNUG__
Usage(void)
{
    Die("\nUsage: %s [options] filename\n\n"
	"Valid options are:\n"
	"    -h, --help            display this usage information\n"
	"    -p, --primal number   number of primal vanishing moments (default: 2)\n"
	"    -d, --dual number     number of dual vanishing moments (default: 2)\n"
	"    -l, --levels          number of transform levels (default: -1 is max)\n"
	"\n",
	ProgramName);
}


static void GetRange(const Channel &ch, PixType &min, PixType &max)
{
    u_int cols = ch.GetCols();
    u_int rows = ch.GetRows();

    if (!cols || !rows) {
	min = 0;
	max = 0;
    } else {
	min = max = ch(0, 0);
	for (u_int r = 0; r < rows; r++)
	    for (u_int c = 0; c < cols; c++)
		if (ch(c, r) < min)
		    min = ch(c, r);
		else if (ch(c, r) > max)
		    max = ch(c, r);
    }
}


int main(int argc, char *argv[])
{
    const char *infile = NULL;
    const char *outfile = NULL;
    Image im1, *im2 = NULL;
    u_int channels;
    int maxlevels = -1;
    int np = 2, nd = 2;
    Timer timer;

    ProgramName = argv[0];
    while (--argc > 0) {
	argv++;
	if (!strcmp(argv[0], "-h") || !strcmp(argv[0], "--help"))
	    Usage();
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
		maxlevels = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!infile)
	    infile = argv[0];
	else
	    Usage();
    }
    if (!infile)
	Usage();
    im1.Import(infile);
    im2 = im1.Clone();
    Wavelet *wavelet = Wavelet::CreateCDF(np, nd);
    u8 id = wavelet->GetID();
    delete wavelet;
    channels = im1.GetChannels();
    printf("%dx%dx%d picture\n", im1.GetCols(0), im1.GetRows(0), channels);
    TransformDescriptor transform[] = {
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
	{ TT_ColsRows, id },
    };
    int transformdepth = sizeof(transform)/sizeof(*transform);
    if (maxlevels < transformdepth)
	transformdepth = maxlevels;
    u_int depth;
    puts("Forward FWT transform");
    timer.Tic();
    im1.Fwt(transform, transformdepth);
    depth = im1[0]->GetDepth();
    timer.Toc();
    printf("Transform depth = %d\n", depth);

#if 1
    if (transformdepth == 1) {
	LChannel &channel = *(LChannel *)im1[0];
	PixType min, max;
	for (u_int i = SubBand_LL; i < SubBand_HH+1; i++) {
	    GetRange(*channel[(SubBand)i], min, max);
	    printf("%d range: (%d, %d)\n", i, min, max);
	}
    }
#endif

    outfile = channels > 1 ? "out.ppm" : "out.pgm";
    im1.Export(outfile);
    puts("Inverse FWT transform");
    timer.Tic();
    im1.IFwt();
    timer.Toc();

    double maxerr = 0;
    for (u_int ch = 0; ch < channels; ch++)
	for (u_int r = 0; r < im1.GetRows(ch); r++)
	    for (u_int c = 0; c < im1.GetCols(ch); c++) {
		double d = fabs(im1(c, r, ch)-(*im2)(c, r, ch));
		if (d > maxerr)
		    maxerr = d;
	    }
    printf("Maxerr = %f\n", maxerr);

    outfile = channels > 1 ? "reco.ppm" : "reco.pgm";
    im1.Export(outfile);
    delete im2;

    return(0);
}
