//
//	Counter Example for the `Beer Theorem'
//
//  $Id: Proof.C,v 4.0.2.1.2.1 1999/07/20 16:16:19 geert Exp $
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
	"    -l, --levels          number of transform levels (default: 3)\n"
	"    -s, --scale           scaling factor (default: 1.5)\n"
	"\n",
	ProgramName);
}


static void DoScale(Channel &ch1, Channel &ch2)
{
    u_int c1 = ch1.GetCols();
    u_int r1 = ch1.GetRows();
    u_int c2 = ch2.GetCols();
    u_int r2 = ch2.GetRows();

    for (u_int r = 0; r < r2; r++) {
	f32 rold = (f32)r*(f32)(r1-1)/(f32)(r2-1);
	u_int rx1 = (u_int)rold;
	u_int rx2 = rx1+1 < r1 ? rx1+1 : rx1;
	f32 r2c = rold-rx1;
	f32 r1c = 1-r2c;
	for (u_int c = 0; c < c2; c++) {
	    f32 cold = (f32)c*(f32)(c1-1)/(f32)(c2-1);
	    u_int cx1 = (u_int)cold;
	    u_int cx2 = cx1+1 < c1 ? cx1+1 : cx1;
	    f32 c2c = cold-cx1;
	    f32 c1c = 1-c2c;
	    ch2(c, r) = PixType(c1c*(r1c*ch1(cx1, rx1)+
				     r2c*ch1(cx1, rx2))+
				c2c*(r1c*ch1(cx2, rx1)+
				     r2c*ch1(cx2, rx2)));
	}
    }
}


int main(int argc, char *argv[])
{
    const char *infile = NULL;
    const char *outfile = NULL;
    Image im1, im2;
    u_int channels, cols, rows;
    u_int levels = 3;
    int np = 2, nd = 2;
    f32 scale = 1.5;

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
		levels = atoi(argv[1]);
		argc--;
		argv++;
	    } else
		Usage();
	else if (!strcmp(argv[0], "-s") || !strcmp(argv[0], "--scale"))
	    if (argc > 1) {
		scale = atof(argv[1]);
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
    Wavelet *wavelet = Wavelet::CreateCDF(np, nd);
    u8 id = wavelet->GetID();
    delete wavelet;
    channels = im1.GetChannels();
    if (channels != 1)
	Die("The image must have exactly 1 channel\n");
    cols = im1[0]->GetCols();
    rows = im1[0]->GetRows();
    printf("%dx%dx%d picture\n", cols, rows, channels);
    im2.Resize(u_int(scale*cols), u_int(scale*rows), 1);
    if (levels) {
	TransformDescriptor *transform = new TransformDescriptor[levels];
	for (u_int i = 0; i < levels; i++) {
	    transform[i].type = TT_ColsRows;
	    transform[i].filter = id;
	};
	puts("Forward FWT transform");
	im1.Fwt(transform, levels);
	u_int depth = im1[0]->GetDepth();
	if (depth != levels)
	    Die("depth1 = %d != levels\n", depth);
	printf("Transform depth 1 = %d\n", depth);
	im2.Fwt(transform, levels);
	depth = im2[0]->GetDepth();
	if (depth != levels)
	    Die("depth2 = %d != levels\n", depth);
    }

    Channel *ch1 = im1[0];
    Channel *ch2 = im2[0];
    for (u_int i = 0; i < levels; i++) {
	LChannel *lch1 = (LChannel *)ch1;
	LChannel *lch2 = (LChannel *)ch2;
	DoScale(*(*lch1)[(SubBand)1], *(*lch2)[(SubBand)1]);
	DoScale(*(*lch1)[(SubBand)2], *(*lch2)[(SubBand)2]);
	DoScale(*(*lch1)[(SubBand)3], *(*lch2)[(SubBand)3]);
	ch1 = (*lch1)[(SubBand)0];
	ch2 = (*lch2)[(SubBand)0];
    }
    DoScale(*ch1, *ch2);

    outfile = "out1.pgm";
    im1.Export(outfile);
    outfile = "out2.pgm";
    im2.Export(outfile);
    if (levels) {
	puts("Inverse FWT transform");
	im1.IFwt();
	im2.IFwt();
    }

    outfile = "reco1.pgm";
    im1.Export(outfile);
    outfile = "reco2.pgm";
    im2.Export(outfile);

    return(0);
}
